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
void NDR_SetTreeTokenInfoNodeNumber(NDR_TreeTokenInfo* tokenInformation, size_t nodeNumber){
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
size_t NDR_GetTreeTokenInfoNodeNumber(NDR_TreeTokenInfo* tokenInformation){
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
