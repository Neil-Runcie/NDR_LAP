
/*********************************************************************************
*                                    NDR Parser                                  *
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
#include "ndr_parser.h"
#include "ndr_sequenceinformation.h"
#include "ndr_asttokeninformation.h"
#include "ndr_matchstate.h"
#include "ndr_fileprocessor.h"
#include "ndr_astnode.h"
#include "ndr_debug.h"

typedef struct PStateRepresentation {
    bool newPattern;
    bool startedID;
    bool separatorWasPrevious;
} PStateRepresentation;

typedef struct SequenceMatchingState{
    char* currentSequence;
    char* potentialSequence;
    int allocatedLength;

    int sequenceNumber;
    int startIndex;
    int endIndex;

    bool matched;
    bool matchBeforeLookAhead;
    NDR_MatchState highestMatchSeen;
} SequenceMatchingState;


static void InitializeSequenceMatchingState(SequenceMatchingState* matchingState);
static void DestroySequenceMatchingState(SequenceMatchingState* matchingState);
static void addStringToSequence(SequenceMatchingState* matchingState, char* src);
static void AcknowledgePotentialSequence(SequenceMatchingState* matchingState, int treeIndex, int patternIndex);
static void AcknowledgeCompleteSequence(SequenceMatchingState* matchingState);
static void CapturePotentialSequence(SequenceMatchingState* matchingState, char* sequence);
static char* GetCapturedSequence(SequenceMatchingState* matchingState);
static bool IsPotentialSequence(SequenceMatchingState* matchingState, int RSIndex);
static bool IsPartialSequence(SequenceMatchingState* matchingState, int RSIndex);
static bool IsPotentialSequenceWrong(SequenceMatchingState* matchingState);
static bool IsCompleteSequence(SequenceMatchingState* matchingState);

static void InitializePSR(PStateRepresentation* pStateRepresentation);
static void AcknowledgeFoundKeyword(PStateRepresentation* pStateRepresentation);
static void AcknowledgeTokenSeparator(PStateRepresentation* pStateRepresentation);
static void AcknowledgeFoundToken(PStateRepresentation* pStateRepresentation);

static bool compareTokenToParsingTable();
static void condenseTable(int startingIndex, int amount, char* ID);
static bool isTokenInTable(char* token);
static bool verifyParseTokens(char* token, char* currentToken, PStateRepresentation* PSRep);
static void findEscapedParsing(char* string, int* indices);
static void removeColonEscape(char* string);
//static bool validateID(char* ID);
static bool findParseID(char* ID);
static bool sContainsInt(int* arr, int index);
//static bool isEntryADuplicate(char* entry);
//static int getLongestParseSequence();
static void copyTTToMT();

extern bool ST;
extern bool TT;
extern bool TL;
extern bool M;
extern bool R;
extern bool PT;

static NDR_SequenceInformationWrapper* PIWrapper = NULL;
extern NDR_TokenInformationWrapper* TIWrapper;
static NDR_TreeTokenInfoWrapper* TTIWrapper = NULL;
static NDR_ASTNodeHolder* NWrapper;
// head refers to the top level node in the syntax tree
NDR_ASTNode* NDR_ASThead;

// The Parse function is the driver for the Parser and should be called after Lex
int NDR_Configure_Parser(char* fileName){

    if(fileName == NULL || strcmp(fileName, "") == 0){
        printf("A non-empty filename must be provided for lexer configuration\n");
        return 1;
    }

    FILE *parserConfigFile = fopen(fileName, "r");
    if(parserConfigFile == NULL){
        printf("Cannot open the parser configuration file");
        return 1;
    }

    NDR_FileInformation* fileInfo = malloc(sizeof(NDR_FileInformation));
    if(NDR_ProcessFile(fileName, fileInfo, " ") == 1){
        printf("Failed to read the parser configuration file\n");
        return 1;
    }

    PIWrapper = malloc(sizeof(NDR_SequenceInformationWrapper));
    NDR_InitSequenceInfoWrapper(PIWrapper);
    PStateRepresentation* PSRepresentation = malloc(sizeof(PStateRepresentation));
    InitializePSR(PSRepresentation);


    size_t parserLineNumber = 1;

    char* currentToken = malloc(NDR_GetLongestLineLength(fileInfo));
    currentToken[0] = '\0';

    for(size_t i = 0; i < NDR_GetNumberOfLines(fileInfo); i++){

        if(strcmp(NDR_GetToken(NDR_GetLine(fileInfo, i), 0), "\n") == 0 || memcmp(NDR_GetToken(NDR_GetLine(fileInfo, i), 0), "//", 2) == 0){
            parserLineNumber++;
            continue;
        }

        for(size_t x = 0; x < NDR_GetNumberOfTokens(NDR_GetLine(fileInfo, i)); x++){
            if(!verifyParseTokens(NDR_GetToken(NDR_GetLine(fileInfo, i), x), currentToken, PSRepresentation)){
                printf("Error parsing parser config file at line %u\n", (unsigned int) parserLineNumber);
                return 1;
            }
        }

        if(NDR_GetNumberOfSequences(PIWrapper) > 0 && NDR_FindSequenceBeforeLast(PIWrapper, NDR_GetLastSequenceInfo(PIWrapper)->sequence) != -1){
            printf("A duplicate parsing string \"%s\" has been found on line %u\n", NDR_GetLastSequenceInfo(PIWrapper)->sequence, (unsigned int) PIWrapper->numSequences);
            return 1;
        }

        parserLineNumber++;
    }

    if(PSRepresentation->separatorWasPrevious == true){
        printf("\nAnother token was expected after \"\\|\" for keyword \"%s\" but it was not found\n", NDR_GetLastSequenceInfo(PIWrapper)->keyword);
        return 1;
    }
    else if(PSRepresentation->newPattern == true){
        printf("\nA token was not found after the last keyword in the parser configuration file\n");
        return 1;
    }

    if(NDR_GetNumberOfSequences(PIWrapper) == 0){
        printf("\nNo parsing sequences were found in the parser configuration file\n");
        return 1;
    }

    if(findParseID("*Accept") == false){
        printf("\n\"*Accept\" token not found\n");
        return 1;
    }

    if (PT == true)
        NDR_PrintParseTable();

    fclose(parserConfigFile);

    NDR_FreeFileInformation(fileInfo);
    free(fileInfo);
    free(PSRepresentation);

    free(currentToken);

    return 0;

}
int NDR_Parse(){

    if(TIWrapper == NULL){
        printf("\nCall function \"int Lex(char* fileName)\" to read the source file before calling function \"int Parse()\"\n");
        return 1;
    }

    if(PIWrapper == NULL){
        printf("\nCall function \"int Configure_Parser(char* fileName)\" to setup the parser configuration before calling function \"int Parse()\"\n");
        return 1;
    }

    // Copying needed information from the tokenTable and tokenLocationTable to the modifiedTokenTable which will be manipulated during processing
    TTIWrapper = malloc(sizeof(NDR_TokenInformationWrapper));
    NDR_InitTreeTokenInfoWrapper(TTIWrapper);
    NWrapper = malloc(sizeof(NDR_ASTNodeHolder));
    NDR_InitASTNodeHolder(NWrapper);
    copyTTToMT(TTIWrapper);

    NDR_ASThead = malloc(sizeof(NDR_ASTNode));
    if(compareTokenToParsingTable()){
        printf("\nParsing successful\n");
        return 0;
    }
    else{
        printf("\nUnable to complete parsing and verify\n");
        return 1;
    }
}

// verifyParseTokens validates each token seen to make sure the parser config file is valid and registers the tokens in the parseTable
bool verifyParseTokens(char* token, char* currentToken, PStateRepresentation* PSRep){

    // if the token is only a newline then return true otherwise remove the newline from the end of the token
    if(strcmp(token, "\n") == 0)
        return true;
    else if(token[strlen(token) - 1] == '\n')
        token[strlen(token) - 1] = '\0';

    int escapedCharacters[strlen(token)+1];
    findEscapedParsing(token, escapedCharacters);

    // If the token is longer than 1 (because we know it should have a ':' at the end), and the last character is a ':', and the last character has not been escaped
    if(strlen(token) > 1 && token[strlen(token) - 1] == ':' && !sContainsInt(escapedCharacters, strlen(token) - 1)){
        if(PSRep->separatorWasPrevious == true){
            printf("No parsing token was found in between \"\\|\" and the following parsing declaration\n");
            return false;
        }
        else if(PSRep->newPattern == true){
            printf("No parsing token was found after keyword \"%s\"\n", currentToken);
            return false;
        }
        token[strlen(token) - 1] = '\0';
        strcpy(currentToken, token);
        removeColonEscape(currentToken);

        AcknowledgeFoundKeyword(PSRep);
    }
    // If the token is an escaped vertical line, then add to the number of patterns because an escaped vertical line means or in the parser config syntax
    else if(strcmp(token, "\\|") == 0 && sContainsInt(escapedCharacters, 2)){
        if(PSRep->separatorWasPrevious == true){
            printf("No parsing token was found in between two \"\\|\" tokens\n");
            return false;
        }
        else if(PSRep->newPattern == true){
            printf("No parsing token was found in between keyword \"%s\" and \"\\|\"\n", currentToken);
            return false;
        }

        AcknowledgeTokenSeparator(PSRep);
    }
    // If the token is greater than zero then get the token and add to the ID unless started does not equal true
    else if(strlen(token) > 0){
        if (PSRep->startedID == true){
            if(PSRep->newPattern == true){
                NDR_AddNewSequenceTokenInfo(PIWrapper);
                NDR_SetSTokenInfoKeyword(NDR_GetLastSequenceInfo(PIWrapper), currentToken);
                NDR_SetSTokenInfoSequence(NDR_GetLastSequenceInfo(PIWrapper), "");
            }
            NDR_AddToSTokenInfoSequence(NDR_GetLastSequenceInfo(PIWrapper), token);
            removeColonEscape(NDR_GetLastSequenceInfo(PIWrapper)->sequence);
            NDR_AddToSTokenInfoSequence(NDR_GetLastSequenceInfo(PIWrapper), " ");

            AcknowledgeFoundToken(PSRep);
        }
        else if(PSRep->startedID == false){
            printf("ID has not been found\n");
            return false;
        }
    }
    else{
        printf("Parsing error\n");
        return false;
    }

    return true;
}



// compareTokenToParsingTable compares the current tokens to the parsing table to find the longest full match it can find
bool compareTokenToParsingTable(){

    SequenceMatchingState* matchingState = malloc(sizeof(SequenceMatchingState));
    InitializeSequenceMatchingState(matchingState);

    size_t originalNumberOfTreeTokens = NDR_GetNumberOfTreeTokens(TTIWrapper);
    size_t nextStep = 0;

    while(memcmp(NDR_GetTreeTokenInfo(TTIWrapper, 0)->tokenInfo->keyword, "*Accept", 6) != 0 || NDR_GetNumberOfTreeTokens(TTIWrapper) != 1){
        nextStep = 0;
        for (size_t i = 0; i < NDR_GetNumberOfTreeTokens(TTIWrapper)+1; i++){

            if(NDR_GetNumberOfTreeTokens(TTIWrapper) == i && NDR_GetNumberOfTreeTokens(TTIWrapper) == originalNumberOfTreeTokens){
                return false;
            }

            if(matchingState->highestMatchSeen == PARTIALMATCH)
                matchingState->highestMatchSeen = COMPLETEMATCH;

            addStringToSequence(matchingState, NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->keyword);
            addStringToSequence(matchingState, " ");
            matchingState->sequenceNumber++;

            for (size_t x = 0; x < NDR_GetNumberOfSequences(PIWrapper); x++){
                if(IsPotentialSequence(matchingState, x) == true){
                    AcknowledgePotentialSequence(matchingState, i, x);
                    break;
                }
                else if(IsPartialSequence(matchingState, x) == true){
                    matchingState->matched = true;
                }
            }

            if(IsCompleteSequence(matchingState) == true){

                condenseTable(matchingState->startIndex, matchingState->endIndex, GetCapturedSequence(matchingState));
                AcknowledgeCompleteSequence(matchingState);

                nextStep = 0;
                i = nextStep - 1;

                break;
            }
            else if(IsPotentialSequenceWrong(matchingState) == true){
                matchingState->matchBeforeLookAhead = false;
                matchingState->highestMatchSeen = NOMATCH;
            }

            if(matchingState->matched == false){
                nextStep++;
                strcpy(matchingState->currentSequence, "");
                matchingState->sequenceNumber = 0;
                i = nextStep - 1;
            }
            else{
                matchingState->matched = false;
            }

            if(NDR_GetNumberOfTreeTokens(TTIWrapper) == i){
                return false;
            }

        }
        if (TT == true)
            NDR_PrintModifiedTokenTable();
    }

    DestroySequenceMatchingState(matchingState);
    free(matchingState);

    return true;
}
// condenseTable takes the entries in the modifiedTokenTable and consolidates the rows between startingIndex and startingIndex+amount into just one row and moves all of the following rows up by amount to keep the table together
// A parent node is created and all of the consolidated rows become children of the parent node
void condenseTable(int startingIndex, int amount, char* ID){

    // Initial creation of the new parent node that will be added into the syntax tree
    NDR_ASTNode* parent = malloc(sizeof(NDR_ASTNode));
    NDR_InitASTNode(parent);
    NDR_SetASTNodeKeyword(parent, ID);
    NDR_SetASTNodeOrderNumber(parent, NDR_GetNumberOfASTNodes(NWrapper));
    NDR_SetASTNodeNodeType(parent, 1);

    // The loop below adds all nodes that are being grouped together as children nodes of the new parent node
    char* newEntry = malloc(2);
    strcpy(newEntry, "");
    for(int i = startingIndex; i < startingIndex+amount; i++){
        newEntry = realloc(newEntry, strlen(newEntry) + strlen(NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->token) + 2);
        strcat(newEntry, NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->token);
        strcat(newEntry, " ");
        // If the entry has not corresponding children meaning it has no node associated with it, a new node leaf is created to represent it and it is added to the parent node
        if(isTokenInTable(NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->keyword)){
            NDR_ASTNode* leaf = malloc(sizeof(NDR_ASTNode));
            NDR_InitASTNode(leaf);
            NDR_SetASTNodeKeyword(leaf, NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->keyword);
            NDR_SetASTNodeToken(leaf, NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->token);
            NDR_SetASTNodeOrderNumber(leaf, -1);
            NDR_SetASTNodeNodeType(leaf, 0);
            NDR_SetASTNodeLineNumber(leaf, NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->lineNumber);
            NDR_SetASTNodeColumnNumber(leaf, NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->columnNumber);

            NDR_AddChildASTNode(parent, leaf);
            NDR_IncASTTotalNode(NWrapper);

            /*leaf->token = malloc(strlen(getTreeTokenInfo(TTIWrapper, i)->tokenInfo->token)+1);
            strcpy(leaf->token, getTreeTokenInfo(TTIWrapper, i)->tokenInfo->token);
            leaf->keyword = malloc(strlen(getTreeTokenInfo(TTIWrapper, i)->tokenInfo->keyword)+1);
            strcpy(leaf->keyword, getTreeTokenInfo(TTIWrapper, i)->tokenInfo->keyword);
            leaf->numberOfChildren = 0;
            leaf->nodeType = 0;
            leaf->orderNumber = -1;
            leaf->lineNumber = getTreeTokenInfo(TTIWrapper, i)->tokenInfo->lineNumber;
            leaf->columnNumber = getTreeTokenInfo(TTIWrapper, i)->tokenInfo->columnNumber;
            addChildNode(parent, leaf);
            NWrapper->totalNodesInTree++;*/
        }
        // If the entry has children and has already been allocated a node then the node is searched for and added to the parent node
        else{
            for(size_t x = 0; x < NWrapper->numNodes; x++){
                if(NDR_GetTreeTokenInfo(TTIWrapper, i)->nodeNumber == NWrapper->nodes[x]->orderNumber){
                    NDR_AddChildASTNode(parent, NWrapper->nodes[x]);
                    break;
                }
            }
        }
    }
    //parent->lineNumber = getTreeTokenInfo(TTIWrapper, startingIndex)->tokenInfo->lineNumber;
    //parent->columnNumber = getTreeTokenInfo(TTIWrapper, startingIndex)->tokenInfo->columnNumber;
    NDR_SetASTNodeLineNumber(parent, NDR_GetTreeTokenInfo(TTIWrapper, startingIndex)->tokenInfo->lineNumber);
    NDR_SetASTNodeColumnNumber(parent, NDR_GetTreeTokenInfo(TTIWrapper, startingIndex)->tokenInfo->columnNumber);
    newEntry[strlen(newEntry) - 1] = '\0';

    // Updating the modifiedTokenTable so that the entries are still accurate after the nodes are grouped together and the table is consolidated
    NDR_SetTreeTokenInfoToken(NDR_GetTreeTokenInfo(TTIWrapper, startingIndex), newEntry);
    NDR_SetTreeTokenInfoKeyword(NDR_GetTreeTokenInfo(TTIWrapper, startingIndex), ID);
    NDR_GetTreeTokenInfo(TTIWrapper, startingIndex)->nodeNumber = NDR_GetNumberOfASTNodes(NWrapper);

    // For each row being consolidated update the modified token table to accurately reflect the changes
    if(amount > 1){
        size_t x;
        for(x = (startingIndex+1) ; x + (amount - 1) < NDR_GetNumberOfTreeTokens(TTIWrapper); x++){
            NDR_SetTreeTokenInfoToken(NDR_GetTreeTokenInfo(TTIWrapper, x), NDR_GetTreeTokenInfo(TTIWrapper, x + (amount - 1))->tokenInfo->token);
            NDR_SetTreeTokenInfoKeyword(NDR_GetTreeTokenInfo(TTIWrapper, x), NDR_GetTreeTokenInfo(TTIWrapper, x + (amount - 1))->tokenInfo->keyword);
            NDR_SetTreeTokenInfoLine(NDR_GetTreeTokenInfo(TTIWrapper, x), NDR_GetTreeTokenInfo(TTIWrapper, x + (amount - 1))->tokenInfo->lineNumber);
            NDR_SetTreeTokenInfoColumn(NDR_GetTreeTokenInfo(TTIWrapper, x), NDR_GetTreeTokenInfo(TTIWrapper, x + (amount - 1))->tokenInfo->columnNumber);
            NDR_GetTreeTokenInfo(TTIWrapper, x)->nodeNumber = NDR_GetTreeTokenInfo(TTIWrapper, x + (amount - 1))->nodeNumber;
        }
        TTIWrapper->numTokens = x;
    }

    NDR_IncASTTotalNode(NWrapper);
    // If the token equals *Accept but the modifiedTokenTable is not exhausted, add the parent node to the node array and continue
    // Otherwise, if the token equals *Accept make the parent the head and be done
    if(NDR_GetNumberOfTreeTokens(TTIWrapper) != 1 || strcmp(ID, "*Accept") != 0){
        NDR_AddNewASTNode(NWrapper, parent);
    }
    else if(strcmp(ID, "*Accept") == 0){
       NDR_ASThead = parent;
    }

    free(newEntry);

}


