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

    // Initial blocks
    const BLOCKS_I : [Coord; 4] = [Coord {x:3,y:20}, Coord {x:4,y:20}, Coord {x:5,y:20}, Coord {x:6,y:20}];
    const BLOCKS_J : [Coord; 4] = [Coord {x:3,y:20}, Coord {x:3,y:21}, Coord {x:4,y:20}, Coord {x:5,y:20}];
    const BLOCKS_L : [Coord; 4] = [Coord {x:3,y:20}, Coord {x:4,y:20}, Coord {x:5,y:20}, Coord {x:5,y:21}];
    const BLOCKS_O : [Coord; 4] = [Coord {x:4,y:20}, Coord {x:4,y:21}, Coord {x:5,y:20}, Coord {x:5,y:21}];
    const BLOCKS_S : [Coord; 4] = [Coord {x:3,y:20}, Coord {x:4,y:20}, Coord {x:4,y:21}, Coord {x:5,y:21}];
    const BLOCKS_T : [Coord; 4] = [Coord {x:3,y:20}, Coord {x:4,y:20}, Coord {x:4,y:21}, Coord {x:5,y:20}];
    const BLOCKS_Z : [Coord; 4] = [Coord {x:3,y:21}, Coord {x:4,y:20}, Coord {x:4,y:21}, Coord {x:5,y:20}];

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
        let mut expected_blocks: [Coord;4];

        // I block
        expected_blocks = BLOCKS_I;
        {
            let test_piece = PieceImpl::new(super::Type::I, test_delay, &mut test_field);
            let test_coord = test_piece.get_center();
            assert_eq!(ORIGIN_I, test_coord);
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(super::Type::I, test_piece.get_type());
        }
        // O block
        expected_blocks = BLOCKS_O;
        {
            let test_piece = PieceImpl::new(super::Type::O, test_delay, &mut test_field);
            let test_coord = test_piece.get_center();
            assert_eq!(ORIGIN_O, test_coord);
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(super::Type::O, test_piece.get_type());
        }
        // J block
        expected_blocks = BLOCKS_J;
        check_normal_blocks(super::Type::J, &expected_blocks, &mut test_field, test_delay);
        // L block
        expected_blocks = BLOCKS_L;
        check_normal_blocks(super::Type::L, &expected_blocks, &mut test_field, test_delay);
        // S block
        expected_blocks = BLOCKS_S;
        check_normal_blocks(super::Type::S, &expected_blocks, &mut test_field, test_delay);
        // T block
        expected_blocks = BLOCKS_T;
        check_normal_blocks(super::Type::T, &expected_blocks, &mut test_field, test_delay);
        // Z block
        expected_blocks = BLOCKS_Z;
        check_normal_blocks(super::Type::Z, &expected_blocks, &mut test_field, test_delay);
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
        let mut test_field = MockField {set_args: Vec::new(),get_args: Vec::new(), get_results: HashMap::new()};
        fn per_block(test_field: &mut MockField, typ: super::Type, origin: Coord, expected_initial_blocks: [Coord; 4], space_to_right_wall: i32, space_to_left_wall: i32, rightward_blocks: Vec<Coord>, leftward_blocks: Vec<Coord> ) {
            let mut test_piece: PieceImpl<MockField> = PieceImpl::new(typ, 0, test_field);
            // Shift right
            assert!(test_piece.handle_input(super::Input::ShiftRight).is_ok());

            let mut expected_coord = origin;
            expected_coord.x = expected_coord.x + 1;
            let mut expected_blocks = expected_initial_blocks;
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(expected_coord, test_piece.get_center());
            // Press against right wall.
            for _ in 0..5 {
                assert!(test_piece.handle_input(super::Input::ShiftRight).is_ok());
            }
            // Although we shifted 5 times, the right wall should block some.
            expected_coord.x += space_to_right_wall;
            for coord in expected_blocks.iter_mut() {
                coord.x += space_to_right_wall;
            }
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(expected_coord, test_piece.get_center());
            // Shift left
            assert!(test_piece.handle_input(super::Input::ShiftLeft).is_ok());
            expected_coord.x -= 1;
            for coord in expected_blocks.iter_mut() {
                coord.x -= 1;
            }
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(expected_coord, test_piece.get_center());
            // Press against left wall
            for _ in 0..10 {
                assert!(test_piece.handle_input(super::Input::ShiftLeft).is_ok());
            }
            for coord in expected_blocks.iter_mut() {
                coord.x -= space_to_left_wall;
            }
            expected_coord.x -= space_to_left_wall;
            // Press against a block in the field
            for rightward_block in rightward_blocks {
                test_field.get_results.insert(rightward_block, true);
            }
            for _ in 0..2 {
                assert!(test_piece.handle_input(super::Input::ShiftRight).is_ok());
            }
            for coord in expected_blocks.iter_mut() {
                coord.x += 1;
            }
            expected_coord.x += 1;
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(expected_coord, test_piece.get_center());
            // press left
            for leftward_block in leftward_blocks {
                test_field.get_results.insert(leftward_block, true);
            }
            assert!(test_piece.handle_input(super::Input::ShiftLeft).is_ok());
            test_field.get_results.clear();
        }
        /*
         *__________*
         *          *21
         *XIIIIX    *20
         *          *19
         *          *18
         *0123456789*/
        per_block(
            &mut test_field,
            super::Type::I,
            ORIGIN_I,
            BLOCKS_I,
            2,
            5,
            [Coord::new(5,20),].into_iter().cloned().collect(),
            [Coord::new(0,20),].into_iter().cloned().collect());
        /*
         *__________*
         * OOX      *21
         *XOO       *20
         *          *19
         *          *18
         *0123456789*/
        per_block(
            &mut test_field,
            super::Type::O,
            ORIGIN_O,
            BLOCKS_O,
            3,
            7,
            [Coord::new(3,21)].into_iter().cloned().collect(),
            [Coord::new(0,20)].into_iter().cloned().collect());
        /*
         *__________*
         * XTX      *21
         * TTTX     *20
         *          *19
         *          *18
         *0123456789*/
        per_block(
            &mut test_field,
            super::Type::T,
            ORIGIN_COORD,
            BLOCKS_T,
            3,
            6,
            [Coord::new(4,20), Coord::new(3,21)].into_iter().cloned().collect(),
            [Coord::new(1,21)].into_iter().cloned().collect());
        /*
         *__________*
         *XJX       *21
         * JJJ      *20
         *          *19
         *          *18
         *0123456789*/
        per_block(
            &mut test_field,
            super::Type::J,
            ORIGIN_COORD,
            BLOCKS_J,
            3,
            6,
            [Coord::new(2,21)].into_iter().cloned().collect(),
            [Coord::new(0,21)].into_iter().cloned().collect());
        /*
         *__________*
         *  XLX     *21
         * LLL      *20
         *          *19
         *          *18
         *0123456789*/
        per_block(
            &mut test_field,
            super::Type::L,
            ORIGIN_COORD,
            BLOCKS_L,
            3,
            6,
            [Coord::new(4,21)].into_iter().cloned().collect(),
            [Coord::new(2,21)].into_iter().cloned().collect());
        /*
         *__________*
         * XSS      *21
         * SSX      *20
         *          *19
         *          *18
         *0123456789*/
        per_block(
            &mut test_field,
            super::Type::L,
            ORIGIN_COORD,
            BLOCKS_S,
            3,
            6,
            [Coord::new(3,20)].into_iter().cloned().collect(),
            [Coord::new(1,21)].into_iter().cloned().collect());
        /*
         *__________*
         * ZZX      *21
         * XZZ      *20
         *          *19
         *          *18
         *0123456789*/
        per_block(
            &mut test_field,
            super::Type::Z,
            ORIGIN_COORD,
            BLOCKS_Z,
            3,
            6,
            [Coord::new(3,21)].into_iter().cloned().collect(),
            [Coord::new(1,20)].into_iter().cloned().collect());
    }
    #[test]
    fn test_rotate() {
        let mut test_field = MockField {set_args: Vec::new(),get_args: Vec::new(), get_results: HashMap::new()};
        fn per_block(test_field: &mut MockField, typ: super::Type, expected_blocks: [[Coord; 4]; 8], obstruction: &Coord)
        {
            fn asserts(piece: &mut PieceImpl<MockField>, input: super::Input, center: Coord, expected_blocks: & [Coord; 4]) {
                assert!(piece.handle_input(input).is_ok());
                assert_eq!(center, piece.get_center());
                assert!(test_same_coords(expected_blocks, &piece.get_blocks()));
            }
            let mut test_piece = PieceImpl::new(typ, 0, test_field);
            // CCW
            // 1
            asserts(& mut test_piece, super::Input::RotateCCW, ORIGIN_COORD, &expected_blocks[0]);
            //2
            asserts(& mut test_piece, super::Input::RotateCCW, ORIGIN_COORD, & expected_blocks[1]);
            //3
            asserts(& mut test_piece, super::Input::RotateCCW, ORIGIN_COORD, &expected_blocks[2]);
            // Does rotation persist through timestep?
            let mut expected_blocks_dropped = expected_blocks[2];
            for block in expected_blocks_dropped.iter_mut() {
                block.y -= 1;
            }
            assert!(test_piece.time_step(1).is_ok());
            const DROP_ONCE: Coord = Coord {x:ORIGIN_COORD.x, y:ORIGIN_COORD.y-1};
            assert_eq!(DROP_ONCE, test_piece.get_center());
            assert!(test_same_coords(&expected_blocks_dropped, &test_piece.get_blocks()));
            // Return to original orientation (but not original position)
            asserts(& mut test_piece, super::Input::RotateCCW, DROP_ONCE, &expected_blocks[3]);
            // CW
            // 1
            asserts(& mut test_piece, super::Input::RotateCW, DROP_ONCE, &expected_blocks[4]);
            //2
            asserts(& mut test_piece, super::Input::RotateCW, DROP_ONCE, &expected_blocks[5]);
            //3
            asserts(& mut test_piece, super::Input::RotateCW, DROP_ONCE, &expected_blocks[6]);
            // Blocked rotations
            test_field.get_results.insert(*obstruction, true);
            // rotation is blocked, expected blocks unchanged
            asserts(& mut test_piece, super::Input::RotateCW, DROP_ONCE, &expected_blocks[6]);
            asserts(& mut test_piece, super::Input::RotateCCW, DROP_ONCE, &expected_blocks[7]);
            test_field.get_results.insert(*obstruction, false);
        }
        // L block
        // Turn CCW
        /*
         *__________*
         *     L    *21
         *   LcL    *20
         *          *19
         *          *18
         *0123456789*
        1 rotation
         *__________*
         *   LL     *21
         *    c     *20
         *    L     *19
         *          *18
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *   LcL    *20
         *   L      *19
         *          *18
         *0123456789*
        3 rotations
         *__________*
         *    L     *21
         *    c     *20
         *    LL    *19
         *          *18
         *0123456789*
         */
        // CW
        /*
         *__________*
         *          *21
         *     L    *20
         *   LcL    *19
         *          *18
         *          *17
         *0123456789*
        1 rotation
         *__________*
         *          *21
         *    L     *20
         *    c     *19
         *    LL    *18
         *          *17
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *          *20
         *   LcL    *19
         *   L      *18
         *          *17
         *0123456789*
        3 rotations
         *__________*
         *          *21
         *   LL     *20
         *    c     *19
         *    L     *18
         *          *17
         *0123456789*
         */
        // Blocked rotations
        /* Prevent CW rotation, but not CCW
         *__________*
         *          *21
         *   LLb    *20
         *    c     *19
         *    L     *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *     b    *20
         *   LcL    *19
         *          *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *     b    *20
         *   LcL    *19
         *   L      *18
         *          *17
         *0123456789*
         */
        const EXPECTED_BLOCKS_L: [[Coord; 4]; 8] =
            [[Coord{x:3,y:21}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}],
             [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:20}, Coord{x:5,y:20}],
             [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:19}],
             [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:5,y:19}],
             [Coord{x:4,y:20}, Coord{x:4,y:19}, Coord{x:4,y:18}, Coord{x:5,y:18}],
             [Coord{x:3,y:18}, Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}],
             [Coord{x:3,y:20}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}],
             [Coord{x:3,y:18}, Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}]];
        per_block(&mut test_field, super::Type::L, EXPECTED_BLOCKS_L, &Coord{x:5,y:20});
        // J block
        // Turn CCW
        /*
         *__________*
         *   J      *21
         *   JcJ    *20
         *          *19
         *          *18
         *0123456789*
        1 rotation
         *__________*
         *    J     *21
         *    c     *20
         *   JJ     *19
         *          *18
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *   JcJ    *20
         *     J    *19
         *          *18
         *0123456789*
        3 rotations
         *__________*
         *    JJ    *21
         *    c     *20
         *    J     *19
         *          *18
         *0123456789*
         */
        // CW
        /*
         *__________*
         *          *21
         *   J      *20
         *   JcJ    *19
         *          *18
         *          *17
         *0123456789*
        1 rotation
         *__________*
         *          *21
         *    JJ    *20
         *    c     *19
         *    J     *18
         *          *17
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *          *20
         *   JcJ    *19
         *     J    *18
         *          *17
         *0123456789*
        3 rotations
         *__________*
         *          *21
         *    J     *20
         *    c     *19
         *   JJ     *18
         *          *17
         *0123456789*
         */
        // Blocked rotations
        /* Prevent CW rotation, but not CCW
         *__________*
         *          *21
         *   bJ     *20
         *    c     *19
         *   JJ     *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *   b      *20
         *   JcJ    *19
         *          *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *   b      *20
         *   JcJ    *19
         *     J    *18
         *          *17
         *0123456789*
         */
        const EXPECTED_BLOCKS_J: [[Coord; 4]; 8] = [
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}],
            [Coord{x:3,y:20}, Coord{x:4,y:20}, Coord{x:5,y:20}, Coord{x:5,y:19}],
            [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:21}],
            [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:5,y:19}],
            [Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:18}, Coord{x:5,y:19}],
            [Coord{x:3,y:18}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:18}, Coord{x:5,y:19}]];
        per_block(&mut test_field, super::Type::J, EXPECTED_BLOCKS_J, &Coord{x:3,y:20});
        // S block
        // Turn CCW
        /*
         *__________*
         *    SS    *21
         *   Sc     *20
         *          *19
         *          *18
         *0123456789*
        1 rotation
         *__________*
         *   S      *21
         *   Sc     *20
         *    S     *19
         *          *18
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *    cS    *20
         *   SS     *19
         *          *18
         *0123456789*
        3 rotations
         *__________*
         *    S     *21
         *    cS    *20
         *     S    *19
         *          *18
         *0123456789*
         */
        // CW
        /*
         *__________*
         *          *21
         *    SS    *20
         *   Sc     *19
         *          *18
         *          *17
         *0123456789*
        1 rotation
         *__________*
         *          *21
         *    S     *20
         *    cS    *19
         *     S    *18
         *          *17
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *          *20
         *    cS    *19
         *   SS     *18
         *          *17
         *0123456789*
        3 rotations
         *__________*
         *          *21
         *   S      *20
         *   Sc     *19
         *    S     *18
         *          *17
         *0123456789*
         */
        /* Prevent CW rotation, but not CCW
         *__________*
         *          *21
         *   Sb     *20
         *   Sc     *19
         *    S     *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *    bS    *20
         *   Sc     *19
         *          *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *    b     *20
         *    cS    *19
         *   SS     *18
         *          *17
         *0123456789*
         */
        const EXPECTED_BLOCKS_S: [[Coord; 4]; 8] = [
            [Coord{x:3,y:20}, Coord{x:3,y:21}, Coord{x:4,y:19}, Coord{x:4,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}],
            [Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:19}, Coord{x:5,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}],
            [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:18}, Coord{x:5,y:19}],
            [Coord{x:3,y:18}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:19}],
            [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:18}, Coord{x:4,y:19}],
            [Coord{x:3,y:18}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:19}]];
        per_block(&mut test_field, super::Type::S, EXPECTED_BLOCKS_S, &Coord{x:4,y:20});
        // Z block
        // Turn CCW
        /*
         *__________*
         *   ZZ     *21
         *    cZ    *20
         *          *19
         *          *18
         *0123456789*
        1 rotation
         *__________*
         *    Z     *21
         *   Zc     *20
         *   Z      *19
         *          *18
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *   Zc     *20
         *    ZZ    *19
         *          *18
         *0123456789*
        3 rotations
         *__________*
         *     Z    *21
         *    cZ    *20
         *    Z     *19
         *          *18
         *0123456789*
         */
        // CW
        /*
         *__________*
         *          *21
         *   ZZ     *20
         *    cZ    *19
         *          *18
         *          *17
         *0123456789*
        1 rotation
         *__________*
         *          *21
         *     Z    *20
         *    cZ    *19
         *    Z     *18
         *          *17
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *          *20
         *   Zc     *19
         *    ZZ    *18
         *          *17
         *0123456789*
        3 rotations
         *__________*
         *          *21
         *    Z     *20
         *   Zc     *19
         *   Z      *18
         *          *17
         *0123456789*
         */
        // Blocked rotations
        /* Prevent CCW rotation, but not CW
         *__________*
         *          *21
         *    Z     *20
         *   Zc     *19
         *   Zb     *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *          *20
         *   Zc     *19
         *    bZ    *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *   ZZ     *20
         *    cZ    *19
         *    b     *18
         *          *17
         *0123456789*
         */
        const EXPECTED_BLOCKS_Z:  [[Coord; 4]; 8] = [
            [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:20}, Coord{x:4,y:21}],
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:19}],
            [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}, Coord{x:5,y:21}],
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:19}],
            [Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:5,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:18}],
            [Coord{x:3,y:18}, Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:4,y:20}],
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:19}]];
        per_block(&mut test_field, super::Type::Z, EXPECTED_BLOCKS_Z, &Coord{x:4,y:18});
        // T block
        // Turn CCW
        /*
         *__________*
         *    T     *21
         *   TcT    *20
         *          *19
         *          *18
         *0123456789*
        1 rotation
         *__________*
         *    T     *21
         *   Tc     *20
         *    T     *19
         *          *18
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *   TcT    *20
         *    T     *19
         *          *18
         *0123456789*
        3 rotations
         *__________*
         *    T     *21
         *    cT    *20
         *    T     *19
         *          *18
         *0123456789*
         */
        // CW
        /*
         *__________*
         *          *21
         *    T     *20
         *   TcT    *19
         *          *18
         *          *17
         *0123456789*
        1 rotation
         *__________*
         *          *21
         *    T     *20
         *    cT    *19
         *    T     *18
         *          *17
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *          *20
         *   TcT    *19
         *    T     *18
         *          *17
         *0123456789*
        3 rotations
         *__________*
         *          *21
         *    T     *20
         *   Tc     *19
         *    T     *18
         *          *17
         *0123456789*
         */
        // Blocked rotations
        /* Prevent CW rotation, but not CCW
         *__________*
         *          *21
         *    T     *20
         *   Tc     *19
         *    T     *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *    b     *20
         *   TcT    *19
         *          *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *    b     *20
         *   TcT    *19
         *    T     *18
         *          *17
         *0123456789*
         */
        const EXPECTED_BLOCKS_T:  [[Coord; 4]; 8] = [
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}],
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}],
            [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:4,y:20}],
            [Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:19}],
            [Coord{x:3,y:19}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:19}],
            [Coord{x:3,y:19}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:19}]];
        per_block(&mut test_field, super::Type::T, EXPECTED_BLOCKS_T, &Coord{x:4,y:20});
        // I block
        // Turn CCW
        /*
         *__________*
         *          *21
         *   IIcI   *20
         *          *19
         *          *18
         *0123456789*
        1 rotation
         *__________*
         *    I     *21
         *    Ic    *20
         *    I     *19
         *    I     *18
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *     c    *20
         *   IIII   *19
         *          *18
         *0123456789*
        3 rotations
         *__________*
         *     I    *21
         *     c    *20
         *     I    *19
         *     I    *18
         *0123456789*
         */
        // CW
        /*
         *__________*
         *          *21
         *          *20
         *   IIcI   *19
         *          *18
         *          *17
         *0123456789*
        1 rotation
         *__________*
         *          *21
         *     I    *20
         *     c    *19
         *     I    *18
         *     I    *17
         *0123456789*
        2 rotations
         *__________*
         *          *21
         *          *20
         *     c    *19
         *   IIII   *18
         *          *17
         *0123456789*
        3 rotations
         *__________*
         *          *21
         *    I     *20
         *    Ic    *19
         *    I     *18
         *    I     *17
         *0123456789*
         */
        // Blocked rotations
        /* Prevent CW rotation, but not CCW
         *__________*
         *          *21
         *    I     *20
         *   bIc    *19
         *    I     *18
         *    I     *17
         *0123456789*
         *__________*
         *          *21
         *          *20
         *   bIcI   *19
         *          *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *          *20
         *   b c    *19
         *   IIII   *18
         *          *17
         *0123456789*
         */
        const EXPECTED_BLOCKS_I:  [[Coord; 4]; 8] = [
            [Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:6,y:19}],
            [Coord{x:5,y:21}, Coord{x:5,y:20}, Coord{x:5,y:19}, Coord{x:5,y:18}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:6,y:19}],
            [Coord{x:5,y:20}, Coord{x:5,y:19}, Coord{x:5,y:18}, Coord{x:5,y:17}],
            [Coord{x:3,y:18}, Coord{x:4,y:18}, Coord{x:5,y:18}, Coord{x:6,y:18}],
            [Coord{x:4,y:20}, Coord{x:4,y:19}, Coord{x:4,y:18}, Coord{x:4,y:17}],
            [Coord{x:3,y:18}, Coord{x:4,y:18}, Coord{x:5,y:18}, Coord{x:6,y:18}]];
        // per-block assumes normal center
        //per_block(&mut test_field, super::Type::_, EXPECTED_BLOCKS__, &Coord{x:_,y:_});
        // O rly? yarly.
        /* O block should not change when rotated.
         *__________*
         *    Oc    *21
         *    OO    *20
         *          *19
         *          *18
         *          *17
         *0123456789*
         */
        const EXPECTED_BLOCKS_O: [Coord; 4] = [Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:20}, Coord{x:5,y:21}];
        unimplemented!()
    }
    #[test]
    fn test_drop() {
        unimplemented!()
    }
}
