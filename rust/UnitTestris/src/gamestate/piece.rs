use std::fmt::Display;
use std::fmt::Formatter;
use std::error::Error;
use std::result::Result;
use super::field;
use super::field::Field;
use super::Coord;

#[derive(PartialEq, Eq, Debug, Clone, Copy)]
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

#[derive(Debug, PartialEq, Eq)]
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

trait IField {
    fn get(&self, c:Coord) -> Result<bool, field::SizeError>;
    fn set(&mut self, c:Coord) -> field::Result<()>;
}

impl IField for Field {
    fn get(&self, c:Coord) -> Result<bool, field::SizeError> {
        self.get(c)
    }
    fn set(&mut self, c:Coord) -> field::Result<()> {
        self.set(c)
    }
}

#[allow(dead_code)]
struct PieceImpl<F: IField> {
    field: F
}

#[allow(dead_code)]
pub struct Piece {
    pimpl: PieceImpl<Field>
}


#[allow(dead_code, unused_variables)]
impl Piece {

    pub fn new(t:Type, d:u32, f:&mut Field) -> Piece {
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

#[allow(dead_code, unused_variables)]
impl<F: IField> PieceImpl<F> {
    fn new(t:Type, d:u32, f:&mut F) -> Self {
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
    use super::super::field;
    use super::PieceImpl;
    use std::collections::HashMap;
    use std::collections::HashSet;
    // Origins
    const ORIGIN_COORD : Coord = Coord {x:4,y:20};
    const ORIGIN_I :     Coord = Coord {x:5,y:20};
    const ORIGIN_O :     Coord = Coord {x:5,y:21};

    /*
    A typical piece
     *__________*
     *     L    *21
     *   LcL    *20
     *          *19
     *          *18
     *0123456789*
    special case: O block
     *__________*
     *    Oc    *21
     *    OO    *20
     *          *19
     *          *18
     *0123456789*
    special case: I block
     *__________*
     *          *21
     *   IIcI   *20
     *          *19
     *          *18
     *0123456789*
     */


    struct MockField {
        set_args: Vec<Coord>,
        get_args: Vec<Coord>,
        get_results: HashMap<Coord, bool>,
    }
    impl super::IField for MockField {
        fn get(&self, c:Coord) -> Result<bool, field::SizeError> {
            // TODO: Find a way to allow this
            //self.get_args.push(c);
            Result::Ok(*(self.get_results.get(&c).unwrap()))
        }
        fn set(&mut self, c:Coord) -> field::Result<()> {
            const WIDTH: i32 = field::WIDTH as i32;
            const HEIGHT: i32 = field::HEIGHT as i32;
            if c.x >= WIDTH || c.x < 0 || c.y >= HEIGHT || c.y < 0 {
                Result::Err(field::Error::Size(field::SizeError))
            } else {
                self.set_args.push(c);
                Result::Ok(())
            }
        }
    }
    // helper fns
    //
    fn test_same_coords(a: &[Coord;4], b: &[Coord;4]) -> bool {
        let (mut ah, mut bh): (HashSet<Coord>, HashSet<Coord>) = (HashSet::with_capacity(4), HashSet::with_capacity(4));
        for x in a.into_iter() {
            ah.insert(*x);
        }
        for x in b.into_iter() {
            bh.insert(*x);
        }
        ah == bh
    }
    //tests
    #[test]
    fn test_constructor() {
        fn check_normal_blocks(t : super::Type, expected: &[Coord; 4], test_field: &mut MockField, test_delay: u32) {
            let test_piece = PieceImpl::new(t, test_delay, test_field);
            let test_coord = test_piece.get_center();
            assert_eq!(ORIGIN_COORD, test_coord);
            assert!(test_same_coords(expected, &test_piece.get_blocks()));
            assert_eq!(t, test_piece.get_type());
        }
        let test_delay = 1;
        let mut test_field = MockField{set_args: Vec::new(),get_args: Vec::new(), get_results: HashMap::new()};
        let mut expected_blocks: [Coord;4] = ::std::default::Default::default();

        // I block
        expected_blocks[0] = Coord::new(3,20);
        expected_blocks[1] = Coord::new(4,20);
        expected_blocks[2] = Coord::new(5,20);
        expected_blocks[3] = Coord::new(6,20);
        {
            let test_piece = PieceImpl::new(super::Type::I, test_delay, &mut test_field);
            let test_coord = test_piece.get_center();
            assert_eq!(ORIGIN_I, test_coord);
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(super::Type::I, test_piece.get_type());
        }
        // O block
        expected_blocks[0] = Coord::new(4,20);
        expected_blocks[1] = Coord::new(4,21);
        expected_blocks[2] = Coord::new(5,20);
        expected_blocks[3] = Coord::new(5,21);
        {
            let test_piece = PieceImpl::new(super::Type::O, test_delay, &mut test_field);
            let test_coord = test_piece.get_center();
            assert_eq!(ORIGIN_O, test_coord);
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(super::Type::O, test_piece.get_type());
        }
        // L block
        expected_blocks[0] = Coord::new(3,20);
        expected_blocks[1] = Coord::new(4,20);
        expected_blocks[2] = Coord::new(5,20);
        expected_blocks[3] = Coord::new(5,21);
        check_normal_blocks(super::Type::L, &expected_blocks, &mut test_field, test_delay);
        // S block
        expected_blocks[0] = Coord::new(3,20);
        expected_blocks[1] = Coord::new(4,20);
        expected_blocks[2] = Coord::new(4,21);
        expected_blocks[3] = Coord::new(5,21);
        check_normal_blocks(super::Type::S, &expected_blocks, &mut test_field, test_delay);
        // T block
        expected_blocks[0] = Coord::new(3,20);
        expected_blocks[1] = Coord::new(4,20);
        expected_blocks[2] = Coord::new(4,21);
        expected_blocks[3] = Coord::new(5,20);
        check_normal_blocks(super::Type::T, &expected_blocks, &mut test_field, test_delay);
        // Z block
        expected_blocks[0] = Coord::new(3,21);
        expected_blocks[1] = Coord::new(4,20);
        expected_blocks[2] = Coord::new(4,21);
        expected_blocks[3] = Coord::new(5,20);
        check_normal_blocks(super::Type::T, &expected_blocks, &mut test_field, test_delay);
    }
    #[test]
    fn test_step() {
        fn test_center_and_lock(test_field: &mut MockField, t: super::Type) {
            fn test_same_coords(blocks: &[Coord; 4], args: &Vec<Coord>) -> bool {
                let (mut blocks_set, mut args_set) =  (HashSet::with_capacity(4), HashSet::with_capacity(4));
                for item in blocks {
                    blocks_set.insert(item);
                }
                for item in args {
                    args_set.insert(item);
                }
                blocks_set == args_set
            }
            let fail_safe = 44;
            let test_delay = 0;
            let mut i = 0;
            let mut expected_coord = match t {
                super::Type::I => { ORIGIN_I }
                super::Type::O => { ORIGIN_O }
                _ => { ORIGIN_COORD }
            };
            let mut test_piece = PieceImpl::new(t, test_delay, test_field);
            while !(test_piece.time_step(1).unwrap()) {
                // If time step never returns false (i.e. the block falls forever)
                // we need to detect and fail.
                i = i+1;
                assert!(i < fail_safe);
                --expected_coord.y;
                assert_eq!(expected_coord, test_piece.get_center());
            }
            // Test that we hit bottom
            expected_coord.y = if t == super::Type::O {1} else {0};
            assert_eq!(expected_coord, test_piece.get_center());
            // Test error when time stepping a locked piece
            assert_eq!(super::LockError, test_piece.time_step(1).unwrap_err());
            // Test that error does not change center.
            assert_eq!(expected_coord, test_piece.get_center());
            // Test that the piece attempted to place its blocks into the field
            assert!(test_same_coords(&test_piece.get_blocks(), &test_field.set_args));
        }
        let mut test_field: MockField = MockField{set_args: Vec::new(),get_args: Vec::new(), get_results: HashMap::new()};
        test_center_and_lock(&mut test_field, super::Type::J);
        test_center_and_lock(&mut test_field, super::Type::L);
        test_center_and_lock(&mut test_field, super::Type::S);
        test_center_and_lock(&mut test_field, super::Type::Z);
        test_center_and_lock(&mut test_field, super::Type::T);
        test_center_and_lock(&mut test_field, super::Type::I);
        test_center_and_lock(&mut test_field, super::Type::O);
        // test that stepped pieces report correct block positions before & after lockinng
        {
            let test_delay = 0;
            let mut test_piece = PieceImpl::new(super::Type::I, test_delay, &mut test_field);
            // before lock
            assert!(test_piece.time_step(3).is_ok());
            let mut expected_blocks = [Coord::new(3,20-3,), Coord::new(4,20-3), Coord::new(5,20-3), Coord::new(6,20-3)];
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            while !(test_piece.time_step(1).unwrap()) {} // previous test assures this loop is not divergent
            expected_blocks = [Coord::new(3,0), Coord::new(4, 0), Coord::new(5, 0), Coord::new(6, 0)];
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
        }
        // Test lock delay
        {
            let mut test_delay = 0;
            let mut test_piece = PieceImpl::new(super::Type::O, test_delay, &mut test_field);
            assert_eq!(false, test_piece.time_step(19).unwrap());
            assert!(test_piece.time_step(1).unwrap());
            assert!(test_piece.time_step(1).is_err());

            test_delay = 1;
            test_piece = PieceImpl::new(super::Type::I, test_delay, &mut test_field);
            assert_eq!(false, test_piece.time_step(20).unwrap());
            assert!(test_piece.time_step(1).unwrap());
            assert!(test_piece.time_step(1).is_err());

            test_delay = 3;
            test_piece = PieceImpl::new(super::Type::I, test_delay, &mut test_field);
            assert_eq!(false, test_piece.time_step(22).unwrap());
            assert!(test_piece.time_step(1).unwrap());
            assert!(test_piece.time_step(1).is_err());
        }
        // Test that locking with a larger-than-necessary time_step is not an error,
        // that time_step()ing after such a lock is an error,
        // and finally, that erroneous calls do not change the result of get_blocks
        {
            let test_delay = 3;
            let (mut test_O, mut test_I) = (PieceImpl::new(super::Type::O, test_delay, &mut test_field), PieceImpl::new(super::Type::I, test_delay, &mut test_field));
            assert!(test_O.time_step(30).unwrap());
            assert!(test_I.time_step(30).unwrap());
            assert!(test_O.time_step(2).is_err());
            assert!(test_I.time_step(2).is_err());
            let mut expected_blocks = [Coord::new(4,0), Coord::new(4,1), Coord::new(5,0), Coord::new(5,1)];
            assert!(test_same_coords(&expected_blocks, &test_O.get_blocks()));
            expected_blocks = [Coord::new(3,0), Coord::new(4,0), Coord::new(5,0), Coord::new(6,0)];
            assert!(test_same_coords(&expected_blocks, &test_I.get_blocks()));
        }
        // Test block collision
        test_field.get_results.insert(Coord::new(3,0), true);
        test_field.set_args.clear();
        {
            fn compare_arguments(blocks: &[Coord; 4], args: &Vec<Coord>) -> bool {
                let mut blocks_set: HashSet<Coord> = HashSet::new();
                let mut args_set: HashSet<Coord> = HashSet::new();
                for x in blocks {
                    blocks_set.insert(*x);
                }
                for x in args {
                    args_set.insert(*x);
                }
                blocks_set == args_set
            }
            let test_delay = 0;
            let mut test_T = PieceImpl::new(super::Type::T, test_delay, &mut test_field);
            let mut test_Z = PieceImpl::new(super::Type::Z, test_delay, &mut test_field);

            assert!(test_T.time_step(30).unwrap());
            assert!(test_Z.time_step(30).unwrap());

            let mut expected_blocks = [Coord::new(3,1), Coord::new(4,1), Coord::new(5,1), Coord::new(4,2)];
            assert!(test_same_coords(&expected_blocks, &test_T.get_blocks()));
            assert!(compare_arguments(&expected_blocks, &test_field.set_args));
            test_field.set_args.clear();
            expected_blocks = [Coord::new(3,1), Coord::new(4,1), Coord::new(4,0), Coord::new(5,0)];
            assert!(test_same_coords(&expected_blocks, &test_Z.get_blocks()));
            assert!(compare_arguments(&expected_blocks, &test_field.set_args));
        }
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
