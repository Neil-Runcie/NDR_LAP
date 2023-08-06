
/*********************************************************************************
*                               NDR Regex Tracker                                *
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

#include "ndr_regextracker.h"

void NDR_InitRTracker(NDR_RegexTracker* ref, NDR_RegexNode* node){
    ref->reference = node;
    ref->numberOfRepeats = 0;
    ref->currentChild = 0;
    ref->stringPosition = 0;
}


// Utility function to initialize the stack
void NDR_InitTrackerStack(NDR_TrackerStack* ndrstack){
    ndrstack->memoryAllocated = 50;
    ndrstack->references = malloc(ndrstack->memoryAllocated * sizeof(NDR_RegexTracker*));
    ndrstack->disposalMemoryAllocated = 50;
    ndrstack->disposal = malloc(ndrstack->disposalMemoryAllocated * sizeof(NDR_RegexTracker*));
    ndrstack->numNodes = 0;
    ndrstack->numDisposalNodes = 0;
}

// Utility function to return the size of the stack
size_t NDR_TrackerStackSize(NDR_TrackerStack* ndrstack){
    return ndrstack->numNodes;
}

// Utility function to check if the stack is empty or not
bool NDR_TrackerStackIsEmpty(NDR_TrackerStack* ndrstack) {
    return ndrstack->numNodes <= 0;
}

// Utility function to add an element `x` to the stack
void NDR_TrackerStackPush(NDR_TrackerStack* ndrstack, NDR_RegexTracker* node){
    if(ndrstack->numNodes > ndrstack->memoryAllocated - 5){
        ndrstack->memoryAllocated = ndrstack->memoryAllocated * 2;
        ndrstack->references = realloc(ndrstack->references, sizeof(NDR_RegexTracker*) * ndrstack->memoryAllocated);
    }
    if(ndrstack->numNodes > ndrstack->disposalMemoryAllocated - 5){
        ndrstack->disposalMemoryAllocated = ndrstack->disposalMemoryAllocated * 2;
        ndrstack->references = realloc(ndrstack->references, sizeof(NDR_RegexTracker*) * ndrstack->disposalMemoryAllocated);
    }
    ndrstack->references[ndrstack->numNodes++] = node;
    bool check = false;
    for(int x = 0; x < ndrstack->numDisposalNodes; x++){
        if(ndrstack->disposal[x] == node){
            check = true;
        }
    }
    if(check == false){
        ndrstack->disposal[ndrstack->numDisposalNodes++] = node;
    }
}

NDR_RegexTracker* NDR_TrackerStackPeek(NDR_TrackerStack* ndrstack){
    if(NDR_TrackerStackIsEmpty(ndrstack))
        return NULL;
    else{
        return ndrstack->references[ndrstack->numNodes - 1];
    }
}


NDR_RegexTracker* NDR_TrackerStackPop(NDR_TrackerStack* ndrstack){
    if (NDR_TrackerStackIsEmpty(ndrstack))
        return NULL;
    else{
        return ndrstack->references[--ndrstack->numNodes];
    }
}

void NDR_DestroyRegexTracker(NDR_RegexTracker* tracker){
    free(tracker->reference);
}

void NDR_DestroyRegexTrackerStack(NDR_TrackerStack* stack){
    for(int x = 0; x < stack->numDisposalNodes; x++){
        free(stack->disposal[x]);
    }

    free(stack->references);
    free(stack->disposal);
}
