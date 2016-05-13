use std::result;
use std::error;
use std::fmt::Display;
use std::fmt::Formatter;
use super::Coord;

#[allow(dead_code)]
pub const HEIGHT:usize= 22;
#[allow(dead_code)]
pub const WIDTH: usize= 10;
#[allow(dead_code)]
pub const SIZE:  usize= HEIGHT*WIDTH;


#[derive(Clone, Debug, Default)]
pub struct Field {
    blocks: [[bool; WIDTH]; HEIGHT],
    score: u32
}

#[derive(Debug)]
#[allow(dead_code)]
pub struct SizeError;

impl Display for SizeError {
    fn fmt(&self, f: &mut Formatter) -> ::std::fmt::Result {
        use std::error::Error;
        write!(f, "{}", self.description())
    }
}
impl error::Error for SizeError {
    fn description(&self) -> &str {
        "Argument does not match Tetris field dimensions."
    }
}

#[derive(Debug)]
#[allow(dead_code)]
pub struct DuplicateBlockError;

impl Display for DuplicateBlockError {
    fn fmt(&self, f: &mut Formatter) -> ::std::fmt::Result {
        use std::error::Error;
        write!(f, "{}", self.description())
    }
}
impl error::Error for DuplicateBlockError {
    fn description(&self) -> &str {
        "Block already exists"
    }
}

#[derive(Debug)]
#[allow(dead_code)]
pub enum Error {
    Size(SizeError),
    DuplicateBlock(DuplicateBlockError)
}

impl Display for Error {
    fn fmt(&self, f: &mut Formatter) -> ::std::fmt::Result {
        use std::error::Error;
        write!(f, "{}", self.description())
    }
}

impl error::Error for Error {
    fn description(&self) -> &str {
        match *self {
            Error::Size(ref e) => {e.description()}
            Error::DuplicateBlock(ref e) => {e.description()}
        }
    }
    fn cause(&self) -> Option<&error::Error> {
        match *self {
            Error::Size(ref e) => {Some(e)}
            Error::DuplicateBlock(ref e) => {Some(e)}
        }
    }
}

pub type Result<T> = result::Result<T, Error>;

#[allow(dead_code, unused_variables)]
impl Field {
    // true if coord is in the field
    fn size_check(c: &Coord) -> bool {
        !(c.x < 0 || c.y < 0 || c.x >= WIDTH as i32 || c.y >= HEIGHT as i32)
    }
    fn check_line(&mut self, y: usize) {
        for block in & self.blocks[y] {
            if !*block {return;}
        }
        self.score += 1;
        for j in y..HEIGHT {
            self.blocks[j] = if j == HEIGHT-1 {
                Default::default()
            } else {
                self.blocks[j+1]
            }
        }
    }

    pub fn new() -> Field {
        Field::default()
    }

    pub fn from_rectangular_vector(rect: & Vec<Vec<bool>>) -> result::Result<Field, SizeError> {
        let mut field = Field::default();
        if rect.len() != WIDTH {return Err(SizeError)}
        for (i,column) in rect.into_iter().enumerate() {
            if column.len() != HEIGHT {return Err(SizeError)}
            for (j,block) in column.into_iter().enumerate().rev() {
                if *block {
                    match field.set(Coord{x:i as i32, y:j as i32}) {
                        Ok(_) => {}
                        _ => panic!("this should be impossible")
                    }
                }
            }
        }
        Ok(field)
    }

    pub fn from_rectangular_array(rect: &[[bool; HEIGHT]; WIDTH]) -> Field {
        let mut field = Field::default();
        for (i, column) in rect.into_iter().enumerate() {
            for (j, block) in column.into_iter().enumerate().rev() {
                if *block {
                    match field.set(Coord{x:i as i32, y:j as i32}) {
                        Ok(_) => {}
                        _ => panic!("this should be impossible")
                    }
                }
            }
        }
        field
    }

    pub fn get(&self, c:Coord) -> result::Result<bool, SizeError> {
        if Field::size_check(&c) {
            Ok(self.blocks[c.y as usize][c.x as usize])
        } else {
            Err(SizeError)
        }
    }

