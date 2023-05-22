
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

/**
* @struct NDR_ASTNode
* @brief A container for a single token that is represented as part an abstract syntax tree
*/
typedef struct NDR_ASTNode{
    char* token;
    char* keyword;
    long orderNumber;
    size_t numberOfChildren;
    size_t nodeType;
    size_t lineNumber;
    size_t columnNumber;
    size_t childrenAllocated;
    struct NDR_ASTNode** children;
} NDR_ASTNode;
/**
* @struct NDR_ASTNodeHolder
* @brief A holder for abstract syntax tree nodes to be used during tree building
*/
typedef struct NDR_ASTNodeHolder{
    size_t totalNodesInTree;
    size_t numNodes;
    size_t memoryAllocated;
    NDR_ASTNode** nodes;
} NDR_ASTNodeHolder;
/**
* @struct NDR_ASTNodeQueue
* @brief A queue structure for traversing an abstract syntax tree
*/
typedef struct NDR_ASTNodeQueue{
    NDR_ASTNode** nodes;
    size_t memoryAllocated;
    size_t numNodes;
} NDR_ASTNodeQueue;
/**
* @struct NDR_ASTNodeStack
* @brief A stack structure for traversing an abstract syntax tree
*/
typedef struct NDR_ASTNodeStack{
    NDR_ASTNode** nodes;
    size_t memoryAllocated;
    size_t numNodes;
} NDR_ASTNodeStack;

/** @brief Initialize an NDR_ASTNodeHolder structure
*
* @param nodeWrapper is a structure that has memory allocated to it
*/
void NDR_InitASTNodeHolder(NDR_ASTNodeHolder* nodeWrapper);
/** @brief Initialize an NDR_ASTNode structure
*
* @param node is a structure that has memory allocated to it
*/
void NDR_InitASTNode(NDR_ASTNode* node);
/** @brief Free the memory of an NDR_ASTNodeHolder structure
*
* @param nodeWrapper is a structure that has memory allocated to it
*/
void NDR_FreeASTNodeHolder(NDR_ASTNodeHolder* nodeWrapper);
/** @brief Free the memory of an NDR_ASTNode structure
*
* @param node is a structure that has memory allocated to it
*/
void NDR_FreeASTNode(NDR_ASTNode* node);
/** @brief Free every node in an abstract syntax tree starting from the provided node
*
* @param node is a structure that is at the head of an abstract syntax tree
*/
void NDR_DestroyASTTree(NDR_ASTNode* head);

/** @brief Add an abstract syntax tree node to an NDR_ASTNodeHolder structure
*
* @param nodeWrapper is a structure that is will hold the newly provided node
* @param nodeToAdd is an abstract syntax tree node to be added to the nodeWrapper
*/
void NDR_AddNewASTNode(NDR_ASTNodeHolder* nodeWrapper, NDR_ASTNode* nodeToAdd);
/** @brief Add an abstract syntax tree node as a child to another abstract syntax tree node
*
* @param node is a structure that is will be the parent of the nodeToAdd
* @param nodeToAdd is an abstract syntax tree node to be added to the node provided as a child
*/
void NDR_AddChildASTNode(NDR_ASTNode* node, NDR_ASTNode* nodeToAdd);

/** @brief Set the keyword string in the abstract syntax tree node
*
* @param node is the structure to be modified
* @param keyword is the string to be used
*/
void NDR_SetASTNodeKeyword(NDR_ASTNode* node, char* keyword);
/** @brief Set the token string in the abstract syntax tree node
*
* @param node is the structure to be modified
* @param token is the string to be used
*/
void NDR_SetASTNodeToken(NDR_ASTNode* node, char* token);
/** @brief Set the orderNumber in the abstract syntax tree node
*
* @param node is the structure to be modified
* @param orderNumber is the number to be used
*/
void NDR_SetASTNodeOrderNumber(NDR_ASTNode* node, long orderNumber);
/** @brief Set the nodeType in the abstract syntax tree node
*
* @param node is the structure to be modified
* @param nodeType is the number to be used
*/
void NDR_SetASTNodeNodeType(NDR_ASTNode* node, size_t nodeType);
/** @brief Set the lineNumber in the abstract syntax tree node
*
* @param node is the structure to be modified
* @param lineNumber is the number to be used
*/
void NDR_SetASTNodeLineNumber(NDR_ASTNode* node, size_t lineNumber);
/** @brief Set the columnNumber in the abstract syntax tree node
*
* @param node is the structure to be modified
* @param columnNumber is the number to be used
*/
void NDR_SetASTNodeColumnNumber(NDR_ASTNode* node, size_t columnNumber);
/** @brief Increment the total number of nodes int in the abstract syntax tree node by 1
*
* @param nodeWrapper is the structure to be modified
*/
void NDR_IncASTTotalNode(NDR_ASTNodeHolder* nodeWrapper);

