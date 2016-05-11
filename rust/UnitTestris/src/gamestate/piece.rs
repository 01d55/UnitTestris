use std::fmt::Display;
use std::fmt::Formatter;
use std::error::Error;
use std::result::Result;
use std::rc::Rc;
use std::cell::RefCell;
use super::field;
use super::field::Field;
use super::Coord;

#[derive(PartialEq, Eq, Debug, Clone, Copy, Hash)]
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
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
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

trait IField : ::std::fmt::Debug {
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
#[derive(Debug)]
struct PieceImpl<F: IField> {
    typ: Type,
    base_delay: u32,
    lock_delay: u32,
    field: Rc<RefCell<F>>,
    center: Coord,
    lock: bool,
    relative_blocks: [Coord; 4]
}

#[allow(dead_code)]
#[derive(Debug)]
pub struct Piece {
    pimpl: PieceImpl<Field>
}


#[allow(dead_code, unused_variables)]
impl Piece {

    pub fn new(t:Type, d:u32, f:Rc<RefCell<Field>>) -> Piece {
        Piece{pimpl: PieceImpl::new(t, d, f)}
    }

    pub fn time_step(&mut self, g:u32) -> Result<bool, LockError> {
        self.pimpl.time_step(g)
    }
    pub fn handle_input(&mut self, input:Input) -> Result<bool, LockError> {
        self.pimpl.handle_input(input)
    }

