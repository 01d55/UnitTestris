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
use super::field::Field;
use super::piece::{Piece, Type};
use super::piece;

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
}

impl IField for Field {
    fn new() -> Self {
        Field::new()
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

struct GameInfo<F: IField, P:IPiece<F>> {
    field: Rc<RefCell<F>>,
    piece: P,
    // shared
    callback: Arc<Mutex<Box<Fn(cell::Ref<F>, &P, Option<&P>)->() + Send>>>,
    input_queue: Arc<Mutex<Vec<piece::Input>>>,
    game_live: Arc<AtomicBool>,
    pause_pair: Arc<(Condvar, Mutex<bool>)>,

}

#[allow(dead_code)]
struct GameImpl<F : IField + 'static, P : IPiece<F> + 'static> {
    join_token: Option<thread::JoinHandle<()>>,
    // shared
    callback: Arc<Mutex<Box<Fn(cell::Ref<F>, &P, Option<&P>)->() + Send>>>,
    input_queue: Arc<Mutex<Vec<piece::Input>>>,
    game_live: Arc<AtomicBool>,
    pause_pair: Arc<(Condvar, Mutex<bool>)>,
}
/*
#[allow(dead_code)]
pub struct Game {
    pimpl: GameImpl<Field, Piece>
}
#[allow(dead_code, unused_variables)]
impl Game {
    pub fn new(callback:&Fn(&Field, &Piece, Option<&Piece>)->()) -> Game {
        unimplemented!()
    }

    pub fn run(&mut self) -> Result<(), RunningError> {
        unimplemented!()
    }

    pub fn pause(&mut self) -> Result<(), NotRunningError> {
        unimplemented!()
    }

    pub fn set_renderer(&mut self, callback:&Fn(&Field, &Piece, Option<&Piece>) -> ()) -> Result<(), RunningError> {
        unimplemented!()
    }

    pub fn queue_input(&mut self, input: piece::Input) -> Result<(), NotRunningError> {
        unimplemented!()
    }

    pub fn is_game_over(&self) -> bool {
        unimplemented!()
    }
}*/

#[allow(dead_code, unused_variables)]
impl<F: IField + 'static, P: IPiece<F> + 'static> GameImpl<F, P> {
    // internal stuff
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
            let mut weird_lifetime_bullshit = game.input_queue.lock().unwrap();
            queue = weird_lifetime_bullshit.drain(0..).collect();
        }
        for input in queue {
            let result = game.piece.handle_input(input).unwrap();
            if result {
                // scan for loss
                // if loss game over
                // else new piece
            }
        }
    }
    fn game_over(&mut self) -> () {
        // can't panic here because this is called by drop

        let (ref pause_cond, ref pause_lock) = *self.pause_pair;

        *pause_lock.lock().unwrap() = false;
        pause_cond.notify_all();

        self.game_live.store(false, Ordering::Relaxed);

        // joining directly would move, which we can't do anywhere because impl Drop
        let option = self.join_token.take();
        match option {
            Some(token) => {
                let result = token.join();
            }
            None => ()
        };
    }
    fn scan_for_loss(game: &GameInfo<F, P>) -> bool {
        unimplemented!()
    }
    fn thread_func(mut game_info: GameInfo<F, P>) -> () {
        let (ref pause_var, ref pause_lock) = *game_info.pause_pair.clone();
        while game_info.game_live.load(Ordering::Relaxed) {
            // check pause
            let mut paused = pause_lock.lock().unwrap();
            while *paused {
                paused = pause_var.wait(paused).unwrap();
            }
            // consume input
            GameImpl::consume_input(&mut game_info);
            // render callback
            let cb = game_info.callback.lock().unwrap();
            cb(game_info.field.borrow(), &game_info.piece, None);
        }
    }
    // used by Game's pub functions
    fn new(callback: Box<Fn(cell::Ref<F>, &P, Option<&P>)->() + Send>) -> Self {
        GameImpl {
            callback: Arc::new(Mutex::new(callback)),
            input_queue: Arc::new(Mutex::new(Vec::new())),
            game_live: Arc::new(AtomicBool::new(true)),
            pause_pair: Arc::new((Condvar::new(), Mutex::new(true))),
            join_token: None
        }
    }

