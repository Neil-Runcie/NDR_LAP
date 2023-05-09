
/*********************************************************************************
*                                     NDR Lexer                                  *
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

#include "ndr_lexer.h"
#include "ndr_fileprocessor.h"
#include "ndr_matchstate.h"
#include "ndr_statecategories.h"

#include "ndr_tokeninformation.h"
#include "ndr_regexstate.h"
#include "ndr_debug.h"

#define PCRRE2_REGEX_ENGINE

#ifdef PCRRE2_REGEX_ENGINE
#define PCRE2_STATIC
#define PCRE2_CODE_UNIT_WIDTH 8
#include "regex_engines/include/pcre2.h"
#endif // PCRRE2_REGEX_ENGINE
#ifdef NDR_REGEX_ENGINE
#include "NDR_REGEX.h"
#endif // NDR_REGEX_ENGINE

// the NEWLINEMULTIPLIER is used because the newline takes two bytes in windows '\r''\n' whereas linux uses one byte '\n'
#ifdef _WIN32
#define NEWLINEMULTIPLIER 1
#endif

#ifdef linux
#define NEWLINEMULTIPLIER 0
#endif


typedef struct LexerLineCategorizer {
    char** tokens;
    int numberOfTokens;
    NDR_StateCategories* categories;
} LexerLineCategorizer;

typedef struct StateRepresentation {
    bool started;
    bool startState;
    bool allowState;
    bool escapeState;
    bool endState;
} StateRepresentation;

typedef struct TokenMatchingState{
    char ch;
    char* currentToken;
    char* potentialFinalToken;
    int allocatedLength;

    int backtrackAmount;
    int numNewLinesSeen;

    int matchValue;
    int indexOfBestMatch;
    bool completeMatchFound;
    bool potentialMatchFound;
    int numberOfCompleteMatches;
    NDR_MatchState highestMatchSeen;
} TokenMatchingState;


static void InitializeTokenMatchingState(TokenMatchingState* matchingState);
static void DestroyTokenMatchingState(TokenMatchingState* matchingState);
static void ResetTokenMatchingState(TokenMatchingState* matchingState);
static void addCharToToken(TokenMatchingState* matchingState, char ch);
static void addStringToToken(TokenMatchingState* matchingState, char* src);
static char* getMatchToken(TokenMatchingState* matchingState);
static void AcknowledgePotentialMatch(TokenMatchingState* matchingState);
static void AcknowledgeCompleteMatch(TokenMatchingState* matchingState);
static bool completeMatchFound(TokenMatchingState* matchingState);
static bool hasMatchingStarted(TokenMatchingState* matchingState);
static size_t getNumberOfCompleteMatches(TokenMatchingState* matchingState);
static void calcBackTrack(TokenMatchingState* matchingState, char ch);
static int getBackTrackAmount(TokenMatchingState* matchingState);
static int getFileBackTrackAmount(TokenMatchingState* matchingState);
static void resetBackTrackAmount(TokenMatchingState* matchingState);
static void setMatchingChar(TokenMatchingState* matchingState, char ch);
void CapturePotentialMatch(TokenMatchingState* matchingState);
char* GetCapturedMatch(TokenMatchingState* matchingState);
static bool IsBestMatch(TokenMatchingState* matchingState, int index);
static void SetBestMatchIndex(TokenMatchingState* matchingState, int index);


static void InitializeStateRepresentation(StateRepresentation* stateRepresentation);
static void DestroyLexerLineCategorizer(LexerLineCategorizer* lineCategorizer);
static bool verifyLineTokens(LexerLineCategorizer* lineCategorizer, StateRepresentation* stateRepresentation);
static void ResetStates(StateRepresentation* stateRep);
static void ResetLineCategories(LexerLineCategorizer* lineCategorizer, int memAllocated);
static int containsCategory(LexerLineCategorizer* lineCategorizer, NDR_StateCategories category);
static bool HandleSettings(LexerLineCategorizer* lineCategorizer);
static bool IsOneTimeSettingSeen(LexerLineCategorizer* lineCategorizer);
static bool ProcessTokensAfterItems(LexerLineCategorizer* lineCategorizer, int index);
static int ExtractRegexStrings(char* regex, char** extractedStrings);

static bool doesCharMatchAllowRegex(int stateIndex, char* comparisonString);
static bool doesCharMatchEscapeRegex(int stateIndex, char* comparisonString);

static void updatefilePosition(int* lineNumber, int* columnNumber, char* token);

static void appendCharToString(char* dest, char src);
static void trimString(char* string);
static bool containsInt(int* arr, int index);
static void findEscaped(char* string, int* indices, int length);
static void removeEscaped(char* string);
static void lowerCaseString(char* lowerCaseArr, char* arr);
static char* lowerCaseStringReturn(char* arr, size_t neededLength);
static void findItems(char* string, int* indices);
static void stringsBetween(char* string, int* indices, char** answer);
static int lastOccurrence(char* string, char c, int length);
static void encapsulateString(const char* prepend, char* string, const char* append);


// AUTO_CAP adds ^ to the beginning and \z to the end of every regex descriptor in lexer config file
static bool AUTO_CAP = false;
// AUTO_TRIM removes the space from the beginning and end of every regex descriptor in lexer config file
static bool AUTO_TRIM = false;
// One Time Settings are below
// MATCH_ALL toggles error catching for failure to match all characters in lexer config file
static bool MATCH_ALL = true;
static bool MATCH_ALL_isSeen = false;


static NDR_RegexStateWrapper* RSWrapper = NULL;

NDR_TokenInformationWrapper* TIWrapper = NULL;

int NDR_Configure_Lexer(char* fileName){

    if(fileName == NULL || strcmp(fileName, "") == 0){
        printf("A non-empty filename must be provided for lexer configuration\n");
        return 1;
    }

    FILE *lexerConfigFile = fopen(fileName, "r");
    if(lexerConfigFile == NULL){
        printf("Cannot open the lexer configuration file");
        return 1;
    }

    NDR_FileInformation* fileInfo = malloc(sizeof(NDR_FileInformation));
    if(NDR_ProcessFile(fileName, fileInfo, " ") == 1){
        printf("Failed to read the lexer configuration file\n");
        return 1;
    }

    RSWrapper = malloc(sizeof(NDR_RegexStateWrapper));
    NDR_InitializeRegexStateWrapper(RSWrapper);

    LexerLineCategorizer* lineCategorizer = malloc(sizeof(LexerLineCategorizer));
    lineCategorizer->categories = malloc(sizeof(NDR_StateCategories) * NDR_GetLongestTokenAmount(fileInfo));
    char** extractedStrings = malloc(sizeof(char*) * NDR_GetLongestLineLength(fileInfo));

    StateRepresentation* stateRepresentation = malloc(sizeof(stateRepresentation));
    InitializeStateRepresentation(stateRepresentation);

    // beginningString is prepended to regex strings when AUTO_CAP is true to mark the beginning of a token for PCRE2
    // endingString is appended to regex strings when AUTO_CAP is true to mark the end of a token for PCRE2
    const char beginningString[] = "^";
    const char endingString[] = "\\z";


    int lexerLineNumber = 1;


    for(size_t i = 0; i < NDR_GetNumberOfLines(fileInfo); i++){
        lineCategorizer->tokens = NDR_GetTokens(NDR_GetLine(fileInfo, i));
        lineCategorizer->numberOfTokens = NDR_GetNumberOfTokens(NDR_GetLine(fileInfo, i));
        ResetLineCategories(lineCategorizer, NDR_GetLongestTokenAmount(fileInfo));

        if(strcmp(NDR_GetToken(NDR_GetLine(fileInfo, i), 0), "\n") == 0 || memcmp(NDR_GetToken(NDR_GetLine(fileInfo, i), 0), "//", 2) == 0){
            lexerLineNumber++;
            continue;
        }

        if(!verifyLineTokens(lineCategorizer, stateRepresentation)){
            printf("Error parsing lexer config file at line %i\n", lexerLineNumber);
            return 1;
        }

        // if statement to find if the states keyword has been used
        // only the start and end token are required, the allow and escape will be defaulted to accept all and accept none respectively
        if (containsCategory(lineCategorizer, STATES) != -1){
            int index = containsCategory(lineCategorizer, KEYWORD);
            if (index != -1){
                NDR_AddRegexState(RSWrapper);
                NDR_RSTrimAndSetKeyword(NDR_RSGetLastRegexState(RSWrapper), lineCategorizer->tokens[index]);
                NDR_RSSetLiteralFlag(NDR_RSGetLastRegexState(RSWrapper), false);
                NDR_RSSetStateFlag(NDR_RSGetLastRegexState(RSWrapper), true);
                NDR_RSSetCategory(NDR_RSGetLastRegexState(RSWrapper), lineCategorizer->categories[0]);
            }
            else{

                printf("The states token must be associated with a keyword");
                return 1;
            }
        }
        else if(containsCategory(lineCategorizer, STARTSTATE) != -1 ||
                containsCategory(lineCategorizer, ALLOWSTATE) != -1 ||
                containsCategory(lineCategorizer, ESCAPESTATE) != -1 ||
                containsCategory(lineCategorizer, ENDSTATE) != -1){

            int numberOfRegexStrings = ExtractRegexStrings(NDR_GetOriginalLine(NDR_GetLine(fileInfo, i)), extractedStrings);
            if(numberOfRegexStrings == -1){
                printf("Malformed item tokens at line %i\n", lexerLineNumber);
                return 1;
            }

            for (int counts = 0; counts < numberOfRegexStrings; counts++){
                if (AUTO_TRIM == true){
                    trimString(extractedStrings[counts]);
                }
                if (AUTO_CAP == true){
                    extractedStrings[counts] = realloc(extractedStrings[counts], strlen(extractedStrings[counts]) + strlen(beginningString) + strlen(endingString) + 1);
                    encapsulateString(beginningString, extractedStrings[counts], endingString);
                }
                removeEscaped(extractedStrings[counts]);
                int result = NDR_CheckAndAddStateRegex(RSWrapper, lineCategorizer->categories[0], extractedStrings[counts]);
                if(result == -1){
                    printf("\nDuplicate end symbol regex \"%s\" for keyword \"%s\" found on line %i\n", extractedStrings[counts], NDR_RSGetKeyword(NDR_RSGetLastRegexState(RSWrapper)), lexerLineNumber);
                    return 1;
                }
                else if(result == -2){
                    printf("\nError compiling symbol regex \"%s\" for keyword \"%s\" found on line %i\n", extractedStrings[counts], NDR_RSGetKeyword(NDR_RSGetLastRegexState(RSWrapper)), lexerLineNumber);
                    return 1;
                }
                else if(NDR_R == true){
                    printf("Success for token \"%s\" for keyword %s\n", extractedStrings[counts], NDR_RSGetKeyword(NDR_RSGetLastRegexState(RSWrapper)));
                }
                free(extractedStrings[counts]);
            }


            if(containsCategory(lineCategorizer, ENDSTATE) != -1){
                if(NDR_RSGetNumStartStates(NDR_RSGetLastRegexState(RSWrapper)) == 0){
                    printf("Missing start state token around line %i for keyword %s\n", lexerLineNumber, NDR_RSGetKeyword(NDR_RSGetLastRegexState(RSWrapper)));
                    return 1;
                }

                if(NDR_RSGetNumAllowStates(NDR_RSGetLastRegexState(RSWrapper)) == 0){
                    NDR_AddAllowRegex(NDR_RSGetLastRegexState(RSWrapper), "^[\\s\\S]\\z");
                }
                if(NDR_RSGetNumEscapeStates(NDR_RSGetLastRegexState(RSWrapper)) == 0){
                    NDR_AddEscapeRegex(NDR_RSGetLastRegexState(RSWrapper), "^(?!)\\z");
                }
                stateRepresentation->started = false;
            }
        }
        else if(containsCategory(lineCategorizer, STATES) == -1 && containsCategory(lineCategorizer, SETTING) == -1){
            NDR_AddRegexState(RSWrapper);

            char* items;
            int index = containsCategory(lineCategorizer, KEYWORD);
            // Decisioning for how to parse lines based on whether a keyword is present or literal modifier is present
            if (index != -1){
                items = strstr(NDR_GetOriginalLine(NDR_GetLine(fileInfo, i)), lineCategorizer->tokens[index]);
                items = items + strlen(lineCategorizer->tokens[index]);
                NDR_RSTrimAndSetKeyword(NDR_RSGetLastRegexState(RSWrapper), lineCategorizer->tokens[index]);
                NDR_RSSetLiteralFlag(NDR_RSGetLastRegexState(RSWrapper), false);
            }
            else{
                items = NDR_GetOriginalLine(NDR_GetLine(fileInfo, i));
                NDR_RSSetKeyword(NDR_RSGetLastRegexState(RSWrapper), "literal");
                NDR_RSSetLiteralFlag(NDR_RSGetLastRegexState(RSWrapper), true);
            }

            int numberOfRegexStrings = ExtractRegexStrings(items, extractedStrings);
            if(numberOfRegexStrings == -1){
                printf("Malformed item tokens at line %i\n", lexerLineNumber);
                return 1;
            }

            NDR_RSSetCategory(NDR_RSGetLastRegexState(RSWrapper), lineCategorizer->categories[0]);
            NDR_RSSetStateFlag(NDR_RSGetLastRegexState(RSWrapper), false);

            for(int counts = 0; counts < numberOfRegexStrings; counts++){

                if (AUTO_TRIM == true){
                    trimString(extractedStrings[counts]);
                }
                if (AUTO_CAP == true){
                    extractedStrings[counts] = realloc(extractedStrings[counts], strlen(extractedStrings[counts]) + strlen(beginningString) + strlen(endingString) + 1);
                    encapsulateString(beginningString, extractedStrings[counts], endingString);
                }
                removeEscaped(extractedStrings[counts]);

                int result = NDR_CheckAndAddStateRegex(RSWrapper, lineCategorizer->categories[0], extractedStrings[counts]);
                if(result == -1){
                    printf("Duplicate end symbol regex \"%s\" found on line %i\n", extractedStrings[counts], lexerLineNumber);
                    return 1;
                }
                else if(result == -2){
                    printf("\nError compiling symbol regex \"%s\" for keyword \"%s\" found on line %i\n", extractedStrings[counts], NDR_RSGetKeyword(NDR_RSGetLastRegexState(RSWrapper)), lexerLineNumber);
                    return 1;
                }
                free(extractedStrings[counts]);
            }
        }

        lexerLineNumber++;

    }

    if(NDR_RSGetNumberOfStates(RSWrapper) == 0){
        printf("\nNo lexer tokens were found in the lexer configuration file\n");
        return 1;
    }
    if(stateRepresentation->started == true){
        printf("\nMissing end state Token around line %i\n", lexerLineNumber);
        return 1;
    }
    if (NDR_ST == true){
        NDR_PrintSymbolTable();
    }

    fclose(lexerConfigFile);

    NDR_FreeFileInformation(fileInfo);
    free(fileInfo);
    free(stateRepresentation);
    DestroyLexerLineCategorizer(lineCategorizer);
    free(lineCategorizer);
    free(extractedStrings);

    return 0;

}


int CompareUsingRegex(TokenMatchingState* matchingState, int RSIndex, int RegIndex){

    matchingState->matchValue = NDR_RSGetMatchResult(NDR_RSGetRegexState(RSWrapper, RSIndex), getMatchToken(matchingState), STARTSTATE, RegIndex);

    if(matchingState->matchValue == 0) {
        printf("offset vector too small for \"%s\", using regex %s\n", getMatchToken(matchingState), NDR_RSGetStartRegex(NDR_RSGetRegexState(RSWrapper, RSIndex), RegIndex));
        return 1;
    }
    else if(matchingState->matchValue == PCRE2_ERROR_NOMATCH){
        if (NDR_M == true)
            printf("No match for \"%s\", using regex %s\n", getMatchToken(matchingState), NDR_RSGetStartRegex(NDR_RSGetRegexState(RSWrapper, RSIndex), RegIndex));
    }
    else if(matchingState->matchValue == PCRE2_ERROR_PARTIAL && matchingState->highestMatchSeen == NOMATCH){
        if (NDR_M == true)
            printf("Partial Match for %s, using regex %s\n", getMatchToken(matchingState), NDR_RSGetStartRegex(NDR_RSGetRegexState(RSWrapper, RSIndex), RegIndex));

        calcBackTrack(matchingState, matchingState->ch);
        AcknowledgePotentialMatch(matchingState);
    }
    else if (matchingState->matchValue == 1){
        if (NDR_M == true)
            printf("Match success for %s, using regex %s\n", getMatchToken(matchingState), NDR_RSGetStartRegex(NDR_RSGetRegexState(RSWrapper, RSIndex), RegIndex));

        if (IsBestMatch(matchingState, RSIndex) == true)
            SetBestMatchIndex(matchingState, RSIndex);

        resetBackTrackAmount(matchingState);
        AcknowledgeCompleteMatch(matchingState);
    }
    else if(matchingState->matchValue != PCRE2_ERROR_PARTIAL){
        printf("Matching error for \"%s\", using regex %s\n", getMatchToken(matchingState), NDR_RSGetStartRegex(NDR_RSGetRegexState(RSWrapper, RSIndex), RegIndex));
        return 1;
    }

    return 0;
}

int NDR_Lex(char* fileName){

    if(RSWrapper == NULL){
        printf("\nCall function \"int Configure_Lexer(char* fileName)\" to setup the lexer configuration before calling function \"int Lex(char* fileName)\"\n");
        return 1;
    }

    if(fileName == NULL || strcmp(fileName, "") == 0){
        printf("A non-empty filename must be provided for processing\n");
        return 1;
    }

    FILE *code = fopen(fileName, "r");
    if (code == NULL)
        printf("Cannot open code file");

    TokenMatchingState* matchingState = malloc(sizeof(TokenMatchingState));
    InitializeTokenMatchingState(matchingState);
    TIWrapper = malloc(sizeof(NDR_TokenInformationWrapper));
    NDR_InitTokenInfoWrapper(TIWrapper);


    int lineNumber = 1;
    int columnNumber = 1;

    if (NDR_R == true)
        printf("\n\n************** Regex Symbol Compilation ****************\n\n");

    if (NDR_M == true){
        printf("\n\n************** Text File Matching ****************\n\n");
        printf("line 1 ------------- column 1\n");
    }

    // Loop to go through the code file character by character and find matches using PCRE2
    while(matchingState->ch != EOF){

        setMatchingChar(matchingState, fgetc(code));
        addCharToToken(matchingState, matchingState->ch);
        matchingState->highestMatchSeen = NOMATCH;
        // For each entry in the symbol table we will make a comparison
        // This can be optimized by not checking already failed matches potentially
        for(size_t x = 0; x < NDR_RSGetNumberOfStates(RSWrapper); x++){
            for(size_t i = 0; i < NDR_RSGetNumStartStates(NDR_RSGetRegexState(RSWrapper, x)); i++){
                CompareUsingRegex(matchingState, x, i);
            }
        }

        if(completeMatchFound(matchingState) == true && NDR_RSGetStateFlag(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)) == true){

            if(getBackTrackAmount(matchingState) > 0){
                getMatchToken(matchingState)[strlen(getMatchToken(matchingState))-getBackTrackAmount(matchingState)] = '\0';
                fseek(code, -1 * (getFileBackTrackAmount(matchingState) + 1), SEEK_CUR);
            }
            else{
                getMatchToken(matchingState)[strlen(getMatchToken(matchingState)) - 1] = '\0';
                fseek(code, -1, SEEK_CUR);
            }

            // The below loop goes through a token after the start state token match has been found and checks to see if the token can be validated using the states
            int ch = 0;
            char sString[2];
            sString[1] = '\0';

            bool currentlyEscaped = false;
            bool allowMatch = false;
            bool escapeMatch = false;
            bool endMatch = false;
            bool endCheckComplete = false;

            TokenMatchingState* endMatchingState = malloc(sizeof(TokenMatchingState));
            InitializeTokenMatchingState(endMatchingState);

            while(ch != EOF){
                if (NDR_M == true)
                    printf("Currently matching: %s\n", getMatchToken(matchingState));

                ch = fgetc(code);
                if (ch == EOF){
                    printf("Reached end of file during parsing\n");
                    return 1;
                }
                fseek(code, -1, SEEK_CUR);
                sString[0] = (char) ch;

                allowMatch = doesCharMatchAllowRegex(matchingState->indexOfBestMatch, sString);
                escapeMatch = doesCharMatchEscapeRegex(matchingState->indexOfBestMatch, sString);

                ResetTokenMatchingState(endMatchingState);
                endCheckComplete = false;

                while(endMatchingState->ch != EOF && endCheckComplete == false){

                    setMatchingChar(endMatchingState, fgetc(code));
                    addCharToToken(endMatchingState, endMatchingState->ch);
                    endMatchingState->highestMatchSeen = NOMATCH;

                    for(size_t x = 0; x < NDR_RSGetNumberOfStates(RSWrapper); x++){
                        if(NDR_RSGetStateFlag(NDR_RSGetRegexState(RSWrapper, x)) == true && strcmp(NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)), NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, x))) == 0)
                        for(size_t i = 0; i < NDR_RSGetNumEndStates(NDR_RSGetRegexState(RSWrapper, x)); i++){
                            CompareUsingRegex(endMatchingState, x, i);
                        }
                    }

                    if(endMatchingState->completeMatchFound == false && endMatchingState->potentialMatchFound == false){
                        if(getBackTrackAmount(endMatchingState) > 0){
                            fseek(code, -1 * getFileBackTrackAmount(endMatchingState), SEEK_CUR);
                        }

                        endMatch = false;
                        endCheckComplete = true;
                        break;
                    }

                    if(completeMatchFound(endMatchingState) == true){
                        if(getBackTrackAmount(endMatchingState) > 0){
                            fseek(code, -1 * getFileBackTrackAmount(endMatchingState), SEEK_CUR);
                        }

                        getMatchToken(endMatchingState)[strlen(getMatchToken(endMatchingState))-(1+getBackTrackAmount(endMatchingState))] = '\0';
                        CapturePotentialMatch(endMatchingState);
                        endMatch = true;
                        endCheckComplete = true;

                        fseek(code, -1, SEEK_CUR);
                        updatefilePosition(&lineNumber, &columnNumber, getMatchToken(endMatchingState));

                        if (NDR_M == true)
                            printf("\nline %i ------------- column %i\n", lineNumber, columnNumber);

                        break;
                    }

                    endMatchingState->numberOfCompleteMatches = 0;
                    endMatchingState->potentialMatchFound = false;
                }


                // Need to decide whether or not to match end token based on whether or not the token is escaped
                if(escapeMatch == true && currentlyEscaped == false){
                    currentlyEscaped = true;
                }
                else if(escapeMatch == true && currentlyEscaped == true){
                    currentlyEscaped = false;
                    getMatchToken(matchingState)[strlen(getMatchToken(matchingState)) - 1] = '\0';
                }
                else if(endMatch == true && currentlyEscaped == true){
                    currentlyEscaped = false;
                    getMatchToken(matchingState)[strlen(getMatchToken(matchingState)) - 1] = '\0';
                }
                else if(endMatch == true && currentlyEscaped == false){
                    addStringToToken(matchingState, GetCapturedMatch(endMatchingState));
                    ch = fgetc(code);
                    break;
                }
                else if(allowMatch == true){
                    currentlyEscaped = false;
                }
                else{
                    printf("Found invalid character \"%c\" during parsing of state for keyword \"%s\"\n", ch, NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)));
                    return 1;
                }
                addCharToToken(matchingState, ch);
            }
            DestroyTokenMatchingState(endMatchingState);
            free(endMatchingState);
            // Entering the matched tokens into the tokenTable
            if(NDR_RSGetCategory(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)) == ERROR){
                printf("\nError token found: %s\n", getMatchToken(matchingState));
                return 1;
            }
            if(NDR_RSGetCategory(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)) == ACCEPT){
                NDR_AddNewToken(TIWrapper);
                NDR_SetTokenInfoKeyword(NDR_TIGetLastTokenInfo(TIWrapper), NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)));
                NDR_SetTokenInfoToken(NDR_TIGetLastTokenInfo(TIWrapper), getMatchToken(matchingState));
                NDR_SetTokenInfoLine(NDR_TIGetLastTokenInfo(TIWrapper), lineNumber);
                NDR_SetTokenInfoColumn(NDR_TIGetLastTokenInfo(TIWrapper), columnNumber);
            }
            // Resetting variables for finding tokens and updating line and column numbers

            matchingState->completeMatchFound = false;
            fseek(code, -1, SEEK_CUR);
            updatefilePosition(&lineNumber, &columnNumber, getMatchToken(matchingState));
            strcpy(getMatchToken(matchingState), "");
            matchingState->indexOfBestMatch = 0;

            if (NDR_M == true)
                printf("\nline %i ------------- column %i\n", lineNumber, columnNumber);
        }
        else if(completeMatchFound(matchingState) == true){
            getMatchToken(matchingState)[strlen(getMatchToken(matchingState))-(1+getBackTrackAmount(matchingState))] = '\0';
            if(getBackTrackAmount(matchingState) > 0){
                fseek(code, -1 * getFileBackTrackAmount(matchingState), SEEK_CUR);
            }

            if(NDR_RSGetCategory(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)) == ERROR){
                printf("\nError token found: %s\n", getMatchToken(matchingState));
                return 1;
            }
            if(NDR_RSGetCategory(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)) == ACCEPT){
                NDR_AddNewToken(TIWrapper);
                NDR_SetTokenInfoLine(NDR_TIGetLastTokenInfo(TIWrapper), lineNumber);
                NDR_SetTokenInfoColumn(NDR_TIGetLastTokenInfo(TIWrapper), columnNumber);
                NDR_SetTokenInfoToken(NDR_TIGetLastTokenInfo(TIWrapper), getMatchToken(matchingState));
                if(NDR_RSGetLiteralFlag(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)) == true)
                    NDR_SetTokenInfoKeyword(NDR_TIGetLastTokenInfo(TIWrapper), getMatchToken(matchingState));
                else
                    NDR_SetTokenInfoKeyword(NDR_TIGetLastTokenInfo(TIWrapper), NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, matchingState->indexOfBestMatch)));
            }
            // Resetting variables for finding tokens and updating line and column numbers
            fseek(code, -1, SEEK_CUR);
            updatefilePosition(&lineNumber, &columnNumber, getMatchToken(matchingState));
            matchingState->completeMatchFound = false;
            strcpy(getMatchToken(matchingState), "");
            matchingState->indexOfBestMatch = 0;



            if (NDR_M == true)
                printf("\nline %i ------------- column %i\n", lineNumber, columnNumber);
        }
        if(hasMatchingStarted(matchingState) && matchingState->ch == EOF){
            printf("\nIncomplete matching from line: %i column: %i\nPotentially an opening of item with no closing\n", lineNumber, columnNumber);
            return 1;
        }
        else if(getNumberOfCompleteMatches(matchingState) == 0 && strlen(getMatchToken(matchingState)) > 1 && matchingState->ch == EOF && MATCH_ALL == true){
            printf("\nMatching error from line: %i column: %i\n", lineNumber, columnNumber);
            return 1;
        }

        matchingState->numberOfCompleteMatches = 0;
        matchingState->potentialMatchFound = false;
    }


    if(NDR_TIGetNumberOfTokens(TIWrapper) == 0){
        printf("\nNo text was matched during parsing of the source file.\n");
        return 1;
    }
    if (NDR_TT == true)
        NDR_PrintTokenTable();
    if (NDR_TL == true)
        NDR_PrintTokenTableLocations();


    DestroyTokenMatchingState(matchingState);
    free(matchingState);
    NDR_FreeRegexStateWrapper(RSWrapper);
    free(RSWrapper);


    fclose(code);
    code = NULL;
    free(code);

    printf("\nLexical analysis successful\n");

    return 0;
}




// verifyTokens is used to validate each line of the lexer config file and assign the config values to the tokens array
bool verifyLineTokens(LexerLineCategorizer* lineCategorizer, StateRepresentation* stateRep){

    char* firstTokenLowerCase = malloc(strlen(lineCategorizer->tokens[0])+1);
    lowerCaseString(firstTokenLowerCase, lineCategorizer->tokens[0]);

    if(HandleSettings(lineCategorizer) == true){
        if(lineCategorizer->numberOfTokens > 1){
            printf("Tokens are not allowed after setting \"%s\"\n", lineCategorizer->tokens[0]);
            return false;
        }
        lineCategorizer->categories[0] = SETTING;

        if(IsOneTimeSettingSeen(lineCategorizer) == true){
            printf("\"%s\" should only be set once\n", lineCategorizer->tokens[0]);
            return false;
        }

    }
    else if(strcmp(firstTokenLowerCase, "accept") == 0 ||
            strcmp(firstTokenLowerCase, "ignore") == 0 ||
            strcmp(firstTokenLowerCase, "error") == 0){

        if(stateRep->started == true){
            printf("end of state not found for previous state declaration\n");
            return false;
        }

        bool literal = false;
        bool keyword = false;
        bool states = false;
        bool items = false;
        for(int x = 1; x < lineCategorizer->numberOfTokens; x++){
            if(lineCategorizer->tokens[x][strlen(lineCategorizer->tokens[x]) - 1] == '\n')
                lineCategorizer->tokens[x][strlen(lineCategorizer->tokens[x]) - 1] = '\0';
            if (lineCategorizer->tokens[x] == NULL)
                break;
            if(items == true){
                if(ProcessTokensAfterItems(lineCategorizer, x) == false){
                    printf("Tokens were found after the regex string\n");
                    return false;
                }
                continue;
            }
            else if ((strlen(lineCategorizer->tokens[x]) == 1 && strcmp(lowerCaseStringReturn(lineCategorizer->tokens[x], 1), "l") == 0)
                      || (strlen(lineCategorizer->tokens[x]) == 7 && strcmp(lowerCaseStringReturn(lineCategorizer->tokens[x], 7), "literal") == 0)){
                if (x != 1 || keyword == true){
                    printf("\"literal\" should be the second token and should not be used with \"keyword\"n");
                    return false;
                }
                else{
                    literal = true;
                    lineCategorizer->categories[x] = LITERAL;
                }
            }
            else if ((memcmp(lineCategorizer->tokens[x], "k{", 2) == 0 || memcmp(lineCategorizer->tokens[x], "K{", 2) == 0) && lineCategorizer->tokens[x][strlen(lineCategorizer->tokens[x]) - 1] == '}'){
                if(lineCategorizer->tokens[x][2] == '}'){
                   printf("No name found within keyword brackets\n");
                   return false;
                }
                else if(keyword == true){
                    printf("Cannot have more than one keyword\n");
                    return false;
                }
                else if(x != 1){
                    printf("keywords should be the second token\n");
                    return false;
                }
                else if(literal == true){
                    printf("keywords cannot be used with literal modifier\n");
                    return false;
                }
                else{
                    keyword = true;
                    lineCategorizer->categories[x] = KEYWORD;
                }
            }
            else if(strcmp(lowerCaseStringReturn(lineCategorizer->tokens[x], 7), "states:") == 0){

                if(strcmp(firstTokenLowerCase, "accept") == 0){
                    if(literal == true){
                        printf("\"literal\" cannot be used with \"states\" keyword\n");
                        return false;
                    }
                    else if (keyword != true){
                        printf("keyword must be given with \"states\" keyword\n");
                        return false;
                    }
                }
                states = true;
                stateRep->started = true;
                ResetStates(stateRep);
                lineCategorizer->categories[x] = STATES;
            }
            else if (lineCategorizer->tokens[x][0] == '{'){
                items = true;
                if(lineCategorizer->tokens[x][1] == '}'){
                    printf("No regex tokens found in between the braces\n");
                    return false;
                }
                if(ProcessTokensAfterItems(lineCategorizer, x) == false){
                    printf("Tokens were found after the regex string\n");
                    return false;
                }
            }
            else{
                printf("Unexpected token \"%s\"\n", lineCategorizer->tokens[x]);
                return false;
            }
        }
        if (items == false && states == false){
            printf("items cannot be found\n");
            return false;
        }



        if(strcmp(firstTokenLowerCase, "accept") == 0){
            lineCategorizer->categories[0] = ACCEPT;

            if (literal == false && keyword == false){
                printf("\"literal\" or keyword must be present\n");
                return false;
            }
        }
        if(strcmp(firstTokenLowerCase, "ignore") == 0){
            lineCategorizer->categories[0] = IGNORE;
        }
        if(strcmp(firstTokenLowerCase, "error") == 0){
            lineCategorizer->categories[0] = ERROR;
        }

    }
    else if(strcmp(firstTokenLowerCase, "start") == 0 ||
            strcmp(firstTokenLowerCase, "allow") == 0 ||
            strcmp(firstTokenLowerCase, "escape") == 0 ||
            strcmp(firstTokenLowerCase, "end") == 0){

        if(stateRep->started == false){
            printf("Unexpected state token\n");
            return false;
        }
        else if (lineCategorizer->numberOfTokens < 2){
            printf("missing regex\n");
            return false;
        }

        if(lineCategorizer->tokens[1][0] == '{'){
            if(lineCategorizer->tokens[1][1] == '}'){
                printf("No regex tokens found in between the braces\n");
                return false;
            }
            if(ProcessTokensAfterItems(lineCategorizer, 1) == false){
                printf("Tokens were found after the regex string\n");
                return false;
            }
        }
        else{
            printf("The regex string is expected as the second token surrounded by '{' and '}'\n");
            return false;
        }

        if(strcmp(firstTokenLowerCase, "start") == 0){
            if(stateRep->startState == true || stateRep->allowState == true || stateRep->escapeState == true || stateRep->endState == true){
                printf("The \"start\" token must be the first token and appear only once\n");
                return false;
            }
            stateRep->startState = true;
            lineCategorizer->categories[0] = STARTSTATE;
        }
        else if(strcmp(firstTokenLowerCase, "allow") == 0){
            if(stateRep->startState == false || stateRep->allowState == true){
                printf("The \"allow\" token must be the after the \"start\" token and appear only once if used\n");
                return false;
            }
            stateRep->allowState = true;
            lineCategorizer->categories[0] = ALLOWSTATE;
        }
        else if(strcmp(firstTokenLowerCase, "escape") == 0){
            if(stateRep->startState == false || stateRep->escapeState == true){
                printf("The \"escape\" token must be the after the \"start\" token and appear only once if used\n");
                return false;
            }
            stateRep->escapeState = true;
            lineCategorizer->categories[0] = ESCAPESTATE;
        }
        else if(strcmp(firstTokenLowerCase, "end") == 0){
            if(stateRep->startState == false || stateRep->endState == true){
                printf("The \"end\" token must be the last token and appear only once\n");
                return false;
            }
            stateRep->endState = true;
            lineCategorizer->categories[0] = ENDSTATE;
        }
    }
    else{
        printf("Unknown first token \"%s\"\n", lineCategorizer->tokens[0]);
        return false;
    }

    free(firstTokenLowerCase);
    return true;
}


/*
Tokens to manipulate TokenMatchingState structures
*/