void InitializeSequenceMatchingState(SequenceMatchingState* matchingState){
    matchingState->allocatedLength = 50;
    matchingState->currentSequence = malloc(matchingState->allocatedLength);
    strcpy(matchingState->currentSequence, "");
    matchingState->potentialSequence = malloc(matchingState->allocatedLength);
    strcpy(matchingState->potentialSequence, "");

    matchingState->sequenceNumber = 0;
    matchingState->startIndex = 0;
    matchingState->endIndex = 0;

    matchingState->matched = false;
    matchingState->matchBeforeLookAhead = false;
    matchingState->highestMatchSeen = NOMATCH;
}

void DestroySequenceMatchingState(SequenceMatchingState* matchingState){
    free(matchingState->currentSequence);
    free(matchingState->potentialSequence);
}

void addStringToSequence(SequenceMatchingState* matchingState, char* src){
    if(strlen(matchingState->currentSequence) + strlen(src) > matchingState->allocatedLength - 5){
        matchingState->allocatedLength = (matchingState->allocatedLength * 2) + strlen(src);
        matchingState->currentSequence = realloc(matchingState->currentSequence, matchingState->allocatedLength);
        matchingState->potentialSequence = realloc(matchingState->potentialSequence, matchingState->allocatedLength);
    }
    strcat(matchingState->currentSequence, src);
}

