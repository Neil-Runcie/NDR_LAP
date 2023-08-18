
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

#ifndef NDRREGEXTRACKER_H
#define NDRREGEXTRACKER_H

// Forward declaration of Regex node for reference with the tracker struct
typedef struct NDR_RegexNode NDR_RegexNode;

// Declaration of Regex tracker to keep track of regex matching process
typedef struct NDR_RegexTracker {
    // Node reference to know which "word" start node corresponds to this tracker
    NDR_RegexNode* reference;
    // numberOfRepeats keeps track of the number of times that the graph path has been followed
    size_t numberOfRepeats;
    // currentChild refers to the the child path currently being followed in an "or" path
    size_t currentChild;
    // stringPosition keeps a reference to the character in the token seen whenever the "word" started
    int stringPosition;
} NDR_RegexTracker;

// Declaration of a stack structure for the NDR_RegexTracker struct
typedef struct NDR_TrackerStack{
    // references holds a pointer for each tracker struct pushed into the stack
    NDR_RegexTracker** references;
    // disposal holds unique pointers for each tracker struct pushed into the stack for later memory freeing
    NDR_RegexTracker** disposal;
    // memoryAllocated to the references pointer
    size_t memoryAllocated;
    // numNodes holds the number of nodes in the references array
    size_t numNodes;
    // disposalMemoryAllocated to the disposal pointer
    size_t disposalMemoryAllocated;
    // numDisposalNodes holds the number of nodes in the references array
    size_t numDisposalNodes;
} NDR_TrackerStack;

// Utility function to initialize the struct used to keep track of progression through the regex graph
void NDR_InitRTracker(NDR_RegexTracker* ref, NDR_RegexNode* node);
// Utility function to initialize the stack
void NDR_InitTrackerStack(NDR_TrackerStack* ndrstack);
// Utility function to return the size of the stack
size_t NDR_TrackerStackSize(NDR_TrackerStack* ndrstack);
// Utility function to check if the stack is empty or not
bool NDR_TrackerStackIsEmpty(NDR_TrackerStack* ndrstack);
// Utility function to add an element `x` to the stack
void NDR_TrackerStackPush(NDR_TrackerStack* ndrstack, NDR_RegexTracker* node);
// Utility function to return a reference to the top of the stack
NDR_RegexTracker* NDR_TrackerStackPeek(NDR_TrackerStack* ndrstack);
// Utility function to return a reference to the top of the stack and then remove it
NDR_RegexTracker* NDR_TrackerStackPop(NDR_TrackerStack* ndrstack);

// Utility function to free the memory allocated to items with the struct
void NDR_DestroyRegexTracker(NDR_RegexTracker* tracker);
// Utility function to free the memory allocated to items with the stack
void NDR_DestroyRegexTrackerStack(NDR_TrackerStack* stack);


#endif
