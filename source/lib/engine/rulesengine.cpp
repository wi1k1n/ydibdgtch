#include "rulesengine.h"
#include "chs_common.h"

////////////////////////////////////////////////////////////////////////////
//////////////////////////////// ChessPiece ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

ChessPiece::ChessPiece(char c, CHESSHISTORY history) {
	init(c, history);
}

ChessPiece::ChessPiece(const String& s, CHESSHISTORY history) {
	if (s.isEmpty())
		return;
	init(s[0], history);

	if (s.length() > 1)
		m_history = s[1] == '.' ? CHESSHISTORY::MOVED : CHESSHISTORY::UNKNOWN; // TODO: double check and fix
}

String ChessPiece::ToString(bool symbolic) const {
	if (!IsValid())
		return String('?');
		if (!symbolic)
			return String(static_cast<uint8_t>(m_color)) + "-" + String(static_cast<uint8_t>(m_piece)) + "." + String(static_cast<uint8_t>(m_history));
		
		char pieceSymbol;
		switch (m_piece) {
			case CHESSPIECE::PAWN: pieceSymbol = 'P'; break;
			case CHESSPIECE::KNIGHT: pieceSymbol = 'N'; break;
			case CHESSPIECE::BISHOP: pieceSymbol = 'B'; break;
			case CHESSPIECE::ROOK: pieceSymbol = 'R'; break;
			case CHESSPIECE::QUEEN: pieceSymbol = 'Q'; break;
			case CHESSPIECE::KING: pieceSymbol = 'K'; break;
			default: return String('?');
		}
		if (m_color == CHESSCOLOR::BLACK)
			pieceSymbol += 'a' - 'A';
		return String(pieceSymbol);
}

void ChessPiece::init(char c, CHESSHISTORY history) {
	const char blackSymbol = c < 'a' ? (c + ('a' - 'A')) : c;
	switch (blackSymbol) {
		case 'p': m_piece = CHESSPIECE::PAWN; break;
		case 'n': m_piece = CHESSPIECE::KNIGHT; break;
		case 'b': m_piece = CHESSPIECE::BISHOP; break;
		case 'r': m_piece = CHESSPIECE::ROOK; break;
		case 'q': m_piece = CHESSPIECE::QUEEN; break;
		case 'k': m_piece = CHESSPIECE::KING; break;
		default: m_piece = CHESSPIECE::UNKNOWN; break;
	}
	if (m_piece != CHESSPIECE::UNKNOWN)
		m_color = c < 'a' ? CHESSCOLOR::WHITE : CHESSCOLOR::BLACK;
	m_history = history;
}

ChessPieceLocation::ChessPieceLocation(const String& s) {
	if (s.length() != 2)
		return;

	const char row = s[1];
	char col = s[0];
	if (col < 'a')
		col += 'a' - 'A';
	m_row = (row < '1' || row > '8') ? CHESSLOCATION_INVALID : (row - '1');
	m_col = (col < 'a' || col > 'h') ? CHESSLOCATION_INVALID : (col - 'a');
}

ChessGameState::ChessGameState(CHESSINITSTATE initState, CHESSCOLOR colorToMove)
	: m_colorToMove(colorToMove) {
	if (initState == CHESSINITSTATE::CLASSIC) {
		m_pieces.reserve(32);
		static const std::initializer_list<CHESSPIECE> startLine = {
			CHESSPIECE::ROOK,
			CHESSPIECE::KNIGHT,
			CHESSPIECE::BISHOP,
			CHESSPIECE::QUEEN,
			CHESSPIECE::KING,
			CHESSPIECE::BISHOP,
			CHESSPIECE::KNIGHT,
			CHESSPIECE::ROOK
		};
		fillRow(0, startLine, CHESSCOLOR::WHITE);
		fillRow(1, CHESSPIECE::PAWN, CHESSCOLOR::WHITE);
		fillRow(6, CHESSPIECE::PAWN, CHESSCOLOR::BLACK);
		fillRow(7, startLine, CHESSCOLOR::BLACK);
	}
}

