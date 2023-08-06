
/*********************************************************************************
*                                  NDR Regex Node                                *
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

#ifndef NDRREGEXNODE_H
#define NDRREGEXNODE_H

typedef struct NDR_RegexNode {
    bool start;
    bool end;
    bool optionalPath;
    bool repeatPath;
    bool orPath;
    bool wordStart;
    bool wordEnd;
    int minMatches;
    int maxMatches;

    struct NDR_RegexNode* wordReference;

    bool allButNewLine;
    bool everything;
    bool nothing;
    bool decimalDigit;
    bool notDecimalDigit;
    bool whiteSpace;
    bool notWhiteSpace;
    bool wordChar;
    bool notWordChar;

    bool negatedClass;

    size_t numberOfChars;
    size_t memoryAllocated;
    char* acceptChars;

    size_t numberOfChildren;
    size_t memoryAllocatedChildren;
    struct NDR_RegexNode** children;

} NDR_RegexNode;

typedef struct NDR_RNodeStack{
    NDR_RegexNode** nodes;
    size_t memoryAllocated;
    size_t numNodes;
} NDR_RNodeStack;


void ContinueAfterWord(NDR_RNodeStack* startStack, NDR_RNodeStack* endStack);
int HandleSpecialCharacters(NDR_RegexNode* node, char comp);
bool IsCharacterAccepted(NDR_RegexNode* node, char comp);

void NDR_InitRegexNode(NDR_RegexNode* node);
bool NDR_IsRNodeEmpty(NDR_RegexNode* node);
bool NDR_IsRNodeSetForComparison(NDR_RegexNode* node);
void NDR_AddRNodeChar(NDR_RegexNode* node, char character);
void NDR_AddRNodeChild(NDR_RegexNode* node, NDR_RegexNode* child);
bool NDR_RNodeDuplicate(NDR_RegexNode* address, NDR_RegexNode** nodes, size_t index);

void NDR_InitRNodeStack(NDR_RNodeStack* ndrstack);
size_t NDR_RNodeStackSize(NDR_RNodeStack* ndrstack);
bool NDR_RNodeStackIsEmpty(NDR_RNodeStack* ndrstack);
void NDR_RNodeStackPush(NDR_RNodeStack* ndrstack, NDR_RegexNode* node);
NDR_RegexNode* NDR_RNodeStackPeek(NDR_RNodeStack* ndrstack);
NDR_RegexNode* NDR_RNodeStackPop(NDR_RNodeStack* ndrstack);
void NDR_RNodeStackSet(NDR_RNodeStack* ndrstack, NDR_RegexNode* node);

void NDR_DestroyRegexNode(NDR_RegexNode* node);
void NDR_DestroyRegexStack(NDR_RNodeStack* stack);
void NDR_RemoveRNodeChild(NDR_RegexNode* node);

#endif
