
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
