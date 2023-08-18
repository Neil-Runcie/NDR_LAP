
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "ndr_cregex.h"

void NDR_InitializeRegexState(NDR_RegexState* state){
    state->keyword = malloc(1);
    state->isState = false;
    state->isLiteral = false;
    state->category = NDR_STATE_ACCEPT;
    state->numStartStates = 0;
    state->numAllowStates = 0;
    state->numEscapeStates = 0;
    state->numEndStates = 0;
    state->startRegex = malloc(sizeof(char*));
    state->allowRegex = malloc(sizeof(char*));
    state->escapeRegex = malloc(sizeof(char*));
    state->endRegex = malloc(sizeof(char*));
    state->compiledStartRegex = malloc(sizeof(NDR_Regex*));
    state->compiledAllowRegex = malloc(sizeof(NDR_Regex*));
    state->compiledEscapeRegex = malloc(sizeof(NDR_Regex*));
    state->compiledEndRegex = malloc(sizeof(NDR_Regex*));
}



void NDR_FreeRegexState(NDR_RegexState* state){

    free(state->keyword);

    for(size_t x = 0; x < state->numStartStates; x++){
        free(state->startRegex[x]);
        NDR_DestroyRegex(state->compiledStartRegex[x]);
    }
    for(size_t x = 0; x < state->numAllowStates; x++){
        free(state->allowRegex[x]);
        NDR_DestroyRegex(state->compiledAllowRegex[x]);
    }
    for(size_t x = 0; x < state->numEscapeStates; x++){
        free(state->escapeRegex[x]);
        NDR_DestroyRegex(state->compiledEscapeRegex[x]);
    }
    for(size_t x = 0; x < state->numEndStates; x++){
        free(state->endRegex[x]);
        NDR_DestroyRegex(state->compiledEndRegex[x]);
    }
    free(state->startRegex);
    free(state->allowRegex);
    free(state->escapeRegex);
    free(state->endRegex);
    free(state->compiledStartRegex);
    free(state->compiledAllowRegex);
    free(state->compiledEscapeRegex);
    free(state->compiledEndRegex);
}

int NDR_AddStartRegex(NDR_RegexState* state, char* regex){

    state->startRegex = realloc(state->startRegex, sizeof(char*) * (state->numStartStates + 2));
    state->startRegex[state->numStartStates] = malloc(strlen(regex) + 1);
    strcpy(state->startRegex[state->numStartStates], regex);

    state->compiledStartRegex = realloc(state->compiledStartRegex, sizeof(NDR_Regex*) * (state->numStartStates + 2));
    int result = NDR_CompileStateRegex(state->compiledStartRegex, state->numStartStates, regex);

    state->numStartStates++;

    return result;
}


int NDR_AddAllowRegex(NDR_RegexState* state, char* regex){

    state->allowRegex = realloc(state->allowRegex, sizeof(char*) * (state->numAllowStates + 2));
    state->allowRegex[state->numAllowStates] = malloc(strlen(regex) + 1);
    strcpy(state->allowRegex[state->numAllowStates], regex);

    state->compiledAllowRegex = realloc(state->compiledAllowRegex, sizeof(NDR_Regex*) * (state->numAllowStates + 2));
    int result = NDR_CompileStateRegex(state->compiledAllowRegex, state->numAllowStates, regex);

    state->numAllowStates++;

    return result;
}


int NDR_AddEscapeRegex(NDR_RegexState* state, char* regex){

    state->escapeRegex = realloc(state->escapeRegex, sizeof(char*) * (state->numEscapeStates + 2));
    state->escapeRegex[state->numEscapeStates] = malloc(strlen(regex) + 1);
    strcpy(state->escapeRegex[state->numEscapeStates], regex);

    state->compiledEscapeRegex = realloc(state->compiledEscapeRegex, sizeof(NDR_Regex*) * (state->numEscapeStates + 2));
    int result = NDR_CompileStateRegex(state->compiledEscapeRegex, state->numEscapeStates, regex);

    state->numEscapeStates++;

    return result;
}

int NDR_AddEndRegex(NDR_RegexState* state, char* regex){

    state->endRegex = realloc(state->endRegex, sizeof(char*) * (state->numEndStates + 2));
    state->endRegex[state->numEndStates] = malloc(strlen(regex) + 1);
    strcpy(state->endRegex[state->numEndStates], regex);

    state->compiledEndRegex = realloc(state->compiledEndRegex, sizeof(NDR_Regex*) * (state->numEndStates + 2));
    int result = NDR_CompileStateRegex(state->compiledEndRegex, state->numEndStates, regex);

    state->numEndStates++;

    return result;
}

int NDR_CompileStateRegex(NDR_Regex** regexTable, int stateIndex, char* regex){

    regexTable[stateIndex] = malloc(sizeof(NDR_Regex));
    NDR_InitRegex(regexTable[stateIndex]);

    if(NDR_CompileRegex(regexTable[stateIndex], regex) != 0){
        printf("%s\n", NDR_Regex_GetErrorMessage(regexTable[stateIndex]));
        return 1;
    }

    return 0;
}


int NDR_RSGetMatchResult(NDR_RegexState* regexState, char* token, NDR_StateCategories category, int regIndex){
    if(category == NDR_STATE_STARTSTATE)
        return NDR_MatchRegex(regexState->compiledStartRegex[regIndex], token);
    else if(category == NDR_STATE_ALLOWSTATE)
        return NDR_MatchRegex(regexState->compiledAllowRegex[regIndex], token);
    else if(category == NDR_STATE_ESCAPESTATE)
        return NDR_MatchRegex(regexState->compiledEscapeRegex[regIndex], token);
    else if(category == NDR_STATE_ENDSTATE)
        return NDR_MatchRegex(regexState->compiledEndRegex[regIndex], token);
    else
        return 404;
}