ChessGameState::ChessGameState(const String& fenString, bool allowPartial) {
	initFromFEN(fenString, allowPartial);
}

bool ChessGameState::IsLocationOccupied(const ChessPieceLocation& location) const {
	return At(location).IsValid();
}

ChessPieceLocation ChessGameState::FindFirst(CHESSPIECE piece, CHESSCOLOR color) const {
	for (const auto& entry : m_pieces)
		if (entry.second.GetPiece() == piece && entry.second.GetColor() == color)
			return entry.first;
	return ChessPieceLocation();
}

ChessPieceLocation ChessGameState::FindFirst(const ChessPiece& piece) const {
	return FindFirst(piece.GetPiece(), piece.GetColor());
}

ChessPiece ChessGameState::At(const ChessPieceLocation& location) const {
	auto entry = m_pieces.find(location);
	return entry != m_pieces.end() ? entry->second : ChessPiece();
}

ChessPiece ChessGameState::At(uint8_t row, uint8_t col) const {
	return At(ChessPieceLocation(row, col));
}

ChessPiece ChessGameState::At(uint8_t idx) const {
	return At(idx / 8, idx % 8);
}

ChessPiece ChessGameState::At(const String& s) const {
	return At(ChessPieceLocation(s));
}

void ChessGameState::Set(const ChessPieceLocation& location, const ChessPiece& piece) {
	if (!location.IsOnBoard())
		return;
	if (piece.IsValid())
 		m_pieces.insert_or_assign(location, piece);
	else
		m_pieces.erase(location);
}

void ChessGameState::Set(uint8_t row, uint8_t col, const ChessPiece& piece) {
	Set(ChessPieceLocation(row, col), piece);
}

void ChessGameState::Set(const String& location, const ChessPiece& piece) {
	Set(ChessPieceLocation(location), piece);
}

void ChessGameState::Set(const String& location, const String& piece) {
	Set(location, ChessPiece(piece));
}

void ChessGameState::Unset(const ChessPieceLocation& location) {
	Set(location, ChessPiece());
}

void ChessGameState::Unset(uint8_t row, uint8_t col) {
	Set(row, col, ChessPiece());
}

void ChessGameState::Unset(const String& location) {
	Set(location, ChessPiece());
}

bool ChessGameState::MakeMove(const ChessStateMove& move) {
	if (!move.from.IsOnBoard() || !move.to.IsOnBoard() || move.to.GetMoveInfo() == CHESSMOVEINFO::UNKNOWN)
		return false;

	const ChessPiece piece = At(move.from);
	if (!piece.IsValid())
		return false;

	switch (move.to.GetMoveInfo()) {
		case CHESSMOVEINFO::UNKNOWN:
			//LOGLN("Unknown move: ", move.from.ToString(), " -> ", move.to.ToString(), ". Treating as ordinary");
		case CHESSMOVEINFO::NONE:
			//LOGLN("Ordinary move: ", move.from.ToString(), " -> ", move.to.ToString());
			Set(move.to, piece);
			Unset(move.from);
			break;
		case CHESSMOVEINFO::TAKING:
			//LOGLN("Taking move: ", move.from.ToString(), " -> ", move.to.ToString());
			Set(move.to, piece);
			Unset(move.from);
			break;
		case CHESSMOVEINFO::CASTLING: {
			//LOGLN("Castling move: ", move.from.ToString(), " -> ", move.to.ToString());
			switch (piece.GetPiece()) {
			case CHESSPIECE::KING:
			{
				const bool isMoveCastle = abs(move.to.GetCol() - move.from.GetCol()) == 2;
				const bool isKingValid = move.from.GetCol() == 4 && (move.from.GetRow() == 0 || move.from.GetRow() == 7);
				const int8_t dir = move.to.GetCol() > move.from.GetCol() ? 1 : -1;

				const ChessPieceLocation rookLoc(move.from.GetRow(), dir > 0 ? 7 : 0);
				const ChessPiece rook = At(rookLoc);
				const bool isRookValid = rook.GetPiece() == CHESSPIECE::ROOK && rook.IsValid() && rook.GetColor() == piece.GetColor();

				const bool isRowToRookEmpty = ([this](int8_t row, int8_t col, int8_t dir) {
					for (col += dir; col != 0 && col != 7; col += dir)
						if (At(row, col).IsValid())
							return false;
					return true;
					})(move.from.GetRow(), move.from.GetCol(), dir);

				if (!isMoveCastle || !isKingValid || !isRookValid || !isRowToRookEmpty)
					return false;

				Set(move.to, piece);
				Unset(move.from);
				Set(ChessPieceLocation(move.from.GetRow(), dir > 0 ? 5 : 3), rook);
				Unset(rookLoc);
				break;
			}
			case CHESSPIECE::ROOK: {
				// TODO: implement
			}
			default:
				return false;
		}
		break;
	}
	case CHESSMOVEINFO::PROMOTION: {
		LOGLN("Promotion move: ", move.from.ToString(), " -> ", move.to.ToString());
		const CHESSPIECE promotionTarget = move.to.GetPromotionTarget();
		if (promotionTarget == CHESSPIECE::UNKNOWN)
			return false;
		Set(move.to, ChessPiece(promotionTarget, piece.GetColor()));
		Unset(move.from);
		break;
	}
	default:
		return false;
	}
	return true;
}

