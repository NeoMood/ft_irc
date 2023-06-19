#include "../includes/Arg.hpp"

Arg::Arg() {}

Arg::Arg(char _c, int _index) {
    this->c = _c;
    this->index = _index;
}

char Arg::getChar() const {
    return c;
}

int Arg::getIndex() const {
    return index;
}