    pub fn get_center(&self) -> Coord {
        self.pimpl.get_center()
    }
    pub fn get_blocks(&self) -> [Coord; 4] {
        self.pimpl.get_blocks()
    }
    pub fn get_type(&self) -> Type {
        self.pimpl.get_type()
    }


}

// I'd like these consts to be associated, but that feature is experimental
// block coords
const RELATIVE_I: [Coord; 4] = [
    Coord{x:-2,y:0},
    Coord{x:-1,y:0},
    Coord{x:0,y:0},
    Coord{x:1,y:0}];
const RELATIVE_O: [Coord; 4] = [
    Coord{x:-1,y:-1},
    Coord{x:-1,y:0},
    Coord{x:0,y:-1},
    Coord{x:0,y:0}];
const RELATIVE_L: [Coord; 4] = [
    Coord{x:-1,y: 0},
    Coord{x: 0,y: 0},
    Coord{x: 1,y: 0},
    Coord{x: 1,y: 1}];
const RELATIVE_J: [Coord; 4] = [
    Coord{x:-1,y: 0},
    Coord{x:-1,y: 1},
    Coord{x: 0,y: 0},
    Coord{x: 1,y: 0}];
const RELATIVE_S: [Coord; 4] = [
    Coord{x:-1,y: 0},
    Coord{x: 0,y: 1},
    Coord{x: 0,y: 0},
    Coord{x: 1,y: 1}];
const RELATIVE_Z: [Coord; 4] = [
    Coord{x:-1,y: 1},
    Coord{x: 0,y: 1},
    Coord{x: 0,y: 0},
    Coord{x: 1,y: 0}];
const RELATIVE_T: [Coord; 4] = [
    Coord{x:-1,y: 0},
    Coord{x: 0,y: 1},
    Coord{x: 0,y: 0},
    Coord{x: 1,y: 0}];
// Origins
const ORIGIN_COORD : Coord = Coord {x:4,y:20};
const ORIGIN_I :     Coord = Coord {x:5,y:20};
const ORIGIN_O :     Coord = Coord {x:5,y:21};
#[allow(dead_code, unused_variables)]
impl<F: IField> PieceImpl<F> {
    // 'private' helpers
    fn cw(a: &Coord) -> Coord {
        Coord{x:a.y,y:-a.x}
    }
    fn ccw(a: &Coord) -> Coord {
        Coord{x:-a.y,y:a.x}
    }
    fn can_place(&self, blocks: &[Coord; 4]) -> bool {
        for block in blocks {
            let result = self.field.borrow().get(*block);
            match result {
                Ok(false) => {}
                _ => return false
            }
        }
        true
    }
    fn can_shift(&self, displacement: &Coord) -> bool {
        for block in &self.get_blocks() {
            let result = self.field.borrow().get(block+displacement);
            match result {
                Ok(false) => {}
                _ => return false
            }
        }
        true
    }
    fn can_drop(&self) -> bool {
        self.can_shift(&Coord{x:0,y:-1})
    }
    fn rotate(&mut self, i: Input) {
        assert!(match i {
            Input::RotateCCW | Input::RotateCW => {true}
            _ => {false}
        },"Incorrect input passed to PieceImpl::rotate");
        let mut rblocks = self.relative_blocks;
        match self.typ {
            Type::O => {}
            Type::I => {
                /*
                 *__________*     *__________*
                 *          * 2   *          * 2
                 *          * 1   *   II II  * 1
                 *   IIcI   * 0   *     c    * 0
                 *          * 1   *          * 1
                 *          * 2   *          * 2
                 *5432101234*     *5432101234*
                1 rotation
                 *__________*     *__________*
                 *          * 2   *    I     * 2
                 *    I     * 1   *    I     * 1
                 *    Ic    * 0   *     c    * 0
                 *    I     * 1   *    I     * 1
                 *    I     * 2   *    I     * 2
                 *5432101234*     *5432101234*
                2 rotations
                 *__________*     *__________*
                 *          * 2   *          * 2
                 *          * 1   *          * 1
                 *     c    * 0   *     c    * 0
                 *   IIII   * 1   *   II II  * 1
                 *          * 2   *          * 2
                 *5432101234*     *5432101234*
                3 rotations
                 *__________*     *__________*
                 *          * 2   *      I   * 2
                 *     I    * 1   *      I   * 1
                 *     c    * 0   *     c    * 0
                 *     I    * 1   *      I   * 1
                 *     I    * 2   *      I   * 2
                 *5432101234*     *5432101234*
                 */
                for block in &mut rblocks {
                    // Shift all nonnegative co-ordinates up by 1
                    if block.x >= 0 {
                        block.x += 1;
                    }
                    if block.y >= 0 {
                        block.y += 1;
                    }
                    // rotate
                    *block = if i == Input::RotateCW {PieceImpl::<F>::cw(block)} else {PieceImpl::<F>::ccw(block)};
                    // un-shift
                    if block.x > 0 {
                        block.x -= 1;
                    }
                    if block.y > 0 {
                        block.y -= 1;
                    }
                }
                let blocks: [Coord; 4] = [
                    rblocks[0] + self.center,
                    rblocks[1] + self.center,
                    rblocks[2] + self.center,
                    rblocks[3] + self.center];
                if self.can_place(&blocks) {
                    self.relative_blocks = rblocks;
                }
            }
            _ => {
                let mut rblocks: [Coord; 4] = self.relative_blocks;
                for block in &mut rblocks {
                    *block = if i == Input::RotateCW {PieceImpl::<F>::cw(block)} else {PieceImpl::<F>::ccw(block)};
                }
                let blocks: [Coord; 4] = [
                    rblocks[0] + self.center,
                    rblocks[1] + self.center,
                    rblocks[2] + self.center,
                    rblocks[3] + self.center];
                if self.can_place(&blocks) {
                    self.relative_blocks = rblocks;
                }
            }
        }
    }
    fn invoke_lock(&mut self) -> () {
        use std::cmp::Ordering;
        let mut blocks: [Coord; 4] = self.get_blocks();
        // blocks must be placed top-down
        blocks.sort_by(|a: &Coord, b: &Coord| -> Ordering {
            match a.y.cmp(&(b.y)) {
                Ordering::Equal => Ordering::Equal,
                Ordering::Greater => Ordering::Less,
                Ordering::Less => Ordering::Greater
            }
        });
        let mut field = self.field.borrow_mut();
        for block in &blocks {
            let result = field.set(*block);
            assert!(result.is_ok());
        }
        self.lock = true;
    }
    // fns used by Piece
    fn new(t:Type, d:u32, f:Rc<RefCell<F>>) -> Self {
        PieceImpl {
            typ: t,
            base_delay: d,
            lock_delay: d,
            field: f,
            center: match t {
                Type::I => ORIGIN_I,
                Type::O => ORIGIN_O,
                _ => ORIGIN_COORD
            },
            lock: false,
            relative_blocks: match t {
                Type::I => RELATIVE_I,
                Type::J => RELATIVE_J,
                Type::L => RELATIVE_L,
                Type::O => RELATIVE_O,
                Type::S => RELATIVE_S,
                Type::T => RELATIVE_T,
                Type::Z => RELATIVE_Z
            }
        }
    }

