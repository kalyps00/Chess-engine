#ifndef FENPARSER_HPP
#define FENPARSER_HPP

#include <string>

class Board;

void load_fen_position(Board &board, const std::string &fen);

#endif