void ChessGameState::NextTurn() {
	m_colorToMove = m_colorToMove == CHESSCOLOR::WHITE ? CHESSCOLOR::BLACK : CHESSCOLOR::WHITE;
}

String ChessGameState::ToString(bool legend, bool transpose, bool zeroBased) const {
	String res;
	auto drawColLegend = [&res, legend, transpose]() {
		res += String(transpose ? "\n" : "") + String(legend ? "    " : "");
		for (uint8_t col = 0; col < 8; ++col)
			res += String(" ") + String(static_cast<char>('a' + col)) + " " + String(col == 7 ? "" : " ");
		res += '\n';
		if (!transpose)
			res += '\n';
	};

	if (legend && !transpose)
		drawColLegend();

	const int8_t rowStart = 7 * transpose + 0; // transpose ? 7 : 0;
	const int8_t rowEnd = -9 * transpose + 8; // transpose ? -1 : 8;
	const int8_t rowIncrement = -2 * transpose + 1; // transpose ? -1 : 1;
	for (int8_t row = rowStart; row != rowEnd; row += rowIncrement) {
		if (legend)
			res += String(row + (!zeroBased)) + "   ";
		for (int8_t col = 0; col < 8; ++col) {
			auto entry = m_pieces.find({ static_cast<int8_t>(row), col });
			res += String(" ") + (entry == m_pieces.end() ? String(" ") : entry->second.ToString()) + String(" ") + String(col == 7 ? "" : "|");
		}
		res += '\n';
	}

	if (legend && transpose)
		drawColLegend();
	
	return res;
}

String ChessGameState::ToFEN() const {
	String fen;
	uint8_t emptyCounter = 0;
	for (int8_t row = 7; row >= 0; --row) {
		for (uint8_t col = 0; col < 8; ++col) {
			ChessPiece piece = At(row, col);
			if (!piece.IsValid()) {
				++emptyCounter;
				continue;
			}
			if (emptyCounter) {
				fen += String(static_cast<int>(emptyCounter));
				emptyCounter = 0;
			}
			fen += piece.ToString();
		}
		if (emptyCounter) {
			fen += String(static_cast<int>(emptyCounter));
			emptyCounter = 0;
		}
		fen += row > 0 ? "/" : "";
		// Serial.println();
	}
	fen += String(" ") + String((m_colorToMove == CHESSCOLOR::BLACK ? "b" : "w"));
	fen += " KQkq"; // TODO: handle castling properly!

	// semantically only a single en-passant pawn exists -> first occurence
	String enPassant("-");
	for (auto& entry : m_pieces) {
		ChessPiece piece = entry.second;
		if (piece.GetPiece() != CHESSPIECE::PAWN)
			continue;
		if (piece.GetHistory() == CHESSHISTORY::MOVED) { // TODO: fix chesshistory entries
			enPassant = entry.first.ToString();
			break;
		}
	}
	fen += String(" ") + enPassant;
	fen += String(" ") + String(m_fullMoves) + String(" ") + String(m_halfMoves);
	return fen;
}

