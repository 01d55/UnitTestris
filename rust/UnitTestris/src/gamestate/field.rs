use std::result::Result;
use super::common::Coord;
use super::common::FIELD_HEIGHT;
use super::common::FIELD_WIDTH;

#[derive(Clone, Debug, Default)]
pub struct Field;

#[derive(Debug)]
#[allow(dead_code)]
pub struct SizeError;

#[allow(dead_code, unused_variables)]
impl Field {

    pub fn new() -> Field {
        Field::default()
    }

    pub fn from_rectangular_vector(rect: & Vec<Vec<bool>>) -> Result<Field, SizeError> {
        unimplemented!()
    }

    pub fn from_rectangular_array(rect: &[[bool; FIELD_HEIGHT]; FIELD_WIDTH]) -> Field {
        unimplemented!()
    }

    pub fn get(&self, c:Coord) -> Result<bool,SizeError> {
        unimplemented!()
    }

    pub fn read_score(&self) -> u32 {
        unimplemented!()
    }

    pub fn set(&mut self, x:u32, y:u32) -> () {
        unimplemented!()
    }

    pub fn reset_score(&mut self) -> () {
        unimplemented!()
    }

    pub fn reset_slocks(&mut self) -> () {
        unimplemented!()
    }
}

#[cfg(test)]
mod test {
    use super::Field;
    use super::super::common::FIELD_HEIGHT;
    use super::super::common::FIELD_WIDTH;
    use super::super::common::Coord;
    #[test]
    fn test_new() {
        let test_field = Field::new();
        for j in 0..FIELD_HEIGHT {
            for i in 0..FIELD_WIDTH {
                assert_eq!(false, test_field.get(Coord::new(i as i32,j as i32)).unwrap())
            }
        }
    }
    #[test]
    fn test_from_rectangular_vector() {
        // test constructing with a two-dimensional vector
        let mut test_input: Vec<Vec<bool> > = Vec::new();
        // size, and set to all false
        test_input.resize(FIELD_WIDTH, Vec::new());
        for ref mut column in & mut test_input {
            column.resize(FIELD_HEIGHT, false)
        }
        // empty field
        {
            let test_field = Field::from_rectangular_vector(&test_input.clone()).unwrap();
            for j in 0..FIELD_HEIGHT {
                for i in 0..FIELD_WIDTH {
                    assert_eq!(false, test_field.get(Coord::new(i as i32,j as i32)).unwrap())
                }
            }
        }
        // check extreme corners
        test_input[0][0] = true;
        // Putting FIELD_HEIGHT-1 in a match pattern is a syntax error.
        const FIELD_WEND: usize = FIELD_HEIGHT-1;
        const FIELD_HEND: usize = FIELD_WIDTH-1;
        test_input[FIELD_WEND][FIELD_HEND] = true;
        {
            let test_field = Field::from_rectangular_vector(&test_input.clone()).unwrap();
            for j in 0..FIELD_HEIGHT {
                for i in 0..FIELD_WIDTH {
                    match (i,j) {
                        (0,0) | (FIELD_WEND, FIELD_HEND) => { assert!(test_field.get(Coord::new(i as i32, j as i32)).unwrap()) }
                        _ => { assert_eq!(false, test_field.get(Coord::new(i as i32, j as i32)).unwrap()) }
                    }
                }
            }
        }
        // a non-full row placed over a full row. The non-full row should drop into the space opened when the full row is cleared.
        for j in 0..FIELD_HEIGHT {
            for i in 0..FIELD_WIDTH {
                match (i,j) {
                    (_,0) => {test_input[i][j] = true}
                    (_,1) => {test_input[i][j] = i%2 == 0}
                    _ => {test_input[i][j] = false}
                }
            }
        }
        {
            let test_field = Field::from_rectangular_vector(&test_input.clone()).unwrap();
            for j in 0..FIELD_HEIGHT {
                for i in 0..FIELD_WIDTH {
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
        for j in 0..FIELD_HEIGHT {
            for i in 0..FIELD_WIDTH {
                match(i,j) {
                    (_,1) | (_,2) | (_,5) => { test_input[i][j] = true }
                    (_,6) => { test_input[i][j] = i%2 != 0 }
                    _ => { test_input[i][j] = false }
                }
            }
        }
        {
            let test_field = Field::from_rectangular_vector(&test_input.clone()).unwrap();
            for j in 0 ..FIELD_HEIGHT {
                for i in 0..FIELD_WIDTH {
                    match(i,j) {
                        (_,3) => {assert_eq!(i%2 != 0, test_field.get(Coord::new(i as i32, j as i32)).unwrap())}
                        _ => {assert_eq!(false, test_field.get(Coord::new(i as i32, j as i32)).unwrap())}
                    }
                }
            }
        }
        // Full column.
        for j in 0..FIELD_HEIGHT {
            for i in 0..FIELD_WIDTH {
                test_input[i][j] = i == 0
            }
        }
        {
            let test_field = Field::from_rectangular_vector(&test_input.clone()).unwrap();
            for j in 0..FIELD_HEIGHT {
                for i in 0..FIELD_WIDTH {
                    assert_eq!( i == 0, test_field.get(Coord::new(i as i32, j as i32)).unwrap())
                }
            }
        }
        // All rows full. The full rows should clear.
        for ref mut x in  &mut test_input {
            x.clear();
            x.resize(FIELD_HEIGHT, true)
        }
        {
            let test_field = Field::from_rectangular_vector(&test_input.clone()).unwrap();
            for j in 0..FIELD_HEIGHT {
                for i in 0..FIELD_WIDTH {
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
        // error if not FIELD_WIDTH columns
        test_input.resize(FIELD_WIDTH+1, ::std::default::Default::default());
        for ref mut column in &mut test_input {
            column.resize(FIELD_HEIGHT, false)
        }
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input.resize(FIELD_WIDTH-1, ::std::default::Default::default());
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        // error if not every column is FIELD_HEIGHT
        test_input.resize(FIELD_WIDTH, ::std::default::Default::default()); // test_input[FIELD_WEND] is empty
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input[FIELD_WEND].resize(FIELD_HEIGHT, false);
        test_input[0].clear();
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input[0].resize(FIELD_HEIGHT, false);
        test_input[5].clear();
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input[5].resize(FIELD_HEIGHT+1, false);
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
        test_input[5].resize(FIELD_HEIGHT-1, false);
        {
            let test_result = Field::from_rectangular_vector(&test_input);
            assert!(test_result.is_err())
        }
    }
    #[test]
    fn test_set() {
        unimplemented!()
    }
    #[test]
    fn test_field_score() {
        unimplemented!()
    }
}