void AcknowledgePotentialSequence(SequenceMatchingState* matchingState, int treeIndex, int patternIndex){
    matchingState->startIndex = (treeIndex+1) - matchingState->sequenceNumber;
    matchingState->endIndex = matchingState->sequenceNumber;
    CapturePotentialSequence(matchingState, NDR_GetSequenceInfo(PIWrapper, patternIndex)->keyword);
    matchingState->matched = true;
    matchingState->matchBeforeLookAhead = true;
    matchingState->highestMatchSeen = PARTIALMATCH;
}

void AcknowledgeCompleteSequence(SequenceMatchingState* matchingState){
    matchingState->matched = true;
    matchingState->highestMatchSeen = NOMATCH;
    strcpy(matchingState->currentSequence, "");
    matchingState->sequenceNumber = 0;
    matchingState->matchBeforeLookAhead = false;
}

void CapturePotentialSequence(SequenceMatchingState* matchingState, char* sequence){
    strcpy(matchingState->potentialSequence, sequence);
}

char* GetCapturedSequence(SequenceMatchingState* matchingState){
    return matchingState->potentialSequence;
}

bool IsPotentialSequence(SequenceMatchingState* matchingState, int RSIndex){
    return strlen(matchingState->currentSequence) <= strlen(NDR_GetSequenceInfo(PIWrapper, RSIndex)->sequence) &&
            strcmp(matchingState->currentSequence, NDR_GetSequenceInfo(PIWrapper, RSIndex)->sequence) == 0;
}