void InitializeTokenMatchingState(TokenMatchingState* matchingState){
    matchingState->ch = 0;

    matchingState->allocatedLength = 50;
    matchingState->currentToken = malloc(matchingState->allocatedLength);
    strcpy(matchingState->currentToken, "");
    matchingState->potentialFinalToken = malloc(matchingState->allocatedLength);
    strcpy(matchingState->potentialFinalToken, "");

    matchingState->backtrackAmount = 0;
    matchingState->numNewLinesSeen = 0;

    matchingState->indexOfBestMatch = 0;
    matchingState->completeMatchFound = false;
    matchingState->potentialMatchFound = false;
    matchingState->numberOfCompleteMatches = 0;
    matchingState->highestMatchSeen = NOMATCH;
}

void DestroyTokenMatchingState(TokenMatchingState* matchingState){
    free(matchingState->currentToken);
    free(matchingState->potentialFinalToken);
}

void ResetTokenMatchingState(TokenMatchingState* matchingState){
    matchingState->ch = 0;

    strcpy(matchingState->currentToken, "");
    strcpy(matchingState->potentialFinalToken, "");

    matchingState->backtrackAmount = 0;
    matchingState->numNewLinesSeen = 0;

    matchingState->indexOfBestMatch = 0;
    matchingState->completeMatchFound = false;
    matchingState->potentialMatchFound = false;
    matchingState->numberOfCompleteMatches = 0;
    matchingState->highestMatchSeen = NOMATCH;
}

