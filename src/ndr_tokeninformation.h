
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
