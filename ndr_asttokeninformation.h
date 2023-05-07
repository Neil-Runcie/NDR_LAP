#ifndef TREETOKENINFORMATION_H
#define TREETOKENINFORMATION_H

#include "ndr_tokeninformation.h"

//typedef struct NDR_TreeTokenInfo NDR_TreeTokenInfo;

//typedef struct NDR_TreeTokenInfoWrapper NDR_TreeTokenInfoWrapper;

typedef struct NDR_TreeTokenInfo{
    size_t nodeNumber;
    NDR_TokenInformation* tokenInfo;
} NDR_TreeTokenInfo;

typedef struct NDR_TreeTokenInfoWrapper {
    size_t numTokens;
    size_t memoryAllocated;
    NDR_TreeTokenInfo** tokens;
} NDR_TreeTokenInfoWrapper;

void NDR_InitTreeTokenInfoWrapper(NDR_TreeTokenInfoWrapper* tokenInfoWrapper);
void NDR_AddTreeNewToken(NDR_TreeTokenInfoWrapper* tokenInfoWrapper);
void NDR_SetTreeTokenInfoKeyword(NDR_TreeTokenInfo* tokenInformation, char* keyword);
void NDR_SetTreeTokenInfoToken(NDR_TreeTokenInfo* tokenInformation, char* token);
void NDR_SetTreeTokenInfoLine(NDR_TreeTokenInfo* tokenInformation, size_t lineNumber);
void NDR_SetTreeTokenInfoColumn(NDR_TreeTokenInfo* tokenInformation, size_t columnNumber);
void NDR_SetTreeTokenInfoNodeNumber(NDR_TreeTokenInfo* tokenInformation, size_t nodeNumber);

char* NDR_GetTreeTokenInfoKeyword(NDR_TreeTokenInfo* tokenInformation);
char* NDR_GetTreeTokenInfoToken(NDR_TreeTokenInfo* tokenInformation);
size_t NDR_GetTreeTokenInfoLine(NDR_TreeTokenInfo* tokenInformation);
size_t NDR_GetTreeTokenInfoColumn(NDR_TreeTokenInfo* tokenInformation);
size_t NDR_GetTreeTokenInfoNodeNumber(NDR_TreeTokenInfo* tokenInformation);

NDR_TreeTokenInfo* NDR_GetTreeTokenInfo(NDR_TreeTokenInfoWrapper* tokenInfo, size_t index);
NDR_TreeTokenInfo* NDR_GetLastTreeTokenInfo(NDR_TreeTokenInfoWrapper* tokenInfo);
size_t NDR_GetNumberOfTreeTokens(NDR_TreeTokenInfoWrapper* tokenInfoWrapper);


#endif
