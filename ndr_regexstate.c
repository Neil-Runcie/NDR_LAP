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

void NDR_InitializeRegexState(NDR_RegexState* state){
    state->keyword = malloc(1);
    state->isState = false;
    state->isLiteral = false;
    state->category = ACCEPT;
    state->numStartStates = 0;
    state->numAllowStates = 0;
    state->numEscapeStates = 0;
    state->numEndStates = 0;
    state->startRegex = malloc(sizeof(char*));
    state->allowRegex = malloc(sizeof(char*));
    state->escapeRegex = malloc(sizeof(char*));
    state->endRegex = malloc(sizeof(char*));
    state->compiledStartRegex = malloc(sizeof(pcre2_code*));
    state->compiledAllowRegex = malloc(sizeof(pcre2_code*));
    state->compiledEscapeRegex = malloc(sizeof(pcre2_code*));
    state->compiledEndRegex = malloc(sizeof(pcre2_code*));
    state->matchStartData = malloc(sizeof(pcre2_match_data*));
    state->matchAllowData = malloc(sizeof(pcre2_match_data*));
    state->matchEscapeData = malloc(sizeof(pcre2_match_data*));
    state->matchEndData = malloc(sizeof(pcre2_match_data*));
}

void NDR_FreeRegexStateWrapper(NDR_RegexStateWrapper* regexStateWrapper){

    for(size_t x = 0; x < regexStateWrapper->numStates; x++){
        NDR_FreeRegexState(regexStateWrapper->regexStates[x]);
        free(regexStateWrapper->regexStates[x]);
    }
    free(regexStateWrapper->regexStates);
}

