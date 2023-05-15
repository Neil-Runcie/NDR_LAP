
/*********************************************************************************
*                               NDR Token Information                            *
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
#include "ndr_tokeninformation.h"

void NDR_InitTokenInfoWrapper(NDR_TokenInformationWrapper* tokenInfoWrapper){
    tokenInfoWrapper->numTokens = 0;
    tokenInfoWrapper->memoryAllocated = 50;
    tokenInfoWrapper->tokens = malloc(sizeof(NDR_TokenInformation*) * tokenInfoWrapper->memoryAllocated);
}

void NDR_FreeTokenInfoWrapper(NDR_TokenInformationWrapper* tokenInfoWrapper){
    for(size_t x = 0; x < tokenInfoWrapper->numTokens; x++){
        NDR_FreeTokenInfo(tokenInfoWrapper->tokens[x]);
        free(tokenInfoWrapper->tokens[x]);
    }
    free(tokenInfoWrapper->tokens);
}
void NDR_FreeTokenInfo(NDR_TokenInformation* tokenInformation){
    free(tokenInformation->token);
    free(tokenInformation->keyword);
}

void NDR_AddNewToken(NDR_TokenInformationWrapper* tokenInfoWrapper){
    if(tokenInfoWrapper->numTokens > tokenInfoWrapper->memoryAllocated - 5){
        tokenInfoWrapper->memoryAllocated = tokenInfoWrapper->memoryAllocated * 2;
        tokenInfoWrapper->tokens = realloc(tokenInfoWrapper->tokens, sizeof(NDR_TokenInformation*) * tokenInfoWrapper->memoryAllocated);
    }
    tokenInfoWrapper->tokens[tokenInfoWrapper->numTokens] = malloc(sizeof(NDR_TokenInformation));
    tokenInfoWrapper->tokens[tokenInfoWrapper->numTokens]->keyword = malloc(1);
    tokenInfoWrapper->tokens[tokenInfoWrapper->numTokens]->token = malloc(1);
    tokenInfoWrapper->numTokens++;
}

void NDR_SetTokenInfoKeyword(NDR_TokenInformation* tokenInformation, char* keyword){
    tokenInformation->keyword = realloc(tokenInformation->keyword, strlen(keyword)+1);
    strcpy(tokenInformation->keyword, keyword);
}
void NDR_SetTokenInfoToken(NDR_TokenInformation* tokenInformation, char* token){
    tokenInformation->token = realloc(tokenInformation->token, strlen(token)+1);
    strcpy(tokenInformation->token, token);
}
void NDR_SetTokenInfoLine(NDR_TokenInformation* tokenInformation, size_t lineNumber){
    tokenInformation->lineNumber = lineNumber;
}
void NDR_SetTokenInfoColumn(NDR_TokenInformation* tokenInformation, size_t columnNumber){
    tokenInformation->columnNumber = columnNumber;
}

char* NDR_GetTokenInfoKeyword(NDR_TokenInformation* tokenInformation){
    return tokenInformation->keyword;
}
char* NDR_GetTokenInfoToken(NDR_TokenInformation* tokenInformation){
    return tokenInformation->token;
}
size_t NDR_GetTokenInfoLine(NDR_TokenInformation* tokenInformation){
    return tokenInformation->lineNumber;
}
size_t NDR_GetTokenInfoColumn(NDR_TokenInformation* tokenInformation){
    return tokenInformation->columnNumber;
}

NDR_TokenInformation* NDR_TIGetTokenInfo(NDR_TokenInformationWrapper* tokenInfo, size_t index){
    return tokenInfo->tokens[index];
}
NDR_TokenInformation* NDR_TIGetLastTokenInfo(NDR_TokenInformationWrapper* tokenInfo){
    return tokenInfo->tokens[tokenInfo->numTokens-1];
}
size_t NDR_TIGetNumberOfTokens(NDR_TokenInformationWrapper* tokenInfoWrapper){
    return tokenInfoWrapper->numTokens;
}