void addCharToToken(TokenMatchingState* matchingState, char ch){
    if(strlen(matchingState->currentToken) > matchingState->allocatedLength - 5){
        matchingState->allocatedLength = matchingState->allocatedLength * 2;
        matchingState->currentToken = realloc(matchingState->currentToken, matchingState->allocatedLength);
        matchingState->potentialFinalToken = realloc(matchingState->potentialFinalToken, matchingState->allocatedLength);
    }
    appendCharToString(matchingState->currentToken, ch);
}

void CapturePotentialMatch(TokenMatchingState* matchingState){
    strcpy(matchingState->potentialFinalToken, matchingState->currentToken);
}

char* GetCapturedMatch(TokenMatchingState* matchingState){
    return matchingState->potentialFinalToken;
}

bool IsBestMatch(TokenMatchingState* matchingState, int index){
    return (matchingState->completeMatchFound == false || matchingState->numberOfCompleteMatches == 0 || matchingState->completeMatchFound == false || index < matchingState->indexOfBestMatch);
}

void SetBestMatchIndex(TokenMatchingState* matchingState, int index){
    matchingState->indexOfBestMatch = index;
}

void addStringToToken(TokenMatchingState* matchingState, char* src){
    if(strlen(matchingState->currentToken) + strlen(src) > matchingState->allocatedLength - 5){
        matchingState->allocatedLength = (matchingState->allocatedLength * 2) + strlen(src);
        matchingState->currentToken = realloc(matchingState->currentToken, matchingState->allocatedLength);
        matchingState->potentialFinalToken = realloc(matchingState->potentialFinalToken, matchingState->allocatedLength);
    }
    strcat(matchingState->currentToken, src);
}

