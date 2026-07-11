#include <iostream>
#include <sstream>
#include <string>
#include "board.h"
#include "movegen.h"
#include "search.h"
#include "attacks.h"
#include "uci.h"

void uciLoop(){
    Board board;
    board.initBoard();
    initPawnAttacks(); initKnightAttacks(); initKingAttacks();

    std::string line;
    while (std::getline(std::cin, line)){
        std::istringstream iss(line);
        std::string token;
        iss >> token;                     // prima parola = il comando

        if (token == "uci"){
            std::cout << "id name IlMioMotore\n";
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
            // BISOGNA AGGIUNGERE TUTTI GLI ALTRI PARAMETRI
            // wtime, btime, movetime ecc -> Non posso finche non ho iterativeDeepening
            int depth = 4;
            std::string param;
            while (iss >> param) {
                if (param == "depth") {
                    iss >> depth;
                }
            }
            
            Move best = findBestMove(board, depth);   // profondità fissa per ora, ignori i parametri di tempo
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