
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "ndr_regexnode.h"

void ContinueAfterWord(NDR_RNodeStack* startStack, NDR_RNodeStack* endStack){

    NDR_RegexNode* holdStart = NDR_RNodeStackPop(startStack);
    NDR_RegexNode* holdEnd = NDR_RNodeStackPop(endStack);

    free(NDR_RNodeStackPeek(endStack)->children[0]);

    NDR_RNodeStackPeek(endStack)->children[0] = holdStart;
    NDR_RNodeStackSet(endStack, holdEnd);

}



void NDR_InitRegexNode(NDR_RegexNode* node){
    node->start = false;
    node->end = false;
    node->wordStart = false;
    node->wordEnd = false;
    node->optionalPath = false;
    node->repeatPath = false;
    node->orPath = false;
    node->minMatches = 1;
    node->maxMatches = 1;

    node->wordReference = NULL;

    node->allButNewLine = false;
    node->everything = false;
    node->nothing = false;
    node->decimalDigit = false;
    node->notDecimalDigit = false;
    node->whiteSpace = false;
    node->notWhiteSpace = false;
    node->wordChar = false;
    node->notWordChar = false;

    node->negatedClass = false;

    node->numberOfChars = 0;
    node->memoryAllocated = 0;
    //node->acceptChars = malloc(node->memoryAllocated);

    node->numberOfChildren = 0;
    node->memoryAllocatedChildren = 5;
    node->children = malloc(sizeof(NDR_RegexNode*) * node->memoryAllocatedChildren);
    NDR_RegexNode* newNode = malloc(sizeof(NDR_RegexNode));
    NDR_AddRNodeChild(node, newNode);
}

void NDR_RemoveRNodeChild(NDR_RegexNode* node){
    free(node->children[0]);
    node->numberOfChildren--;
    if(node->numberOfChildren == 0)
        free(node->children);
}

bool NDR_IsRNodeEmpty(NDR_RegexNode* node){
    if(node->start == false &&
    node->end == false &&
    node->wordStart == false &&
    node->wordEnd == false &&
    node->optionalPath == false &&
    node->repeatPath == false &&
    node->orPath == false &&
    node->minMatches == 1 &&
    node->maxMatches == 1 &&
    node->wordReference == NULL &&
    node->allButNewLine == false &&
    node->everything == false &&
    node->nothing == false &&
    node->decimalDigit == false &&
    node->notDecimalDigit == false &&
    node->whiteSpace == false &&
    node->notWhiteSpace == false &&
    node->wordChar == false &&
    node->notWordChar == false &&
    node->numberOfChars == 0 &&
    node->numberOfChildren == 1){
        return true;
    }
    return false;
}

bool NDR_IsRNodeSetForComparison(NDR_RegexNode* node){
    if(node->allButNewLine == true ||
    node->everything == true ||
    node->nothing == true ||
    node->decimalDigit == true ||
    node->notDecimalDigit == true ||
    node->whiteSpace == true ||
    node->notWhiteSpace == true ||
    node->wordChar == true ||
    node->notWordChar == true ||
    node->numberOfChars > 0){
        return true;
    }
    return false;
}


void NDR_AddRNodeChar(NDR_RegexNode* node, char character){
    if(node->memoryAllocated == 0){
        node->memoryAllocated = 20;
        node->acceptChars = malloc(node->memoryAllocated);
    }
    else if(node->numberOfChars > node->memoryAllocated - 5){
        node->memoryAllocated = node->memoryAllocated * 2;
        node->acceptChars = realloc(node->acceptChars, node->memoryAllocated);
    }
    node->acceptChars[node->numberOfChars] = character;
    node->numberOfChars++;
}

void NDR_AddRNodeChild(NDR_RegexNode* node, NDR_RegexNode* child){
    if(node->numberOfChildren > node->memoryAllocatedChildren - 2){
        node->memoryAllocatedChildren = node->memoryAllocatedChildren * 2;
        node->children = realloc(node->children, sizeof(NDR_RegexNode*) * node->memoryAllocatedChildren);
    }
    node->children[node->numberOfChildren] = child;
    node->numberOfChildren++;
}

bool NDR_RNodeDuplicate(NDR_RegexNode* address, NDR_RegexNode** nodes, size_t index){
    for(size_t i = 0; i < index; i++){
        if(address == nodes[i]){
            return true;
        }
    }
    return false;
}

// Utility function to initialize the stack
void NDR_InitRNodeStack(NDR_RNodeStack* ndrstack){
    ndrstack->memoryAllocated = 50;
    ndrstack->nodes = malloc(ndrstack->memoryAllocated * sizeof(NDR_RegexNode*));
    ndrstack->numNodes = 0;
}

// Utility function to return the size of the stack
size_t NDR_RNodeStackSize(NDR_RNodeStack* ndrstack){
    return ndrstack->numNodes;
}

// Utility function to check if the stack is empty or not
bool NDR_RNodeStackIsEmpty(NDR_RNodeStack* ndrstack) {
    return ndrstack->numNodes <= 0;
}

// Utility function to add an element `x` to the stack
void NDR_RNodeStackPush(NDR_RNodeStack* ndrstack, NDR_RegexNode* node){
    if(ndrstack->numNodes > ndrstack->memoryAllocated - 5){
        ndrstack->memoryAllocated = ndrstack->memoryAllocated * 2;
        ndrstack->nodes = realloc(ndrstack->nodes, sizeof(NDR_RegexNode*) * ndrstack->memoryAllocated);
    }
    ndrstack->nodes[ndrstack->numNodes++] = node;
}

NDR_RegexNode* NDR_RNodeStackPeek(NDR_RNodeStack* ndrstack){
    if(NDR_RNodeStackIsEmpty(ndrstack))
        return NULL;
    else
        return ndrstack->nodes[ndrstack->numNodes - 1];
}


NDR_RegexNode* NDR_RNodeStackPop(NDR_RNodeStack* ndrstack){
    if (NDR_RNodeStackIsEmpty(ndrstack))
        return NULL;
    else
        return ndrstack->nodes[--ndrstack->numNodes];
}

void NDR_RNodeStackSet(NDR_RNodeStack* ndrstack, NDR_RegexNode* node){
    ndrstack->nodes[ndrstack->numNodes - 1] = node;
}




void NDR_DestroyRegexNode(NDR_RegexNode* node){
    if(node->memoryAllocated > 0)
        free(node->acceptChars);
    if(node->numberOfChildren > 0)
        free(node->children);
}

// Free the memory associated with items within the regex struct


void NDR_DestroyRegexStack(NDR_RNodeStack* stack){
    free(stack->nodes);
}


void NDR_FreeRNodeStack(NDR_RNodeStack* ndrstack){
    for(size_t x = 0; x < ndrstack->numNodes; x++){

        NDR_DestroyRegexNode(ndrstack->nodes[x]);
        free(ndrstack->nodes[x]);
    }
    free(ndrstack->nodes);
}
