use std::fmt::Display;
use std::fmt::Debug;
use std::fmt::Formatter;
use std::error::Error;
use std::cell;
use std::cell::RefCell;
use std::rc::Rc;
use std::sync::{Arc, Condvar, Mutex};
use std::sync::atomic::{AtomicBool, Ordering};
use std::thread;
use rand;
use rand::Rng;
use super::Coord;
use super::field;
use super::field::Field;
use super::piece;
use super::piece::{Piece, Type};

#[allow(dead_code)]
#[derive(Debug)]
pub struct RunningError;

impl Display for RunningError {
    fn fmt(&self, f: &mut Formatter) -> ::std::fmt::Result {
        write!(f, "{}", self.description())
    }
}

impl Error for RunningError {
    fn description(&self) -> &str {
        "Attempt to call a configuration function during run."
    }
}

#[allow(dead_code)]
#[derive(Debug)]
pub struct NotRunningError;

impl Display for NotRunningError {
    fn fmt(&self, f: &mut Formatter) -> ::std::fmt::Result {
        write!(f, "{}", self.description())
    }
}

impl Error for NotRunningError {
    fn description(&self) -> &str {
        "Attempt to call a control function on a game which is not running."
    }
}

trait IField : Debug {
    fn new() -> Self;

    fn get(&self, Coord) -> Result<bool, field::SizeError>;
}

impl IField for Field {
    fn new() -> Self {
        Field::new()
    }

    fn get(&self, c:Coord) -> Result<bool, field::SizeError> {
        self.get(c)
    }
}

trait IPiece <F: IField> : Debug {
    fn new(Type, u32, Rc<RefCell<F>>) -> Self;

    fn time_step(&mut self, u32) -> Result<bool, piece::LockError>;

    fn handle_input(&mut self, input: piece::Input) -> Result<bool, piece::LockError>;
}

impl IPiece<Field> for Piece {
    fn new(t: Type, d: u32, f: Rc<RefCell<Field>>) -> Self {
        Piece::new(t, d, f)
    }

    fn time_step(&mut self, g:u32) -> Result<bool, piece::LockError> {
        self.time_step(g)
    }

    fn handle_input(&mut self, input: piece::Input) -> Result<bool, piece::LockError> {
        self.handle_input(input)
    }
}

/*
 * Oh, how dearly I wish to say
type Callback<F, P> = Fn(cell::Ref<F>, &P, Option<&P>)->() + Send
 * so that Arc<Mutex<Box<Fn(cell::Ref<F>, &P, Option<&P>)->() + Send>>>
 * could be Arc<Mutex<Box<Callback>>>
 * alas.
 */

struct SharedState<F: IField, P: IPiece<F>> {
    callback: Arc<Mutex<Box<Fn(cell::Ref<F>, &P, Option<&P>)->() + Send>>>,
    input_queue: Arc<Mutex<Vec<piece::Input>>>,
    game_live: Arc<AtomicBool>,
    pause_pair: Arc<(Condvar, Mutex<bool>)>,
    step_ratio: Arc<Mutex<(u32, u32)>>,
    lock_delay: Arc<Mutex<u32>>,
}
// deriving demands F: Clone and P: Clone
impl<F: IField, P: IPiece<F>> Clone for SharedState<F, P> {
    fn clone(&self) -> Self {
        SharedState {
            callback: self.callback.clone(),
            input_queue: self.input_queue.clone(),
            game_live: self.game_live.clone(),
            pause_pair: self.pause_pair.clone(),
            step_ratio: self.step_ratio.clone(),
            lock_delay: self.lock_delay.clone(),
        }
    }
}

struct GameInfo<F: IField, P:IPiece<F>> {
    field: Rc<RefCell<F>>,
    piece: P,
    step_counter: u32,
    shared: SharedState<F, P>
}

#[allow(dead_code)]
struct GameImpl<F : IField + 'static, P : IPiece<F> + 'static> {
    join_token: RefCell<Option<thread::JoinHandle<()>>>,
    shared: SharedState<F, P>
}

#[allow(dead_code)]
pub struct Game {
    pimpl: GameImpl<Field, Piece>
}
#[allow(dead_code, unused_variables)]
impl Game {
    pub fn new(callback: Box<Fn(cell::Ref<Field>, &Piece, Option<&Piece>)->() + Send>) -> Game {
        Game {
            pimpl: GameImpl::new(callback)
        }
    }

