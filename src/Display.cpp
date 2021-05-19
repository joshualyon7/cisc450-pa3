#include "Display.hpp"
#include <ncurses.h>
#include <string>
#include "config.hpp"
#include <string.h>
#include <iostream>

Display::Display() {
    curRow = 1; curCol = 1;
    initscr();
    inX = 5;
    inY = 1;
    cbreak();
    in = newwin(3, COLS, LINES - 3, 0);
    out = newwin(LINES - 3, COLS, 0, 0);
    box(in, 0, 0); box(out, 0, 0);
    wrefresh(in); wrefresh(out);
    showPrompt();
}
void Display::writeOut(std::string toWrite) {
    curMessages.push_back(toWrite);
    if(curMessages.size() >= LINES - 3) {
        curMessages.pop_front();
        curRow = LINES - 3;
    }
    rewriteMsgs();
    wrefresh(out);
}

void Display::rewriteMsgs() {
    curRow = 1;
    for(std::list<std::string>::iterator it = curMessages.begin();
            it != curMessages.end(); it++) {
        wclrtoeol(out);
        mvwaddstr(out, curRow++, curCol, (*it).c_str());
    }
    box(out, 0, 0);
}

void Display::showPrompt() {
    mvwaddstr(in, inY, 1, "$>> ");
    wrefresh(in);
}

std::string Display::getInput() {
    char buf[MAX_LINE];
    mvwgetstr(in, inY, inX, buf);
    //std::cout << "before out" << buf << std::endl;
    std::string ret(buf);
    writeOut(buf);
    wmove(in, inY, inX);
    wclrtoeol(in);
    box(in, 0, 0);
    showPrompt();
    return ret;
}

Display::~Display() {
    delwin(in);
    delwin(out);
    endwin();
}