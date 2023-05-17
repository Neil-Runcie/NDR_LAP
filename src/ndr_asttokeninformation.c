
/*********************************************************************************
*                            NDR AST Token Information                           *
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
#include "ndr_asttokeninformation.h"



void NDR_InitTreeTokenInfoWrapper(NDR_TreeTokenInfoWrapper* tokenInfoWrapper){
    tokenInfoWrapper->numTokens = 0;
    tokenInfoWrapper->memoryAllocated = 50;
    tokenInfoWrapper->tokens = malloc(sizeof(NDR_TreeTokenInfo*) * tokenInfoWrapper->memoryAllocated);
}

void NDR_AddTreeNewToken(NDR_TreeTokenInfoWrapper* tokenInfoWrapper){
    if(tokenInfoWrapper->numTokens > tokenInfoWrapper->memoryAllocated - 5){
        tokenInfoWrapper->memoryAllocated = tokenInfoWrapper->memoryAllocated * 2;
        tokenInfoWrapper->tokens = realloc(tokenInfoWrapper->tokens, sizeof(NDR_TreeTokenInfo*) * tokenInfoWrapper->memoryAllocated);
    }
    tokenInfoWrapper->tokens[tokenInfoWrapper->numTokens] = malloc(sizeof(NDR_TreeTokenInfo));
    tokenInfoWrapper->tokens[tokenInfoWrapper->numTokens]->nodeNumber = -1;
    tokenInfoWrapper->tokens[tokenInfoWrapper->numTokens]->tokenInfo = malloc(sizeof(NDR_TokenInformation));
    tokenInfoWrapper->tokens[tokenInfoWrapper->numTokens]->tokenInfo->keyword = malloc(1);
    tokenInfoWrapper->tokens[tokenInfoWrapper->numTokens]->tokenInfo->token = malloc(1);
    tokenInfoWrapper->numTokens++;
}

void NDR_SetTreeTokenInfoKeyword(NDR_TreeTokenInfo* tokenInformation, char* keyword){
    NDR_SetTokenInfoKeyword(tokenInformation->tokenInfo, keyword);
}
void NDR_SetTreeTokenInfoToken(NDR_TreeTokenInfo* tokenInformation, char* token){
    NDR_SetTokenInfoToken(tokenInformation->tokenInfo, token);
}
void NDR_SetTreeTokenInfoLine(NDR_TreeTokenInfo* tokenInformation, size_t lineNumber){
    NDR_SetTokenInfoLine(tokenInformation->tokenInfo, lineNumber);
}
void NDR_SetTreeTokenInfoColumn(NDR_TreeTokenInfo* tokenInformation, size_t columnNumber){
    NDR_SetTokenInfoColumn(tokenInformation->tokenInfo, columnNumber);
}
void NDR_SetTreeTokenInfoNodeNumber(NDR_TreeTokenInfo* tokenInformation, long nodeNumber){
    tokenInformation->nodeNumber = nodeNumber;
}

char* NDR_GetTreeTokenInfoKeyword(NDR_TreeTokenInfo* tokenInformation){
    return NDR_GetTokenInfoKeyword(tokenInformation->tokenInfo);
}
char* NDR_GetTreeTokenInfoToken(NDR_TreeTokenInfo* tokenInformation){
    return NDR_GetTokenInfoToken(tokenInformation->tokenInfo);
}
size_t NDR_GetTreeTokenInfoLine(NDR_TreeTokenInfo* tokenInformation){
    return NDR_GetTokenInfoLine(tokenInformation->tokenInfo);
}
size_t NDR_GetTreeTokenInfoColumn(NDR_TreeTokenInfo* tokenInformation){
    return NDR_GetTokenInfoColumn(tokenInformation->tokenInfo);
}
long NDR_GetTreeTokenInfoNodeNumber(NDR_TreeTokenInfo* tokenInformation){
    return tokenInformation->nodeNumber;
}


NDR_TreeTokenInfo* NDR_GetTreeTokenInfo(NDR_TreeTokenInfoWrapper* tokenInfo, size_t index){
    return tokenInfo->tokens[index];
}
NDR_TreeTokenInfo* NDR_GetLastTreeTokenInfo(NDR_TreeTokenInfoWrapper* tokenInfo){
    return tokenInfo->tokens[tokenInfo->numTokens-1];
}

size_t NDR_GetNumberOfTreeTokens(NDR_TreeTokenInfoWrapper* tokenInfoWrapper){
    return tokenInfoWrapper->numTokens;
}