/** @brief Get the keyword associated with the abstract syntax tree node
*
* @param node is an initialized abstract syntax tree node
* @return the keyword in the abstract syntax tree node
*/
char* NDR_GetASTNodeKeyword(NDR_ASTNode* node);
/** @brief Get the token associated with the abstract syntax tree node
*
* @param node is an initialized abstract syntax tree node
* @return the token in the abstract syntax tree node
*/
char* NDR_GetASTNodeToken(NDR_ASTNode* node);
/** @brief Get the orderNumber associated with the abstract syntax tree node
*
* @param node is an initialized abstract syntax tree node
* @return the orderNumber in the abstract syntax tree node
*/
long NDR_GetASTNodeOrderNumber(NDR_ASTNode* node);
/** @brief Get the nodeType associated with the abstract syntax tree node
*
* @param node is an initialized abstract syntax tree node
* @return the nodeType in the abstract syntax tree node
*/
size_t NDR_GetASTNodeNodeType(NDR_ASTNode* node);
/** @brief Get the lineNumber associated with the abstract syntax tree node
*
* @param node is an initialized abstract syntax tree node
* @return the lineNumber in the abstract syntax tree node
*/
size_t NDR_GetASTNodeLineNumber(NDR_ASTNode* node);
/** @brief Get the columnNumber associated with the abstract syntax tree node
*
* @param node is an initialized abstract syntax tree node
* @return the columnNumber in the abstract syntax tree node
*/
size_t NDR_GetASTNodeColumnNumber(NDR_ASTNode* node);
/** @brief Get the number of children associated with the abstract syntax tree node
*
* @param node is an initialized abstract syntax tree node
* @return the number of children in the abstract syntax tree node
*/
size_t NDR_GetASTNodeNumChildren(NDR_ASTNode* node);
/** @brief Get the child from the node by index
*
* @param node is an initialized abstract syntax tree node
* @param index is the index of the child node to be retrieved
* @return a child node from the provided node specified by the provided index
*/
NDR_ASTNode* NDR_GetASTNodeChild(NDR_ASTNode* node, size_t index);
/** @brief Get a node from the provided nodeWrapper
*
* @param nodeWrapper is an initialized NDR_ASTNodeHolder structure
* @param index an number specifying a node within the nodeWrapper structure
* @return a node from the nodeWrapper specified by the provided index
*/
NDR_ASTNode* NDR_GetASTNode(NDR_ASTNodeHolder* nodeWrapper, size_t index);
/** @brief Get the total number of nodes present in a constructed abstract syntax tree
*
* @param nodeWrapper is an initialized NDR_ASTNodeHolder structure
* @return the total number of nodes present in a constructed abstract syntax tree
*/
size_t NDR_GetTotalASTTreeNodes(NDR_ASTNodeHolder* nodeWrapper);
/** @brief Get the number of nodes present in the provided nodeWrapper
*
* @param nodeWrapper is an initialized NDR_ASTNodeHolder structure
* @return the number of nodes present in the provided nodeWrapper
*/
size_t NDR_GetNumberOfASTNodes(NDR_ASTNodeHolder* nodeWrapper);


