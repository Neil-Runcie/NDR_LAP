#include <stdbool.h>
#include <string.h>
#include "ndr_debug.h"

// ST is the toggle for printing the symbolTable
bool ST = false;
// TT is the toggle for printing the tokenTable
bool TT = false;
// TL is the toggle for printing the tokenLocations
bool TL = false;
// M is the toggle for printing the matching process during the parsing process
bool M = false;
// R is the toggle for regex symbol compilation
bool R = false;
// PT is the toggle for printing the parseTable
bool PT = false;

void Set_Toggles(int argc, char* argv[]){
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--ST") == 0)
            ST = true;
        if (strcmp(argv[i], "--TT") == 0)
            TT = true;
        if (strcmp(argv[i], "--TL") == 0)
            TL = true;
        if (strcmp(argv[i], "--M") == 0)
            M = true;
        if (strcmp(argv[i], "--R") == 0)
            R = true;
        if (strcmp(argv[i], "--PT") == 0)
            PT = true;
    }
}
