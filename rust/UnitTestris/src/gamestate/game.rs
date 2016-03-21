use std::fmt::Display;
use std::fmt::Formatter;
use std::error::Error;
use super::field::Field;
use super::piece::Piece;
use super::piece;

#[allow(dead_code)]
#[derive(Debug)]
struct RunningError;

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
struct NotRunningError;

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


#[allow(dead_code)]
struct Game;
#[allow(dead_code, non_snake_case, unused_variables)]
impl Game {
    pub fn new(callback:&Fn(&Field, &Piece, Option<&Piece>)->()) -> Game {
        unimplemented!()
    }

    pub fn run(&mut self) -> Option<RunningError> {
        unimplemented!()
    }

    pub fn pause(&mut self) -> Option<NotRunningError> {
        unimplemented!()
    }

    pub fn setRenderer(&mut self) -> Option<RunningError> {
        unimplemented!()
    }

    pub fn queueInput(input: piece::Type) -> Option<NotRunningError> {
        unimplemented!()
    }

    pub fn isGameOver(&self) -> bool {
        unimplemented!()
    }
}