void AcknowledgePotentialMatch(TokenMatchingState* matchingState){
    matchingState->potentialMatchFound = true;
    matchingState->highestMatchSeen = PARTIALMATCH;
}

void AcknowledgeCompleteMatch(TokenMatchingState* matchingState){
    matchingState->completeMatchFound = true;
    matchingState->numberOfCompleteMatches++;
    matchingState->highestMatchSeen = COMPLETEMATCH;
}

bool completeMatchFound(TokenMatchingState* matchingState){
    return (matchingState->numberOfCompleteMatches == 0 && matchingState->potentialMatchFound == false && matchingState->completeMatchFound == true);
}

char* getMatchToken(TokenMatchingState* matchingState){
    return matchingState->currentToken;
}

bool hasMatchingStarted(TokenMatchingState* matchingState){
    return (matchingState->numberOfCompleteMatches == 1 && strlen(getMatchToken(matchingState)) > 1);
}

size_t getNumberOfCompleteMatches(TokenMatchingState* matchingState){
    return matchingState->numberOfCompleteMatches;
}

static void calcBackTrack(TokenMatchingState* matchingState, char ch){
    matchingState->backtrackAmount++;
    if(ch == ((int)'\n'))
        matchingState->numNewLinesSeen++;
}

static int getBackTrackAmount(TokenMatchingState* matchingState){
    return matchingState->backtrackAmount;
}

