#pragma once

class Arg {
	private:
		char c;
		int index;
    public:
        Arg();
        Arg(char _c, int _index);
        char getChar() const;
        int getIndex() const;
};