bool IsPartialSequence(SequenceMatchingState* matchingState, int RSIndex){
    return strlen(matchingState->currentSequence) <= strlen(NDR_GetSequenceInfo(PIWrapper, RSIndex)->sequence) &&
            memcmp(matchingState->currentSequence, NDR_GetSequenceInfo(PIWrapper, RSIndex)->sequence, strlen(matchingState->currentSequence)) == 0;
}

bool IsPotentialSequenceWrong(SequenceMatchingState* matchingState){
    return matchingState->matchBeforeLookAhead == true && matchingState->matched == true && matchingState->highestMatchSeen == COMPLETEMATCH;
}

bool IsCompleteSequence(SequenceMatchingState* matchingState){
    return matchingState->matchBeforeLookAhead == true && matchingState->matched == false && matchingState->highestMatchSeen == COMPLETEMATCH;
}




void InitializePSR(PStateRepresentation* pStateRepresentation){
    pStateRepresentation->newPattern = false;
    pStateRepresentation->startedID = false;
    pStateRepresentation->separatorWasPrevious = false;
}

void AcknowledgeFoundKeyword(PStateRepresentation* pStateRepresentation){
    pStateRepresentation->startedID = true;
    pStateRepresentation->newPattern = true;
    pStateRepresentation->separatorWasPrevious = false;
}
void AcknowledgeTokenSeparator(PStateRepresentation* pStateRepresentation){
    pStateRepresentation->separatorWasPrevious = true;
    pStateRepresentation->newPattern = true;
}
void AcknowledgeFoundToken(PStateRepresentation* pStateRepresentation){
    pStateRepresentation->newPattern = false;
    pStateRepresentation->separatorWasPrevious = false;
}