static int getFileBackTrackAmount(TokenMatchingState* matchingState){
    return (matchingState->backtrackAmount + (matchingState->numNewLinesSeen * NEWLINEMULTIPLIER));
}

static void resetBackTrackAmount(TokenMatchingState* matchingState){
    matchingState->backtrackAmount = 0;
    matchingState->numNewLinesSeen = 0;
}

static void setMatchingChar(TokenMatchingState* matchingState, char ch){
    matchingState->ch = (int) ch;
}

// containsToken checks the tokens array to see if a given tokenType is present
void DestroyLexerLineCategorizer(LexerLineCategorizer* lineCategorizer){
    free(lineCategorizer->categories);
}

int containsCategory(LexerLineCategorizer* lineCategorizer, NDR_StateCategories category){
    for(int i = 0; i < lineCategorizer->numberOfTokens; i++){
        if(lineCategorizer->categories[i] == category)
            return i;
    }
    return -1;
}
bool ProcessTokensAfterItems(LexerLineCategorizer* lineCategorizer, int index){
    int* indices = malloc(sizeof(int));
    indices = realloc(indices, sizeof(int)*strlen(lineCategorizer->tokens[index]));
    findEscaped(lineCategorizer->tokens[index], indices, strlen(lineCategorizer->tokens[index]));
    for(size_t i = 0; i < strlen(lineCategorizer->tokens[index])-1; i++){
        if(lineCategorizer->tokens[index][i+1] == '}' && containsInt(indices, i+1) == false){
            if(index != lineCategorizer->numberOfTokens - 1){
                free(indices);
                return false;
            }
            break;
        }
    }
    lineCategorizer->categories[index] = NOTAPPLICABLE;

    free(indices);
    return true;
}

