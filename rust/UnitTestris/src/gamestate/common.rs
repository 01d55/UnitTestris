#[allow(dead_code)]
pub const FIELD_HEIGHT:usize= 22;
#[allow(dead_code)]
pub const FIELD_WIDTH: usize= 10;
#[allow(dead_code)]
pub const FIELD_SIZE:  usize= FIELD_HEIGHT*FIELD_WIDTH;



#[derive(PartialEq, PartialOrd, Eq, Ord, Default, Debug, Hash, Clone, Copy)]
#[allow(non_snake_case, non_camel_case_types, dead_code)]
pub struct coord {
    pub x:i32,
    pub y:i32,
}

#[allow(non_snake_case, non_camel_case_types, dead_code)]
impl coord {
    pub fn new(x:i32, y:i32) -> coord {
        coord {
            x:x,
            y:y,
        }
    }

}
