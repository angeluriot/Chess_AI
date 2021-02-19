#include "PieceType.h"

PieceType PieceType::rook = PieceType(Rook, { {0, 1}, {0, -1}, {-1, 0}, {1, 0} }, true, 5);
PieceType PieceType::bishop = PieceType(Bishop, { {1, 1}, {1, -1}, {-1, 1}, {1, 1} }, true, 3);
PieceType PieceType::queen = PieceType(Queen, { {0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {1, 1} }, true, 9);
PieceType PieceType::knight = PieceType(Knight, { {2, 1}, {2, -1}, {-2, -1}, {-2, 1}, {1, 2}, {-1, 2}, {-1, -2}, {1, -2} }, false, 3);
PieceType PieceType::king = PieceType(King, { {0, 1}, {0, -1}, {-1, 0}, {1, 0}, {1, 1}, {1, -1}, {-1, 1}, {1, 1} }, false, 1000);
PieceType PieceType::white_pawn = PieceType(Pawn, { {-1, -1}, {0, -1}, {1, -1}, {0, -2} }, false, 1);
PieceType PieceType::black_pawn = PieceType(Pawn, { {-1, 1}, {0, 1}, {1, 1}, {0, 2} }, false, 1);