bool HandleSettings(LexerLineCategorizer* lineCategorizer){

    bool isSetting = false;

    if(strcmp(lineCategorizer->tokens[0], "AUTO_CAP_ON") == 0){
        AUTO_CAP = true;
        isSetting = true;
    }
    else if(strcmp(lineCategorizer->tokens[0], "AUTO_TRIM_ON") == 0){
        AUTO_TRIM = true;
        isSetting = true;
    }
    else if(strcmp(lineCategorizer->tokens[0], "MATCH_ALL_ON") == 0){
        MATCH_ALL = true;
        isSetting = true;
    }
    else if(strcmp(lineCategorizer->tokens[0], "AUTO_CAP_OFF") == 0){
        AUTO_CAP = false;
        isSetting = true;
    }
    else if(strcmp(lineCategorizer->tokens[0], "AUTO_TRIM_OFF") == 0){
        AUTO_TRIM = false;
        isSetting = true;
    }
    else if(strcmp(lineCategorizer->tokens[0], "MATCH_ALL_OFF") == 0){
        MATCH_ALL = false;
        isSetting = true;
    }

    return isSetting;
}

bool IsOneTimeSettingSeen(LexerLineCategorizer* lineCategorizer){

    if(strcmp(lineCategorizer->tokens[0], "MATCH_ALL_ON") == 0 || strcmp(lineCategorizer->tokens[0], "MATCH_ALL_OFF") == 0){
        if(MATCH_ALL_isSeen != true)
            MATCH_ALL_isSeen = true;
        else
            return true;
    }

    return false;
}

