
/*********************************************************************************
*                                    NDR Debug                                   *
**********************************************************************************/

/*
BSD 3-Clause License

Copyright (c) 2023, Neil Runcie

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdbool.h>
#include <string.h>
#include "ndr_debug.h"

// ST is the toggle for printing the symbolTable
bool NDR_ST = false;
// TT is the toggle for printing the tokenTable
bool NDR_TT = false;
// TL is the toggle for printing the tokenLocations
bool NDR_TL = false;
// M is the toggle for printing the matching process during the parsing process
bool NDR_M = false;
// R is the toggle for regex symbol compilation
bool NDR_R = false;
// PT is the toggle for printing the parseTable
bool NDR_PT = false;

void NDR_Set_Toggles(int argc, char* argv[]){
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--ST") == 0)
            NDR_ST = true;
        else if (strcmp(argv[i], "--TT") == 0)
            NDR_TT = true;
        else if (strcmp(argv[i], "--TL") == 0)
            NDR_TL = true;
        else if (strcmp(argv[i], "--M") == 0)
            NDR_M = true;
        else if (strcmp(argv[i], "--R") == 0)
            NDR_R = true;
        else if (strcmp(argv[i], "--PT") == 0)
            NDR_PT = true;
    }
}
