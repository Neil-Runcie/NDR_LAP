
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

typedef struct NDR_RegexNode NDR_RegexNode;
typedef struct NDR_RegexTracker {
    NDR_RegexNode* reference;
    size_t numberOfRepeats;
    size_t currentChild;
    int stringPosition;
} NDR_RegexTracker;

typedef struct NDR_TrackerStack{
    NDR_RegexTracker** references;
    NDR_RegexTracker** disposal;
    size_t memoryAllocated;
    size_t numNodes;
    size_t disposalMemoryAllocated;
    size_t numDisposalNodes;
} NDR_TrackerStack;

void NDR_InitRTracker(NDR_RegexTracker* ref, NDR_RegexNode* node);
void NDR_InitTrackerStack(NDR_TrackerStack* ndrstack);
size_t NDR_TrackerStackSize(NDR_TrackerStack* ndrstack);
bool NDR_TrackerStackIsEmpty(NDR_TrackerStack* ndrstack);
void NDR_TrackerStackPush(NDR_TrackerStack* ndrstack, NDR_RegexTracker* node);
NDR_RegexTracker* NDR_TrackerStackPeek(NDR_TrackerStack* ndrstack);
NDR_RegexTracker* NDR_TrackerStackPop(NDR_TrackerStack* ndrstack);

void NDR_DestroyRegexTracker(NDR_RegexTracker* tracker);
void NDR_DestroyRegexTrackerStack(NDR_TrackerStack* stack);


#endif
