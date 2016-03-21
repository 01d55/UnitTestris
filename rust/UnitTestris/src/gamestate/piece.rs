use std::fmt::Display;
use std::fmt::Formatter;
use std::error::Error;
use std::result::Result;
use super::field::Field;
use super::common::coord;

#[allow(non_snake_case, non_camel_case_types, dead_code)]
pub enum Type {
    I,
    J,
    L,
    O,
    S,
    T,
    Z,
}
#[allow(non_snake_case, non_camel_case_types, dead_code)]
pub enum Input {
    shift_right,
    shift_left,
    rotate_cw,
    rotate_ccw,
    hard_drop,
}

#[derive(Debug)]
#[allow(non_snake_case, non_camel_case_types, dead_code)]
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

#[allow(non_snake_case, non_camel_case_types, dead_code, unused_variables)]
impl Piece {

    pub fn new(t:Type,d:u32, f:&Field) -> Piece {
        unimplemented!()
    }

    pub fn timeStep(&mut self, g:u32) -> Result<bool, LockError> {
        unimplemented!()
    }
    pub fn handleInput(&mut self, input:Input) -> Result<bool, LockError> {
        unimplemented!()
    }

    pub fn getCenter(&self) -> coord {
        unimplemented!()
    }
    pub fn getBlocks(&self) -> [coord; 4] {
        unimplemented!()
    }
    pub fn getType(&self) -> Type {
        unimplemented!()
    }
}



#[cfg(test)]
#[allow(dead_code, non_upper_case_globals, non_snake_case)]
mod tests {
    use super::super::common::coord;
    // piece test
    const originCoord : coord = coord {x:4,y:20};
    const originI :     coord = coord {x:5,y:20};
    const originO :     coord = coord {x:5,y:21};

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
    fn testConstructor() {
        unimplemented!()/*
        let mut testDelay = 1;
        // let testField: Field;
        let mut testCoord = {-1;-1};
        let mut expectedBlocks:[coord;4];*/
    }
    #[test]
    fn testStep() {
        unimplemented!()
    }
    #[test]
    fn testShift() {
        unimplemented!()
    }
    #[test]
    fn testRotate() {
        unimplemented!()
    }
    #[test]
    fn testDrop() {
        unimplemented!()
    }
}