// getLongestParseSequence gets the longest parsing string in the parseTable
/*int getLongestParseSequence(){
    int longestSequence = 0;
    int count = 0;
    while(count < getNumberOfPatterns(PIWrapper)){
        if(strlen(getParseInfo(PIWrapper, count)->parsingString) > longestSequence)
            longestSequence = strlen(getParseInfo(PIWrapper, count)->parsingString);
        count++;
    }
    return longestSequence;
}*/

// findParseID finds the ID string in the parseTable
bool findParseID(char* ID){
    for(size_t i = 0; i < NDR_GetNumberOfSequences(PIWrapper); i++){
        if(strcmp(NDR_GetSequenceInfo(PIWrapper, i)->keyword, ID) == 0){
            return true;
        }
    }
    return false;
}

void removeColonEscape(char* string){
    for(size_t i = 0; i < strlen(string) - 1; i++){
        if(string[i] == '\\' && string[i+1] == ':'){
            for(size_t x = i; x < strlen(string); x++){
                    string[x] = string[x+1];
            }
        }
    }
}
// findEscapedParsing checks a string for escape sequences and places the indices of any escape character found into an integer array
void findEscapedParsing(char* string, int* indices){
    int current = 1;
    for(size_t i = 0; i < strlen(string) - 1; i++){
        if(string[i] == '\\' && (string[i+1] == ':' || string[i+1] == '|' || string[i+1] == '\\')){
            indices[current] = i+1;
            current++;
            i++;
        }
    }
    if (current == 1)
        indices[0] = 0;
    else
        indices[0] = current;
}