    pub fn run(&mut self) -> Result<(), RunningError> {
        self.pimpl.run()
    }

    pub fn pause(&mut self) -> Result<(), NotRunningError> {
        self.pimpl.pause()
    }

    pub fn set_renderer(&mut self, callback: Box<Fn(cell::Ref<Field>, &Piece, Option<&Piece>)->() + Send>) -> Result<(), RunningError> {
        self.pimpl.set_renderer(callback)
    }

    pub fn queue_input(&mut self, input: piece::Input) -> Result<(), NotRunningError> {
        self.pimpl.queue_input(input)
    }

    pub fn is_game_over(&self) -> bool {
        self.pimpl.is_game_over()
    }

    pub fn set_step_ratio(&mut self, denominator: u32, numerator: u32) -> Result<(), RunningError> {
        self.pimpl.set_step_ratio(denominator, numerator)
    }

    pub fn set_lock_delay(&mut self, delay: u32) -> Result<(), RunningError> {
        self.pimpl.set_lock_delay(delay)
    }
}


#[allow(dead_code, unused_variables)]
impl<F: IField + 'static, P: IPiece<F> + 'static> GameImpl<F, P> {
    // internal stuff

    // static fns used by thread
    fn consume_input(game: &mut GameInfo<F, P>) -> () {
        let queue: Vec<piece::Input>;
        {
            /*
            this lock should live for only the statement which drains the vec, but:

            error: borrowed value does not live long enough
            let queue: Vec<piece::Input> = self.input_queue.lock().unwrap().drain(0..).collect();

            note: reference must be valid for the destruction scope surrounding statement at
            let queue: Vec<piece::Input> = self.input_queue.lock().unwrap().drain(0..).collect();

            note: ...but borrowed value is only valid for the statement at
            let queue: Vec<piece::Input> = self.input_queue.lock().unwrap().drain(0..).collect();

            help: consider using a `let` binding to increase its lifetime
            let queue: Vec<piece::Input> = self.input_queue.lock().unwrap().drain(0..).collect();
             */
            let mut weird_lifetime_bullshit = game.shared.input_queue.lock().unwrap();
            queue = weird_lifetime_bullshit.drain(0..).collect();
        }
        for input in queue {
            let result = game.piece.handle_input(input).unwrap();
            if result {
                if GameImpl::scan_for_loss(game) {
                    GameImpl::game_over(game)
                } else {
                    game.piece = P::new(rand::thread_rng().gen(), *(game.shared.lock_delay.lock().unwrap()), game.field.clone());
                }
            }
        }
    }
    fn game_over(game: &mut GameInfo<F, P>) -> () {
        game.shared.game_live.store(false, Ordering::Relaxed);
    }
    fn scan_for_loss(game: &GameInfo<F, P>) -> bool {
        for i in 0..field::WIDTH {
            for j in 20..field::HEIGHT {
                if game.field.borrow().get(Coord{x:i as i32,y:j as i32}).unwrap() {
                    return true;
                }
            }
        }
        false
    }
    fn thread_func(mut game_info: GameInfo<F, P>) -> () {
        use std::time::{Duration, Instant};
        use std::thread::sleep;
        let (ref pause_var, ref pause_lock) = *game_info.shared.pause_pair.clone();
        while game_info.shared.game_live.load(Ordering::Relaxed) {
            // check pause
            let mut paused = pause_lock.lock().unwrap();
            while *paused {
                paused = pause_var.wait(paused).unwrap();
            }
            drop(paused);
            let start_time: Instant = Instant::now();
            // consume input
            GameImpl::consume_input(&mut game_info);
            // time step
            let (denominator, numerator) = *(game_info.shared.step_ratio.lock().unwrap());
            game_info.step_counter += 1;
            if game_info.step_counter >= denominator {
                let piece_locked = game_info.piece.time_step(numerator).unwrap();
                if piece_locked {
                    if GameImpl::scan_for_loss(&game_info) {
                        GameImpl::game_over(&mut game_info);
                    } else {
                        // new piece
                        game_info.piece = P::new(rand::thread_rng().gen(), *(game_info.shared.lock_delay.lock().unwrap()), game_info.field.clone());
                    }
                }
                game_info.step_counter = 0;
            }
            // render callback
            let cb = game_info.shared.callback.lock().unwrap();
            cb(game_info.field.borrow(), &game_info.piece, None);
            const TARGET_FPS: u32 = 60;
            const NANO: u32 = 1000000000;
            let target_duration: Duration = Duration::new(0, NANO/TARGET_FPS);
            let elapsed = start_time.elapsed();
            if target_duration > elapsed {
                sleep(target_duration - elapsed);
            }
        }
    }
    // private fns
    fn end_game(&mut self) -> () {
        // can't panic here because this is called by drop
        self.shared.game_live.store(false, Ordering::Relaxed);
        {
            let (ref pause_cond, ref pause_lock) = *self.shared.pause_pair;

            match pause_lock.lock() {
                Ok(mut paused) => {
                    if *paused {
                        *paused = false;
                        pause_cond.notify_all();
                    }
                }
                Err(err) => println!("internal thread poisoned pause lock. {:?}", err)
            }
        }

        // joining directly would move, which we can't do anywhere because impl Drop
        let option = self.join_token.borrow_mut().take();
        match option {
            Some(token) => {
                let result = token.join();
                if let Err(err) = result {
                    println!("internal thread panicked. {:?}", err);
                }
            }
            None => ()
        };
    }