/** @brief Initialize an NDR_ASTNodeQueue structure
*
* @param ndrqueue is a structure that has memory allocated to it
*/
void NDR_InitASTQueue(NDR_ASTNodeQueue* ndrqueue);
/** @brief Free the memory of an NDR_ASTNodeQueue structure
*
* @param ndrqueue is a structure that has memory allocated to it
*/
void NDR_FreeASTQueue(NDR_ASTNodeQueue* ndrqueue);
/** @brief Get the current size of the queue structure provided
*
* @param ndrqueue is an initialized NDR_ASTNodeQueue structure
* @return the size of the queue
*/
size_t NDR_ASTQSize(NDR_ASTNodeQueue* ndrqueue);
/** @brief Check if the provided queue is empty
*
* @param ndrqueue is an initialized NDR_ASTNodeQueue structure
* @return true if the queue is empty and false if the queue is not empty
*/
bool NDR_ASTQIsEmpty(NDR_ASTNodeQueue* ndrqueue);
/** @brief Get a reference to the node at the front of the queue without removing it from the queue
*
* @param ndrqueue is an initialized NDR_ASTNodeQueue structure
* @return the node at the front of the queue
*/
NDR_ASTNode* NDR_ASTQFront(NDR_ASTNodeQueue* ndrqueue);
/** @brief Add a new node to the back of the queue
*
* @param ndrqueue is an initialized NDR_ASTNodeQueue structure
* @param node is an initialized NDR_ASTNode structure
*/
void NDR_ASTQEnqueue(NDR_ASTNodeQueue* ndrqueue, NDR_ASTNode* node);
/** @brief Get a reference to the node at the front of the queue and remove it
*
* @param ndrqueue is an initialized NDR_ASTNodeQueue structure
*/
NDR_ASTNode* NDR_ASTQDequeue(NDR_ASTNodeQueue* ndrqueue);


/** @brief Initialize an NDR_ASTNodeStack structure
*
* @param ndrstack is a structure that has memory allocated to it
*/
void NDR_InitASTStack(NDR_ASTNodeStack* ndrstack);
/** @brief Free the memory of an NDR_ASTNodeStack structure
*
* @param ndrstack is a structure that has memory allocated to it
*/
void NDR_FreeASTStack(NDR_ASTNodeStack* ndrstack);
/** @brief Get the current size of the stack structure provided
*
* @param ndrstack is an initialized NDR_ASTNodeStack structure
* @return the size of the stack
*/
size_t NDR_ASTStackSize(NDR_ASTNodeStack* ndrstack);
/** @brief Check if the provided stack is empty
*
* @param ndrstack is an initialized NDR_ASTNodeStack structure
* @return true if the stack is empty and false if the stack is not empty
*/
bool NDR_ASTStackIsEmpty(NDR_ASTNodeStack* ndrstack);
/** @brief Get a reference to the node at the top of the stack without removing it from the stack
*
* @param ndrstack is an initialized NDR_ASTNodeStack structure
* @return the node at the top of the stack
*/
NDR_ASTNode* NDR_ASTStackPeek(NDR_ASTNodeStack* ndrstack);
/** @brief Add a new node to the top of the stack
*
* @param ndrstack is an initialized NDR_ASTNodeStack structure
* @param node is an initialized NDR_ASTNode structure
*/
void NDR_ASTStackPush(NDR_ASTNodeStack* ndrstack, NDR_ASTNode* node);
/** @brief Get a reference to the node at the top of the stack and remove it
*
* @param ndrstack is an initialized NDR_ASTNodeStack structure
*/
NDR_ASTNode* NDR_ASTStackPop(NDR_ASTNodeStack* ndrstack);


/** @brief Perform a preorder traversal on the abstract syntax tree provided
*
* @param head is the node top level abstract syntax node of the abstract syntax tree to be traversed
*/
NDR_ASTNode* NDR_ASTPreOrderTraversal(NDR_ASTNode* head);
/** @brief Perform an inorder traversal on the abstract syntax tree provided
*
* @param head is the node top level abstract syntax node of the abstract syntax tree to be traversed
*/
NDR_ASTNode* NDR_ASTInOrderTraversal(NDR_ASTNode* head);
/** @brief Perform a postorder traversal on the abstract syntax tree provided
*
* @param head is the node top level abstract syntax node of the abstract syntax tree to be traversed
*/
NDR_ASTNode* NDR_ASTPostOrderTraversal(NDR_ASTNode* head);
/** @brief Perform a breadth first traversal on the abstract syntax tree provided
*
* @param head is the node top level abstract syntax node of the abstract syntax tree to be traversed
*/
NDR_ASTNode* NDR_ASTBreadthFirstTraversal(NDR_ASTNode* head);


#endif
