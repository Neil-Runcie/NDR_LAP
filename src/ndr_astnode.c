
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "ndr_astnode.h"


static void MovePointersDown(NDR_ASTNodeQueue* ndrqueue);
static bool NDR_ASTDuplicate(NDR_ASTNode* address, NDR_ASTNode** nodes, size_t index);

void NDR_InitASTNodeHolder(NDR_ASTNodeHolder* nodeWrapper){
    nodeWrapper->totalNodesInTree = 0;
    nodeWrapper->numNodes = 0;
    nodeWrapper->memoryAllocated = 50;
    nodeWrapper->nodes = malloc(sizeof(NDR_ASTNode*) * nodeWrapper->memoryAllocated);
}

void NDR_InitASTNode(NDR_ASTNode* node){
    node->token = malloc(1);
    strcpy(node->token, "");
    node->keyword = malloc(1);
    strcpy(node->keyword, "");

    node->orderNumber = 0;
    node->numberOfChildren = 0;
    node->nodeType = 0;
    node->lineNumber = 0;
    node->columnNumber = 0;

    node->childrenAllocated = 50;
    node->children = malloc(sizeof(NDR_ASTNode*) * node->childrenAllocated);
}

void NDR_FreeASTNodeHolder(NDR_ASTNodeHolder* nodeWrapper){
    for(size_t x = 0; x < nodeWrapper->numNodes; x++){
        NDR_FreeASTNode(nodeWrapper->nodes[x]);
        free(nodeWrapper->nodes[x]);
    }
    free(nodeWrapper->nodes);
}

void NDR_FreeASTNode(NDR_ASTNode* node){
    free(node->token);
    free(node->keyword);
    for(size_t x = 0; x < node->numberOfChildren; x++){
        free(node->children[x]);
    }
    free(node->children);
}

void NDR_DestroyASTNode(NDR_ASTNode* node){
    free(node->token);
    free(node->keyword);
    free(node->children);
}

void NDR_DestroyASTTree(NDR_ASTNode* head){

    if(head != NULL){
        size_t duplicatePostDepthTrackerCount = 0;

        NDR_ASTNodeStack* depthTracker = malloc(sizeof(NDR_ASTNodeStack));
        NDR_InitASTStack(depthTracker);
        NDR_ASTNode** duplicateTracker = malloc(sizeof(NDR_ASTNode*) * 5000);
        NDR_ASTNode* follow = head;

        NDR_ASTStackPush(depthTracker, follow);

        size_t count;
        while(!NDR_ASTStackIsEmpty(depthTracker)){
            count = 0;
            while(count < follow->numberOfChildren){
                if(!NDR_ASTDuplicate(follow->children[count], duplicateTracker, duplicatePostDepthTrackerCount)){
                    follow = follow->children[count];
                    NDR_ASTStackPush(depthTracker, follow);
                    count = 0;
                    continue;
                }
                count++;
            }

            if(!NDR_ASTDuplicate(follow, duplicateTracker, duplicatePostDepthTrackerCount)){
                duplicateTracker[duplicatePostDepthTrackerCount++] = (follow);
            }

            NDR_ASTStackPop(depthTracker);

            if(!NDR_ASTStackIsEmpty(depthTracker))
                follow = NDR_ASTStackPeek(depthTracker);
        }

        NDR_FreeASTStack(depthTracker);
        free(depthTracker);
        free(duplicateTracker);
    }

}

void NDR_AddNewASTNode(NDR_ASTNodeHolder* nodeWrapper, NDR_ASTNode* nodeToAdd){
    if(nodeWrapper->numNodes > nodeWrapper->memoryAllocated - 5){
        nodeWrapper->memoryAllocated = nodeWrapper->memoryAllocated * 2;
        nodeWrapper->nodes = realloc(nodeWrapper->nodes, sizeof(NDR_ASTNode*) * nodeWrapper->memoryAllocated);
    }

    nodeWrapper->nodes[nodeWrapper->numNodes] = nodeToAdd;
    nodeWrapper->numNodes++;
}

void NDR_AddChildASTNode(NDR_ASTNode* node, NDR_ASTNode* nodeToAdd){
    if(node->numberOfChildren > node->childrenAllocated - 5){
        node->childrenAllocated = node->childrenAllocated * 2;
        node->children = realloc(node->children, sizeof(NDR_ASTNode*) * node->childrenAllocated);
    }

    node->children[node->numberOfChildren] = nodeToAdd;
    node->numberOfChildren++;
}


