use std::result::Result;
use super::common::coord;

#[allow(non_snake_case, non_camel_case_types)]
#[derive(Clone, Debug)]
pub struct Field;

#[derive(Debug)]
#[allow(dead_code)]
pub struct SizeError;

#[allow(dead_code, non_snake_case, unused_variables)]
impl Field {
    pub fn new(spec:Option<Vec<Vec<bool>>>) -> Field {
        unimplemented!()
    }


    pub fn get(&self, c:coord) -> Result<bool,SizeError> {
        unimplemented!()
    }

    pub fn readScore(&self) -> u32 {
        unimplemented!()
    }

    pub fn set(&mut self, x:u32, y:u32) -> () {
        unimplemented!()
    }

    pub fn resetScore(&mut self) -> () {
        unimplemented!()
    }

    pub fn resetBlocks(&mut self) -> () {
        unimplemented!()
    }
}

