#include <iostream>
#include <sstream>
#include <string>
#include "board.h"
#include "movegen.h"
#include "search.h"
#include "attacks.h"
#include "zobrist.h"
#include "uci.h"


void uciLoop(){
    Board board;
    board.initBoard();
    initZobrist();
    initPawnAttacks(); initKnightAttacks(); initKingAttacks();

    std::string line;
    while (std::getline(std::cin, line)){
        std::istringstream iss(line);
        std::string token;
        iss >> token;                     // prima parola = il comando

        if (token == "uci"){
            std::cout << "id name Torrino v7\n";
            std::cout << "id author Giorgio\n";
            std::cout << "uciok" << std::endl;
        }
        else if (token == "isready"){
            std::cout << "readyok" << std::endl;
        }
        else if (token == "ucinewgame"){
            board.initBoard();
        }
        else if (token == "position"){
            std::string sub;
            iss >> sub;                           // "startpos" oppure "fen"

            if (sub == "startpos"){
                board.initBoard();
                iss >> sub;                       // consuma la parola dopo (forse "moves")
            }
            else if (sub == "fen"){
                std::string fen, part;
                for (int i = 0; i < 6 && (iss >> part); i++)
                    fen += part + " ";            // ricompone i 6 campi FEN in una stringa
                board.loadFEN(fen);
                iss >> sub;                       // consuma la parola dopo (forse "moves")
            }

            if (sub == "moves"){
                std::string mv;
                while (iss >> mv){                // ogni parola rimanente è una mossa
                    Move m = parseMove(board, mv);
                    if (m.fromSquare == -1) {
                        std::cout << "info string parseMove FALLITO su '" << mv << "'" << std::endl;
                        // qui NON chiamare makeMove
                        continue;   // o break
                    }
                    UndoInfo undo;
                    board.makeMove(m, undo);
                }
            }
        }
        else if (token == "go"){
            int depth = -1;             // -1 NON specificata
            int wtime = 0, btime = 0, winc = 0, binc = 0, moveTime = 0;

            std::string param;
            while (iss >> param) {
                if      (param == "depth")    iss >> depth;
                else if (param == "wtime")    iss >> wtime;      // tempo rimasto al bianco (ms)
                else if (param == "btime")    iss >> btime;      // tempo rimasto al nero (ms)
                else if (param == "winc")     iss >> winc;       // incremento bianco (ms)
                else if (param == "binc")     iss >> binc;       // incremento nero (ms)
                else if (param == "movetime") iss >> moveTime;   // tempo fisso per QUESTA mossa
            }

            // calcolo del budget di tempo per questa mossa
            int timeForMove;
            if (moveTime > 0) {
                timeForMove = moveTime;                          // se imposto, lo uso diretto
            } else {
                int timeLeft = (board.sideToMove == WHITE) ? wtime : btime;
                int inc      = (board.sideToMove == WHITE) ? winc  : binc;
                timeForMove  = timeLeft / 30 + inc / 2;          // stima semplice
                if (timeForMove > timeLeft - 50) timeForMove = timeLeft - 50;  // rete di sicurezza
                if (timeForMove < 1) timeForMove = 1;
            }

            // scelta della ricerca: profondita fissa (per i test) oppure a tempo
            Move best;
            if (depth > 0) {
                best = findBestMoveTime(board, 86400000, depth);
            } else {
                best = findBestMoveTime(board, timeForMove, 64);
            }

            Move endMove {-1, -1, QUIET};


            // se il motore non trova una mossa, per patta, stallo o scacco matto, lo fa sapere al protocollo UCI 
            if (best.fromSquare == endMove.fromSquare &&
                best.toSquare   == endMove.toSquare   &&
                best.flags      == endMove.flags) {
                    std::cout << "bestmove 0000" << std::endl; 
            } 
            // altrimenti restituisce la miglior mossa
            else {
                std::cout << "bestmove " << moveToString(best) << std::endl;        
            }
        }
        else if (token == "quit"){
            break;
        }
    }
}