void NDR_SetASTNodeKeyword(NDR_ASTNode* node, char* keyword){
    node->keyword = realloc(node->keyword, strlen(keyword)+1);
    strcpy(node->keyword, keyword);
}
void NDR_SetASTNodeToken(NDR_ASTNode* node, char* token){
    node->token = realloc(node->token, strlen(token)+1);
    strcpy(node->token, token);
}
void NDR_SetASTNodeOrderNumber(NDR_ASTNode* node, long orderNumber){
    node->orderNumber = orderNumber;
}
void NDR_SetASTNodeNodeType(NDR_ASTNode* node, size_t nodeType){
    node->nodeType = nodeType;
}
void NDR_SetASTNodeLineNumber(NDR_ASTNode* node, size_t lineNumber){
    node->lineNumber = lineNumber;
}
void NDR_SetASTNodeColumnNumber(NDR_ASTNode* node, size_t columnNumber){
    node->columnNumber = columnNumber;
}

void NDR_IncASTTotalNode(NDR_ASTNodeHolder* nodeWrapper){
    nodeWrapper->totalNodesInTree = nodeWrapper->totalNodesInTree + 1;
}


char* NDR_GetASTNodeKeyword(NDR_ASTNode* node){
    return node->keyword;
}
char* NDR_GetASTNodeToken(NDR_ASTNode* node){
    return node->token;
}
long NDR_GetASTNodeOrderNumber(NDR_ASTNode* node){
    return node->orderNumber;
}
size_t NDR_GetASTNodeNodeType(NDR_ASTNode* node){
    return node->nodeType;
}
size_t NDR_GetASTNodeLineNumber(NDR_ASTNode* node){
    return node->lineNumber;
}
size_t NDR_GetASTNodeColumnNumber(NDR_ASTNode* node){
    return node->columnNumber;
}

NDR_ASTNode* NDR_GetASTNode(NDR_ASTNodeHolder* nodeWrapper, size_t index){
    return nodeWrapper->nodes[index];
}
size_t NDR_GetTotalASTTreeNodes(NDR_ASTNodeHolder* nodeWrapper){
    return nodeWrapper->totalNodesInTree;
}
size_t NDR_GetNumberOfASTNodes(NDR_ASTNodeHolder* nodeWrapper){
    return nodeWrapper->numNodes;
}






// Utility function to initialize a queue
void NDR_InitASTQueue(NDR_ASTNodeQueue* ndrqueue){
    ndrqueue->memoryAllocated = 50;
    ndrqueue->nodes = malloc(ndrqueue->memoryAllocated * sizeof(NDR_ASTNode*));
    ndrqueue->numNodes = 0;
}

void NDR_FreeASTQueue(NDR_ASTNodeQueue* ndrqueue){
    for(size_t x = 0; x < ndrqueue->numNodes; x++){
        NDR_DestroyASTNode(ndrqueue->nodes[x]);
        free(ndrqueue->nodes[x]);
    }
    free(ndrqueue->nodes);
}

// Utility function to return the size of the queue
size_t NDR_ASTQSize(NDR_ASTNodeQueue* ndrqueue){
    return ndrqueue->numNodes;
}

// Utility function to check if the queue is empty or not
bool NDR_ASTQIsEmpty(NDR_ASTNodeQueue* ndrqueue){
    return (ndrqueue->numNodes <= 0);
}

// Utility function to return the front element of the queue
NDR_ASTNode* NDR_ASTQFront(NDR_ASTNodeQueue* ndrqueue){
    if (NDR_ASTQIsEmpty(ndrqueue))
        return NULL;
    else
        return ndrqueue->nodes[0];
}

// Utility function to add an element `x` to the queue
void NDR_ASTQEnqueue(NDR_ASTNodeQueue* ndrqueue, NDR_ASTNode* node){
    if(ndrqueue->numNodes > ndrqueue->memoryAllocated - 5){
        ndrqueue->memoryAllocated = ndrqueue->memoryAllocated * 2;
        ndrqueue->nodes = realloc(ndrqueue->nodes, sizeof(NDR_ASTNode*) * ndrqueue->memoryAllocated);
    }
    ndrqueue->nodes[ndrqueue->numNodes++] = node;
}

// Utility function to dequeue the front element
NDR_ASTNode* NDR_ASTQDequeue(NDR_ASTNodeQueue* ndrqueue){
    if(NDR_ASTQIsEmpty(ndrqueue))
        return NULL;
    else{
        NDR_ASTNode* hold = ndrqueue->nodes[0];
        MovePointersDown(ndrqueue);
        ndrqueue->numNodes--;
        return hold;
    }
}

