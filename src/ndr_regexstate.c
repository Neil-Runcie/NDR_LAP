
/*********************************************************************************
*                                 NDR Regex State                                *
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
#include "ndr_regexstate.h"


void NDR_InitializeRegexStateWrapper(NDR_RegexStateWrapper* regexStateWrapper){
    regexStateWrapper->numStates = 0;
    regexStateWrapper->memoryAllocated = 50;
    regexStateWrapper->regexStates = malloc(sizeof(NDR_RegexState*) * regexStateWrapper->memoryAllocated);
}


void NDR_FreeRegexStateWrapper(NDR_RegexStateWrapper* regexStateWrapper){

    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        NDR_FreeRegexState(regexStateWrapper->regexStates[x]);
        free(regexStateWrapper->regexStates[x]);
    }
    free(regexStateWrapper->regexStates);
}

void NDR_AddRegexState(NDR_RegexStateWrapper* regexStateWrapper){
    if(regexStateWrapper->numStates > regexStateWrapper->memoryAllocated - 5){
        regexStateWrapper->memoryAllocated = regexStateWrapper->memoryAllocated * 2;
        regexStateWrapper->regexStates = realloc(regexStateWrapper->regexStates, sizeof(NDR_RegexState*) * regexStateWrapper->memoryAllocated);
    }
    regexStateWrapper->regexStates[regexStateWrapper->numStates] = malloc(sizeof(NDR_RegexState));
    NDR_InitializeRegexState(regexStateWrapper->regexStates[regexStateWrapper->numStates]);
    regexStateWrapper->numStates++;
}


int NDR_CheckAndAddStateRegex(NDR_RegexStateWrapper* regexStateWrapper, NDR_StateCategories state, char* regexString){

    if(state == STARTSTATE){
        if(NDR_FindStartRegex(regexStateWrapper, regexString) != -1)
            return -1;
        if(NDR_AddStartRegex(NDR_RSGetLastRegexState(regexStateWrapper), regexString) != 0)
            return -2;
    }
    else if(state == ALLOWSTATE){
        if(NDR_CheckAllowTableDuplicate(regexStateWrapper, NDR_RSGetLastRegexState(regexStateWrapper)->keyword, regexString) != -1)
            return -1;
        if(NDR_AddAllowRegex(NDR_RSGetLastRegexState(regexStateWrapper), regexString) != 0)
            return -2;
    }
    else if(state == ESCAPESTATE){
        if(NDR_CheckEscapeTableDuplicate(regexStateWrapper, NDR_RSGetLastRegexState(regexStateWrapper)->keyword, regexString) != -1)
            return -1;
        if(NDR_AddEscapeRegex(NDR_RSGetLastRegexState(regexStateWrapper), regexString) != 0)
            return -2;
    }
    else if(state == ENDSTATE){
        if(NDR_CheckEndTableDuplicate(regexStateWrapper, NDR_RSGetLastRegexState(regexStateWrapper)->keyword, regexString) != -1)
            return -1;
        if(NDR_AddEndRegex(NDR_RSGetLastRegexState(regexStateWrapper), regexString) != 0)
            return -2;
    }
    else{
        if(NDR_FindStartRegex(regexStateWrapper, regexString) != -1)
            return -1;
        if(NDR_AddStartRegex(NDR_RSGetLastRegexState(regexStateWrapper), regexString) != 0)
            return -2;
    }

    return 0;
}

int NDR_FindStartRegex(NDR_RegexStateWrapper* regexStateWrapper, char* regexString){
    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        for(size_t i = 0; i < NDR_RSGetRegexState(regexStateWrapper, x)->numStartStates; i++){
            if(strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->startRegex[i], regexString) == 0){
                return x;
            }
        }
    }
    return -1;
}

int NDR_FindAllowRegex(NDR_RegexStateWrapper* regexStateWrapper, char* regexString){
    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        for(size_t i = 0; i < NDR_RSGetRegexState(regexStateWrapper, x)->numAllowStates; i++){
            if(strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->allowRegex[i], regexString) == 0){
                return x;
            }
        }
    }
    return -1;
}

int NDR_FindEscapeRegex(NDR_RegexStateWrapper* regexStateWrapper, char* regexString){
    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        for(size_t i = 0; i < NDR_RSGetRegexState(regexStateWrapper, x)->numEscapeStates; i++){
            if(strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->escapeRegex[i], regexString) == 0){
                return x;
            }
        }
    }
    return -1;
}

int NDR_FindEndRegex(NDR_RegexStateWrapper* regexStateWrapper, char* regexString){
    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        for(size_t i = 0; i < NDR_RSGetRegexState(regexStateWrapper, x)->numEndStates; i++){
            if(strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->endRegex[i], regexString) == 0){
                return x;
            }
        }
    }
    return -1;
}

int NDR_CheckStartTableDuplicate(NDR_RegexStateWrapper* regexStateWrapper, char* keyword, char* regexString){
    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        for(size_t i = 0; i < NDR_RSGetRegexState(regexStateWrapper, x)->numStartStates; i++){
            if(strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->keyword, keyword) == 0 && strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->startRegex[i], regexString) == 0){
                return x;
            }
        }
    }
    return -1;
}
int NDR_CheckAllowTableDuplicate(NDR_RegexStateWrapper* regexStateWrapper, char* keyword, char* regexString){
    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        for(size_t i = 0; i < NDR_RSGetRegexState(regexStateWrapper, x)->numAllowStates; i++){
            if(strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->keyword, keyword) == 0 && strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->allowRegex[i], regexString) == 0){
                return x;
            }
        }
    }
    return -1;
}
int NDR_CheckEscapeTableDuplicate(NDR_RegexStateWrapper* regexStateWrapper, char* keyword, char* regexString){
    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        for(size_t i = 0; i < NDR_RSGetRegexState(regexStateWrapper, x)->numEscapeStates; i++){
            if(strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->keyword, keyword) == 0 && strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->escapeRegex[i], regexString) == 0){
                return x;
            }
        }
    }
    return -1;
}
int NDR_CheckEndTableDuplicate(NDR_RegexStateWrapper* regexStateWrapper, char* keyword, char* regexString){
    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        for(size_t i = 0; i < NDR_RSGetRegexState(regexStateWrapper, x)->numEndStates; i++){
            if(strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->keyword, keyword) == 0 && strcmp(NDR_RSGetRegexState(regexStateWrapper, x)->endRegex[i], regexString) == 0){
                return x;
            }
        }
    }
    return -1;
}


void NDR_RSSetKeyword(NDR_RegexState* regexState, char* keyword){
    regexState->keyword = realloc(regexState->keyword, strlen(keyword)+1);
    strcpy(regexState->keyword, keyword);
}
void NDR_RSTrimAndSetKeyword(NDR_RegexState* regexState, char* keyword){
    regexState->keyword = realloc(regexState->keyword, strlen(keyword)+1);
    strcpy(regexState->keyword, keyword);

    char* temp = malloc(strlen(keyword)+1);
    strcpy(temp, keyword);
    size_t pointerMove;
    for(pointerMove = 0; pointerMove < strlen(temp); pointerMove++){
        if(temp[pointerMove] == '{'){
            temp = temp + pointerMove + 1;
            break;
        }
    }
    temp[strlen(temp) - 1] = '\0';

    strcpy(regexState->keyword, temp);
    free(temp - (pointerMove + 1));
}
void NDR_RSSetStateFlag(NDR_RegexState* regexState, bool isState){
    regexState->isState = isState;
}
void NDR_RSSetLiteralFlag(NDR_RegexState* regexState, bool isLiteral){
    regexState->isLiteral = isLiteral;
}
void NDR_RSSetCategory(NDR_RegexState* regexState, NDR_StateCategories category){
    regexState->category = category;
}

char* NDR_RSGetKeyword(NDR_RegexState* regexState){
    return regexState->keyword;
}
bool NDR_RSGetStateFlag(NDR_RegexState* regexState){
    return regexState->isState;
}
bool NDR_RSGetLiteralFlag(NDR_RegexState* regexState){
    return regexState->isLiteral;
}
NDR_StateCategories NDR_RSGetCategory(NDR_RegexState* regexState){
    return regexState->category;
}


size_t NDR_RSGetNumStartStates(NDR_RegexState* regexState){
    return regexState->numStartStates;
}
size_t NDR_RSGetNumAllowStates(NDR_RegexState* regexState){
    return regexState->numAllowStates;
}
size_t NDR_RSGetNumEscapeStates(NDR_RegexState* regexState){
    return regexState->numEscapeStates;
}
size_t NDR_RSGetNumEndStates(NDR_RegexState* regexState){
    return regexState->numEndStates;
}

char* NDR_RSGetStartRegex(NDR_RegexState* regexState, int index){
    return regexState->startRegex[index];
}
char* NDR_RSGetAllowRegex(NDR_RegexState* regexState, int index){
    return regexState->allowRegex[index];
}
char* NDR_RSGetEscapeRegex(NDR_RegexState* regexState, int index){
    return regexState->escapeRegex[index];
}
char* NDR_RSGetEndRegex(NDR_RegexState* regexState, int index){
    return regexState->endRegex[index];
}

NDR_RegexState** NDR_RSGetRegexStates(NDR_RegexStateWrapper* regexStateWrapper){
    return regexStateWrapper->regexStates;
}

NDR_RegexState* NDR_RSGetRegexState(NDR_RegexStateWrapper* regexStateWrapper, int index){
    return regexStateWrapper->regexStates[index];
}

NDR_RegexState* NDR_RSGetLastRegexState(NDR_RegexStateWrapper* regexStateWrapper){
    return regexStateWrapper->regexStates[regexStateWrapper->numStates - 1];
}

size_t NDR_RSGetNumberOfStates(NDR_RegexStateWrapper* regexStateWrapper){
    return regexStateWrapper->numStates;
}

