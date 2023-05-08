
/*********************************************************************************
*                                 NDR AST NODE                                   *
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

#ifndef ASTNODE_H
#define ASTNODE_H

#include <stdbool.h>

//typedef struct NDR_ASTNode NDR_ASTNode;

//typedef struct NDR_ASTNodeHolder NDR_ASTNodeHolder;

typedef struct NDR_ASTNode{
    char* token;
    char* keyword;
    size_t orderNumber;
    size_t numberOfChildren;
    size_t nodeType;
    size_t lineNumber;
    size_t columnNumber;
    size_t childrenAllocated;
    struct NDR_ASTNode** children;
} NDR_ASTNode;

typedef struct NDR_ASTNodeHolder{
    size_t totalNodesInTree;
    size_t numNodes;
    size_t memoryAllocated;
    NDR_ASTNode** nodes;
} NDR_ASTNodeHolder;

typedef struct NDR_ASTNodeQueue{
    NDR_ASTNode** nodes;
    size_t memoryAllocated;
    size_t numNodes;
} NDR_ASTNodeQueue;

typedef struct NDR_ASTNodeStack{
    NDR_ASTNode** nodes;
    size_t memoryAllocated;
    size_t numNodes;
} NDR_ASTNodeStack;

/*
* \fn NDR_InitASTNodeHolder
* \brief A constructor style function for initializing the FileInformation structure
* \param fileInfo is a NDR_FileInformation* with memory allocated to it
*/
void NDR_InitASTNodeHolder(NDR_ASTNodeHolder* nodeWrapper);
void NDR_InitASTNode(NDR_ASTNode* node);
void NDR_FreeASTNodeHolder(NDR_ASTNodeHolder* nodeWrapper);
void NDR_FreeASTNode(NDR_ASTNode* node);
void NDR_DestroyASTTree(NDR_ASTNode* head);

void NDR_AddNewASTNode(NDR_ASTNodeHolder* nodeWrapper, NDR_ASTNode* nodeToAdd);
void NDR_AddChildASTNode(NDR_ASTNode* node, NDR_ASTNode* nodeToAdd);

void NDR_SetASTNodeKeyword(NDR_ASTNode* node, char* keyword);
void NDR_SetASTNodeToken(NDR_ASTNode* node, char* token);
void NDR_SetASTNodeOrderNumber(NDR_ASTNode* node, size_t orderNumber);
void NDR_SetASTNodeNodeType(NDR_ASTNode* node, size_t nodeType);
void NDR_SetASTNodeLineNumber(NDR_ASTNode* node, size_t lineNumber);
void NDR_SetASTNodeColumnNumber(NDR_ASTNode* node, size_t columnNumber);

void NDR_IncASTTotalNode(NDR_ASTNodeHolder* nodeWrapper);

char* NDR_GetASTNodeKeyword(NDR_ASTNode* node);
char* NDR_GetASTNodeToken(NDR_ASTNode* node);
size_t NDR_GetASTNodeOrderNumber(NDR_ASTNode* node);
size_t NDR_GetASTNodeNodeType(NDR_ASTNode* node);
size_t NDR_GetASTNodeLineNumber(NDR_ASTNode* node);
size_t NDR_GetASTNodeColumnNumber(NDR_ASTNode* node);

NDR_ASTNode* NDR_GetASTNode(NDR_ASTNodeHolder* nodeWrapper, size_t index);
size_t NDR_GetTotalASTTreeNodes(NDR_ASTNodeHolder* nodeWrapper);
size_t NDR_GetNumberOfASTNodes(NDR_ASTNodeHolder* nodeWrapper);



void NDR_InitASTQueue(NDR_ASTNodeQueue* ndrqueue);
void NDR_FreeASTQueue(NDR_ASTNodeQueue* ndrqueue);
size_t NDR_ASTQSize(NDR_ASTNodeQueue* ndrqueue);
bool NDR_ASTQIsEmpty(NDR_ASTNodeQueue* ndrqueue);
NDR_ASTNode* NDR_ASTQFront(NDR_ASTNodeQueue* ndrqueue);
void NDR_ASTQEnqueue(NDR_ASTNodeQueue* ndrqueue, NDR_ASTNode* node);
NDR_ASTNode* NDR_ASTQDequeue(NDR_ASTNodeQueue* ndrqueue);



void NDR_InitASTStack(NDR_ASTNodeStack* ndrstack);
void NDR_FreeASTStack(NDR_ASTNodeStack* ndrstack);
size_t NDR_ASTStackSize(NDR_ASTNodeStack* ndrstack);
bool NDR_ASTStackIsEmpty(NDR_ASTNodeStack* ndrstack);
void NDR_ASTStackPush(NDR_ASTNodeStack* ndrstack, NDR_ASTNode* node);
NDR_ASTNode* NDR_ASTStackPeek(NDR_ASTNodeStack* ndrstack);
NDR_ASTNode* NDR_ASTStackPop(NDR_ASTNodeStack* ndrstack);



NDR_ASTNode* NDR_ASTPreOrderTraversal(NDR_ASTNode* head);
NDR_ASTNode* NDR_ASTInOrderTraversal(NDR_ASTNode* head);
NDR_ASTNode* NDR_ASTPostOrderTraversal(NDR_ASTNode* head);
NDR_ASTNode* NDR_ASTBreadthFirstTraversal(NDR_ASTNode* head);

#endif