/*bool isEntryADuplicate(char* entry){
    for(int i = 0; i < getNumberOfPatterns(PIWrapper); i++){
        if(strcmp(getParseInfo(PIWrapper, i)->parsingString, entry) == 0){
            return true;
        }
    }
    return false;
}*/

// copyTTToMT copies the content of the token table and token locations
void copyTTToMT(NDR_TreeTokenInfoWrapper* tokenInfoWrapper){
    for (size_t i = 0; i < NDR_TIGetNumberOfTokens(TIWrapper); i++){
        NDR_AddTreeNewToken(tokenInfoWrapper);
        NDR_GetTreeTokenInfo(tokenInfoWrapper, i)->nodeNumber = -1;
        NDR_SetTreeTokenInfoKeyword(NDR_GetTreeTokenInfo(tokenInfoWrapper, i), NDR_TIGetTokenInfo(TIWrapper, i)->keyword);
        NDR_SetTreeTokenInfoToken(NDR_GetTreeTokenInfo(tokenInfoWrapper, i), NDR_TIGetTokenInfo(TIWrapper, i)->token);
        NDR_SetTreeTokenInfoLine(NDR_GetTreeTokenInfo(tokenInfoWrapper, i), NDR_TIGetTokenInfo(TIWrapper, i)->lineNumber);
        NDR_SetTreeTokenInfoColumn(NDR_GetTreeTokenInfo(tokenInfoWrapper, i), NDR_TIGetTokenInfo(TIWrapper, i)->columnNumber);
    }
}

