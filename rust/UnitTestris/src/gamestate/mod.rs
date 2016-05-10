mod game;
mod field;
mod piece;

use std::ops::*;

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

impl Add<Coord> for Coord {
    type Output = Self;
    fn add(self, rhs: Self) -> Self {
        Coord{x:self.x+rhs.x, y:self.y+rhs.y}
    }
}

impl<'a> Add<&'a Coord> for Coord {
    type Output = Self;
    fn add(self, rhs: &'a Coord) -> Self {
        Coord{x:self.x+rhs.x, y:self.y+rhs.y}
    }
}

impl<'a> Add<Coord> for &'a Coord {
    type Output = Coord;
    fn add(self, rhs: Coord) -> Coord {
        Coord{x:self.x+rhs.x, y:self.y+rhs.y}
    }
}

impl<'a, 'b> Add<&'b Coord> for &'a Coord {
    type Output = Coord;
    fn add(self, rhs: &'b Coord) -> Coord {
        Coord{x:self.x+rhs.x, y:self.y+rhs.y}
    }
}