    fn run(&mut self) -> Result<(), RunningError> {
        let (ref pause_cond, ref pause_lock) = *self.pause_pair;
        let mut paused = pause_lock.lock().unwrap();
        if ! *paused {
            Err(RunningError)
        } else if self.join_token.is_none() {
            let i = self.input_queue.clone();
            let cb = self.callback.clone();
            let live = self.game_live.clone();
            let p = self.pause_pair.clone();
            *paused = false;
            self.join_token = Some(thread::Builder::new().name("GameImpl internal thread".to_string()).spawn(move || {
                let f: Rc<RefCell<F>> = Rc::new(RefCell::new(F::new()));
                let info = GameInfo {
                    field: f.clone(),
                    piece: P::new(Type::I, 0, f),
                    callback: cb,
                    input_queue: i,
                    game_live: live,
                    pause_pair: p,
                };

                GameImpl::thread_func(info)
            }).unwrap());
            Ok(())
        } else {
            *paused = false;
            pause_cond.notify_all();
            Ok(())
        }
    }

    fn pause(&mut self) -> Result<(), NotRunningError> {
        let (_, ref pause_lock) = *self.pause_pair;
        let mut paused = pause_lock.lock().unwrap();
        if *paused || self.join_token.is_none() {
            Err(NotRunningError)
        } else {
            *paused = true;
            Ok(())
        }
    }

    fn set_renderer(&mut self, callback: Box<Fn(cell::Ref<F>, &P, Option<&P>)->() + Send>) -> Result<(), RunningError> {
        let (_, ref pause_lock) = *self.pause_pair;
        let paused = *pause_lock.lock().unwrap();
        if !paused {
            Err(RunningError)
        } else {
            let mut my_cb = self.callback.lock().unwrap();
            *my_cb = callback;
            Ok(())
        }
    }

    fn queue_input(&mut self, input: piece::Input) -> Result<(), NotRunningError> {
        let (_, ref pause_lock) = *self.pause_pair;
        let paused = *pause_lock.lock().unwrap();
        if paused {
            Err(NotRunningError)
        } else {
            let mut queue = self.input_queue.lock().unwrap();
            queue.push(input);
            Ok(())
        }
    }

    fn is_game_over(&self) -> bool {
        unimplemented!()
    }
}

impl<F: IField + 'static, P: IPiece<F> + 'static> Drop for GameImpl<F, P> {
    fn drop(&mut self) -> () {
        self.game_over();
    }
}

#[cfg(test)]
mod test {
    use super::{GameImpl, IField, IPiece};
    use super::super::piece::{Type, Input, LockError};
    use super::super::piece;
    use std::sync::{Mutex, Arc};
    use std::thread::sleep;
    use std::time::Duration;
    use std::rc::Rc;
    use std::cell;
    use std::cell::RefCell;

    #[derive(Debug)]
    struct MockField;
    impl IField for MockField {
        fn new() -> Self {
            MockField
        }
    }
    #[derive(Debug)]
    struct MockPiece {
        time_step_call_count: u32,
        time_step_step_count: u32,
        input_log: Vec<Input>
    }
    impl IPiece<MockField> for MockPiece {

        fn new(_: Type, _: u32, _: Rc<RefCell<MockField>>) -> Self {
            MockPiece {
                time_step_call_count: 0,
                time_step_step_count: 0,
                input_log: Vec::new()
            }
        }

        fn time_step(&mut self, g:u32) -> Result<bool, LockError> {
            self.time_step_call_count += 1;
            self.time_step_step_count += g;
            Ok(false)
        }

        fn handle_input(&mut self, input: piece::Input) -> Result<bool, piece::LockError> {
            self.input_log.push(input);
            Ok(false)
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
        // c++ tests attempt to verify that game runs approx 60fps
        //   but it's squirrely, so skipping that for now

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
    fn whitebox_test_run_callback() {
        let most_recent_time_step_call_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));
        let most_recent_time_step_step_mutex: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));
        let mut test_game: GameImpl<MockField, MockPiece>;
        {
            let most_recent_time_step_call_mutex = most_recent_time_step_call_mutex.clone();
            let most_recent_time_step_step_mutex = most_recent_time_step_step_mutex.clone();
            let matching_render_function = move |_: cell::Ref<MockField>, piece: &MockPiece, _: Option<&MockPiece>| {
                let mut call = most_recent_time_step_call_mutex.lock().unwrap();
                let mut step = most_recent_time_step_step_mutex.lock().unwrap();
                *call = (*piece).time_step_call_count;
                *step = (*piece).time_step_step_count;
            };
            test_game = GameImpl::new(Box::new(matching_render_function));
        }
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.pause().is_ok());
        let calls = most_recent_time_step_call_mutex.lock().unwrap();
        let step = most_recent_time_step_step_mutex.lock().unwrap();
        assert!(*calls > 0);
        assert!(*step > 0);
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
}