void MovePointersDown(NDR_ASTNodeQueue* ndrqueue){
    for(size_t x = 1; x < ndrqueue->numNodes; x++){
        ndrqueue->nodes[x-1] = ndrqueue->nodes[x];
    }
}





// Utility function to initialize the stack
void NDR_InitASTStack(NDR_ASTNodeStack* ndrstack){
    ndrstack->memoryAllocated = 50;
    ndrstack->nodes = malloc(ndrstack->memoryAllocated * sizeof(NDR_ASTNode*));
    ndrstack->numNodes = 0;
}


void NDR_FreeASTStack(NDR_ASTNodeStack* ndrstack){
    for(size_t x = 0; x < ndrstack->numNodes; x++){
        NDR_DestroyASTNode(ndrstack->nodes[x]);
        free(ndrstack->nodes[x]);
    }
    free(ndrstack->nodes);
}

// Utility function to return the size of the stack
size_t NDR_ASTStackSize(NDR_ASTNodeStack* ndrstack){
    return ndrstack->numNodes;
}

// Utility function to check if the stack is empty or not
bool NDR_ASTStackIsEmpty(NDR_ASTNodeStack* ndrstack) {
    return ndrstack->numNodes <= 0;
}

// Utility function to add an element `x` to the stack
void NDR_ASTStackPush(NDR_ASTNodeStack* ndrstack, NDR_ASTNode* node){
    if(ndrstack->numNodes > ndrstack->memoryAllocated - 5){
        ndrstack->memoryAllocated = ndrstack->memoryAllocated * 2;
        ndrstack->nodes = realloc(ndrstack->nodes, sizeof(NDR_ASTNode*) * ndrstack->memoryAllocated);
    }
    ndrstack->nodes[ndrstack->numNodes++] = node;
}

NDR_ASTNode* NDR_ASTStackPeek(NDR_ASTNodeStack* ndrstack){
    if(NDR_ASTStackIsEmpty(ndrstack))
        return NULL;
    else
        return ndrstack->nodes[ndrstack->numNodes - 1];
}


NDR_ASTNode* NDR_ASTStackPop(NDR_ASTNodeStack* ndrstack){
    if (NDR_ASTStackIsEmpty(ndrstack))
        return NULL;
    else
        return ndrstack->nodes[ndrstack->numNodes--];
}







//  Need to allocate memory according to code file or else there may not be enough memory
NDR_ASTNode* NDR_ASTPreOrderTraversal(NDR_ASTNode* head){

    static NDR_ASTNode** preOrder;
    static size_t currentPreDepthNode;
    static size_t duplicatePreDepthTrackerCount;

     if(head != NULL){
        size_t memoryAllocated = 1000;
        preOrder = realloc(preOrder, sizeof(NDR_ASTNode) * memoryAllocated);
        currentPreDepthNode = 0;
        duplicatePreDepthTrackerCount = 0;

        NDR_ASTNodeStack* depthTracker = malloc(sizeof(NDR_ASTNodeStack));
        NDR_InitASTStack(depthTracker);
        NDR_ASTNode** duplicateTracker = malloc(sizeof(NDR_ASTNode*) * memoryAllocated);
        NDR_ASTNode* follow = head;

        NDR_ASTStackPush(depthTracker, follow);
        duplicateTracker[duplicatePreDepthTrackerCount++] = follow;
        preOrder[currentPreDepthNode++] = follow;

        size_t count;
        bool breakout;
        while(!NDR_ASTStackIsEmpty(depthTracker)){

            count = 0;
            breakout = false;
            while(count < follow->numberOfChildren){
                if(!NDR_ASTDuplicate(follow->children[count], duplicateTracker, duplicatePreDepthTrackerCount)){
                    NDR_ASTStackPush(depthTracker, follow->children[count]);

                    if(currentPreDepthNode > memoryAllocated - 5){
                        memoryAllocated = memoryAllocated * 2;
                        duplicateTracker = realloc(duplicateTracker, sizeof(NDR_ASTNode*) * memoryAllocated);
                        preOrder = realloc(preOrder, sizeof(NDR_ASTNode*) * memoryAllocated);
                    }

                    duplicateTracker[duplicatePreDepthTrackerCount++] = (follow->children[count]);
                    preOrder[currentPreDepthNode++] = follow->children[count];
                    follow = follow->children[count];
                    breakout = true;
                    break;
                }
                count++;
            }
            if(breakout == true)
                continue;
            else{
                NDR_ASTStackPop(depthTracker);

                if(!NDR_ASTStackIsEmpty(depthTracker))
                    follow = NDR_ASTStackPeek(depthTracker);
            }

        }
        currentPreDepthNode = 0;

        free(depthTracker->nodes);
        free(depthTracker);
        free(duplicateTracker);

        return preOrder[currentPreDepthNode];
    }
    else{
        currentPreDepthNode++;
     }

    if(currentPreDepthNode < duplicatePreDepthTrackerCount)
        return preOrder[currentPreDepthNode];
    else{
        return NULL;
    }

}

