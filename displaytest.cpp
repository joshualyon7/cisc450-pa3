#include "Display.hpp" 
#include <unistd.h>

int main() {
    Display d;

    std::string out = "hello!";
    //d.writeOut(out);
    std::string input = "in";
    while(true) {
        input = d.getInput();
    }
    
    return 0;
}