void NDR_FreeRegexState(NDR_RegexState* state){

    free(state->keyword);

    for(size_t x = 0; x < state->numStartStates; x++){
        free(state->startRegex[x]);
        pcre2_match_data_free(state->matchStartData[x]);              // Release memory used for the match
        pcre2_code_free(state->compiledStartRegex[x]);                // data and the compiled pattern.
    }
    for(size_t x = 0; x < state->numAllowStates; x++){
        free(state->allowRegex[x]);
        pcre2_match_data_free(state->matchAllowData[x]);              // Release memory used for the match
        pcre2_code_free(state->compiledAllowRegex[x]);                // data and the compiled pattern.
    }
    for(size_t x = 0; x < state->numEscapeStates; x++){
        free(state->escapeRegex[x]);
        pcre2_match_data_free(state->matchEscapeData[x]);              // Release memory used for the match
        pcre2_code_free(state->compiledEscapeRegex[x]);                // data and the compiled pattern.
    }
    for(size_t x = 0; x < state->numEndStates; x++){
        free(state->endRegex[x]);
        pcre2_match_data_free(state->matchEndData[x]);              // Release memory used for the match
        pcre2_code_free(state->compiledEndRegex[x]);                // data and the compiled pattern.
    }
    free(state->startRegex);
    free(state->allowRegex);
    free(state->escapeRegex);
    free(state->endRegex);
    free(state->compiledStartRegex);
    free(state->compiledAllowRegex);
    free(state->compiledEscapeRegex);
    free(state->compiledEndRegex);
    free(state->matchStartData);
    free(state->matchAllowData);
    free(state->matchEscapeData);
    free(state->matchEndData);
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

int NDR_AddStartRegex(NDR_RegexState* state, char* regex){

    state->startRegex = realloc(state->startRegex, sizeof(char*) * (state->numStartStates + 2));
    state->startRegex[state->numStartStates] = malloc(strlen(regex) + 1);
    strcpy(state->startRegex[state->numStartStates], regex);

    state->compiledStartRegex = realloc(state->compiledStartRegex, sizeof(pcre2_code*) * (state->numStartStates + 2));
    state->matchStartData = realloc(state->matchStartData, sizeof(pcre2_match_data*) * (state->numStartStates + 2));
    int result = NDR_CompileStateRegex(state->compiledStartRegex, state->matchStartData, state->numStartStates, regex);

    state->numStartStates++;

    return result;
}

int NDR_AddAllowRegex(NDR_RegexState* state, char* regex){

    state->allowRegex = realloc(state->allowRegex, sizeof(char*) * (state->numAllowStates + 2));
    state->allowRegex[state->numAllowStates] = malloc(strlen(regex) + 1);
    strcpy(state->allowRegex[state->numAllowStates], regex);

    state->compiledAllowRegex = realloc(state->compiledAllowRegex, sizeof(pcre2_code*) * (state->numAllowStates + 2));
    state->matchAllowData = realloc(state->matchAllowData, sizeof(pcre2_match_data*) * (state->numAllowStates + 2));
    int result = NDR_CompileStateRegex(state->compiledAllowRegex, state->matchAllowData, state->numAllowStates, regex);

    state->numAllowStates++;

    return result;
}

int NDR_AddEscapeRegex(NDR_RegexState* state, char* regex){

    state->escapeRegex = realloc(state->escapeRegex, sizeof(char*) * (state->numEscapeStates + 2));
    state->escapeRegex[state->numEscapeStates] = malloc(strlen(regex) + 1);
    strcpy(state->escapeRegex[state->numEscapeStates], regex);

    state->compiledEscapeRegex = realloc(state->compiledEscapeRegex, sizeof(pcre2_code*) * (state->numEscapeStates + 2));
    state->matchEscapeData = realloc(state->matchEscapeData, sizeof(pcre2_match_data*) * (state->numEscapeStates + 2));
    int result = NDR_CompileStateRegex(state->compiledEscapeRegex, state->matchEscapeData, state->numEscapeStates, regex);

    state->numEscapeStates++;

    return result;
}

int NDR_AddEndRegex(NDR_RegexState* state, char* regex){

    state->endRegex = realloc(state->endRegex, sizeof(char*) * (state->numEndStates + 2));
    state->endRegex[state->numEndStates] = malloc(strlen(regex) + 1);
    strcpy(state->endRegex[state->numEndStates], regex);

    state->compiledEndRegex = realloc(state->compiledEndRegex, sizeof(pcre2_code*) * (state->numEndStates + 2));
    state->matchEndData = realloc(state->matchEndData, sizeof(pcre2_match_data*) * (state->numEndStates + 2));
    int result = NDR_CompileStateRegex(state->compiledEndRegex, state->matchEndData, state->numEndStates, regex);

    state->numEndStates++;

    return result;
}

int NDR_CompileStateRegex(pcre2_code** regexTable, pcre2_match_data** matchDataTable, int stateIndex, char* regex){
    // PRCRE2 variables for handling the compilation and error handling of regex
    int errornumber;
    PCRE2_SIZE erroroffset;
    PCRE2_UCHAR buffer[256];

    regexTable[stateIndex] = pcre2_compile((PCRE2_SPTR8)regex, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
    if (regexTable[stateIndex] == NULL)
    {
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      return 1;
    }
    matchDataTable[stateIndex] = pcre2_match_data_create_from_pattern(regexTable[stateIndex], NULL);

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


int NDR_RSGetMatchResult(NDR_RegexState* regexState, char* token, NDR_StateCategories category, int regIndex){
    if(category == STARTSTATE)
        return pcre2_match(regexState->compiledStartRegex[regIndex], (PCRE2_SPTR8)token, strlen(token), 0, PCRE2_PARTIAL_SOFT, regexState->matchStartData[regIndex], NULL);
    else if(category == ALLOWSTATE)
        return pcre2_match(regexState->compiledAllowRegex[regIndex], (PCRE2_SPTR8)token, strlen(token), 0, PCRE2_PARTIAL_SOFT, regexState->matchAllowData[regIndex], NULL);
    else if(category == ESCAPESTATE)
        return pcre2_match(regexState->compiledEscapeRegex[regIndex], (PCRE2_SPTR8)token, strlen(token), 0, PCRE2_PARTIAL_SOFT, regexState->matchEscapeData[regIndex], NULL);
    else if(category == ENDSTATE)
        return pcre2_match(regexState->compiledEndRegex[regIndex], (PCRE2_SPTR8)token, strlen(token), 0, PCRE2_PARTIAL_SOFT, regexState->matchEndData[regIndex], NULL);
    else
        return 404;
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

