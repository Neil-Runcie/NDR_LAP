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