void ChessGameState::fillRow(uint8_t row, CHESSPIECE piece, CHESSCOLOR color) {
	for (uint8_t i = 0; i < 8; ++i)
		m_pieces.emplace(ChessPieceLocation(row, i), ChessPiece(piece, color));
}

void ChessGameState::fillRow(uint8_t row, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color) {
	auto it = pieces.begin();
	for (uint8_t i = 0; i < 8 && it != pieces.end(); ++i, ++it)
		m_pieces.emplace(ChessPieceLocation(row, i), ChessPiece(*it, color));
}

void ChessGameState::fillCol(uint8_t col, CHESSPIECE piece, CHESSCOLOR color) {
	for (uint8_t i = 0; i < 8; ++i)
		m_pieces.emplace(ChessPieceLocation(i, col), ChessPiece(piece, color));
}

void ChessGameState::fillCol(uint8_t col, const std::initializer_list<CHESSPIECE>& pieces, CHESSCOLOR color) {
	auto it = pieces.begin();
	for (uint8_t i = 0; i < 8 && it != pieces.end(); ++i, ++it)
		m_pieces.emplace(ChessPieceLocation(i, col), ChessPiece(*it, color));
}

bool ChessGameState::initFromFEN(const String& fenString, bool allowPartial) {
	LOGLN("Loading from FEN: ", fenString);

	bool minimallyInitialized = false;
	auto invalidate = [&]() {
		// for (const auto& entry : this->_pieces) {
		// 	LOG(entry.first.toString());
		// 	LOG(" => "_f);
		// 	LOGLN(entry.second.toString());
		// }
		if (allowPartial && minimallyInitialized)
			return true;

		DLOGLN("Invalidated with false");
		this->m_pieces.clear();
		this->m_colorToMove = CHESSCOLOR::UNKNOWN;
		this->m_fullMoves = 0;
		this->m_halfMoves = 1;
		return false;
	};

	// 1. Handle pieces placement part
	uint8_t fenCursor = 0;
	Array<String> rows(8);
	{ // retrieve strings that corresponds to rows
		uint8_t rowIdx = 0;
		for (fenCursor = 0; fenCursor < fenString.length(); ++fenCursor) {
			char cc = fenString[fenCursor];
			if (cc == ' ')
				break;
			if (cc == '/') {
				++rowIdx;
				continue;
			}
			rows[rowIdx] += fenString[fenCursor];
		}
		// Validate content and create pieces
		if (rowIdx != 7)
			return invalidate();
	}
	
	// process rows
	for (int8_t rowIdx = 7; rowIdx >= 0; --rowIdx) {
		const String& row = rows[7 - rowIdx];

		uint8_t pieceCounter = 0;
		for (uint8_t i = 0; i < row.length(); ++i) {
			char cc = row[i];
			if (cc >= '1' && cc <= '8') {
				uint8_t n = cc - '0';
				if (pieceCounter + n > 8)
					return invalidate();
				pieceCounter += n;
				continue;
			}
			if (pieceCounter > 7)
				return invalidate();
			ChessPiece piece(cc);
			if (!piece.IsValid())
				return invalidate();
			Set(rowIdx, pieceCounter++, piece);
			// LOG(at(rowIdx, pieceCounter-1).toString()); LOG(" -> "_f); LOG(rowIdx); LOG(";"); LOGLN(pieceCounter - 1);
		}
		if (pieceCounter > 8)
			return invalidate();
	}
	minimallyInitialized = true;
	LOGLN("1. Handle pieces placement part");

	// 2. Color to move
	if (++fenCursor >= fenString.length())
		return invalidate();
	
	char clrToMoveChar = fenString[fenCursor++];
	if (clrToMoveChar == 'w')
		m_colorToMove = CHESSCOLOR::WHITE;
	else if (clrToMoveChar == 'b')
		m_colorToMove = CHESSCOLOR::BLACK;
	else
		return invalidate();
	
	if (fenCursor >= fenString.length() || fenString[fenCursor++] != ' ')
		return invalidate();
	LOGLN("2. Color to move");
	
	// 3. Castling options
	if (fenCursor >= fenString.length())
		return invalidate();
	char castlingCharFirst = fenString[fenCursor++];
	if (castlingCharFirst != '-') {
		if (castlingCharFirst == ' ')
			return invalidate();
		// TODO: implement castling incl. Shredder-FEN standard
		uint8_t castlingCharCount = 1;
		for (; fenCursor < fenString.length(); ++fenCursor, ++castlingCharCount)
			if (fenString[fenCursor] == ' ')
				break;
		if (castlingCharCount > 4)
			return invalidate();
	}
	LOGLN("3. Castling options");
	
	// 4. En-passant pawn
	if (++fenCursor >= fenString.length())
		return invalidate();

	char epCharFirst = fenString[fenCursor++];
	if (epCharFirst != '-') {
		if (epCharFirst < 'a')
			epCharFirst += 'a' - 'A';
		if (epCharFirst < 'a' || epCharFirst > 'h')
			return invalidate();
		// get en-passant sqare row
		if (fenCursor >= fenString.length())
			return invalidate();
		char epCharRow = fenString[fenCursor++];
		// get pawn sucseptible to en-passant take
		int8_t pawnRow = -1;
		if (m_colorToMove == CHESSCOLOR::BLACK && epCharRow == '3')
			pawnRow = 4;
		else if (m_colorToMove == CHESSCOLOR::WHITE && epCharRow == '6')
			pawnRow = 5;
		if (pawnRow < 0)
			return invalidate();
		ChessPiece pawn = At(pawnRow, epCharFirst - 'a');
		if (!pawn.IsValid() || pawn.GetPiece() != CHESSPIECE::PAWN || pawn.GetColor() == m_colorToMove)
			invalidate();
		pawn.SetHistory(CHESSHISTORY::MOVED); // TODO: fix this
	}
	LOGLN("4. En-passant pawn");
	
	// 5. Half-moves since last pawn advance
	if (fenCursor >= fenString.length() || fenString[fenCursor++] != ' ')
		return invalidate();
	
	String halfMoves;
	for (; fenCursor < fenString.length(); ++fenCursor) {
		char cc = fenString[fenCursor];
		if (cc == ' ')
			break;
		if (!isDigit(cc))
			return invalidate();
		halfMoves += static_cast<char>(cc);
	}
	m_halfMoves = (uint8_t)halfMoves.toInt();
	LOGLN("5. Half-moves since last pawn advance");
	
	// 6. Full-moves since start
	if (fenCursor >= fenString.length() || fenString[fenCursor++] != ' ')
		return invalidate();
	
	String fullMoves;
	for (; fenCursor < fenString.length(); ++fenCursor) {
		char cc = fenString[fenCursor];
		if (cc == ' ')
			break;
		if (!isDigit(cc))
			return invalidate();
		fullMoves += static_cast<char>(cc);
	}
	m_fullMoves = (uint8_t)fullMoves.toInt();
	if (m_fullMoves == 0)
		return invalidate();
	LOGLN("6. Full-moves since start");
	
	return true;
}

