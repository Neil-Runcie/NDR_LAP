#ifndef TOKENINFORMATION_H
#define TOKENINFORMATION_H

typedef struct NDR_TokenInformation {
    char* token;
    char* keyword;
    size_t lineNumber;
    size_t columnNumber;
} NDR_TokenInformation;

typedef struct NDR_TokenInformationWrapper {
    size_t numTokens;
    size_t memoryAllocated;
    NDR_TokenInformation** tokens;
} NDR_TokenInformationWrapper;

void NDR_InitTokenInfoWrapper(NDR_TokenInformationWrapper* tokenInfoWrapper);
void NDR_FreeTokenInfoWrapper(NDR_TokenInformationWrapper* tokenInfoWrapper);
void NDR_FreeTokenInfo(NDR_TokenInformation* tokenInformation);

void NDR_AddNewToken(NDR_TokenInformationWrapper* tokenInfoWrapper);
NDR_TokenInformation* NDR_GetTokenInfo(NDR_TokenInformationWrapper* tokenInfo, size_t index);
NDR_TokenInformation* NDR_GetLastTokenInfo(NDR_TokenInformationWrapper* tokenInfo);

void NDR_SetTokenInfoKeyword(NDR_TokenInformation* tokenInformation, char* keyword);
void NDR_SetTokenInfoToken(NDR_TokenInformation* tokenInformation, char* token);
void NDR_SetTokenInfoLine(NDR_TokenInformation* tokenInformation, size_t lineNumber);
void NDR_SetTokenInfoColumn(NDR_TokenInformation* tokenInformation, size_t columnNumber);

char* NDR_GetTokenInfoKeyword(NDR_TokenInformation* tokenInformation);
char* NDR_GetTokenInfoToken(NDR_TokenInformation* tokenInformation);
size_t NDR_GetTokenInfoLine(NDR_TokenInformation* tokenInformation);
size_t NDR_GetTokenInfoColumn(NDR_TokenInformation* tokenInformation);

NDR_TokenInformation* NDR_TIGetTokenInfo(NDR_TokenInformationWrapper* tokenInfo, size_t index);
NDR_TokenInformation* NDR_TIGetLastTokenInfo(NDR_TokenInformationWrapper* tokenInfo);
size_t NDR_TIGetNumberOfTokens(NDR_TokenInformationWrapper* tokenInfoWrapper);

#endif
