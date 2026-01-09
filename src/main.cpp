#include "controller/GameControler.hpp"
#include "model/Board.hpp"
#include "model/MoveGenerator.hpp"
#include <iostream>
#include <chrono>
void test()
{

    std::string kiwi = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    std::string pos4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    std::string pos5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ";
    Board test_board;
    load_fen_position(test_board, kiwi);
    int depth = 5;

    std::cout << "Perft test depth: " << depth << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    long long result = MoveGenerator::perft(test_board, depth);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Result: " << result << " nodes." << std::endl;
    std::cout << "Time: " << elapsed.count() << "s" << std::endl;
    if (elapsed.count() > 0)
        std::cout << "NPS: " << static_cast<long long>(result / elapsed.count()) << std::endl;
}
int main()
{
    test();
    GameControler engine;
    engine.run();

    return 0;
}