Array<ChessMoveLocation> ClassicChessRules::GetPossibleMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location, bool takesOnly) const {
	Array<ChessMoveLocation> moves;

	const ChessPiece chessPiece = state.At(location);
	const CHESSPIECE piece = chessPiece.GetPiece();
	const CHESSCOLOR clr = chessPiece.GetColor();
	const CHESSCOLOR clrOpposite = chessPiece.GetColorOpposite();
	
	// Serial.print(F("Generating moves for piece "));
	// Serial.print(chessPiece.toString());
	// Serial.print(F(" at location "));
	// Serial.println(location.toString());

	if (piece == CHESSPIECE::UNKNOWN || clr == CHESSCOLOR::UNKNOWN) {
		// Serial.println(F("No piece at given location"));
		return moves;
	}

	const int8_t row = location.GetRow();
	const int8_t col = location.GetCol();

	static const Hashmap<CHESSPIECE, Array<Pair<int8_t, int8_t>>> dirsMap = {
		{ CHESSPIECE::KNIGHT, {{-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}} },
		{ CHESSPIECE::BISHOP, {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}} },
		{ CHESSPIECE::ROOK, {{-1, 0}, {1, 0}, {0, -1}, {0, 1}} },
		{ CHESSPIECE::QUEEN, {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}} },
		{ CHESSPIECE::KING, {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}} }
	};

	switch (piece) {
		case CHESSPIECE::PAWN: {
			// Serial.println(F("> Pawn"));
			const int8_t dir = clr == CHESSCOLOR::WHITE ? 1 : -1;
			// diagonal moves
			const bool isOnEnPassantRow = location.GetRow() == ((dir + 1) / 2 + 3); // 4 for white, 3 for black
			for (int8_t dx = -1; dx < 2; dx += 2) {
				// ordinary diagonal
				const ChessMoveLocation diagonal(static_cast<int8_t>(row) + dir, col + dx, CHESSMOVEINFO::TAKING);
				if (diagonal.IsOnBoard() && state.At(diagonal).GetColor() == clrOpposite) {
					moves.push_back(diagonal);
					// Serial.print(F(">>> diagonal "));
					// Serial.println(dir > 0 ? F("right") : F("left"));
				}

				// en-passant
				if (!isOnEnPassantRow)
					continue;

				const ChessMoveLocation diagEnPassant(static_cast<int8_t>(row), col + dx, CHESSMOVEINFO::TAKING);
				if (diagEnPassant.IsOnBoard()) {
					const ChessPiece pieceOpp = state.At(diagEnPassant);
					if (pieceOpp.GetColor() == clrOpposite && pieceOpp.GetPiece() == CHESSPIECE::PAWN && pieceOpp.GetHistory() == CHESSHISTORY::ENPASSANT_VULNERABLE) {
						moves.push_back(diagEnPassant);
						// Serial.print(F(">>> en-passant "));
						// Serial.println(dir > 0 ? F("right") : F("left"));
					}
				}
			}

			if (takesOnly)
				break;

			// single forward move
			const ChessMoveLocation singleForward(static_cast<int8_t>(row) + dir, col, CHESSMOVEINFO::NONE);
			if (singleForward.IsOnBoard() && state.At(singleForward).GetColor() == CHESSCOLOR::UNKNOWN) {
				moves.push_back(singleForward);
				// Serial.println(F(">>> single forward"));
				// double forward move
				const bool isOnInitialRow = location.GetRow()  == ((7 + dir) % 7); // 1 for white, 6 for black
				const ChessMoveLocation doubleForward(static_cast<int8_t>(row) + dir * 2, col, CHESSMOVEINFO::NONE);
				if (isOnInitialRow && doubleForward.IsOnBoard() && state.At(doubleForward).GetColor() == CHESSCOLOR::UNKNOWN) {
					moves.push_back(doubleForward);
					// Serial.println(F(">>> double forward"));
				}
			}

			// TODO: handle promotion
			break;
		}
		case CHESSPIECE::KNIGHT:
		case CHESSPIECE::BISHOP:
		case CHESSPIECE::ROOK:
		case CHESSPIECE::QUEEN:
		case CHESSPIECE::KING: {
			// TODO: handle castling
			// Serial.print(F("> "));
			// Serial.println(chessPiece.toString());
			const auto dirsEntry = dirsMap.find(piece);
			if (dirsEntry == dirsMap.end()) // sanity check, normally doesn't reach here at all
				return Array<ChessMoveLocation>();
			const Array<Pair<int8_t, int8_t>>& dirs = dirsEntry->second;
			for (const auto& dir : dirs) {
				ChessMoveLocation loc(row + dir.first, col + dir.second);
				while (loc.IsOnBoard()) {
					const ChessPiece curPiece = state.At(loc);
					const CHESSCOLOR curClr = curPiece.GetColor();
					if (curClr == clr)
						break;
					if (curClr == clrOpposite)
						loc.SetMoveInfo(CHESSMOVEINFO::TAKING);
					if (!takesOnly || loc.GetMoveInfo() == CHESSMOVEINFO::TAKING)
						moves.push_back(loc);
					if (curClr != CHESSCOLOR::UNKNOWN || piece == CHESSPIECE::KNIGHT || piece == CHESSPIECE::KING) // TODO: store flag for that in the map
						break;
					loc = ChessMoveLocation(loc.GetRow() + dir.first, loc.GetCol() + dir.second);
				}
			}
			break;
		}
		default:
			// Serial.println(F("> default"));
			break;
	}

	return moves;
}

