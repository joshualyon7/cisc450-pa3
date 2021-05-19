#pragma once

#include <ncurses.h>
#include <string>
#include "config.hpp"
#include <list>

#define INWIN_H 3 

class Display {
private:
    WINDOW *in, *out;
    int curRow, curCol;
    int inX, inY;
    std::list<std::string> curMessages;
    void rewriteMsgs();
    void showPrompt();
    
public:
    Display();
    void writeOut(std::string toWrite);
    std::string getInput();
    ~Display();
};


