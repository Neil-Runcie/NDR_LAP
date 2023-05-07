#ifndef SEQUENCEINFORMATION_H
#define SEQUENCEINFORMATION_H

//typedef struct NDR_SequenceInformation NDR_SequenceInformation;

//typedef struct NDR_SequenceInformationWrapper NDR_SequenceInformationWrapper;

typedef struct NDR_SequenceInformation {
    char* keyword;
    char* sequence;
} NDR_SequenceInformation;

typedef struct NDR_SequenceInformationWrapper {
    size_t numSequences;
    size_t memoryAllocated;
    NDR_SequenceInformation** sequences;
} NDR_SequenceInformationWrapper;

void NDR_InitSequenceInfoWrapper(NDR_SequenceInformationWrapper* sequenceInfoWrapper);

void NDR_AddNewSequenceTokenInfo(NDR_SequenceInformationWrapper* sequenceInfoWrapper);
void NDR_SetSTokenInfoKeyword(NDR_SequenceInformation* sequenceInfo, char* keyword);
void NDR_AddToSTokenInfoKeyword(NDR_SequenceInformation* sequenceInfo, char* keyword);
void NDR_SetSTokenInfoSequence(NDR_SequenceInformation* sequenceInfo, char* token);
void NDR_AddToSTokenInfoSequence(NDR_SequenceInformation* sequenceInfo, char* sequence);

NDR_SequenceInformation* NDR_GetSequenceInfo(NDR_SequenceInformationWrapper* sequenceInfoWrapper, size_t index);
NDR_SequenceInformation* NDR_GetLastSequenceInfo(NDR_SequenceInformationWrapper* sequenceInfoWrapper);
size_t NDR_GetNumberOfSequences(NDR_SequenceInformationWrapper* sequenceInfoWrapper);
int NDR_FindSequenceBeforeLast(NDR_SequenceInformationWrapper* sequenceInfoWrapper, char* sequence);

#endif
