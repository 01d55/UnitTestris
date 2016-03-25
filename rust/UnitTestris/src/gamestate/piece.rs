use std::fmt::Display;
use std::fmt::Formatter;
use std::error::Error;
use std::result::Result;
use super::field::Field;
use super::Coord;

#[allow(dead_code)]
pub enum Type {
    I,
    J,
    L,
    O,
    S,
    T,
    Z,
}
#[allow(dead_code)]
pub enum Input {
    ShiftRight,
    ShiftLeft,
    RotateCW,
    RotateCCW,
    HardDrop,
}

#[derive(Debug)]
#[allow(dead_code)]
pub struct LockError;

impl Display for LockError {
    fn fmt(&self, f: &mut Formatter) -> ::std::fmt::Result {
        write!(f, "{}", self.description())
    }
}

impl Error for LockError {
    fn description(&self) -> &str {
        "Piece is already locked!"
    }
}
#[allow(dead_code)]
pub struct Piece; // stub

#[allow(dead_code, unused_variables)]
impl Piece {

    pub fn new(t:Type,d:u32, f:&Field) -> Piece {
        unimplemented!()
    }

    pub fn time_step(&mut self, g:u32) -> Result<bool, LockError> {
        unimplemented!()
    }
    pub fn handle_input(&mut self, input:Input) -> Result<bool, LockError> {
        unimplemented!()
    }

    pub fn get_center(&self) -> Coord {
        unimplemented!()
    }
    pub fn get_blocks(&self) -> [Coord; 4] {
        unimplemented!()
    }
    pub fn get_type(&self) -> Type {
        unimplemented!()
    }
}



#[cfg(test)]
#[allow(dead_code)]
mod tests {
    use super::super::Coord;
    // piece test
    const ORIGIN_COORD : Coord = Coord {x:4,y:20};
    const ORIGIN_I :     Coord = Coord {x:5,y:20};
    const ORIGIN_O :     Coord = Coord {x:5,y:21};

    /*
     *__________*
     *     L    *21
     *   LcL    *20
     *          *19
     *          *18
     *0123456789*

     *__________*
     *    Oc    *21
     *    OO    *20
     *          *19
     *          *18
     *0123456789*

     *__________*
     *          *21
     *   IIcI   *20
     *          *19
     *          *18
     *0123456789*
     */
    #[test]
    fn test_constructor() {
        unimplemented!()/*
        let mut testDelay = 1;
        // let testField: Field;
        let mut testCoord = {-1;-1};
        let mut expectedBlocks:[Coord;4];*/
    }
    #[test]
    fn test_step() {
        unimplemented!()
    }
    #[test]
    fn test_shift() {
        unimplemented!()
    }
    #[test]
    fn test_rotate() {
        unimplemented!()
    }
    #[test]
    fn test_drop() {
        unimplemented!()
    }
}
