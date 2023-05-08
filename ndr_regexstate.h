
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

#ifndef REGEXSTATE_H
#define REGEXSTATE_H

#include "ndr_statecategories.h"

#define PCRE2_STATIC
#define PCRE2_CODE_UNIT_WIDTH 8
#include "regex_engines/include/pcre2.h"

//typedef struct NDR_RegexState NDR_RegexState;

//typedef struct NDR_RegexStateWrapper NDR_RegexStateWrapper;

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
    pcre2_code** compiledStartRegex;
    pcre2_code** compiledAllowRegex;
    pcre2_code** compiledEscapeRegex;
    pcre2_code** compiledEndRegex;
    pcre2_match_data** matchStartData;
    pcre2_match_data** matchAllowData;
    pcre2_match_data** matchEscapeData;
    pcre2_match_data** matchEndData;
} NDR_RegexState;

typedef struct NDR_RegexStateWrapper {
    size_t numStates;
    size_t memoryAllocated;
    NDR_RegexState** regexStates;
} NDR_RegexStateWrapper;


void NDR_InitializeRegexStateWrapper(NDR_RegexStateWrapper* regexStateWrapper);
void NDR_InitializeRegexState(NDR_RegexState* state);
void NDR_FreeRegexStateWrapper(NDR_RegexStateWrapper* regexStateWrapper);
void NDR_FreeRegexState(NDR_RegexState* state);

void NDR_AddRegexState(NDR_RegexStateWrapper* regexStateWrapper);

int NDR_CheckAndAddStateRegex(NDR_RegexStateWrapper* regexState, NDR_StateCategories state, char* regexString);
int NDR_AddStartRegex(NDR_RegexState* state, char* regexString);
int NDR_AddAllowRegex(NDR_RegexState* state, char* regexString);
int NDR_AddEscapeRegex(NDR_RegexState* state, char* regexString);
int NDR_AddEndRegex(NDR_RegexState* state, char* regexString);
int NDR_CompileStateRegex(pcre2_code** regexTable, pcre2_match_data** matchDataTable, int stateIndex, char* regexString);

int NDR_FindStartRegex(NDR_RegexStateWrapper* regexStateWrapper, char* regexString);
int NDR_FindAllowRegex(NDR_RegexStateWrapper* regexStateWrapper, char* regexString);
int NDR_FindEscapeRegex(NDR_RegexStateWrapper* regexStateWrapper, char* regexString);
int NDR_FindEndRegex(NDR_RegexStateWrapper* regexStateWrapper, char* regexString);
int NDR_CheckStartTableDuplicate(NDR_RegexStateWrapper* regexStateWrapper, char* keyword, char* regexString);
int NDR_CheckAllowTableDuplicate(NDR_RegexStateWrapper* regexStateWrapper, char* keyword, char* regexString);
int NDR_CheckEscapeTableDuplicate(NDR_RegexStateWrapper* regexStateWrapper, char* keyword, char* regexString);
int NDR_CheckEndTableDuplicate(NDR_RegexStateWrapper* regexStateWrapper, char* keyword, char* regexString);

void NDR_RSSetKeyword(NDR_RegexState* regexState, char* keyword);
void NDR_RSTrimAndSetKeyword(NDR_RegexState* regexState, char* keyword);
void NDR_RSSetStateFlag(NDR_RegexState* regexState, bool isState);
void NDR_RSSetLiteralFlag(NDR_RegexState* regexState, bool literal);
void NDR_RSSetCategory(NDR_RegexState* regexState, NDR_StateCategories category);

char* NDR_RSGetKeyword(NDR_RegexState* regexState);
bool NDR_RSGetStateFlag(NDR_RegexState* regexState);
bool NDR_RSGetLiteralFlag(NDR_RegexState* regexState);
NDR_StateCategories NDR_RSGetCategory(NDR_RegexState* regexState);

size_t NDR_RSGetNumStartStates(NDR_RegexState* regexState);
size_t NDR_RSGetNumAllowStates(NDR_RegexState* regexState);
size_t NDR_RSGetNumEscapeStates(NDR_RegexState* regexState);
size_t NDR_RSGetNumEndStates(NDR_RegexState* regexState);

char* NDR_RSGetStartRegex(NDR_RegexState* regexState, int index);
char* NDR_RSGetAllowRegex(NDR_RegexState* regexState, int index);
char* NDR_RSGetEscapeRegex(NDR_RegexState* regexState, int index);
char* NDR_RSGetEndRegex(NDR_RegexState* regexState, int index);

int NDR_RSGetMatchResult(NDR_RegexState* regexState, char* token, NDR_StateCategories category, int regIndex);

NDR_RegexState** NDR_RSGetRegexStates(NDR_RegexStateWrapper* regexStateWrapper);
NDR_RegexState* NDR_RSGetRegexState(NDR_RegexStateWrapper* regexStateWrapper, int index);
NDR_RegexState* NDR_RSGetLastRegexState(NDR_RegexStateWrapper* regexStateWrapper);
size_t NDR_RSGetNumberOfStates(NDR_RegexStateWrapper* regexStateWrapper);

#endif