    // cleans up join token
    fn is_game_running(&self) -> bool {
        if ! self.shared.game_live.load(Ordering::Relaxed) {
            let (ref pause_cond, ref pause_lock) = *self.shared.pause_pair;
            let mut paused = pause_lock.lock().unwrap();
            if *paused {
                *paused = false;
                pause_cond.notify_all();
            }
            let option = self.join_token.borrow_mut().take();
            if let Some(token) = option {
                if let Err(message) = token.join() {
                    println!("Game internal thread panicked. {:?}", message);
                }
            }
            false
        } else {
            let (_, ref pause_lock) = *self.shared.pause_pair;
            let paused = pause_lock.lock().unwrap();
            ! *paused
        }
    }

    // used by Game's pub functions
    fn new(callback: Box<Fn(cell::Ref<F>, &P, Option<&P>)->() + Send>) -> Self {
        const DEFAULT_STEP_RATIO: (u32, u32) = (15, 1);
        const DEFAULT_LOCK_DELAY: u32 = 0;
        GameImpl {
            shared: SharedState {
                callback: Arc::new(Mutex::new(callback)),
                input_queue: Arc::new(Mutex::new(Vec::new())),
                game_live: Arc::new(AtomicBool::new(false)),
                pause_pair: Arc::new((Condvar::new(), Mutex::new(true))),
                step_ratio: Arc::new(Mutex::new(DEFAULT_STEP_RATIO)),
                lock_delay: Arc::new(Mutex::new(DEFAULT_LOCK_DELAY)),
            },
            join_token: RefCell::new(None)
        }
    }

    fn run(&mut self) -> Result<(), RunningError> {
        if self.is_game_running() {
            Err(RunningError)
        } else {
            // using the is_none() result directly causes the borrow to live during the if block, which causes a panic at borrow_mut
            let thread_absent: bool = self.join_token.borrow().is_none();
            let (ref pause_cond, ref pause_lock) = *self.shared.pause_pair;
            let mut paused = pause_lock.lock().unwrap();
            if thread_absent {
                let share = self.shared.clone();
                let ld: u32 = *(self.shared.lock_delay.lock().unwrap()); // appease borrowck
                *paused = false;
                self.shared.game_live.store(true, Ordering::Relaxed);
                *self.join_token.borrow_mut() = Some(thread::Builder::new().name("GameImpl internal thread".to_string()).spawn(move || {
                    let f: Rc<RefCell<F>> = Rc::new(RefCell::new(F::new()));
                    let info = GameInfo {
                        field: f.clone(),
                        piece: P::new(rand::thread_rng().gen(), ld, f), // borrow `share` here ...
                        step_counter: 0,
                        shared: share // ... will block moving `share` here
                    };

                    GameImpl::thread_func(info)
                }).unwrap());
            } else {
                *paused = false;
                pause_cond.notify_all();
            }
            Ok(())
        }
    }

