
/*********************************************************************************
*                            NDR Sequence Information                            *
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
#include "ndr_sequenceinformation.h"



void NDR_InitSequenceInfoWrapper(NDR_SequenceInformationWrapper* sequenceInfoWrapper){
    sequenceInfoWrapper->numSequences = 0;
    sequenceInfoWrapper->memoryAllocated = 50;
    sequenceInfoWrapper->sequences = malloc(sizeof(NDR_SequenceInformation*) * sequenceInfoWrapper->memoryAllocated);
}

void NDR_AddNewSequenceTokenInfo(NDR_SequenceInformationWrapper* sequenceInfoWrapper){
    if(sequenceInfoWrapper->numSequences > sequenceInfoWrapper->memoryAllocated - 5){
        sequenceInfoWrapper->memoryAllocated = sequenceInfoWrapper->memoryAllocated * 2;
        sequenceInfoWrapper->sequences = realloc(sequenceInfoWrapper->sequences, sizeof(NDR_SequenceInformation*) * sequenceInfoWrapper->memoryAllocated);
    }
    sequenceInfoWrapper->sequences[sequenceInfoWrapper->numSequences] = malloc(sizeof(NDR_SequenceInformation));
    sequenceInfoWrapper->sequences[sequenceInfoWrapper->numSequences]->keyword = malloc(1);
    sequenceInfoWrapper->sequences[sequenceInfoWrapper->numSequences]->sequence = malloc(1);
    sequenceInfoWrapper->numSequences++;
}

void NDR_SetSTokenInfoKeyword(NDR_SequenceInformation* sequenceInfo, char* keyword){
    sequenceInfo->keyword = realloc(sequenceInfo->keyword, strlen(keyword)+1);
    strcpy(sequenceInfo->keyword, keyword);
}
void NDR_AddToSTokenInfoKeyword(NDR_SequenceInformation* sequenceInfo, char* keyword){
    sequenceInfo->keyword = realloc(sequenceInfo->keyword, strlen(sequenceInfo->keyword) + strlen(keyword)+1);
    strcat(sequenceInfo->keyword, keyword);
}
void NDR_SetSTokenInfoSequence(NDR_SequenceInformation* sequenceInfo, char* sequence){
    sequenceInfo->sequence = realloc(sequenceInfo->sequence, strlen(sequence)+1);
    strcpy(sequenceInfo->sequence, sequence);
}
void NDR_AddToSTokenInfoSequence(NDR_SequenceInformation* sequenceInfo, char* token){
    sequenceInfo->sequence = realloc(sequenceInfo->sequence, strlen(sequenceInfo->sequence) + strlen(token)+1);
    strcat(sequenceInfo->sequence, token);
}

NDR_SequenceInformation* NDR_GetSequenceInfo(NDR_SequenceInformationWrapper* sequenceInfoWrapper, size_t index){
    return sequenceInfoWrapper->sequences[index];
}

NDR_SequenceInformation* NDR_GetLastSequenceInfo(NDR_SequenceInformationWrapper* sequenceInfoWrapper){
    return sequenceInfoWrapper->sequences[sequenceInfoWrapper->numSequences-1];
}

size_t NDR_GetNumberOfSequences(NDR_SequenceInformationWrapper* sequenceInfoWrapper){
    return sequenceInfoWrapper->numSequences;
}

int NDR_FindSequenceBeforeLast(NDR_SequenceInformationWrapper* sequenceInfoWrapper, char* sequence){
    for(size_t i = 0; i < sequenceInfoWrapper->numSequences-1; i++){
        if(strcmp(NDR_GetSequenceInfo(sequenceInfoWrapper, i)->sequence, sequence) == 0){
            return i;
        }
    }
    return -1;
}
