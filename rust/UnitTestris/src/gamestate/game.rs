use std::fmt::Display;
use std::fmt::Formatter;
use std::error::Error;
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

trait IField {
    fn new() -> Self;
}

impl IField for Field {
    fn new() -> Self {
        Field::new()
    }
}

trait IPiece <F: IField>{
    fn new(Type, u32, &mut F) -> Self;

    fn time_step(&mut self, u32) -> Result<bool, piece::LockError>;

    fn handle_input(&mut self, input: piece::Input) -> Result<bool, piece::LockError>;
}

impl IPiece<Field> for Piece {
    fn new(t: Type, d: u32, f: &mut Field) -> Self {
        Piece::new(t, d, f)
    }

    fn time_step(&mut self, g:u32) -> Result<bool, piece::LockError> {
        self.time_step(g)
    }

    fn handle_input(&mut self, input: piece::Input) -> Result<bool, piece::LockError> {
        self.handle_input(input)
    }
}

#[allow(dead_code)]
struct GameImpl<F: IField, P: IPiece<F>> {
    field: F,
    piece: P
}

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
}

#[allow(dead_code, unused_variables)]
impl<F: IField, P: IPiece<F>> GameImpl<F, P> {
    fn new(callback: &Fn(&F, &P, Option<&P>)->()) -> Self {
        unimplemented!()
    }

    fn run(&mut self) -> Result<(), RunningError> {
        unimplemented!()
    }

    fn pause(&mut self) -> Result<(), NotRunningError> {
        unimplemented!()
    }

    fn set_renderer(&mut self, callback: &Fn(&F, &P, Option<&P>) -> ()) -> Result<(), RunningError> {
        unimplemented!()
    }

    fn queue_input(&mut self, input: piece::Input) -> Result<(), NotRunningError> {
        unimplemented!()
    }

    fn is_game_over(&self) -> bool {
        unimplemented!()
    }
}

#[cfg(test)]
mod test {
    use super::{GameImpl, IField, IPiece};
    use super::super::piece::{Type, Input, LockError};
    use super::super::piece;
    use std::thread::sleep;
    use std::time::Duration;
    use std::sync::Mutex;

    struct MockField;
    impl IField for MockField {
        fn new() -> Self {
            MockField
        }
    }
    struct MockPiece {
        time_step_call_count: u32,
        time_step_step_count: u32,
        input_log: Vec<Input>
    }
    impl IPiece<MockField> for MockPiece {

        fn new(_: Type, _: u32, _: &mut MockField) -> Self {
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

    fn dummy_render_function(_: &MockField, _: &MockPiece, _: Option<&MockPiece>) {
    }
    fn alt_render_function(_: &MockField, _: &MockPiece, _: Option<&MockPiece>) {
    }
    #[test]
    fn test_new() {
        // check that the constructor doesn't panic
        let _test_game: GameImpl<MockField, MockPiece> = GameImpl::new(&dummy_render_function);
    }
    #[test]
    fn test_run_callback() {
        let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(&dummy_render_function);
        // set callback
        assert!(test_game.set_renderer(&alt_render_function).is_ok());
        // check run/stop
        assert!(test_game.run().is_ok());
        sleep(Duration::from_secs(1));
        assert!(test_game.pause().is_ok());
        // c++ tests attempt to verify that game runs approx 60fps
        //   but it's squirrely, so skipping that for now

        // test that destructor drops the internal thread
        {
            let count_mutex: Mutex<u32> = Mutex::new(0);
            let counting_render_function = |_: &MockField, _: &MockPiece, _: Option<&MockPiece>| {
                let mut count = count_mutex.lock().unwrap();
                *count += 1;
            };
            assert!(test_game.set_renderer(&counting_render_function).is_ok());
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
    #[ignore]
    fn whitebox_test_run_callback() {
        // In c++ version this test relies on PieceDummy
        // to check whether game is time stepping a piece
        unimplemented!()
    }
    #[test]
    fn whitebox_test_input() {
        let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(&dummy_render_function);
        const INPUTS: [Input; 5] = [Input::RotateCCW, Input::RotateCW, Input::ShiftLeft, Input::ShiftRight, Input::HardDrop];
        assert!(test_game.run().is_ok());
        for input in &INPUTS {
            assert!(test_game.queue_input(*input).is_ok());
        }
        sleep(Duration::from_secs(2));
        assert!(test_game.pause().is_ok());
        // Todo: port this?
        // CPPUNIT_ASSERT( PieceDummy::compare_handleInput_arg(inputs) );
    }
    #[test]
    fn test_exceptions() {
        let mut test_game: GameImpl<MockField, MockPiece> = GameImpl::new(&dummy_render_function);
        assert!(test_game.queue_input(Input::ShiftRight).is_err());
        assert!(test_game.pause().is_err());

        assert!(test_game.run().is_ok());

        assert!(test_game.set_renderer(&alt_render_function).is_err());
        assert!(test_game.run().is_err());
    }
}
