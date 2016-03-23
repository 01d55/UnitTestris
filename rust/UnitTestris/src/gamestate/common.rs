#[allow(dead_code)]
pub const FIELD_HEIGHT:usize= 22;
#[allow(dead_code)]
pub const FIELD_WIDTH: usize= 10;
#[allow(dead_code)]
pub const FIELD_SIZE:  usize= FIELD_HEIGHT*FIELD_WIDTH;



#[derive(PartialEq, PartialOrd, Eq, Ord, Default, Debug, Hash, Clone, Copy)]
#[allow(dead_code)]
pub struct Coord {
    pub x:i32,
    pub y:i32,
}

#[allow(dead_code)]
impl Coord {
    pub fn new(x:i32, y:i32) -> Coord {
        Coord {
            x:x,
            y:y,
        }
    }

}
