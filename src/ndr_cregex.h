
/*********************************************************************************
*                                NDR C Regex State                               *
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

#ifndef NDRCREGEXSTATE_H
#define NDRCREGEXSTATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "ndr_statecategories.h"

#include "regex_engines/NDR_CRegex/include/ndr_regex.h"

typedef struct NDR_RegexState {
    char* keyword;
    bool isState;
    bool isLiteral;
    NDR_StateCategories category;

    size_t numStartStates;
    size_t numAllowStates;
    size_t numEscapeStates;
    size_t numEndStates;
    char** startRegex;
    char** allowRegex;
    char** escapeRegex;
    char** endRegex;
    NDR_Regex** compiledStartRegex;
    NDR_Regex** compiledAllowRegex;
    NDR_Regex** compiledEscapeRegex;
    NDR_Regex** compiledEndRegex;
} NDR_RegexState;

void NDR_InitializeRegexState(NDR_RegexState* state);
void NDR_FreeRegexState(NDR_RegexState* state);
int NDR_AddStartRegex(NDR_RegexState* state, char* regex);
int NDR_AddAllowRegex(NDR_RegexState* state, char* regex);
int NDR_AddEscapeRegex(NDR_RegexState* state, char* regex);
int NDR_AddEndRegex(NDR_RegexState* state, char* regex);
int NDR_CompileStateRegex(NDR_Regex** regexTable, int stateIndex, char* regex);
int NDR_RSGetMatchResult(NDR_RegexState* regexState, char* token, NDR_StateCategories category, int regIndex);

#endif