Array<ChessMoveLocation> ClassicChessRules::GetValidMovesForPiece(const ChessGameState& state, const ChessPieceLocation& location) const {
	// DLOGLN("getValidMovesForPiece: ");

	Array<ChessMoveLocation> possibleMoves = GetPossibleMovesForPiece(state, location);
	if (possibleMoves.empty())
		return possibleMoves;
	
	const ChessPiece piece = state.At(location);
	possibleMoves.erase(std::remove_if(possibleMoves.begin(), possibleMoves.end(), [&](const ChessMoveLocation& move) {
		ChessGameState stateTemp(state);
		
		// LOG("Pieces: ");
		// for (auto& p : stateTemp.getPieces())
		// 	LOG(p.first.toString() + String("(") + p.second.toString() + "),");
		// LOGLN();
		
		stateTemp.Unset(location);
		stateTemp.Set(move, piece);
		
		// DLOGLN(String("Moved ") + piece.toString() + " from " + location.toString() + " to " + move.toString());
		// LOG("Pieces: ");
		// for (auto& p : stateTemp.getPieces())
		// 	LOG(p.first.toString() + String("(") + p.second.toString() + "),");
		// LOGLN();
		
		bool check = IsCheck(stateTemp, piece.GetColor());
		return check;
	}), possibleMoves.end());

	return possibleMoves;
}