    fn pause(&mut self) -> Result<(), NotRunningError> {
        let (_, ref pause_lock) = *self.shared.pause_pair;
        if ! self.is_game_running() {
            Err(NotRunningError)
        } else {
            let mut paused = pause_lock.lock().unwrap();
            *paused = true;
            Ok(())
        }
    }

    fn set_renderer(&mut self, callback: Box<Fn(cell::Ref<F>, &P, Option<&P>)->() + Send>) -> Result<(), RunningError> {
        if self.is_game_running() {
            return Err(RunningError);
        }
        let mut my_cb = self.shared.callback.lock().unwrap();
        *my_cb = callback;
        Ok(())
    }

    fn queue_input(&mut self, input: piece::Input) -> Result<(), NotRunningError> {
        if self.is_game_running() {
            let mut queue = self.shared.input_queue.lock().unwrap();
            queue.push(input);
            Ok(())
        } else {
            Err(NotRunningError)
        }
    }

    fn is_game_over(&self) -> bool {
        if self.is_game_running() {
            // game is neither over nor paused
            false
        } else {
            // game is either over (& thread cleaned), or thread is live but paused
            ! self.shared.game_live.load(Ordering::Relaxed)
        }
    }

    fn set_step_ratio(&mut self, denominator: u32, numerator: u32) -> Result<(), RunningError> {
        if self.is_game_running() {
            return Err(RunningError)
        } else {
            *(self.shared.step_ratio.lock().unwrap()) = (denominator, numerator);
            Ok(())
        }
    }

    pub fn set_lock_delay(&mut self, delay: u32) -> Result<(), RunningError> {
        if self.is_game_running() {
            return Err(RunningError)
        } else {
            *(self.shared.lock_delay.lock().unwrap()) = delay;
            Ok(())
        }
    }
}

impl<F: IField + 'static, P: IPiece<F> + 'static> Drop for GameImpl<F, P> {
    fn drop(&mut self) -> () {
        self.end_game();
    }
}

#[cfg(test)]
mod test {
    use super::{GameImpl, IField, IPiece};
    use super::super::Coord;
    use super::super::field;
    use super::super::piece::{Type, Input, LockError};
    use super::super::piece;
    use std::collections::HashMap;
    use std::sync::{Mutex, Arc};
    use std::thread::sleep;
    use std::time::Duration;
    use std::rc::Rc;
    use std::cell;
    use std::cell::{Cell, RefCell};

    #[derive(Debug)]
    struct MockField {
        piece_type_log: RefCell<Vec<Type>>,
        get_results: RefCell<HashMap<Coord, bool>>,
    }
    impl MockField {
        pub fn size_check(c: &Coord) -> bool {
            (c.x < 0 || c.y < 0 || c.x >= field::WIDTH as i32 || c.y >= field::HEIGHT as i32)
        }
    }
    impl IField for MockField {
        fn new() -> Self {
            MockField {
                piece_type_log: RefCell::new(Vec::new()),
                get_results: RefCell::new(HashMap::new())
            }
        }

        fn get(&self, c:Coord) -> Result<bool, field::SizeError> {
            if MockField::size_check(&c) {
                Err(field::SizeError)
            } else {
                Ok(*(self.get_results.borrow().get(&c).unwrap_or(&false)))
            }
        }
    }
    #[derive(Debug)]
    struct MockPiece {
        lock_delay: u32,
        time_step_call_count: u32,
        time_step_step_count: u32,
        time_step_result: Cell<bool>,
        input_log: Vec<Input>
    }
    impl IPiece<MockField> for MockPiece {

        fn new(t: Type, lock_delay: u32, f: Rc<RefCell<MockField>>) -> Self {
            let fref = f.borrow();
            fref.piece_type_log.borrow_mut().push(t);
            MockPiece {
                lock_delay: lock_delay,
                time_step_call_count: 0,
                time_step_step_count: 0,
                time_step_result: Cell::new(false),
                input_log: Vec::new()
            }
        }

        fn time_step(&mut self, g:u32) -> Result<bool, LockError> {
            self.time_step_call_count += 1;
            self.time_step_step_count += g;
            Ok(self.time_step_result.get())
        }

