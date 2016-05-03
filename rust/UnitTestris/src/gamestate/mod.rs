mod game;
mod field;
mod piece;

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

impl ::std::ops::Add for Coord {
    type Output = Self;
    fn add(self, rhs: Self) -> Self {
        Coord{x:self.x+rhs.x, y:self.y+rhs.y}
    }
}