NDR_ASTNode* NDR_ASTInOrderTraversal(NDR_ASTNode* head){

    static NDR_ASTNode** inOrder;
    static size_t currentInDepthNode;
    static size_t duplicateInDepthTrackerCount;

     if(head != NULL){
        size_t memoryAllocated = 1000;
        inOrder = realloc(inOrder, sizeof(NDR_ASTNode) * memoryAllocated);
        currentInDepthNode = 0;
        duplicateInDepthTrackerCount = 0;

        NDR_ASTNodeStack* depthTracker = malloc(sizeof(NDR_ASTNodeStack));
        NDR_InitASTStack(depthTracker);
        NDR_ASTNode** duplicateTracker = malloc(sizeof(NDR_ASTNode*) * memoryAllocated);
        NDR_ASTNode* follow = head;

        NDR_ASTStackPush(depthTracker, follow);

        size_t count;
        while(!NDR_ASTStackIsEmpty(depthTracker)){
            count = 0;
            while(count < follow->numberOfChildren){
                if(!NDR_ASTDuplicate(follow->children[count], duplicateTracker, duplicateInDepthTrackerCount) && count != follow->numberOfChildren - 1){
                    follow = follow->children[count];
                    NDR_ASTStackPush(depthTracker, follow);
                    count = 0;
                    continue;
                }
                else if(!NDR_ASTDuplicate(follow, duplicateTracker, duplicateInDepthTrackerCount) && count == follow->numberOfChildren - 1){

                    if(currentInDepthNode > memoryAllocated - 5){
                        memoryAllocated = memoryAllocated * 2;
                        duplicateTracker = realloc(duplicateTracker, sizeof(NDR_ASTNode*) * memoryAllocated);
                        inOrder = realloc(inOrder, sizeof(NDR_ASTNode*) * memoryAllocated);
                    }

                    duplicateTracker[duplicateInDepthTrackerCount++] = (follow);
                    inOrder[currentInDepthNode++] = follow;
                    count = 0;
                    continue;
                }
                else if(NDR_ASTDuplicate(follow, duplicateTracker, duplicateInDepthTrackerCount) && !NDR_ASTDuplicate(follow->children[count], duplicateTracker, duplicateInDepthTrackerCount)){
                    follow = follow->children[count];
                    NDR_ASTStackPush(depthTracker, follow);
                    count = 0;
                    continue;
                }
                count++;
            }

            if(follow->numberOfChildren == 0 && !NDR_ASTDuplicate(follow, duplicateTracker, duplicateInDepthTrackerCount)){
                duplicateTracker[duplicateInDepthTrackerCount++] = follow;
                inOrder[currentInDepthNode++] = follow;
            }

            NDR_ASTStackPop(depthTracker);

            if(!NDR_ASTStackIsEmpty(depthTracker))
                follow = NDR_ASTStackPeek(depthTracker);
        }
        currentInDepthNode = 0;

        free(depthTracker->nodes);
        free(depthTracker);
        free(duplicateTracker);

        return inOrder[currentInDepthNode];
    }
    else{
        currentInDepthNode++;
     }

    if(currentInDepthNode < duplicateInDepthTrackerCount)
        return inOrder[currentInDepthNode];
    else{
        return NULL;
    }
}