        fn handle_input(&mut self, input: piece::Input) -> Result<bool, piece::LockError> {
            if self.time_step_result.get() {
                Err(piece::LockError)
            } else {
                self.input_log.push(input);
                Ok(self.time_step_result.get())
            }
        }
    }
    fn dummy_render_function(_: cell::Ref<MockField>, _: &MockPiece, _: Option<&MockPiece>) {
    }
    fn alt_render_function(_: cell::Ref<MockField>, _: &MockPiece, _: Option<&MockPiece>) {
    }
    #[test]
    fn test_new() {
        // check that the constructor doesn't panic
        let _test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(dummy_render_function));
    }
    #[test]
    fn test_run_callback() {
        let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(dummy_render_function));
        // set callback
        assert!(test_game.set_renderer(Box::new(alt_render_function)).is_ok());
        // check run/stop
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.pause().is_ok());

        // test that destructor drops the internal thread
        {
            let count_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));
            {
                let count_mutex = count_mutex.clone();
                let counting_render_function = move |_: cell::Ref<MockField>, _: &MockPiece, _: Option<&MockPiece>| {
                    let mut count = count_mutex.lock().unwrap();
                    *count += 1;
                };
                assert!(test_game.set_renderer(Box::new(counting_render_function)).is_ok());
            }
            assert!(test_game.run().is_ok());
            drop(test_game);
            {
                let mut count = count_mutex.lock().unwrap();
                *count = 0;
            }
            sleep(Duration::from_secs(1));
            let count = count_mutex.lock().unwrap();
            assert_eq!(0, *count);
        }
    }
    #[test]
    fn test_framerate() {
        const EXPECTED_FPS: u32 = 60;
        const DELTA: u32 = 15;
        let callback_call_count_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));
        let mut test_game: GameImpl<MockField, MockPiece>;
        // Check that the game loop limits itself reasonably
        {
            let callback_call_count_mutex = callback_call_count_mutex.clone();
            let counting_render_function = move |_: cell::Ref<MockField>, _: &MockPiece, _: Option<&MockPiece>| {
                let mut count = callback_call_count_mutex.lock().unwrap();
                *count = *count + 1;
            };
            test_game = GameImpl::new(Box::new(counting_render_function));
        }
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.pause().is_ok());
        let count = *(callback_call_count_mutex.lock().unwrap());
        assert!(count < EXPECTED_FPS + DELTA);
        assert!(count > EXPECTED_FPS - DELTA);
        // Check that the game loop tolerates a slow callback
        {
            let callback_call_count_mutex = callback_call_count_mutex.clone();
            {
                let mut count = callback_call_count_mutex.lock().unwrap();
                *count = 0;
            }
            let slow_render_function = move|_: cell::Ref<MockField>, _: &MockPiece, _: Option<&MockPiece>| {
                let mut count = callback_call_count_mutex.lock().unwrap();
                *count = *count + 1;
                sleep(Duration::from_secs(1));
            };
            assert!(test_game.set_renderer(Box::new(slow_render_function)).is_ok());
        }
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(3));
        assert!(test_game.pause().is_ok());
        let count = *(callback_call_count_mutex.lock().unwrap());
        assert!(count < 5);
        assert!(count > 1);
    }
    #[test]
    fn test_step_ratio() {
        let most_recent_time_step_call_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));
        let most_recent_time_step_step_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));
        let callback_call_count_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));
        let mut test_game: GameImpl<MockField, MockPiece>;
        {
            let most_recent_time_step_call_mutex = most_recent_time_step_call_mutex.clone();
            let most_recent_time_step_step_mutex = most_recent_time_step_step_mutex.clone();
            let callback_call_count_mutex = callback_call_count_mutex.clone();
            let matching_render_function = move |_: cell::Ref<MockField>, piece: &MockPiece, _: Option<&MockPiece>| {
                let mut call = most_recent_time_step_call_mutex.lock().unwrap();
                let mut step = most_recent_time_step_step_mutex.lock().unwrap();
                let mut count = callback_call_count_mutex.lock().unwrap();
                *call = (*piece).time_step_call_count;
                *step = (*piece).time_step_step_count;
                *count = *count + 1;
            };
            test_game = GameImpl::new(Box::new(matching_render_function));
            assert!(test_game.set_step_ratio(1, 1).is_ok());
        }
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.pause().is_ok());
        {
            let calls = most_recent_time_step_call_mutex.lock().unwrap();
            let step = most_recent_time_step_step_mutex.lock().unwrap();
            let mut count = callback_call_count_mutex.lock().unwrap();
            assert!(*calls > 0);
            assert_eq!(*count, *calls);
            assert!(*step > 0);
            *count = 0;
            test_game.end_game();
        }
        assert!(test_game.set_step_ratio(3, 1).is_ok());
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.pause().is_ok());
        {
            let mut callbacks = callback_call_count_mutex.lock().unwrap();
            let timesteps = most_recent_time_step_call_mutex.lock().unwrap();
            assert!(*callbacks / *timesteps == 3, "{} / {}", *callbacks, *timesteps);
            *callbacks = 0;
            test_game.end_game();
        }
        assert!(test_game.set_step_ratio(1, 3).is_ok());
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.pause().is_ok());
        {
            let callbacks = callback_call_count_mutex.lock().unwrap();
            let timestep_calls = most_recent_time_step_call_mutex.lock().unwrap();
            let timesteps = most_recent_time_step_step_mutex.lock().unwrap();
            assert_eq!(*callbacks, *timestep_calls);
            assert_eq!(3 * (*callbacks), *timesteps);
        }
    }
    #[test]
    fn whitebox_test_input() {
        use std::collections::HashSet;
        let most_recent_input_log_mutex: Arc<Mutex<Vec<Input>>> = Arc::new(Mutex::new(Vec::new()));
        let mut test_game: GameImpl<MockField, MockPiece>;
        {
            let most_recent_input_log_mutex = most_recent_input_log_mutex.clone();
            let logging_callback = move |_: cell::Ref<MockField>, piece: &MockPiece, _: Option<&MockPiece>| {
                let mut log = most_recent_input_log_mutex.lock().unwrap();
                *log = piece.input_log.clone();// this is a lot of copying but it's a test
            };
            test_game = GameImpl::new(Box::new(logging_callback));
        }
        const INPUTS: [Input; 5] = [Input::RotateCCW, Input::RotateCW, Input::ShiftLeft, Input::ShiftRight, Input::HardDrop];
        assert!(test_game.run().is_ok());
        for input in &INPUTS {
            assert!(test_game.queue_input(*input).is_ok());
        }
        sleep(Duration::from_secs(2));
        assert!(test_game.pause().is_ok());
        // Todo: port this?
        // CPPUNIT_ASSERT( PieceDummy::compare_handleInput_arg(inputs) );
        let log = most_recent_input_log_mutex.lock().unwrap();
        let (observed_set, expected_set): (HashSet<Input>, HashSet<Input>) = (log.iter().cloned().collect(), INPUTS.iter().cloned().collect());
        assert_eq!(observed_set, expected_set);
    }
    #[test]
    fn test_exceptions() {
        let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(dummy_render_function));
        assert!(test_game.queue_input(Input::ShiftRight).is_err());
        assert!(test_game.pause().is_err());

        assert!(test_game.run().is_ok());

        assert!(test_game.set_renderer(Box::new(alt_render_function)).is_err());
        assert!(test_game.run().is_err());
    }
    #[test]
    fn test_game_over() {
        use std::sync::mpsc;
        let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(dummy_render_function));
        test_game.set_step_ratio(1, 1).unwrap();
        let (send, recieve) = mpsc::channel();
        // check that blocks in each corner cause game over
        const RIGHT: i32 = field::WIDTH as i32 - 1;
        const TOP: i32 = field::HEIGHT as i32 - 1;
        for block_ref in [Coord{x:0,y:20}, Coord{x:RIGHT,y:20}, Coord{x:0, y:TOP}, Coord{x:RIGHT,y:TOP}].iter() {
            let send = send.clone();
            let block = *block_ref;
            let game_ending_render_function = move |field: cell::Ref<MockField>, piece: &MockPiece, _: Option<&MockPiece>|->() {
                if field.get(block).unwrap() {
                    field.get_results.borrow_mut().insert(block, false);
                    send.send(()).expect("send failed");
                } else {
                    field.get_results.borrow_mut().insert(block, true);
                    piece.time_step_result.set(true);
                }
            };
            assert!(test_game.set_renderer(Box::new(game_ending_render_function)).is_ok());
            assert!(test_game.run().is_ok());
            assert!(recieve.recv().is_ok());
            assert!(test_game.is_game_over(), "block {:?}", block);
            assert!(test_game.pause().is_err(), "block {:?}", block);
        }
        // test that blocks in corners of visible field do not cause game over
        for block_ref in [Coord{x:0,y:0}, Coord{x:RIGHT,y:0}, Coord{x:0,y:19}, Coord{x:RIGHT,y:19}].iter() {
            let block = *block_ref;
            let game_ending_render_function = move |field: cell::Ref<MockField>, piece: &MockPiece, _: Option<&MockPiece>|->() {
                if ! field.get(block).unwrap() {
                    field.get_results.borrow_mut().insert(block, true);
                    piece.time_step_result.set(true);
                }
            };
            assert!(test_game.set_renderer(Box::new(game_ending_render_function)).is_ok());
            assert!(test_game.run().is_ok());
            sleep(Duration::from_secs(5));
            assert!( ! test_game.is_game_over(), "block {:?}", block);
            assert!( test_game.pause().is_ok(), "block {:?}", block);
        }
    }
    #[test]
    fn test_replaces_piece_on_lock() {
        let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(dummy_render_function));
        let type_log: Arc<Mutex<Vec<Type>>> = Arc::new(Mutex::new(Vec::new()));
        {
            let type_log = type_log.clone();
            let render_func = move |field: cell::Ref<MockField>, piece: &MockPiece, _: Option<&MockPiece>|->() {
                let mut log = type_log.lock().unwrap();
                let mut new_log_ref = field.piece_type_log.borrow_mut();
                log.extend(new_log_ref.drain(..));
                piece.time_step_result.set(true);
            };
            assert!(test_game.set_renderer(Box::new(render_func)).is_ok());
        }
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(5));
        test_game.end_game();
        let log = type_log.lock().unwrap();

        // assert that locked pieces were replaced
        assert!(log.len() > 1);
        // assert that new pieces were not all the same type
        {
            let mut types = log.clone();
            types.sort();
            types.dedup();
            assert!(types.len() > 1);
        }
    }
    #[test]
    fn test_panic_tolerance() {
        use std::panic::catch_unwind;
        fn panicing_render_func(_: cell::Ref<MockField>, _: &MockPiece, _: Option<&MockPiece>) -> () {
            panic!("dummy panic");
        };
        {
            let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(panicing_render_func));
            assert!(test_game.run().is_ok());
            sleep(Duration::from_secs(1));
        }
        {
            let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(panicing_render_func));
            assert!(test_game.run().is_ok());
            sleep(Duration::from_secs(1));
            assert!(test_game.pause().is_ok());
        }
        {
            let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(panicing_render_func));
            assert!(test_game.run().is_ok());
            assert!(test_game.queue_input(Input::HardDrop).is_ok());
            sleep(Duration::from_secs(1));
            assert!(test_game.queue_input(Input::RotateCCW).is_ok());
            assert!(test_game.pause().is_ok());
            assert!(test_game.run().is_ok());
        }
        // if test_game panics during drop, catch_unwind won't save us
        let _ = catch_unwind(|| {
            let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(Box::new(panicing_render_func));
            let _ = test_game.run();
            sleep(Duration::from_secs(1));
            panic!("let's unwind");
        });
    }
    #[test]
    fn test_set_lock_delay() {
        let mut test_game: GameImpl<MockField, MockPiece>;
        let observed_lock_delay_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(1));
        {
            let observed_lock_delay_mutex = observed_lock_delay_mutex.clone();
            let time_step_checking_render_func = move |_: cell::Ref<MockField>, p: &MockPiece, _: Option<&MockPiece>|->() {
                *(observed_lock_delay_mutex.lock().unwrap()) = p.lock_delay;
                p.time_step_result.set(true);
            };
            test_game = GameImpl::new(Box::new(time_step_checking_render_func));
        }
        assert!(test_game.set_lock_delay(0).is_ok());
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.set_lock_delay(1).is_err());
        assert!(test_game.pause().is_ok());
        assert_eq!(*(observed_lock_delay_mutex.lock().unwrap()), 0);

        assert!(test_game.set_lock_delay(1).is_ok());
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.pause().is_ok());
        assert_eq!(*(observed_lock_delay_mutex.lock().unwrap()), 1);
    }
}