    pub fn read_score(&self) -> u32 {
        self.score
    }

    pub fn set(&mut self, c:Coord) -> Result<()> {
        match self.get(c) {
            Ok(false) => {
                self.blocks[c.y as usize][c.x as usize] = true;
                self.check_line(c.y as usize);
                Ok(())
            }
            Ok(true) => Err(Error::DuplicateBlock(DuplicateBlockError)),
            Err(SizeError) => Err(Error::Size(SizeError))
        }
    }

    pub fn reset_score(&mut self) -> () {
        self.score = 0
    }

    pub fn reset_blocks(&mut self) -> () {
        self.blocks = Default::default();
    }
}

#[cfg(test)]
mod test {
    use super::Field;
    use super::HEIGHT;
    use super::WIDTH;
    use super::super::Coord;
    #[test]
    fn test_new() {
        let test_field = Field::new();
        for j in 0..HEIGHT {
            for i in 0..WIDTH {
                assert_eq!(false, test_field.get(Coord::new(i as i32,j as i32)).unwrap())
            }
        }
    }
    #[test]
    fn test_from_rectangular_vector() {
        let mut test_input: Vec<Vec<bool> > = Vec::new();
        // size, and set to all false
        test_input.resize(WIDTH, Vec::new());
        for ref mut column in & mut test_input {
            column.resize(HEIGHT, false)
        }
        // empty field
        {
            let test_field = Field::from_rectangular_vector(&test_input).unwrap();
            for j in 0..HEIGHT {
                for i in 0..WIDTH {
                    assert_eq!(false, test_field.get(Coord::new(i as i32,j as i32)).unwrap())
                }
            }
        }
        // check extreme corners
        test_input[0][0] = true;
        // Putting HEIGHT-1 in a match pattern is a syntax error.
        const FIELD_HEND: usize = HEIGHT-1;
        const FIELD_WEND: usize = WIDTH-1;
        test_input[FIELD_WEND][FIELD_HEND] = true;
        {
            let test_field = Field::from_rectangular_vector(&test_input).unwrap();
            for j in 0..HEIGHT {
                for i in 0..WIDTH {
                    match (i,j) {
                        (0,0) | (FIELD_WEND, FIELD_HEND) => { assert!(test_field.get(Coord::new(i as i32, j as i32)).unwrap()) }
                        _ => { assert_eq!(false, test_field.get(Coord::new(i as i32, j as i32)).unwrap()) }
                    }
                }
            }
        }
        // a non-full row placed over a full row. The non-full row should drop into the space opened when the full row is cleared.
        for j in 0..HEIGHT {
            for i in 0..WIDTH {
                match (i,j) {
                    (_,0) => {test_input[i][j] = true}
                    (_,1) => {test_input[i][j] = i%2 == 0}
                    _ => {test_input[i][j] = false}
                }
            }
        }
        {
            let test_field = Field::from_rectangular_vector(&test_input).unwrap();
            for j in 0..HEIGHT {
                for i in 0..WIDTH {
                    match (i,j) {
                        (_,0) => { assert_eq!(i%2 == 0, test_field.get(Coord::new(i as i32, j as i32)).unwrap()) }
                        _ => { assert_eq!(false, test_field.get(Coord::new(i as i32, j as i32)).unwrap())}
                    }
                }
            }
        }
        // Clear multiple rows.
        // Three full rows, two contiguous, one non contiguous,
        // and a row with alternating empty and full blocks above.
        // The row with alternating blocks should shift down 3 as the full
        // rows beneath it are removed.
        for j in 0..HEIGHT {
            for i in 0..WIDTH {
                match(i,j) {
                    (_,1) | (_,2) | (_,5) => { test_input[i][j] = true }
                    (_,6) => { test_input[i][j] = i%2 != 0 }
                    _ => { test_input[i][j] = false }
                }
            }
        }
        {
            let test_field = Field::from_rectangular_vector(&test_input).unwrap();
            for j in 0 ..HEIGHT {
                for i in 0..WIDTH {
                    match(i,j) {
                        (_,3) => {assert_eq!(i%2 != 0, test_field.get(Coord::new(i as i32, j as i32)).unwrap())}
                        _ => {assert_eq!(false, test_field.get(Coord::new(i as i32, j as i32)).unwrap())}
                    }
                }
            }
        }
        // Full column.
        for j in 0..HEIGHT {
            for i in 0..WIDTH {
                test_input[i][j] = i == 0
            }
        }
        {
            let test_field = Field::from_rectangular_vector(&test_input).unwrap();
            for j in 0..HEIGHT {
                for i in 0..WIDTH {
                    assert_eq!( i == 0, test_field.get(Coord::new(i as i32, j as i32)).unwrap())
                }
            }
        }
        // All rows full. The full rows should clear.
        for ref mut x in  &mut test_input {
            x.clear();
            x.resize(HEIGHT, true)
        }
        {
            let test_field = Field::from_rectangular_vector(&test_input).unwrap();
            for j in 0..HEIGHT {
                for i in 0..WIDTH {
                    assert_eq!(false, test_field.get(Coord::new(i as i32, j as i32)).unwrap())
                }
            }
        }
        // test for errors

        // error if clear
        test_input.clear();
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err());
        }
        // error if not WIDTH columns
        test_input.resize(WIDTH+1, ::std::default::Default::default());
        for ref mut column in &mut test_input {
            column.resize(HEIGHT, false)
        }
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input.resize(WIDTH-1, ::std::default::Default::default());
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        // error if not every column is HEIGHT
        test_input.resize(WIDTH, ::std::default::Default::default()); // test_input[FIELD_WEND] is empty
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input[FIELD_WEND].resize(HEIGHT, false);
        test_input[0].clear();
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input[0].resize(HEIGHT, false);
        test_input[5].clear();
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input[5].resize(HEIGHT+1, false);
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input[5].resize(HEIGHT-1, false);
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
    }
    #[test]
    fn test_from_rectangular_array() {
        use std::default::Default;
        let mut test_input: [[bool; HEIGHT]; WIDTH] = Default::default();
        // empty field
        {
            let test_field = Field::from_rectangular_array(&test_input);
            for i in 0..WIDTH {
                for j in 0..HEIGHT {
                    assert_eq!(false, test_field.get(Coord::new(i as i32, j as i32)).unwrap())
                }
            }
        }
        // check extreme corners
        test_input[0][0] = true;
        const FIELD_W_END: usize = WIDTH - 1;
        const FIELD_H_END: usize = HEIGHT - 1;
        test_input[FIELD_W_END][FIELD_H_END] = true;
        {
            let test_field = Field::from_rectangular_array(&test_input);
            for i in 0..WIDTH {
                for j in 0..HEIGHT {
                    match (i,j) {
                        (0,0) | (FIELD_W_END, FIELD_H_END) => {
                            assert!(test_field.get(Coord::new(i as i32, j as i32)).unwrap())
                        }
                        _ => {
                            assert_eq!(false, test_field.get(Coord::new(i as i32,j as i32)).unwrap())
                        }
                    }
                }
            }
        }
        // Place blocks over a full row. The blocks should drop on row when the full row is cleared.
        for j in 0..HEIGHT {
            for i in 0..WIDTH {
                match (i,j) {
                    (_,0) => {test_input[i][j] = true}
                    (_,1) => {test_input[i][j] = i%2 == 0}
                    _ => {test_input[i][j] = false}
                }
            }
        }
        {
            let test_field = Field::from_rectangular_array(&test_input);
            for i in 0..WIDTH {
                for j in 0..HEIGHT {
                    let got = test_field.get(Coord::new(i as i32, j as i32)).unwrap();
                    if j == 0 {
                        assert_eq!(i%2 == 0, got)
                    } else {
                        assert_eq!(false, got)
                    }
                }
            }
        }
        // Clear multiple rows.
        // Three full rows, two contiguous, one non contiguous,
        // and a row with alternating empty and full blocks above.
        // The row with alternating blocks should shift down 3 as the full
        // rows beneath it are removed.
        for i in 0..WIDTH {
            for j in 0..HEIGHT {
                match(i,j) {
                    (_,1) | (_,2) | (_,5) => { test_input[i][j] = true }
                    (_,6) => { test_input[i][j] = i%2 != 0 }
                    _ => { test_input[i][j] = false }
                }
            }
        }
        {
            let test_field = Field::from_rectangular_array(&test_input);
            for i in 0..WIDTH {
                for j in 0..HEIGHT {
                    let got = test_field.get(Coord::new(i as i32, j as i32)).unwrap();
                    if j == 3 {
                        assert_eq!(i%2 != 0, got)
                    } else {
                        assert_eq!(false, got)
                    }
                }
            }
        }
        // Full column.
        for i in 0..WIDTH {
            for j in 0..HEIGHT {
                test_input[i][j] = i == 0
            }
        }
        {
            let test_field = Field::from_rectangular_array(&test_input);
            for i in 0..WIDTH {
                for j in 0..HEIGHT {
                    assert_eq!( i == 0, test_field.get(Coord::new(i as i32, j as i32)).unwrap())
                }
            }
        }
        // All full, all clear.
        for column in test_input.iter_mut() {
            for cell in column.iter_mut() {
                *cell = true;
            }
        }
        {
            let test_field = Field::from_rectangular_array(&test_input);
            for i in 0..WIDTH {
                for j in 0..HEIGHT {
                    assert_eq!(false, test_field.get(Coord::new(i as i32, j as i32)).unwrap())
                }
            }
        }
    }
    #[test]
    fn test_get_bounds() {
        const H_END:i32 = (HEIGHT+2) as i32;
        const W_END:i32 = (WIDTH+2) as i32;
        let test_field = Field::new();
        for i in -2..W_END {
            for j in -2..H_END {
                if i < 0 || j < 0 || i >= WIDTH as i32 || j >= HEIGHT as i32 {
                    assert!(test_field.get(Coord::new(i,j)).is_err())
                } else {
                    assert!(test_field.get(Coord::new(i,j)).is_ok())
                }
            }
        }
    }
    #[test]
    fn test_set() {
        let mut test_field = Field::new();
        // check that corners set correctly
        assert!(test_field.set(Coord::new(0, 0)).is_ok());
        assert!(test_field.get(Coord::new(0,0)).unwrap());
        assert!(test_field.set(Coord::new(0, (HEIGHT-1) as i32)).is_ok());
        assert!(test_field.get(Coord::new(0, (HEIGHT-1) as i32)).unwrap());
        assert!(test_field.set(Coord::new((WIDTH-1) as i32, 0)).is_ok());
        assert!(test_field.get(Coord::new((WIDTH-1) as i32, 0)).unwrap());
        assert!(test_field.set(Coord::new((WIDTH-1) as i32, (HEIGHT-1) as i32)).is_ok());

        // Check line removal, including downward movement of lines.
        {
            const RANGE: ::std::ops::Range<usize> = ::std::ops::Range{start: 1, end: WIDTH-1};
            // `for i in 1..(WIDTH-1)` is a syntax error :(
            for i in RANGE {
                let result = test_field.set(Coord::new(i as i32,0));
                assert!(result.is_ok(), "{}, {:?}", i, result);
            }
        }
        for i in 0..WIDTH {
            for j in 0..HEIGHT {
                let got = test_field.get(Coord::new(i as i32, j as i32)).unwrap();
                const TOP_LEFT: (usize, usize) = (0, HEIGHT-1);
                const TOP_RIGHT: (usize, usize) = (WIDTH-1, HEIGHT-1);
                // expect blocks previously in top-left and top-right to shift down 1
                const EXPECTED_LEFT: (usize, usize) = (0, HEIGHT-2);
                const EXPECTED_RIGHT: (usize, usize) = (WIDTH-1, HEIGHT-2);
                match(i,j) {
                    (_,0) | TOP_LEFT | TOP_RIGHT => {
                        assert!(!got, "{:?}, {:?}", (i,j), &test_field)
                    }
                    EXPECTED_LEFT | EXPECTED_RIGHT => {
                        assert!(got)
                    }
                    _ => {
                        assert_eq!(false, got)
                    }
                }
            }
        }
        // Check that line removal does not disturb lower lines
        assert!(test_field.set(Coord::new(5,2)).is_ok());
        for i in 0..WIDTH {
            assert!(test_field.set(Coord::new(i as i32,3)).is_ok());
        }
        // we want to repeat this soon
        let repeatable_asserts = |field: &Field| for i in 0..WIDTH {
            for j in 0..HEIGHT {
                let got = field.get(Coord::new(i as i32, j as i32)).unwrap();
                // expect upper blocks to drop by 1
                const PREVIOUS_LEFT: (usize, usize) = (0, HEIGHT-2);
                const PREVIOUS_RIGHT: (usize, usize) = (WIDTH-1, HEIGHT-2);
                const EXPECTED_LEFT: (usize, usize) = (0, HEIGHT-3);
                const EXPECTED_RIGHT: (usize, usize) = (WIDTH-1, HEIGHT-3);
                // expect block below removed line to not drop
                const EXPECTED_UNCHANGED: (usize, usize) = (5, 2);
                const UNEXPECTED_CHANGE: (usize, usize) = (5, 1);
                match(i,j) {
                    EXPECTED_LEFT | EXPECTED_RIGHT | EXPECTED_UNCHANGED => {
                        assert!(got)
                    }
                    PREVIOUS_LEFT | PREVIOUS_RIGHT | UNEXPECTED_CHANGE => {
                        assert_eq!(false, got)
                    }
                    _ => {
                        assert_eq!(false, got)
                    }
                }
            }
        };
        repeatable_asserts(&test_field);
        // Test for errors
        let mut err: super::Error;
        let panic_unless_size = | e: &super::Error | match e {
            &super::Error::DuplicateBlock(_) => {panic!("Wrong error!")}
            &super::Error::Size(_) => {}//OK
        };
        let panic_unless_dup = | e: &super::Error | match e {
            &super::Error::DuplicateBlock(_) => {}//OK
            &super::Error::Size(_) => {panic!("Wrong error!")}
        };
        err = test_field.set(Coord::new(-1, 0)).unwrap_err();
        panic_unless_size(&err);
        err = test_field.set(Coord::new(0, -1)).unwrap_err();
        panic_unless_size(&err);
        err = test_field.set(Coord::new(WIDTH as i32, 0)).unwrap_err();
        panic_unless_size(&err);
        err = test_field.set(Coord::new(0, HEIGHT as i32)).unwrap_err();
        panic_unless_size(&err);
        err = test_field.set(Coord::new(::std::i32::MIN, 0)).unwrap_err();
        panic_unless_size(&err);
        err = test_field.set(Coord::new(::std::i32::MAX, 0)).unwrap_err();
        panic_unless_size(&err);
        err = test_field.set(Coord::new(5, 2)).unwrap_err();
        panic_unless_dup(&err);
        // repeat our most recent battery of assertions, to check that all of these errors haven't changed anything.
        repeatable_asserts(&test_field);
        // test reset
        test_field.reset_blocks();
        for i in 0..WIDTH {
            for j in 0..HEIGHT {
                assert_eq!(false,  test_field.get(Coord::new(i as i32, j as i32)).unwrap())
            }
        }
    }
    #[test]
    fn test_field_score() {
        let mut test_field = Field::new();
        // Score should begin at zero.
        assert_eq!(0, test_field.read_score());
        for i in 0..WIDTH {
            assert!(test_field.set(Coord::new(i as i32, 0)).is_ok());
        }
        assert_eq!(1, test_field.read_score());
        for i in 0..WIDTH {
            assert!(test_field.set(Coord::new(i as i32, 6)).is_ok());
        }
        assert_eq!(2, test_field.read_score());
        // test reset, and that increasing score after a reset works correctly.
        test_field.reset_score();
        for i in 0..WIDTH {
            assert!(test_field.set(Coord::new(i as i32, 19)).is_ok());
        }
        assert_eq!(1, test_field.read_score());
    }
}
