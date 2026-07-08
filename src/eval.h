#pragma once

#include "board.h"

/*
    !!!     IMPORTANTE      !!!

    Bisogna aggiungere le Maschere per le posizioni dei pezzi durante la partita
    2 per i pedoni, Inizio e Late game
    2 per il re, stesso ragionamento
    1 per ogni pezzo rimanente (oppure 2 a testa, da decidere)

*/

constexpr int PIECE_WEIGHTS[PIECE_TYPE_NB] = {
    100,    // PAWN
    320,    // KNIGHT
    330,    // BISHOP
    500,    // ROOK
    900,    // QUEEN
    0       // KING
};

/*
    !!!     DA MODIFICARE IN FUTURO     !!!

    In questo momento evaluate conta solo i pezzi presenti nella board
    e li moltiplica per il rispettivo peso, poi somma tutto e restituisce il valore.

    Restituisce un valore positivo a prescindere dal Colore, utile per Negamax

*/
int evaluate(Board &board);