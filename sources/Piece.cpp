#include "Piece.h"
#include "BitBoard.h"

int get_value(uint8_t piece)
{
	switch (piece)
	{
		case Piece::White_Pawn: return 100;
		case Piece::Black_Pawn: return -100;
		case Piece::White_Knight: return 300;
		case Piece::Black_Knight: return -300;
		case Piece::White_Bishop: return 300;
		case Piece::Black_Bishop: return -300;
		case Piece::White_Rook: return 500;
		case Piece::Black_Rook: return -500;
		case Piece::White_Queen: return 900;
		case Piece::Black_Queen: return -900;
		case Piece::White_King: return 90000;
		case Piece::Black_King: return -90000;
		default: return 0;
	}
}