bool ClassicChessRules::IsCheck(const ChessGameState& state, CHESSCOLOR color) const {
	if (color == CHESSCOLOR::UNKNOWN)
		color = state.GetColorToMove();
	
	// DLOGLN(String("isCheck (") + (color == CHESSCOLOR::BLACK ? "b" : (color == CHESSCOLOR::WHITE ? "w" : "?")) + String("): "));
	// LOGLN(state.toString());

	// TODO: optimize??
	ChessPieceLocation locKing = state.FindFirst(CHESSPIECE::KING, color);
	if (!locKing.IsOnBoard()) {
		// DLOGLN("FALSE");
		return false;
	}
	for (const auto& entry : state.GetPieces()) {
		const ChessPieceLocation& loc = entry.first;
		const ChessPiece& piece = entry.second;
		if (!loc.IsOnBoard() || !piece.IsValid() || piece.GetColorOpposite() != color) // only iterate over opponent's pieces
			continue;
		Array<ChessMoveLocation> takeMoves = GetPossibleMovesForPiece(state, loc, true);
		// LOG(loc.toString() + "(" + piece.toString() + ")" + ">" + (takeMoves.empty() ? "." : ""));
		// for (auto& move : takeMoves)
		// 	LOG(move.toString() + " ");
		// LOG(";");
		for (const auto& move : takeMoves)
			if (static_cast<ChessPieceLocation>(move) == locKing) {
				// DLOGLN("TRUE");
				return true;
			}
	}
	// LOGLN();
	return false;
}

