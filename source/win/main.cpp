#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <windows.h>

#include "chs_string.h"
#include "rulesengine.h"
#include "stateresolver.h"
#include "senseboard.h"

bool g_CancelRequested = false;

ClassicChessRules gameRules;
ChessGameState gameState;
StateResolver stateResolver;
SenseBoardState senseBoardState;

std::vector<std::string> split(const std::string& s, char seperator);

bool startsWith(const std::string& s, const std::string& token) {
	return s.find(token) == 0;
}

void sendCommand(const std::string& command, const std::string& data) {
	std::cout << command << " " << data << std::endl;
}
void sendCommand(const std::string& command) {
	std::cout << command << std::endl;
}

int runEngineWeb(int argc, char* argv[]) {
	while (!g_CancelRequested) {
		std::string input;
		std::getline(std::cin, input);
		
		if (input == "quit") {
			g_CancelRequested = true;
			break;
		}

		if (input == "ping") {
			sendCommand("pong");
			continue;
		}

		if (input == "setfen") {
			std::string fen;
			std::getline(std::cin, fen);
			gameState = ChessGameState(fen.c_str());
			sendCommand("fen", gameState.ToFEN().c_str());
			continue;
		}

		if (input == "getfen") {
			sendCommand("fen", gameState.ToFEN().c_str());
			continue;
		}

		if (input == "getsenseboard") {
			sendCommand("senseboard", senseBoardState.ToString().c_str());
			continue;
		}

		if (input == "setsenseboard") {
			std::string senseboardStr;
			std::getline(std::cin, senseboardStr);
			senseBoardState = SenseBoardState(senseboardStr.c_str());
			sendCommand("ok");
			continue;
		}

		if (input == "move") {
			std::string move;
			std::getline(std::cin, move);
			
			std::vector<std::string> parts = split(move, ':');
			if (parts.size() != 2) {
				sendCommand("error", "wrong move format");
				continue;
			}

			ChessPieceLocation from(parts[0].c_str());
			ChessMoveLocation to(parts[1].c_str());
			ChessStateMove stateMove(from, to);

			if (!gameState.MakeMove(stateMove)) {
				sendCommand("error", "invalid move");
				continue;
			}

			sendCommand("fen", gameState.ToFEN().c_str());
			continue;
		}

		if (input == "validmoves") {
			std::string location;
			std::getline(std::cin, location);

			ChessPieceLocation loc(location.c_str());
			std::vector<ChessMoveLocation> moves = gameRules.GetValidMovesForPiece(gameState, loc);

			std::ostringstream ss;
			for (const auto& move : moves) {
				ss << move.ToString() << ";";
			}
			sendCommand("validmoves", ss.str());
			continue;
		}

		// Playing commands
		{
			if (input == "start") {
				stateResolver.Init(gameState, SenseBoardState());
				sendCommand("ok");
				continue;
			}

			if (input == "senseboardupdate") {
			}
			
			if (input == "whosturn") {
				sendCommand(gameState.GetColorToMove() == CHESSCOLOR::WHITE ? "white" : "black");
				continue;
			}
		}

		sendCommand("error", "unknown command");
	}
	return 0;
}

std::time_t getExecModificationTime() {
	char exePathChars[MAX_PATH];
	GetModuleFileNameA(NULL, exePathChars, MAX_PATH);
	std::string exePath(exePathChars);

	auto ftime = std::filesystem::last_write_time(exePath);
	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
		ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
	);

	return std::chrono::system_clock::to_time_t(sctp);
}

int main(int argc, char* argv[]) {
	std::time_t cftime = getExecModificationTime();
	std::tm localTime;
	localtime_s(&localTime, &cftime);

  std::cout << "--|-- YDIBDGTCH [" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] --|--" << std::endl;

	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " uci|engineweb|tests" << std::endl;
		return 1;
	}

	std::string command = argv[1];
	if (command == "uci") {
		std::cout << "UCI mode isn't yet implemented" << std::endl;
		return 0;
	}

	if (command == "engineweb") {
		return runEngineWeb(argc, argv);
	}

	if (command == "tests") {
		std::cout << "Tests mode isn't yet implemented" << std::endl;
		return 0;
	}



	//ChessGameState gameState;
	//std::cout << gameState.ToString() << std::endl << std::endl;
	//std::cout << gameState.ToString(false) << std::endl << std::endl;
	//std::cout << gameState.ToString(true, false) << std::endl << std::endl;
	//std::cout << gameState.ToString(true, true, true) << std::endl << std::endl;

	//ChessGameState gameState2(String("rn1qkbnr/pp2pppp/2p5/5b2/3PN3/8/PPP2PPP/R1BQKBNR w KQkq - 0 1"));
	//std::cout << gameState2.ToString() << std::endl << std::endl;
	//gameState2.Set(1, 4, ChessPiece('Q'));
	//std::cout << gameState2.ToFEN() << std::endl << std::endl;

	//std::cout << "========================================" << std::endl;

	//ClassicChessRules rules;
	//ChessGameState state(String("r1bqk2r/pppp1ppp/2nb1n2/1B2p3/4P3/3P1N2/PPP2PPP/RNBQK2R w KQkq - 0 1"));
	//std::cout << state.ToString() << std::endl << std::endl;
	//state.MakeMove(ChessStateMove(ChessPieceLocation(0, 4), ChessMoveLocation(0, 6, CHESSMOVEINFO::CASTLING)));
	//std::cout << state.ToString() << std::endl << std::endl;
    
  return 0;
}

std::vector<std::string> split(const std::string& s, char seperator) {
	std::vector<std::string> output;

	std::string::size_type prev_pos = 0, pos = 0;

	while ((pos = s.find(seperator, pos)) != std::string::npos) {
		std::string substring(s.substr(prev_pos, pos - prev_pos));

		output.push_back(substring);

		prev_pos = ++pos;
	}

	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

	return output;
}