    pub fn time_step(&mut self, g:u32) -> Result<bool, LockError> {
        if self.lock {
            return Err(LockError);
        }
        for _ in 0..g {
            if self.can_drop() {
                self.center.y -= 1;
                if ! (self.can_drop()) {
                    if 0 == self.lock_delay {
                        self.invoke_lock();
                        break;
                    }
                    else {
                        self.lock_delay -= 1;
                    }
                }
            } else {
                if 0 == self.lock_delay {
                    self.invoke_lock();
                    break;
                } else {
                    self.lock_delay -= 1;
                }
            }
        }
        Ok(self.lock)
    }
    pub fn handle_input(&mut self, input:Input) -> Result<bool, LockError> {
        if self.lock {
            return Err(LockError);
        }
        match input {
            Input::HardDrop => {
                let steps = 22 + self.lock_delay;
                return self.time_step(steps);
            }
            Input::RotateCCW | Input::RotateCW => {self.rotate(input);}
            Input::ShiftLeft => {
                if self.can_shift(&Coord::new(-1,0)) {
                    self.center.x -= 1;
                }
            }
            Input::ShiftRight => {
                if self.can_shift(&Coord::new(1,0)) {
                    self.center.x += 1;
                }
            }
        }
        Ok(self.lock)
    }

    pub fn get_center(&self) -> Coord {
        self.center
    }
    pub fn get_blocks(&self) -> [Coord; 4] {
        [
            self.relative_blocks[0]+self.center,
            self.relative_blocks[1]+self.center,
            self.relative_blocks[2]+self.center,
            self.relative_blocks[3]+self.center
        ]
    }
    pub fn get_type(&self) -> Type {
        self.typ
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
    use std::rc::Rc;
    use std::cell::RefCell;
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

    #[derive(Debug)]
    struct MockField {
        set_args: Vec<Coord>,
        get_results: HashMap<Coord, bool>,
    }
    impl super::IField for MockField {
        fn get(&self, c:Coord) -> Result<bool, field::SizeError> {
            const WIDTH: i32 = field::WIDTH as i32;
            const HEIGHT: i32 = field::HEIGHT as i32;
            if c.x < 0 || c.y < 0 || c.x >= WIDTH || c.y >= HEIGHT {
                return Err(field::SizeError);
            }
            Result::Ok(*(self.get_results.get(&c).unwrap_or(&false)))
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
    // helpers
    fn test_same_coords<'a, 'b, T_A, T_B> (a: T_A, b: T_B) -> bool
        where T_A: IntoIterator <Item = &'a Coord>,
              T_B: IntoIterator <Item = &'b Coord>
    {
        let (ah, bh): (HashSet<Coord>, HashSet<Coord>) = (a.into_iter().cloned().collect(), b.into_iter().cloned().collect());
        ah == bh
    }
    fn compare_to_set<'a, T>(a: T, b: &HashSet<Coord>) -> bool
        where T: IntoIterator <Item = &'a Coord>
    {
        let ah: HashSet<Coord> = a.into_iter().cloned().collect();
        ah == *b
    }
    //tests
    #[test]
    fn test_constructor() {
        fn check_normal_blocks(t : super::Type, expected: &[Coord; 4], test_field: Rc<RefCell<MockField>>, test_delay: u32) {
            let test_piece = PieceImpl::new(t, test_delay, test_field);
            let test_coord = test_piece.get_center();
            assert_eq!(ORIGIN_COORD, test_coord);
            assert!(test_same_coords(expected, &test_piece.get_blocks()));
            assert_eq!(t, test_piece.get_type());
        }
        let test_delay = 1;
        let test_field = Rc::new(RefCell::new(MockField{set_args: Vec::new(), get_results: HashMap::new()}));
        let mut expected_blocks: [Coord;4];

        // I block
        expected_blocks = BLOCKS_I;
        {
            let test_piece = PieceImpl::new(super::Type::I, test_delay, test_field.clone());
            let test_coord = test_piece.get_center();
            assert_eq!(ORIGIN_I, test_coord);
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(super::Type::I, test_piece.get_type());
        }
        // O block
        expected_blocks = BLOCKS_O;
        {
            let test_piece = PieceImpl::new(super::Type::O, test_delay, test_field.clone());
            let test_coord = test_piece.get_center();
            assert_eq!(ORIGIN_O, test_coord);
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
            assert_eq!(super::Type::O, test_piece.get_type());
        }
        // J block
        expected_blocks = BLOCKS_J;
        check_normal_blocks(super::Type::J, &expected_blocks, test_field.clone(), test_delay);
        // L block
        expected_blocks = BLOCKS_L;
        check_normal_blocks(super::Type::L, &expected_blocks, test_field.clone(), test_delay);
        // S block
        expected_blocks = BLOCKS_S;
        check_normal_blocks(super::Type::S, &expected_blocks, test_field.clone(), test_delay);
        // T block
        expected_blocks = BLOCKS_T;
        check_normal_blocks(super::Type::T, &expected_blocks, test_field.clone(), test_delay);
        // Z block
        expected_blocks = BLOCKS_Z;
        check_normal_blocks(super::Type::Z, &expected_blocks, test_field, test_delay);
    }
    #[test]
    fn test_step() {
        fn test_center_and_lock(test_field: Rc<RefCell<MockField>>, t: super::Type) {
            let fail_safe = 44;
            let test_delay = 0;
            let mut i = 0;
            let mut expected_coord = match t {
                super::Type::I => { ORIGIN_I }
                super::Type::O => { ORIGIN_O }
                _ => { ORIGIN_COORD }
            };
            let mut test_piece = PieceImpl::new(t, test_delay, test_field.clone());
            while !(test_piece.time_step(1).unwrap()) {
                // If time step never returns false (i.e. the block falls forever)
                // we need to detect and fail.
                i = i+1;
                assert!(i < fail_safe);
                expected_coord.y -= 1;
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
            assert!(test_same_coords(&test_piece.get_blocks(), &test_field.borrow().set_args));
            test_field.borrow_mut().set_args.clear();
        }
        let test_field: Rc<RefCell<MockField>> = Rc::new(RefCell::new(MockField{set_args: Vec::new(), get_results: HashMap::new()}));
        test_center_and_lock(test_field.clone(), super::Type::J);
        test_center_and_lock(test_field.clone(), super::Type::L);
        test_center_and_lock(test_field.clone(), super::Type::S);
        test_center_and_lock(test_field.clone(), super::Type::Z);
        test_center_and_lock(test_field.clone(), super::Type::T);
        test_center_and_lock(test_field.clone(), super::Type::I);
        test_center_and_lock(test_field.clone(), super::Type::O);
        // test that stepped pieces report correct block positions before & after lockinng
        {
            let test_delay = 0;
            let mut test_piece = PieceImpl::new(super::Type::I, test_delay, test_field.clone());
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
            let mut test_piece = PieceImpl::new(super::Type::O, test_delay, test_field.clone());
            assert_eq!(false, test_piece.time_step(19).unwrap());
            assert!(test_piece.time_step(1).unwrap());
            assert!(test_piece.time_step(1).is_err());

            test_delay = 1;
            test_piece = PieceImpl::new(super::Type::I, test_delay, test_field.clone());
            assert_eq!(false, test_piece.time_step(20).unwrap());
            assert!(test_piece.time_step(1).unwrap());
            assert!(test_piece.time_step(1).is_err());

            test_delay = 3;
            test_piece = PieceImpl::new(super::Type::I, test_delay, test_field.clone());
            assert_eq!(false, test_piece.time_step(22).unwrap());
            assert!(test_piece.time_step(1).unwrap());
            assert!(test_piece.time_step(1).is_err());
        }
        // Test that locking with a larger-than-necessary time_step is not an error,
        // that time_step()ing after such a lock is an error,
        // and finally, that erroneous calls do not change the result of get_blocks
        {
            let test_delay = 3;
            let (mut test_O, mut test_I) = (PieceImpl::new(super::Type::O, test_delay, test_field.clone()), PieceImpl::new(super::Type::I, test_delay, test_field.clone()));
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
        test_field.borrow_mut().get_results.insert(Coord::new(3,0), true);
        test_field.borrow_mut().set_args.clear();
        {
            let test_delay = 0;
            let mut test_T = PieceImpl::new(super::Type::T, test_delay, test_field.clone());
            let mut test_Z = PieceImpl::new(super::Type::Z, test_delay, test_field.clone());

            assert!(test_T.time_step(30).unwrap());
            let mut expected_blocks = [Coord::new(3,1), Coord::new(4,1), Coord::new(5,1), Coord::new(4,2)];
            assert!(test_same_coords(&expected_blocks, &test_T.get_blocks()));
            assert!(test_same_coords(&expected_blocks, &test_field.borrow().set_args));
            test_field.borrow_mut().set_args.clear();

            assert!(test_Z.time_step(30).unwrap());
            expected_blocks = [Coord::new(3,1), Coord::new(4,1), Coord::new(4,0), Coord::new(5,0)];
            assert!(test_same_coords(&expected_blocks, &test_Z.get_blocks()));
            assert!(test_same_coords(&expected_blocks, &test_field.borrow().set_args));
        }
    }
    #[test]
    fn test_shift() {
        let test_field = Rc::new(RefCell::new(MockField{set_args: Vec::new(), get_results: HashMap::new()}));
        fn per_block(test_field: Rc<RefCell<MockField>>, typ: super::Type, origin: Coord, expected_initial_blocks: [Coord; 4], space_to_right_wall: i32, space_to_left_wall: i32, rightward_blocks: Vec<Coord>, leftward_blocks: Vec<Coord> ) {
            let mut test_piece: PieceImpl<MockField> = PieceImpl::new(typ, 0, test_field.clone());
            // Shift right
            assert!(test_piece.handle_input(super::Input::ShiftRight).is_ok());

            let mut expected_coord = origin;
            expected_coord.x = expected_coord.x + 1;
            let mut expected_blocks = expected_initial_blocks;
            for block in expected_blocks.iter_mut() {
                block.x += 1;
            }
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()), "expected {:?} observed {:?}", &expected_blocks, &test_piece.get_blocks());
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
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()), "expected {:?} observed {:?}", &expected_blocks, &test_piece.get_blocks());
            assert_eq!(expected_coord, test_piece.get_center());
            // Shift left
            assert!(test_piece.handle_input(super::Input::ShiftLeft).is_ok());
            expected_coord.x -= 1;
            for coord in expected_blocks.iter_mut() {
                coord.x -= 1;
            }
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()), "expected {:?} observed {:?}", &expected_blocks, &test_piece.get_blocks());
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
                test_field.borrow_mut().get_results.insert(rightward_block, true);
            }
            for _ in 0..2 {
                assert!(test_piece.handle_input(super::Input::ShiftRight).is_ok());
            }
            for coord in expected_blocks.iter_mut() {
                coord.x += 1;
            }
            expected_coord.x += 1;
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()), "expected {:?} observed {:?}", &expected_blocks, &test_piece.get_blocks());
            assert_eq!(expected_coord, test_piece.get_center());
            // press left
            for leftward_block in leftward_blocks {
                test_field.borrow_mut().get_results.insert(leftward_block, true);
            }
            assert!(test_piece.handle_input(super::Input::ShiftLeft).is_ok());
            test_field.borrow_mut().get_results.clear();
        }
        /*
         *__________*
         *          *21
         *XIIIIX    *20
         *          *19
         *          *18
         *0123456789*/
        per_block(
            test_field.clone(),
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
            test_field.clone(),
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
            test_field.clone(),
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
            test_field.clone(),
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
            test_field.clone(),
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
            test_field.clone(),
            super::Type::S,
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
            test_field,
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
        let test_field = Rc::new(RefCell::new(MockField{set_args: Vec::new(), get_results: HashMap::new()}));
        fn asserts(piece: &mut PieceImpl<MockField>, input: super::Input, center: Coord, expected_blocks: & [Coord; 4]) {
            assert!(piece.handle_input(input).is_ok());
            assert_eq!(center, piece.get_center());
            assert!(test_same_coords(expected_blocks, &piece.get_blocks()),
                    "Piece {:?} \ninput {:?} \nexpected_blocks {:?} \nobserved {:?}", piece, input, expected_blocks, &piece.get_blocks());
        }
        fn per_block(test_field: Rc<RefCell<MockField>>, typ: super::Type, center: Coord, expected_blocks: [[Coord; 4]; 7], obstruction: &Coord)
        {
            let mut test_piece = PieceImpl::new(typ, 0, test_field.clone());
            // CCW
            // 1
            asserts(& mut test_piece, super::Input::RotateCCW, center, &expected_blocks[0]);
            //2
            asserts(& mut test_piece, super::Input::RotateCCW, center, & expected_blocks[1]);
            //3
            asserts(& mut test_piece, super::Input::RotateCCW, center, &expected_blocks[2]);
            // Does rotation persist through timestep?
            let mut expected_blocks_dropped = expected_blocks[2];
            for block in expected_blocks_dropped.iter_mut() {
                block.y -= 1;
            }
            assert!(test_piece.time_step(1).is_ok());
            let drop_once: Coord = Coord{x:center.x, y:center.y-1};
            assert_eq!(drop_once, test_piece.get_center());
            assert!(test_same_coords(&expected_blocks_dropped, &test_piece.get_blocks()));
            // Return to original orientation (but not original position)
            asserts(& mut test_piece, super::Input::RotateCCW, drop_once, &expected_blocks[3]);
            // CW
            // 1
            asserts(& mut test_piece, super::Input::RotateCW, drop_once, &expected_blocks[4]);
            //2
            asserts(& mut test_piece, super::Input::RotateCW, drop_once, &expected_blocks[5]);
            //3
            asserts(& mut test_piece, super::Input::RotateCW, drop_once, &expected_blocks[6]);
            // Blocked rotations
            test_field.borrow_mut().get_results.insert(*obstruction, true);
            // rotation is blocked, expected blocks unchanged
            asserts(& mut test_piece, super::Input::RotateCW, drop_once, &expected_blocks[6]);
            asserts(& mut test_piece, super::Input::RotateCCW, drop_once, &expected_blocks[5]);
            test_field.borrow_mut().get_results.insert(*obstruction, false);
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
        const EXPECTED_BLOCKS_L: [[Coord; 4]; 7] =
            [[Coord{x:3,y:21}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}],
             [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:20}, Coord{x:5,y:20}],
             [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:19}],
             [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:5,y:20}],
             [Coord{x:4,y:20}, Coord{x:4,y:19}, Coord{x:4,y:18}, Coord{x:5,y:18}],
             [Coord{x:3,y:18}, Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}],
             [Coord{x:3,y:20}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}]];
        per_block(test_field.clone(), super::Type::L, ORIGIN_COORD, EXPECTED_BLOCKS_L, &Coord{x:5,y:20});
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
        const EXPECTED_BLOCKS_J: [[Coord; 4]; 7] = [
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}],
            [Coord{x:3,y:20}, Coord{x:4,y:20}, Coord{x:5,y:20}, Coord{x:5,y:19}],
            [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:21}],
            [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:5,y:19}],
            [Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:18}, Coord{x:5,y:19}],
            [Coord{x:3,y:18}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}]];
        per_block(test_field.clone(), super::Type::J, ORIGIN_COORD, EXPECTED_BLOCKS_J, &Coord{x:3,y:20});
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
        const EXPECTED_BLOCKS_S: [[Coord; 4]; 7] = [
            [Coord{x:3,y:20}, Coord{x:3,y:21}, Coord{x:4,y:19}, Coord{x:4,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}],
            [Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:19}, Coord{x:5,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}],
            [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:18}, Coord{x:5,y:19}],
            [Coord{x:3,y:18}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:19}],
            [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:18}, Coord{x:4,y:19}]];
        per_block(test_field.clone(), super::Type::S, ORIGIN_COORD, EXPECTED_BLOCKS_S, &Coord{x:4,y:20});
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
        /* Prevent CW rotation, but not CCW
         *__________*
         *          *21
         *    Z     *20
         *   ZcB    *19
         *   Z      *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *   ZZ     *20
         *    cB    *19
         *          *18
         *          *17
         *0123456789*
         *__________*
         *          *21
         *          *20
         *   ZcB    *19
         *    ZZ    *18
         *          *17
         *0123456789*
         */
        const EXPECTED_BLOCKS_Z:  [[Coord; 4]; 7] = [
            [Coord{x:3,y:19}, Coord{x:3,y:20}, Coord{x:4,y:20}, Coord{x:4,y:21}],
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:19}],
            [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}, Coord{x:5,y:21}],
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:19}],
            [Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:5,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:18}],
            [Coord{x:3,y:18}, Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:4,y:20}]];
        per_block(test_field.clone(), super::Type::Z, ORIGIN_COORD, EXPECTED_BLOCKS_Z, &Coord{x:5,y:19});
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
        const EXPECTED_BLOCKS_T:  [[Coord; 4]; 7] = [
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}],
            [Coord{x:3,y:20}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:20}],
            [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:20}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:4,y:20}],
            [Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:5,y:19}],
            [Coord{x:3,y:19}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:5,y:19}],
            [Coord{x:3,y:19}, Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}]];
        per_block(test_field.clone(), super::Type::T, ORIGIN_COORD, EXPECTED_BLOCKS_T, &Coord{x:4,y:20});
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
        const EXPECTED_BLOCKS_I:  [[Coord; 4]; 7] = [
            [Coord{x:4,y:18}, Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:6,y:19}],
            [Coord{x:5,y:21}, Coord{x:5,y:20}, Coord{x:5,y:19}, Coord{x:5,y:18}],
            [Coord{x:3,y:19}, Coord{x:4,y:19}, Coord{x:5,y:19}, Coord{x:6,y:19}],
            [Coord{x:5,y:20}, Coord{x:5,y:19}, Coord{x:5,y:18}, Coord{x:5,y:17}],
            [Coord{x:3,y:18}, Coord{x:4,y:18}, Coord{x:5,y:18}, Coord{x:6,y:18}],
            [Coord{x:4,y:20}, Coord{x:4,y:19}, Coord{x:4,y:18}, Coord{x:4,y:17}]];
        per_block(test_field.clone(), super::Type::I, ORIGIN_I, EXPECTED_BLOCKS_I, &Coord{x:5,y:19});
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
        {
            let mut test_piece = PieceImpl::new(super::Type::O, 0, test_field.clone());
            asserts(&mut test_piece, super::Input::RotateCW, ORIGIN_O, &BLOCKS_O);
            assert!(test_piece.handle_input(super::Input::RotateCCW).is_ok());
            asserts(&mut test_piece, super::Input::RotateCCW, ORIGIN_O, &BLOCKS_O);
        }
        // Check error (but not with an O block)
        {
            let mut test_piece = PieceImpl::new(super::Type::L, 0, test_field);
            assert!(test_piece.handle_input(super::Input::RotateCW).is_ok());
            assert!(test_piece.time_step(20).is_ok());
            let mut expected_blocks: [Coord; 4] = [Coord{x:4,y:19}, Coord{x:4,y:20}, Coord{x:4,y:21}, Coord{x:5,y:19}];
            for block in expected_blocks.iter_mut() {
                block.y -= 19;
            }
            assert!(test_piece.handle_input(super::Input::RotateCW).is_err());
            assert_eq!(ORIGIN_COORD+Coord{x:0,y:-19}, test_piece.get_center());
            assert!(test_same_coords(&expected_blocks, &test_piece.get_blocks()));
        }
    }
    #[test]
    fn test_drop() {
        const TEST_DELAY: u32 = 60;
        let test_field = Rc::new(RefCell::new(MockField {set_args: Vec::new(), get_results: HashMap::new()}));
        fn per_block(test_field: Rc<RefCell<MockField>>, typ: super::Type, center: Coord, expected_coords: &[Coord; 4]) {
            let mut test_piece = PieceImpl::new(typ, TEST_DELAY, test_field.clone());
            assert!(test_piece.handle_input(super::Input::HardDrop).is_ok());
            assert!(test_piece.handle_input(super::Input::HardDrop).is_err());
            assert!(test_same_coords(&test_piece.get_blocks(), &test_field.borrow().set_args));
            assert!(test_same_coords(&test_piece.get_blocks(), expected_coords));
            assert_eq!(center, test_piece.get_center());
            test_field.borrow_mut().set_args.clear();
        }
        const DROPPED_CENTER: Coord = Coord{x: ORIGIN_COORD.x, y: 0};

        const DROPPED_BOTTOM_J: [Coord; 4] = [
            Coord{x:BLOCKS_J[0].x,y:BLOCKS_J[0].y-20},
            Coord{x:BLOCKS_J[1].x,y:BLOCKS_J[1].y-20},
            Coord{x:BLOCKS_J[2].x,y:BLOCKS_J[2].y-20},
            Coord{x:BLOCKS_J[3].x,y:BLOCKS_J[3].y-20},
            ];
        per_block(test_field.clone(), super::Type::J, DROPPED_CENTER, &DROPPED_BOTTOM_J);
        const DROPPED_BOTTOM_L: [Coord; 4] = [
            Coord{x:BLOCKS_L[0].x,y:BLOCKS_L[0].y-20},
            Coord{x:BLOCKS_L[1].x,y:BLOCKS_L[1].y-20},
            Coord{x:BLOCKS_L[2].x,y:BLOCKS_L[2].y-20},
            Coord{x:BLOCKS_L[3].x,y:BLOCKS_L[3].y-20},
            ];
        per_block(test_field.clone(), super::Type::L, DROPPED_CENTER, &DROPPED_BOTTOM_L);
        const DROPPED_BOTTOM_S: [Coord; 4] = [
            Coord{x:BLOCKS_S[0].x,y:BLOCKS_S[0].y-20},
            Coord{x:BLOCKS_S[1].x,y:BLOCKS_S[1].y-20},
            Coord{x:BLOCKS_S[2].x,y:BLOCKS_S[2].y-20},
            Coord{x:BLOCKS_S[3].x,y:BLOCKS_S[3].y-20},
            ];
        per_block(test_field.clone(), super::Type::S, DROPPED_CENTER, &DROPPED_BOTTOM_S);
        const DROPPED_BOTTOM_Z: [Coord; 4] = [
            Coord{x:BLOCKS_Z[0].x,y:BLOCKS_Z[0].y-20},
            Coord{x:BLOCKS_Z[1].x,y:BLOCKS_Z[1].y-20},
            Coord{x:BLOCKS_Z[2].x,y:BLOCKS_Z[2].y-20},
            Coord{x:BLOCKS_Z[3].x,y:BLOCKS_Z[3].y-20},
            ];
        per_block(test_field.clone(), super::Type::Z, DROPPED_CENTER, &DROPPED_BOTTOM_Z);
        const DROPPED_BOTTOM_T: [Coord; 4] = [
            Coord{x:BLOCKS_T[0].x,y:BLOCKS_T[0].y-20},
            Coord{x:BLOCKS_T[1].x,y:BLOCKS_T[1].y-20},
            Coord{x:BLOCKS_T[2].x,y:BLOCKS_T[2].y-20},
            Coord{x:BLOCKS_T[3].x,y:BLOCKS_T[3].y-20},
            ];
        per_block(test_field.clone(), super::Type::T, DROPPED_CENTER, &DROPPED_BOTTOM_T);
        const DROPPED_I: Coord = Coord{x: ORIGIN_I.x, y: ORIGIN_I.y-20};
        const DROPPED_BOTTOM_I: [Coord; 4] = [
            Coord{x:BLOCKS_I[0].x,y:BLOCKS_I[0].y-20},
            Coord{x:BLOCKS_I[1].x,y:BLOCKS_I[1].y-20},
            Coord{x:BLOCKS_I[2].x,y:BLOCKS_I[2].y-20},
            Coord{x:BLOCKS_I[3].x,y:BLOCKS_I[3].y-20},
            ];
        per_block(test_field.clone(), super::Type::I, DROPPED_I, &DROPPED_BOTTOM_I);
        const DROPPED_O: Coord = Coord{x: ORIGIN_O.x, y:ORIGIN_O.y-20};
        const DROPPED_BOTTOM_O: [Coord; 4] = [
            Coord{x:BLOCKS_O[0].x,y:BLOCKS_O[0].y-20},
            Coord{x:BLOCKS_O[1].x,y:BLOCKS_O[1].y-20},
            Coord{x:BLOCKS_O[2].x,y:BLOCKS_O[2].y-20},
            Coord{x:BLOCKS_O[3].x,y:BLOCKS_O[3].y-20},
            ];
        per_block(test_field.clone(), super::Type::O, DROPPED_O, &DROPPED_BOTTOM_O);

        // Test falling against blocks in the field
        {
            let mut test_field_borrowed = test_field.borrow_mut();
            test_field_borrowed.get_results.insert(Coord::new(2,5), true);
            test_field_borrowed.get_results.insert(Coord::new(3,4), true);
            test_field_borrowed.get_results.insert(Coord::new(4,2), true);
            test_field_borrowed.get_results.insert(Coord::new(6,4), true);
        }

        /*
         *__________*
         *    SS    *06
         *  xSc     *05
         *   x  x   *04
         *          *03
         *    x     *02
         *0123456789*
         */
        per_block(test_field.clone(), super::Type::S, Coord{x:4,y:5}, &[Coord{x:3,y:5},Coord{x:4,y:5},Coord{x:4,y:6},Coord{x:5,y:6}]);
        /*
         *__________*
         *  xZZ     *05
         *   xcZx   *04
         *          *03
         *    x     *02
         *0123456789*
         */
        per_block(test_field.clone(), super::Type::Z, Coord{x:4,y:4}, &[Coord{x:3,y:5},Coord{x:4,y:5},Coord{x:4,y:4},Coord{x:5,y:4}]);
        /*
         *__________*
         *  xIIcI   *05
         *   x  x   *04
         *          *03
         *    x     *02
         *0123456789*
         */
        per_block(test_field.clone(), super::Type::I, Coord{x:5,y:5}, &[Coord{x:3,y:5},Coord{x:5,y:5},Coord{x:4,y:5},Coord{x:6,y:5}]);
        /*
         *__________*
         *  x       *05
         *   xOcx   *04
         *    OO    *03
         *    x     *02
         *0123456789*
         */
        per_block(test_field, super::Type::O, Coord{x:5,y:4}, &[Coord{x:4,y:3},Coord{x:5,y:3},Coord{x:4,y:4},Coord{x:5,y:4}]);
    }
}
