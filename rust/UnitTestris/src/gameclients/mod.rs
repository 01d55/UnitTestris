use std::convert::From;
use std::ops::Deref;
use gamestate::*;

/// captures current state of a piece without ref baggage
struct PieceData {
    blocks: [Coord; 4],
    typ: piece::Type,
}

impl<T: Deref<Target=piece::Piece>> From<T> for PieceData {
    fn from(piece_ref: T) -> PieceData {
        PieceData {
            blocks: piece_ref.get_blocks(),
            typ: piece_ref.get_type(),
        }
    }
}

struct RenderData {
    current: PieceData,
    ghost: Option<PieceData>,
    field: field::Field,
}