NDR_ASTNode* NDR_ASTPostOrderTraversal(NDR_ASTNode* head){

    static NDR_ASTNode** postOrder;
    static size_t currentPostDepthNode;
    static size_t duplicatePostDepthTrackerCount;

     if(head != NULL){
        size_t memoryAllocated = 1000;
        postOrder = realloc(postOrder, sizeof(NDR_ASTNode) * memoryAllocated);
        currentPostDepthNode = 0;
        duplicatePostDepthTrackerCount = 0;

        NDR_ASTNodeStack* depthTracker = malloc(sizeof(NDR_ASTNodeStack));
        NDR_InitASTStack(depthTracker);
        NDR_ASTNode** duplicateTracker = malloc(sizeof(NDR_ASTNode*) * memoryAllocated);
        NDR_ASTNode* follow = head;

        NDR_ASTStackPush(depthTracker, follow);

        size_t count;
        while(!NDR_ASTStackIsEmpty(depthTracker)){
            count = 0;
            while(count < follow->numberOfChildren){
                if(!NDR_ASTDuplicate(follow->children[count], duplicateTracker, duplicatePostDepthTrackerCount)){
                    follow = follow->children[count];
                    NDR_ASTStackPush(depthTracker, follow);
                    count = 0;
                    continue;
                }
                count++;
            }

            if(!NDR_ASTDuplicate(follow, duplicateTracker, duplicatePostDepthTrackerCount)){

                if(currentPostDepthNode > memoryAllocated - 5){
                    memoryAllocated = memoryAllocated * 2;
                    duplicateTracker = realloc(duplicateTracker, sizeof(NDR_ASTNode*) * memoryAllocated);
                    postOrder = realloc(postOrder, sizeof(NDR_ASTNode*) * memoryAllocated);
                }

                duplicateTracker[duplicatePostDepthTrackerCount++] = follow;
                postOrder[currentPostDepthNode++] = follow;
            }

            NDR_ASTStackPop(depthTracker);

            if(!NDR_ASTStackIsEmpty(depthTracker))
                follow = NDR_ASTStackPeek(depthTracker);
        }
        currentPostDepthNode = 0;

        free(depthTracker->nodes);
        free(depthTracker);
        free(duplicateTracker);

        return postOrder[currentPostDepthNode];
    }
    else{
        currentPostDepthNode++;
     }

    if(currentPostDepthNode < duplicatePostDepthTrackerCount)
        return postOrder[currentPostDepthNode];
    else{
        return NULL;
    }
}

NDR_ASTNode* NDR_ASTBreadthFirstTraversal(NDR_ASTNode* head){

    static NDR_ASTNode** breadthOrder;
    static size_t currentBreadthNode;
    static size_t duplicateBreadthTrackerCount;



     if(head != NULL){
        size_t memoryAllocated = 1000;
        breadthOrder = realloc(breadthOrder, sizeof(NDR_ASTNode) * memoryAllocated);
        currentBreadthNode = 0;
        duplicateBreadthTrackerCount = 0;

        NDR_ASTNodeQueue* breadthTracker = malloc(sizeof(NDR_ASTNodeQueue));
        NDR_InitASTQueue(breadthTracker);
        NDR_ASTNode** duplicateTracker = malloc(sizeof(NDR_ASTNode*) * memoryAllocated);
        NDR_ASTNode* follow = head;

        NDR_ASTQEnqueue(breadthTracker, follow);
        duplicateTracker[duplicateBreadthTrackerCount++] = follow;
        breadthOrder[currentBreadthNode++] = follow;

        size_t count;
        while(!NDR_ASTQIsEmpty(breadthTracker)){

            count = 0;
            while(count < follow->numberOfChildren){
                if(!NDR_ASTDuplicate(follow->children[count], duplicateTracker, duplicateBreadthTrackerCount)){

                    if(currentBreadthNode > memoryAllocated - 5){
                        memoryAllocated = memoryAllocated * 2;
                        duplicateTracker = realloc(duplicateTracker, sizeof(NDR_ASTNode*) * memoryAllocated);
                        breadthOrder = realloc(breadthOrder, sizeof(NDR_ASTNode*) * memoryAllocated);
                    }

                    NDR_ASTQEnqueue(breadthTracker,follow->children[count]);
                    duplicateTracker[duplicateBreadthTrackerCount++] = follow->children[count];
                    breadthOrder[currentBreadthNode++] = follow->children[count];
                }
                count++;
            }

            NDR_ASTQDequeue(breadthTracker);
            if(!NDR_ASTQIsEmpty(breadthTracker)){
                follow = NDR_ASTQFront(breadthTracker);
            }
        }
        currentBreadthNode = 0;

        free(breadthTracker->nodes);
        free(breadthTracker);
        free(duplicateTracker);

        return head;
    }
    else{
        currentBreadthNode++;
     }

    if(currentBreadthNode < duplicateBreadthTrackerCount)
        return breadthOrder[currentBreadthNode];
    else{
        return NULL;
    }

}

bool NDR_ASTDuplicate(NDR_ASTNode* address, NDR_ASTNode** nodes, size_t index){
    for(size_t i = 0; i < index; i++){
        if(address == nodes[i]){
            return true;
        }
    }
    return false;
}