// isTokenInTable returns true if the provided token is in the token table
bool isTokenInTable(char* token){
    for(size_t i = 0; i < NDR_TIGetNumberOfTokens(TIWrapper); i++){
        if(strcmp(NDR_TIGetTokenInfo(TIWrapper, i)->keyword, token) == 0){
            return true;
        }
    }
    return false;
}

bool sContainsInt(int* arr, int index){
    for (int i = 0; i < arr[0]; i++){
        if (arr[i] == index){
            return true;
        }
    }
    return false;
}

// The functions for printing are below

// printParseTable handles printing the parseTable
void NDR_PrintParseTable(){

    printf("\n\n************** Patterns ****************\n\n");

    for(size_t i = 0; i < NDR_GetNumberOfSequences(PIWrapper); i++){
        printf("%s: ", NDR_GetSequenceInfo(PIWrapper, i)->keyword);
        printf("%s ", NDR_GetSequenceInfo(PIWrapper, i)->sequence);
        printf("\n");
    }
    //printModifiedTokenTable();
}
// printModifiedTokenTable handles printing the ModifiedTokenTable
void NDR_PrintModifiedTokenTable(){

    printf("\n\n************** Tokens ****************\n\n");

    for(size_t i = 0; i < NDR_GetNumberOfTreeTokens(TTIWrapper); i++){
        printf("%s  ---   ", NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->token);
        printf("%s  ---   ", NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->keyword);
        printf("%u  ---   ", (unsigned int) NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->lineNumber);
        printf("%u  ---   ", (unsigned int) NDR_GetTreeTokenInfo(TTIWrapper, i)->tokenInfo->columnNumber);
        printf("%u", (unsigned int) NDR_GetTreeTokenInfo(TTIWrapper, i)->nodeNumber);
        printf("\n");
    }
}