bool ClassicChessRules::IsMate(const ChessGameState& state, CHESSCOLOR color) const {
	if (!IsCheck(state, color)) {
		// Serial.println("Not in check!");
		return false;
	}
	
	if (color == CHESSCOLOR::UNKNOWN)
		color = state.GetColorToMove();
	
	// DLOGLN(String("isMate (") + (color == CHESSCOLOR::BLACK ? "b" : (color == CHESSCOLOR::WHITE ? "w" : "?")) + String("): "));

	// TODO: optimize??
	for (const auto& entry : state.GetPieces()) {
		const ChessPieceLocation& loc = entry.first;
		const ChessPiece& piece = entry.second;
		if (!loc.IsOnBoard() || !piece.IsValid() || piece.GetColor() != color) // only iterate over our pieces
			continue;
		auto validMoves = GetValidMovesForPiece(state, loc);
		
		// LOG(loc.toString() + ">" + (validMoves.empty() ? "." : ""));
		// for (auto& move : validMoves)
		// 	LOG(move.toString() + " ");
		// LOG(";");

		if (!validMoves.empty()) {
			// DLOGLN("FALSE");
			return false;
		}
	}
	// LOGLN();
	return true;
}

bool ClassicChessRules::IsDraw(const ChessGameState& state) const {
	// 1. Stalemate
	const CHESSCOLOR clr = state.GetColorToMove();
	if (clr == CHESSCOLOR::UNKNOWN)
		return false;
	
	const ChessPieceLocation locKing = state.FindFirst(CHESSPIECE::KING, clr);
	if (!locKing.IsOnBoard())
		return false;

	if (IsMate(state))
		return false;

	// Serial.println("1. Stalemate");
	
	// 2. Dead position
	// TODO: implement

	// 3. Insufficient material
	static const Array<Pair<Array<ChessPiece>, Array<ChessPiece>>> cases = {
		{ { ChessPiece("K") }, { ChessPiece("k") } },
		{ { ChessPiece("K"), ChessPiece("B") }, { ChessPiece("k") } },
		{ { ChessPiece("K") }, { ChessPiece("k"), ChessPiece("b") } },
		{ { ChessPiece("K"), ChessPiece("N") }, { ChessPiece("k") } },
		{ { ChessPiece("K") }, { ChessPiece("k"), ChessPiece("n") } }
	};

	for (const auto& casePair : cases) {
		ChessGameState stateTemp(state);
		const Array<ChessPiece> piecesSets[] = { casePair.first, casePair.second }; // white and black
		// first find and remove all the pieces from temp state
		for (uint8_t i = 0; i < 2; ++i) {
			const Array<ChessPiece> pieces = piecesSets[i];
			for (const ChessPiece& piece : pieces)
				stateTemp.Unset(stateTemp.FindFirst(piece));
		}

		// if nothing's left in temp state, then it was the case -> draw
		bool somethingsLeft = false;
		for (const auto& piece : stateTemp.GetPieces()) {
			if (piece.first.IsOnBoard() && piece.second.IsValid()) {
				somethingsLeft = true;
				break;
			}
		}

		if (!somethingsLeft)
			return true;
	}
	// 3.1 Extra case: King and bishop vs. king and bishop of the same color as the opponent's bishop
	// TODO: implement
	// Serial.println("3. Insufficient material");

	// 4. Threefold Repetition
	// Not tackable with this class. Should keep history (inherit and implement)

	// 5. 50-Move Rule
	if (state.GetHalfMoves() >= 100)
		return true;
	// Serial.println("5. 50-Move Rule");
	
	return false;
}