int ExtractRegexStrings(char* regex, char** extractedStrings){

    char* stringToExtractFrom = malloc(strlen(regex)+1);
    char* end = malloc(strlen(regex)+1);
    strcpy(stringToExtractFrom, regex);

    if (stringToExtractFrom[strlen(stringToExtractFrom)-1] == '\n')
        stringToExtractFrom[strlen(stringToExtractFrom)-1] = '\0';

    char* items = strchr(stringToExtractFrom, '{');

    int* escapedIndices = malloc(sizeof(int) * strlen(items));
    findEscaped(items, escapedIndices, strlen(items));
    if(lastOccurrence(items, '}', strlen(items)) == -1 || containsInt(escapedIndices, lastOccurrence(items, '}', strlen(items))) == true){
        free(stringToExtractFrom);
        free(end);
        return -1;
    }
    items[lastOccurrence(items, '}', strlen(items))] = '\0';

    strcpy(end, items);

    int* indices = malloc(sizeof(int) * strlen(regex));
    findItems(end, indices);

    if(indices[0] == -1){
        free(stringToExtractFrom);
        free(end);
        return -1;
    }
    int numIndices = indices[0]/2;

    stringsBetween(end, indices, extractedStrings);

    free(stringToExtractFrom);
    free(end);
    free(escapedIndices);
    free(indices);

    return numIndices;
}


bool doesCharMatchAllowRegex(int stateIndex, char* comparisonString){
    int matchValue;
    for(size_t x = 0; x < NDR_RSGetNumberOfStates(RSWrapper); x++){
        if(strcmp(NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, stateIndex)), NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, x))) == 0){
            for(size_t i = 0; i < NDR_RSGetNumAllowStates(NDR_RSGetRegexState(RSWrapper, stateIndex)); i++){
                matchValue = NDR_RSGetMatchResult(NDR_RSGetRegexState(RSWrapper, x), comparisonString, ALLOWSTATE, i);
                if(matchValue > 0){
                    return true;
                }
            }
        }
    }
    return false;
}

bool doesCharMatchEscapeRegex(int stateIndex, char* comparisonString){
    int matchValue;
    for(size_t x = 0; x < NDR_RSGetNumberOfStates(RSWrapper); x++){
        if(strcmp(NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, stateIndex)), NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, x))) == 0){
            for(size_t i = 0; i < NDR_RSGetNumEscapeStates(NDR_RSGetRegexState(RSWrapper, stateIndex)); i++){
                matchValue = NDR_RSGetMatchResult(NDR_RSGetRegexState(RSWrapper, x), comparisonString, ESCAPESTATE, i);
                if(matchValue > 0){
                    return true;
                }
            }
        }
    }
    return false;
}

/*bool doesStringMatchEndRegex(FILE* code, int stateIndex, char* startString, char* endString, int MAX_END_LENGTH){
    strcpy(endString, startString);
    int EOFInt;
    int numNewLines = 0;
    int matchValue;
    bool endMatch = false;
    for(int x = 0; x < NDR_RSGetNumberOfStates(RSWrapper); x++){
        if(strcmp(NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, stateIndex)), NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, x))) == 0){
            for(int i = 0; i < NDR_RSGetNumEndStates(NDR_RSGetRegexState(RSWrapper, stateIndex)); i++){
                strcpy(endString, startString);
                numNewLines = 0;
                for(int endCount = 0; endCount < MAX_END_LENGTH; endCount++){
                    matchValue = NDR_RSGetMatchResult(NDR_RSGetRegexState(RSWrapper, x), endString, ENDSTATE, i);
                    if(matchValue > 0){
                        endMatch = true;
                        break;
                    }
                    else{
                        EOFInt = fgetc(code);
                        // The newline '\n' is represented as '\r''\n' which is two bytes
                        // fseek operates in bytes so for each newline we need to double the count from 1 to 2
                        if(((char)EOFInt) == '\n')
                            numNewLines++;
                        if (EOFInt == EOF){
                            strcpy(endString, "");
                            fseek(code, (-1 * endCount) + (-1 * numNewLines * NEWLINEMULTIPLIER), SEEK_CUR);
                            break;
                        }
                        else{
                            endString[endCount+1] = (char) EOFInt;
                            endString[endCount+2] = '\0';
                        }
                    }
                    if(endCount == MAX_END_LENGTH - 1){
                        strcpy(endString, "");
                        fseek(code, (-1 * MAX_END_LENGTH) + (-1 * numNewLines * NEWLINEMULTIPLIER), SEEK_CUR);
                    }
                }
                if(endMatch == true)
                    break;
            }
        }
    }
    return endMatch;
}*/



void updatefilePosition(int* lineNumber, int* columnNumber, char* token){
    int x = lastOccurrence(token, '\n', strlen(token));
    if(x != -1){
        for(size_t z = 0; z < strlen(token); z++){
            if(token[z] == '\n')
                (*lineNumber)++;
        }
        *columnNumber = strlen(token) - (x - 1);
    }
    else{
        *columnNumber += strlen(token);
    }
}



void InitializeStateRepresentation(StateRepresentation* stateRepresentation){
    stateRepresentation->started = false;
    stateRepresentation->startState = false;
    stateRepresentation->allowState = false;
    stateRepresentation->escapeState = false;
    stateRepresentation->endState = false;
}




void ResetStates(StateRepresentation* stateRep){
    stateRep->startState = false;
    stateRep->allowState = false;
    stateRep->escapeState = false;
    stateRep->endState = false;
}

void ResetLineCategories(LexerLineCategorizer* lineCategorizer, int memAllocated){
    for(int i = 0; i < memAllocated; i++){
        lineCategorizer->categories[i] = -1;
    }
}

// condenseString removes an item from a string and shortens the string to compensate
/*void condenseString(char* text, int num){
    int hold = (strlen(text) - num);
    for(int i = 0; i < strlen(text) - num; i++)
        text[i] = text[i + num];
    text[hold] = '\0';
}*/

