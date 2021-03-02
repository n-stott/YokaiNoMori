#include "piece.h"


static_vector<Pos::type, 8> Piece::kingOffsets {
    Pos::v, Pos::v+Pos::h, Pos::h, Pos::h-Pos::v, -Pos::v, -Pos::v-Pos::h, -Pos::h, -Pos::h+Pos::v
};

static_vector<Pos::type, 8> Piece::towerOffsets {
    Pos::v, Pos::h, -Pos::v, -Pos::h
};

static_vector<Pos::type, 8> Piece::rookOffsets {
    +Pos::v+Pos::h, +Pos::v-Pos::h, -Pos::v+Pos::h, -Pos::v-Pos::h
};

static_vector<Pos::type, 8> Piece::p1PawnOffsets {
    Pos::v
};

static_vector<Pos::type, 8> Piece::p2PawnOffsets {
    -Pos::v
};

static_vector<Pos::type, 8> Piece::p1SuperPawnOffsets {
    Pos::v, Pos::v+Pos::h, Pos::h, -Pos::v, -Pos::h, -Pos::h+Pos::v
};

static_vector<Pos::type, 8> Piece::p2SuperPawnOffsets {
    Pos::v, Pos::h, Pos::h-Pos::v, -Pos::v, -Pos::v-Pos::h, -Pos::h
};