void trimString(char* string){
    char* middle = malloc(strlen(string)+1);
    int first = 0;
    int last = 0;
    for(size_t i = 0; i < strlen(string); i++){
        if (string[i] != ' '){
            first = i;
            break;
        }
    }
    for(int i = strlen(string) - 1; i >= 0; i--){
        if (string[i] != ' '){
            last = i;
            break;
        }
    }
    for(int i = first; i <= last; i++){
        middle[i - first] = string[i];
    }

    middle[(last - first) + 1] = '\0';
    strcpy(string, middle);
    string[(last - first) + 1] = '\0';

    free(middle);
}

bool containsInt(int* arr, int index){
    for (int i = 0; i < arr[0]; i++){
        if (arr[i] == index){
            return true;
        }
    }
    return false;
}
void findEscaped(char* string, int* indices, int length){
    int current = 1;
    for(int i = 0; i < length - 1; i++){
        if(string[i] == '\\' && (string[i+1] == '{' || string[i+1] == '}' || string[i+1] == ',' || string[i+1] == '\\')){
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

void removeEscaped(char* string){
    for(size_t i = 0; i < strlen(string) - 1; i++){
        if(string[i] == '\\' && (string[i+1] == '{' || string[i+1] == '}' || string[i+1] == ',' || string[i+1] == '\\')){
            for(size_t x = i; x < strlen(string); x++){
                    string[x] = string[x+1];
            }
        }
    }
}

char* lowerCaseStringReturn(char* arr, size_t neededLength){
    char answer[50];
    static char lowered[50]; // The longest length needed is 7 for "literal" and "states:" and so on at the moment
    int intUsed;
    if(neededLength < strlen(arr))
        intUsed = neededLength;
    else
        intUsed = strlen(arr);
    for(int x = 0; x < intUsed; x++){
        answer[x] = (char) tolower((unsigned char)arr[x]);
    }
    memcpy(lowered, answer, intUsed);
    lowered[intUsed] = '\0';
    return lowered;
}

void findItems(char* string, int* indices){
    indices[1] = 1;
    int current = 2;
    bool ended = false;

    int* escapes = malloc(sizeof(int) * strlen(string));
    findEscaped(string, escapes, strlen(string));
    for(size_t x = 0; x < strlen(string)-1; x++){
        if(string[x+1] == ',' && containsInt(escapes, x+1) == false){
            indices[current] = x;
            current++;
            indices[current] = x+2;
            current++;
        }
        if(string[x+1] == '}' && containsInt(escapes, x+1) == false){
            indices[current] = x;
            ended = true;
            break;
        }
    }
    if (ended == true)
        indices[0] = current;
    else
        indices[0] = -1;
}

void stringsBetween(char* string, int* indices, char** answer){
    int i;
    for(int x = 0; x < (indices[0]/2); x++){
        answer[x] = malloc((indices[(2*x)+2] - indices[(2*x)+1]) + 2);
        for(i = 0; i < (indices[(2*x)+2] - indices[(2*x)+1]) + 1; i++){
            answer[x][i] = string[indices[((2*x)+1)] + i];
        }
        answer[x][i] = '\0';
    }
}

int lastOccurrence(char* string, char c, int length){
    for(int x = length - 1; x >= 0; x--){
        if (string[x] == c)
            return x+1;
    }
    return -1;
}


void lowerCaseString(char* lowerCaseArr, char* arr){
    strcpy(lowerCaseArr, arr);
    for(size_t x = 0; x < strlen(lowerCaseArr); x++){
        lowerCaseArr[x] = (char) tolower((unsigned char)lowerCaseArr[x]);
    }
}


void encapsulateString(const char* prepend, char* string, const char* append){
    char* strcatString = malloc(strlen(prepend) + strlen(string) + strlen(append) + 3);
    strcpy(strcatString, "");
    strcpy(strcatString, prepend);
    strcat(strcatString, string);
    strcat(strcatString, append);
    strcpy(string, strcatString);
    free(strcatString);
}

void appendCharToString(char* dest, char src){
    int length = strlen(dest);
    dest[length] = (char) src;
    dest[length+1] = '\0';
}

// printSymbolTable prints each part of the symbol table
void NDR_PrintSymbolTable(){
    printf("\n\n************** Symbols ****************\n\n");

    for(size_t i = 0; i < NDR_RSGetNumberOfStates(RSWrapper); i++){

        for(size_t x = 0; x < NDR_RSGetNumStartStates(NDR_RSGetRegexState(RSWrapper, i)); x++){
            printf("%s ", NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%i ", NDR_RSGetCategory(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%i ", NDR_RSGetLiteralFlag(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%s ", NDR_RSGetStartRegex(NDR_RSGetRegexState(RSWrapper, i), x));
            printf("\n");
        }
        for(size_t x = 0; x < NDR_RSGetNumAllowStates(NDR_RSGetRegexState(RSWrapper, i)); x++){
            printf("%s ", NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%i ", NDR_RSGetCategory(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%i ", NDR_RSGetLiteralFlag(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%s ", NDR_RSGetAllowRegex(NDR_RSGetRegexState(RSWrapper, i), x));
            printf("\n");
        }
        for(size_t x = 0; x < NDR_RSGetNumEscapeStates(NDR_RSGetRegexState(RSWrapper, i)); x++){
            printf("%s ", NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%i ", NDR_RSGetCategory(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%i ", NDR_RSGetLiteralFlag(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%s ", NDR_RSGetEscapeRegex(NDR_RSGetRegexState(RSWrapper, i), x));
            printf("\n");
        }
        for(size_t x = 0; x < NDR_RSGetNumEndStates(NDR_RSGetRegexState(RSWrapper, i)); x++){
            printf("%s ", NDR_RSGetKeyword(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%i ", NDR_RSGetCategory(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%i ", NDR_RSGetLiteralFlag(NDR_RSGetRegexState(RSWrapper, i)));
            printf("%s ", NDR_RSGetEndRegex(NDR_RSGetRegexState(RSWrapper, i), x));
            printf("\n");
        }
        printf("\n");
    }
}

// printTokenTable prints each part of the token table
void NDR_PrintTokenTable(){

    printf("\n\n************** Tokens ****************\n\n");

    for(size_t i = 0; i < NDR_TIGetNumberOfTokens(TIWrapper); i++){
        printf("%s  ---  %s\n", NDR_TIGetTokenInfo(TIWrapper, i)->token, NDR_TIGetTokenInfo(TIWrapper, i)->keyword);
    }
}

void NDR_PrintTokenTableLocations(){

    printf("\n\n************** Token Locations ****************\n\n");

    for(size_t i = 0; i < NDR_TIGetNumberOfTokens(TIWrapper); i++){
        printf("%s: line - %u  --- column - %u\n", NDR_TIGetTokenInfo(TIWrapper, i)->token, (unsigned int) NDR_TIGetTokenInfo(TIWrapper, i)->lineNumber, (unsigned int) NDR_TIGetTokenInfo(TIWrapper, i)->columnNumber);
    }
}
