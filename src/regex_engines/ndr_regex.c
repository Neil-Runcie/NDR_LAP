
/*********************************************************************************
*                                    NDR Regex                                   *
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
#include "ndr_regex.h"
#include "ndr_regextracker.h"

bool IsPathOptional(NDR_RegexNode* follow);

int checkAllButNewLine(char comp);
int checkEverything(char comp);
int checkDecimalDigit(char comp);
int checkNotDecimalDigit(char comp);
int checkWhiteSpace(char comp);
int checkNotWhiteSpace(char comp);
int checkWordChar(char comp);
int checkNotWordChar(char comp);

void NDR_InitRegex(NDR_Regex* cRegex);

void NDR_DestroyRegexGraph(NDR_Regex* head);



//Based on a provided regex string, create a reference graph for later matching the regex to other strings
int NDR_CompileRegex(NDR_Regex* cRegex, char* regexString){

    // Initialize needed values
    if(cRegex->initialized == true){
        NDR_DestroyRegex(cRegex);
        NDR_InitRegex(cRegex);
    }

    // If the matching pattern is empty, set the flag and exit
    if(strcmp(regexString, "") == 0){
        cRegex->isEmpty = true;
        NDR_RemoveRNodeChild(cRegex->start);
        cRegex->initialized = true;
        return 0;
    }



    // compilation state variables to understand the compilation time context
    char previousChar = '\0';
    bool isCurrentlyEscaped = false;
    bool startedCharClass = false;
    bool orJustSeen = false;

    // Initializing the stacks that will hold the regex nodes during parsing
    NDR_RNodeStack* startStack = malloc(sizeof(NDR_RNodeStack));
    NDR_InitRNodeStack(startStack);
    NDR_RNodeStack* endStack = malloc(sizeof(NDR_RNodeStack));
    NDR_InitRNodeStack(endStack);
    // Pushing the starting node into the start and end stack
    NDR_RNodeStackPush(startStack, cRegex->start);
    NDR_RNodeStackPush(endStack, cRegex->start);

    // loop through each character in the regex string
    for(int x = 0; x < strlen(regexString); x++){

        // If the escape character is found, set the state variables accordingly and go to the next iteration
        if(regexString[x] == '\\' && isCurrentlyEscaped == false){
            isCurrentlyEscaped = true;
            previousChar = '\\';
            continue;
        }

        // if the first char in string is the begin string anchor
        if(x == 0 && regexString[x] == '$'){
            cRegex->beginString = true;
        }
        // if the last char in string is the end string anchor and it is not escaped
        else if(x == strlen(regexString) - 1 && isCurrentlyEscaped == false && regexString[x] == '%'){
            cRegex->endString = true;
        }
        // Perform look ahead for use of the or operator '|' without a word following it
        else if(orJustSeen == true && regexString[x] != '(' ){
            sprintf(cRegex->errorMessage, "Invalid use of '|' operator in regex at char %i. A parentheses enclosed \"word\" must follow the '|' symbol", x+1);
            NDR_DestroyRegexStack(startStack);
            NDR_DestroyRegexStack(endStack);
            free(startStack);
            free(endStack);
            return -1;
        }
        //Otherwise handle all characters
        else{
            // When the special character '(' is seen, it is assumed to be the beginning of a new "word"
            // Place a new item on the stack to isolate the new word and set the path in the graph
            if(regexString[x] == '(' && isCurrentlyEscaped == false && startedCharClass == false){
                NDR_RegexNode* newNode = malloc(sizeof(NDR_RegexNode));
                NDR_InitRegexNode(newNode);
                //free(newNode->acceptChars);
                NDR_RNodeStackPush(startStack, newNode);
                NDR_RNodeStackPeek(startStack)->wordStart = true;
                // if the or operator '|' was just seen set the beginning of the graph path to or for retroactive updating
                if(orJustSeen == true){
                    NDR_RNodeStackPeek(startStack)->orPath = true;
                }
                NDR_RNodeStackPush(endStack, NDR_RNodeStackPeek(startStack));
            }
            // When the special character ')' is seen, it is assumed to be the end of a "word"
            else if(regexString[x] == ')' && isCurrentlyEscaped == false && startedCharClass == false){
                if(NDR_IsRNodeEmpty(NDR_RNodeStackPeek(endStack)) == false){
                    NDR_InitRegexNode(NDR_RNodeStackPeek(endStack)->children[0]);
                    NDR_RNodeStackSet(endStack, NDR_RNodeStackPeek(endStack)->children[0]);
                }
                NDR_RNodeStackPeek(endStack)->wordEnd = true;
                NDR_RNodeStackPeek(endStack)->wordReference = NDR_RNodeStackPeek(startStack);


                if(NDR_RNodeStackPeek(startStack)->orPath == true){
                    NDR_RNodeStackPeek(endStack)->orPath = true;

                    NDR_RegexNode* holdStart = NDR_RNodeStackPop(startStack);
                    NDR_RegexNode* holdEnd = NDR_RNodeStackPop(endStack);
                    NDR_RegexNode* follow = NDR_RNodeStackPeek(startStack);
                    while(follow != NDR_RNodeStackPeek(endStack)){

                        if(follow->orPath == true){
                            NDR_AddRNodeChild(follow, holdStart->children[0]);
                            follow = holdStart->children[0];
                            while(follow->children[0] != holdEnd){
                                follow = follow->children[0];
                            }
                            NDR_DestroyRegexNode(holdStart);
                            free(holdStart);
                            free(follow->children[0]->children[0]);
                            NDR_DestroyRegexNode(follow->children[0]);
                            free(follow->children[0]);

                            follow->children[0] = NDR_RNodeStackPeek(endStack);

                            break;
                        }
                        else{
                            follow = follow->children[0];
                        }
                    }

                    if(regexString[x+1] == '|'){
                        orJustSeen = true;
                        x++;
                    }
                }
                else if(strlen(regexString) > x+1 && regexString[x+1] == '|'){

                    if(NDR_RNodeStackPeek(startStack)->orPath == false){
                        NDR_RNodeStackPeek(startStack)->orPath = true;
                        NDR_RNodeStackPeek(endStack)->orPath = true;
                        NDR_RegexNode* holdStart = NDR_RNodeStackPop(startStack);
                        NDR_RegexNode* holdEnd = NDR_RNodeStackPop(endStack);
                        NDR_RegexNode* follow = NDR_RNodeStackPeek(startStack);
                        bool isOr = false;
                        // While loop may never be hit
                        while(follow != NDR_RNodeStackPeek(endStack)){
                            if(follow->orPath == true){
                                NDR_AddRNodeChild(follow, holdStart->children[0]);

                                follow = holdStart->children[0];
                                while(follow->children[0] != holdEnd){
                                    follow = follow->children[0];
                                }
                                NDR_DestroyRegexNode(holdStart);
                                free(holdStart);
                                NDR_DestroyRegexNode(follow->children[0]);
                                free(follow->children[0]);
                                follow->children[0] = NDR_RNodeStackPeek(endStack);
                                isOr = true;
                                break;
                            }
                            else{
                                follow = follow->children[0];
                            }
                        }
                        if(isOr == false){
                            free(NDR_RNodeStackPeek(endStack)->children[0]);
                            NDR_RNodeStackPeek(endStack)->children[0] = holdStart;
                            NDR_RNodeStackSet(endStack, holdEnd);
                        }

                    }

                    orJustSeen = true;

                    x++;
                }
                else if(strlen(regexString) > x+1){

                    if(regexString[x+1] == '*'){
                        NDR_RNodeStackPeek(startStack)->repeatPath = true;
                        NDR_RNodeStackPeek(endStack)->repeatPath = true;
                        NDR_RNodeStackPeek(startStack)->optionalPath = true;
                        NDR_RNodeStackPeek(endStack)->optionalPath = true;
                        NDR_RNodeStackPeek(startStack)->minMatches = 0;
                        NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                        NDR_RNodeStackPeek(endStack)->minMatches = 0;
                        NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                        x++;
                    }
                    else if(regexString[x+1] == '+'){
                        NDR_RNodeStackPeek(startStack)->repeatPath = true;
                        NDR_RNodeStackPeek(endStack)->repeatPath = true;
                        NDR_RNodeStackPeek(startStack)->minMatches = 1;
                        NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                        NDR_RNodeStackPeek(endStack)->minMatches = 1;
                        NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                        x++;
                    }
                    else if(regexString[x+1] == '?'){
                        NDR_RNodeStackPeek(startStack)->optionalPath = true;
                        NDR_RNodeStackPeek(endStack)->optionalPath = true;
                        x++;
                    }
                    else if(regexString[x+1] == '{'){
                        x++;
                        if(strlen(regexString) <= x+3){
                            sprintf(cRegex->errorMessage, "Invalid numerator in regex at char %i", x+1);
                            NDR_DestroyRegexStack(startStack);
                            NDR_DestroyRegexStack(endStack);
                            free(startStack);
                            free(endStack);
                            return -1;
                        }
                        // Initialize the repeat numbers
                        char* repeatNum1 = malloc((strlen(regexString) - x) + 2);
                        char* repeatNum2 = malloc((strlen(regexString) - x) + 2);
                        strcpy(repeatNum1, "1");
                        strcpy(repeatNum2, "1");
                        bool isCommaPresent = false;
                        int jump = 0;
                        for(int i = x+1; i < strlen(regexString) && regexString[i] != '}'; i++){
                            if(isdigit(regexString[i]) > 0 && isCommaPresent == false){
                                repeatNum1[i - (x+1)] = regexString[i];
                                repeatNum1[(i - (x+1)) + 1] = '\0';
                            }
                            else if(regexString[i] == ','){
                                isCommaPresent = true;
                            }
                            else if(isdigit(regexString[i]) > 0 && isCommaPresent == true){
                                repeatNum2[i - ((x+1) + strlen(repeatNum1) + 1)] = regexString[i];
                                repeatNum2[(i - ((x+1) + strlen(repeatNum1) + 1)) + 1] = '\0';
                            }
                            else{
                                sprintf(cRegex->errorMessage, "Invalid numerator in regex at char %i. Numerators should either contain one number or two numbers separated by a comma %c", x+1, regexString[x]);
                                NDR_DestroyRegexStack(startStack);
                                NDR_DestroyRegexStack(endStack);
                                free(startStack);
                                free(endStack);
                                return -1;
                            }
                            jump = i;
                        }
                        x = jump+1;

                        NDR_RNodeStackPeek(startStack)->repeatPath = true;
                        NDR_RNodeStackPeek(endStack)->repeatPath = true;
                        NDR_RNodeStackPeek(startStack)->minMatches = atoi(repeatNum1);
                        NDR_RNodeStackPeek(endStack)->minMatches = atoi(repeatNum1);
                        if(isCommaPresent == false){
                            NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum1);
                            NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum1);
                        }
                        else{
                            NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum2);
                            NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum2);
                        }
                        free(repeatNum1);
                        free(repeatNum2);
                    }

                    ContinueAfterWord(startStack, endStack);

                }
                else{
                    ContinueAfterWord(startStack, endStack);
                }

                continue;
            }
            else if(regexString[x] == '[' && isCurrentlyEscaped == false){

                if(startedCharClass == false){
                    NDR_RegexNode* newNode = malloc(sizeof(NDR_RegexNode));
                    NDR_InitRegexNode(newNode);
                    NDR_RNodeStackPush(startStack, newNode);
                    NDR_RNodeStackPush(endStack, NDR_RNodeStackPeek(startStack));

                    startedCharClass = true;

                    if(strlen(regexString) > x+1){
                        if(regexString[x+1] == '^'){
                            NDR_RNodeStackPeek(endStack)->negatedClass = true;
                        }
                    }
                }
                else if(startedCharClass == true){
                    sprintf(cRegex->errorMessage, "Invalid character class in regex at char %i", x+1);
                    NDR_DestroyRegexStack(startStack);
                    NDR_DestroyRegexStack(endStack);
                    free(startStack);
                    free(endStack);
                    return -1;
                }
            }
            else if(regexString[x] == ']' && isCurrentlyEscaped == false){
                if(startedCharClass == false){
                    sprintf(cRegex->errorMessage, "Invalid character class in regex at char %i", x+1);
                    NDR_DestroyRegexStack(startStack);
                    NDR_DestroyRegexStack(endStack);
                    free(startStack);
                    free(endStack);
                    return -1;
                }
                else if(startedCharClass == true){

                    if(strlen(regexString) > x+1){
                        if(regexString[x+1] == '*'){
                            NDR_RNodeStackPeek(startStack)->minMatches = 0;
                            NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                            NDR_RNodeStackPeek(endStack)->minMatches = 0;
                            NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                            x++;
                        }
                        else if(regexString[x+1] == '+'){

                            NDR_RNodeStackPeek(startStack)->minMatches = 1;
                            NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                            NDR_RNodeStackPeek(endStack)->minMatches = 1;
                            NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                            x++;
                        }
                        else if(regexString[x+1] == '?'){

                            NDR_RNodeStackPeek(endStack)->minMatches = 0;
                            NDR_RNodeStackPeek(endStack)->maxMatches = 1;
                            x++;
                        }
                        else if(regexString[x+1] == '{'){
                            x++;
                            if(strlen(regexString) <= x+3){
                                sprintf(cRegex->errorMessage, "Invalid character class in regex at char %i", x+1);
                                NDR_DestroyRegexStack(startStack);
                                NDR_DestroyRegexStack(endStack);
                                free(startStack);
                                free(endStack);
                                return -1;
                            }
                            // trying to repeat a nonexistent character is invalid
                            else if(NDR_RNodeStackPeek(endStack)->numberOfChars == 0){
                                sprintf(cRegex->errorMessage, "Invalid character class in regex at char %i", x+1);
                                NDR_DestroyRegexStack(startStack);
                                NDR_DestroyRegexStack(endStack);
                                free(startStack);
                                free(endStack);
                                return -1;
                            }
                            // Initialize the repeat numbers
                            char* repeatNum1 = malloc((strlen(regexString) - x) + 2);
                            char* repeatNum2 = malloc((strlen(regexString) - x) + 2);
                            strcpy(repeatNum1, "1");
                            strcpy(repeatNum2, "1");
                            bool isCommaPresent = false;
                            int jump = 0;
                            for(int i = x+1; i < strlen(regexString) && regexString[i] != '}'; i++){
                                if(isdigit(regexString[i]) > 0 && isCommaPresent == false){
                                    repeatNum1[i - (x+1)] = regexString[i];
                                    repeatNum1[(i - (x+1)) + 1] = '\0';
                                }
                                else if(regexString[i] == ','){
                                    isCommaPresent = true;
                                }
                                else if(isdigit(regexString[i]) > 0 && isCommaPresent == true){
                                    repeatNum2[i - ((x+1) + strlen(repeatNum1) + 1)] = regexString[i];
                                    repeatNum2[(i - ((x+1) + strlen(repeatNum1) + 1)) + 1] = '\0';
                                }
                                else{
                                    sprintf(cRegex->errorMessage, "Invalid numerator in regex at char %i. Numerators should either contain one number or two numbers separated by a comma %c", x+1, regexString[x]);
                                    NDR_DestroyRegexStack(startStack);
                                    NDR_DestroyRegexStack(endStack);
                                    free(startStack);
                                    free(endStack);
                                    return -1;
                                }
                                jump = i;
                            }
                            x = jump+1;


                            NDR_RNodeStackPeek(startStack)->minMatches = atoi(repeatNum1);
                            NDR_RNodeStackPeek(endStack)->minMatches = atoi(repeatNum1);
                            if(isCommaPresent == false){
                                NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum1);
                                NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum1);
                            }
                            else{
                                NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum2);
                                NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum2);
                            }
                            free(repeatNum1);
                            free(repeatNum2);
                        }
                    }

                    ContinueAfterWord(startStack, endStack);

                    startedCharClass = false;
                }
            }
            else if(regexString[x] == '{' && isCurrentlyEscaped == false && startedCharClass == false){
                sprintf(cRegex->errorMessage, "Invalid '{' operator at char %i", x+1);
                NDR_DestroyRegexStack(startStack);
                NDR_DestroyRegexStack(endStack);
                free(startStack);
                free(endStack);
                return -1;
            }
            else if(regexString[x] == '}' && isCurrentlyEscaped == false && startedCharClass == false){
                sprintf(cRegex->errorMessage, "Invalid numerator closing '}' at char %i", x+1);
                NDR_DestroyRegexStack(startStack);
                NDR_DestroyRegexStack(endStack);
                free(startStack);
                free(endStack);
                return -1;
            }
            else if(regexString[x] == '?' && isCurrentlyEscaped == false && startedCharClass == false){
                sprintf(cRegex->errorMessage, "Invalid '?' operator at char %i", x+1);
                NDR_DestroyRegexStack(startStack);
                NDR_DestroyRegexStack(endStack);
                free(startStack);
                free(endStack);
                return -1;
            }
            else if(regexString[x] == '*' && isCurrentlyEscaped == false && startedCharClass == false){
                sprintf(cRegex->errorMessage, "Invalid '*' operator at char %i", x+1);
                NDR_DestroyRegexStack(startStack);
                NDR_DestroyRegexStack(endStack);
                free(startStack);
                free(endStack);
                return -1;
            }
            else if(regexString[x] == '+' && isCurrentlyEscaped == false && startedCharClass == false){
                sprintf(cRegex->errorMessage, "Invalid '+' operator at char %i", x+1);
                NDR_DestroyRegexStack(startStack);
                NDR_DestroyRegexStack(endStack);
                free(startStack);
                free(endStack);
                return -1;
            }
            else if(regexString[x] == '|' && isCurrentlyEscaped == false && startedCharClass == false){
                sprintf(cRegex->errorMessage, "The '|' \"or\" operator at char %i must be used after a parentheses enclosed \"word\" or it must be escaped for literal use", x+1);
                NDR_DestroyRegexStack(startStack);
                NDR_DestroyRegexStack(endStack);
                free(startStack);
                free(endStack);
                return -1;
            }
            else if(startedCharClass == true){
                if(isCurrentlyEscaped == true){
                    if(regexString[x] == '-'){
                        NDR_AddRNodeChar(NDR_RNodeStackPeek(endStack), regexString[x]);
                    }
                    else{
                        if(HandleSpecialCharacters(NDR_RNodeStackPeek(endStack), regexString[x]) != 0){
                            sprintf(cRegex->errorMessage, "Invalid special character %c at char %i", regexString[x], x+1);
                            NDR_DestroyRegexStack(startStack);
                            NDR_DestroyRegexStack(endStack);
                            free(startStack);
                            free(endStack);
                            return -1;
                        }
                    }
                }
                else{
                    if(regexString[x] == '-'){
                        if(x != strlen(regexString) - 1){
                            int difference = (int)previousChar - (int)regexString[x+1];
                            if(difference < 0)
                                difference = difference * -1;

                            int smallest = 0;
                            if(previousChar < regexString[x+1])
                                smallest = (int) previousChar;
                            else
                                smallest = (int) regexString[x+1];

                            for(int ch = smallest; ch <= smallest + difference; ch++){
                                NDR_AddRNodeChar(NDR_RNodeStackPeek(endStack), (char) ch);
                            }
                            x++;
                        }
                        else{
                            sprintf(cRegex->errorMessage, "Invalid '-' operator at char %i", x+1);
                            NDR_DestroyRegexStack(startStack);
                            NDR_DestroyRegexStack(endStack);
                            free(startStack);
                            free(endStack);
                            return -1;
                        }

                    }
                    else if(regexString[x] == '.'){
                        NDR_RNodeStackPeek(endStack)->allButNewLine = true;
                    }
                    else
                        NDR_AddRNodeChar(NDR_RNodeStackPeek(endStack), regexString[x]);
                }
            }
            else{// Normal character not in char class

                /*if(NDR_RNodeStackPeek(endStack)->numberOfChars != 0 || NDR_RNodeStackPeek(endStack)->wordEnd == true){
                    NDR_RNodeStackSet(endStack, NDR_RNodeStackPeek(endStack)->children[0]);
                    NDR_InitRegexNode(NDR_RNodeStackPeek(endStack));
                }*/
                if(NDR_IsRNodeEmpty(NDR_RNodeStackPeek(endStack)) == false){
                    NDR_RNodeStackSet(endStack, NDR_RNodeStackPeek(endStack)->children[0]);
                    NDR_InitRegexNode(NDR_RNodeStackPeek(endStack));
                }

                if(isCurrentlyEscaped == true){
                    if(HandleSpecialCharacters(NDR_RNodeStackPeek(endStack), regexString[x]) != 0){
                        sprintf(cRegex->errorMessage, "Invalid special character %c at char %i", regexString[x], x+1);
                        NDR_DestroyRegexStack(startStack);
                        NDR_DestroyRegexStack(endStack);
                        free(startStack);
                        free(endStack);
                        return -1;
                    }
                }
                else if(regexString[x] == '.'){
                    NDR_RNodeStackPeek(endStack)->allButNewLine = true;
                }
                else{
                    NDR_AddRNodeChar(NDR_RNodeStackPeek(endStack), regexString[x]);
                }

                if(strlen(regexString) > x+1){

                    if(regexString[x+1] == '*'){

                        NDR_RNodeStackPeek(startStack)->minMatches = 0;
                        NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                        NDR_RNodeStackPeek(endStack)->minMatches = 0;
                        NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                        //ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '+'){

                        NDR_RNodeStackPeek(startStack)->minMatches = 1;
                        NDR_RNodeStackPeek(startStack)->maxMatches = -1;
                        NDR_RNodeStackPeek(endStack)->minMatches = 1;
                        NDR_RNodeStackPeek(endStack)->maxMatches = -1;
                        //ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '?'){

                        NDR_RNodeStackPeek(endStack)->minMatches = 0;
                        NDR_RNodeStackPeek(endStack)->maxMatches = 1;
                        //ContinueAfterWord(startStack, endStack);
                        x++;
                    }
                    else if(regexString[x+1] == '{'){
                        x++;
                        if(strlen(regexString) <= x+3){
                            sprintf(cRegex->errorMessage, "Invalid numerator in regex at char %i", x+1);
                            NDR_DestroyRegexStack(startStack);
                            NDR_DestroyRegexStack(endStack);
                            free(startStack);
                            free(endStack);
                            return -1;
                        }
                        // trying to repeat a nonexistent character is invalid
                        else if(NDR_RNodeStackPeek(endStack)->numberOfChars == 0){
                            sprintf(cRegex->errorMessage, "Invalid numerator in regex at char %i", x+1);
                            NDR_DestroyRegexStack(startStack);
                            NDR_DestroyRegexStack(endStack);
                            free(startStack);
                            free(endStack);
                            return -1;
                        }
                        // Initialize the repeat numbers
                        char* repeatNum1 = malloc((strlen(regexString) - x) + 2);
                        char* repeatNum2 = malloc((strlen(regexString) - x) + 2);
                        strcpy(repeatNum1, "1");
                        strcpy(repeatNum2, "1");
                        bool isCommaPresent = false;
                        int jump = 0;
                        for(int i = x+1; i < strlen(regexString) && regexString[i] != '}'; i++){
                            if(isdigit(regexString[i]) > 0 && isCommaPresent == false){
                                repeatNum1[i - (x+1)] = regexString[i];
                                repeatNum1[(i - (x+1)) + 1] = '\0';
                            }
                            else if(regexString[i] == ','){
                                isCommaPresent = true;
                            }
                            else if(isdigit(regexString[i]) > 0 && isCommaPresent == true){
                                repeatNum2[i - ((x+1) + strlen(repeatNum1) + 1)] = regexString[i];
                                repeatNum2[(i - ((x+1) + strlen(repeatNum1) + 1)) + 1] = '\0';
                            }
                            else{
                                sprintf(cRegex->errorMessage, "Invalid numerator in regex at char %i. Numerators should either contain one number or two numbers separated by a comma %c", x+1, regexString[x]);
                                NDR_DestroyRegexStack(startStack);
                                NDR_DestroyRegexStack(endStack);
                                free(startStack);
                                free(endStack);
                                return -1;
                            }
                            jump = i;
                        }
                        x = jump+1;


                        NDR_RNodeStackPeek(startStack)->minMatches = atoi(repeatNum1);
                        NDR_RNodeStackPeek(endStack)->minMatches = atoi(repeatNum1);
                        if(isCommaPresent == false){
                            NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum1);
                            NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum1);
                        }
                        else{
                            NDR_RNodeStackPeek(startStack)->maxMatches = atoi(repeatNum2);
                            NDR_RNodeStackPeek(endStack)->maxMatches = atoi(repeatNum2);
                        }
                        free(repeatNum1);
                        free(repeatNum2);
                        //ContinueAfterWord(startStack, endStack);
                    }

                }


            }

        }

        isCurrentlyEscaped = false;
        previousChar = regexString[x];
        orJustSeen = false;

    }



    if(NDR_IsRNodeEmpty(NDR_RNodeStackPeek(endStack)) == false){
        NDR_InitRegexNode(NDR_RNodeStackPeek(endStack)->children[0]);
        NDR_RNodeStackPeek(endStack)->children[0]->end = true;
        NDR_RemoveRNodeChild(NDR_RNodeStackPeek(endStack)->children[0]);
    }
    else{
        NDR_RNodeStackPeek(endStack)->end = true;
        NDR_RemoveRNodeChild(NDR_RNodeStackPeek(endStack));
    }

    if(startedCharClass == true){
        sprintf(cRegex->errorMessage, "Invalid character class in regex");
        NDR_DestroyRegexStack(startStack);
        NDR_DestroyRegexStack(endStack);
        free(startStack);
        free(endStack);
        return -1;
    }
    NDR_RNodeStackPop(endStack);
    if(NDR_RNodeStackIsEmpty(endStack) == false){
        sprintf(cRegex->errorMessage, "Invalid word in regex");
        NDR_DestroyRegexStack(startStack);
        NDR_DestroyRegexStack(endStack);
        free(startStack);
        free(endStack);
        return -1;
    }

    /*NDR_RegexNode* x = cRegex->start;
    printf("\n\nSTART\n");
    while(x->end != true){
        printf("AETH %i\n", x);
        x = x->children[0];
    }
    printf("END\n\n");*/


    cRegex->initialized = true;

    NDR_DestroyRegexStack(startStack);
    NDR_DestroyRegexStack(endStack);
    free(startStack);
    free(endStack);

    return 0;

 }


/// Compare a string to a pre-compiled regex graph
NDR_MatchResult NDR_MatchRegex(NDR_Regex* cRegex, char* token){

    if(cRegex->initialized == false){
        printf("Regex is not compiled yet\n");
        return NDR_REGEX_FAILURE;
    }
    // Compare the NDR_CharDescriptor** parts of NDR_Regex type to each consecutive character within the token string

    if(strcmp(token, "") == 0 && cRegex->isEmpty == true){
        return NDR_REGEX_COMPLETEMATCH;
    }
    else if(strcmp(token, "") == 0 || cRegex->isEmpty == true){
        return NDR_REGEX_NOMATCH;
    }

    // Setting match state variables for tracking the state during matching
    NDR_MatchResult result = NDR_REGEX_NOMATCH;
    int currentIndex = 0;
    int numTimesMatched = 0;

    // Setting up the stack for keeping track of all word paths within the graph
    NDR_RegexNode* follow = cRegex->start->children[0];
    NDR_TrackerStack* wordReferences = malloc(sizeof(NDR_TrackerStack));
    NDR_InitTrackerStack(wordReferences);

    for(int i = 0; i < strlen(token); i++){

        if(follow->end == true && cRegex->endString == true){
            NDR_DestroyRegexTrackerStack(wordReferences);
            free(wordReferences);
            return NDR_REGEX_NOMATCH;
        }

        while(follow->end != true){

            if(follow->wordStart == true){

                NDR_RegexTracker* ref = malloc(sizeof(NDR_RegexTracker));
                NDR_InitRTracker(ref, follow);
                NDR_TrackerStackPush(wordReferences, ref);
                NDR_TrackerStackPeek(wordReferences)->stringPosition = i;
                follow = follow->children[0];

                continue;
            }
            else if(follow->wordEnd == true){

                if(NDR_TrackerStackPeek(wordReferences)->reference->repeatPath == true &&
                   NDR_TrackerStackPeek(wordReferences)->reference->maxMatches > NDR_TrackerStackPeek(wordReferences)->numberOfRepeats){

                    (NDR_TrackerStackPeek(wordReferences)->numberOfRepeats)++;
                    follow = NDR_TrackerStackPeek(wordReferences)->reference;
                    NDR_TrackerStackPeek(wordReferences)->stringPosition = i;
                }
                else{
                    NDR_TrackerStackPop(wordReferences);
                }

                follow = follow->children[0];

                continue;

            }

            if(IsCharacterAccepted(follow, token[i]) == true){
                numTimesMatched++;
                result = NDR_REGEX_PARTIALMATCH;

                if(numTimesMatched >= follow->minMatches){
                    if(numTimesMatched >= follow->maxMatches){
                        if(follow->children[0]->end == true && strlen(token)-1 == i){
                            NDR_DestroyRegexTrackerStack(wordReferences);
                            free(wordReferences);
                            return NDR_REGEX_COMPLETEMATCH;
                        }
                    }
                    if(numTimesMatched >= follow->maxMatches && follow->maxMatches != -1){
                        follow = follow->children[0];
                        numTimesMatched = 0;
                    }

                    if(NDR_TrackerStackIsEmpty(wordReferences) == false){
                        if(follow->wordEnd == true && follow->children[0]->end == true){
                            continue;
                        }
                    }

                    break;
                }
                else if(numTimesMatched < follow->minMatches){
                    i++;
                }

            }
            else{
                bool isWord = false;
                while(NDR_TrackerStackIsEmpty(wordReferences) == false){
                    isWord = true;
                    if(NDR_TrackerStackPeek(wordReferences)->reference->optionalPath == true){
                        i = NDR_TrackerStackPeek(wordReferences)->stringPosition;
                        NDR_RegexTracker* holdRef = NDR_TrackerStackPop(wordReferences);

                        while(follow->wordReference != holdRef->reference){
                            follow = follow->children[0];
                        }
                        follow = follow->children[0];

                    }
                    else if(NDR_TrackerStackPeek(wordReferences)->reference->repeatPath == true){

                        if(NDR_TrackerStackPeek(wordReferences)->reference->minMatches > NDR_TrackerStackPeek(wordReferences)->numberOfRepeats && cRegex->beginString == true){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                NDR_DestroyRegexTrackerStack(wordReferences);
                                free(wordReferences);
                                return NDR_REGEX_NOMATCH;
                            }
                            else
                                continue;
                        }
                        else if(NDR_TrackerStackPeek(wordReferences)->reference->minMatches > NDR_TrackerStackPeek(wordReferences)->numberOfRepeats){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                i = currentIndex++;
                                follow = cRegex->start->children[0];
                                numTimesMatched = 0;
                            }
                            else
                                continue;
                        }
                        else if(NDR_TrackerStackPeek(wordReferences)->reference->minMatches <= NDR_TrackerStackPeek(wordReferences)->numberOfRepeats){
                            i = NDR_TrackerStackPeek(wordReferences)->stringPosition;

                            while(follow->wordReference != NDR_TrackerStackPeek(wordReferences)->reference){
                                follow = follow->children[0];
                            }
                            follow = follow->children[0];
                            numTimesMatched = 0;
                            NDR_TrackerStackPop(wordReferences);
                        }

                    }
                    else if(NDR_TrackerStackPeek(wordReferences)->reference->orPath == true){
                        if(NDR_TrackerStackPeek(wordReferences)->reference->numberOfChildren - 1 <= NDR_TrackerStackPeek(wordReferences)->currentChild && cRegex->beginString == true){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                NDR_DestroyRegexTrackerStack(wordReferences);
                                free(wordReferences);
                                return NDR_REGEX_NOMATCH;
                            }
                            else
                                continue;
                        }
                        else if(NDR_TrackerStackPeek(wordReferences)->reference->numberOfChildren - 1 <= NDR_TrackerStackPeek(wordReferences)->currentChild){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                i = currentIndex++;
                                follow = cRegex->start->children[0];
                                numTimesMatched = 0;
                            }
                            else
                                continue;
                        }
                        else if(NDR_TrackerStackPeek(wordReferences)->reference->numberOfChildren - 1 > NDR_TrackerStackPeek(wordReferences)->currentChild){
                            i = NDR_TrackerStackPeek(wordReferences)->stringPosition;
                            follow = NDR_TrackerStackPeek(wordReferences)->reference->children[++(NDR_TrackerStackPeek(wordReferences)->currentChild)];
                            numTimesMatched = 0;
                        }

                    }
                    else{
                        if(cRegex->beginString == true){
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                NDR_DestroyRegexTrackerStack(wordReferences);
                                free(wordReferences);
                                return NDR_REGEX_NOMATCH;
                            }
                            else
                                continue;
                        }
                        else{
                            NDR_TrackerStackPop(wordReferences);
                            if(NDR_TrackerStackIsEmpty(wordReferences) == true){
                                i = currentIndex++;
                                follow = cRegex->start->children[0];
                                numTimesMatched = 0;
                            }
                            else
                                continue;
                        }
                    }
                    break;
                }
                if(isWord == true)
                    continue;

                if(follow->minMatches > numTimesMatched && cRegex->beginString == true){
                    NDR_DestroyRegexTrackerStack(wordReferences);
                    free(wordReferences);
                    return NDR_REGEX_NOMATCH;
                }
                else if(follow->minMatches > numTimesMatched){
                    i = currentIndex++;
                    follow = cRegex->start->children[0];
                    numTimesMatched = 0;
                    while(NDR_TrackerStackIsEmpty(wordReferences) == false){
                        NDR_TrackerStackPop(wordReferences);
                    }
                    result = NDR_REGEX_NOMATCH;
                }
                else if(follow->minMatches <= numTimesMatched){// If matching and there is a failure to match after the min number of matches has been met then proceed to the next node and restart
                    numTimesMatched = 0;
                    follow = follow->children[0];
                    i--;
                }

                break;

            }



        }

    }


    NDR_DestroyRegexTrackerStack(wordReferences);
    free(wordReferences);

    if(follow->end == false && IsPathOptional(follow) == false){
        return result;
    }

    return NDR_REGEX_COMPLETEMATCH;
}

bool IsPathOptional(NDR_RegexNode* follow){

    NDR_TrackerStack* wordReferences = malloc(sizeof(NDR_TrackerStack));
    NDR_InitTrackerStack(wordReferences);

    while(follow->end != true){
        if(follow->wordStart == true){
            NDR_RegexTracker* ref = malloc(sizeof(NDR_RegexTracker));
            NDR_InitRTracker(ref, follow);
            NDR_TrackerStackPush(wordReferences, ref);
        }
        else if(follow->wordEnd == true){
            NDR_TrackerStackPop(wordReferences);
        }
        else if(follow->minMatches != 0 && NDR_TrackerStackIsEmpty(wordReferences) == false){

            while(NDR_TrackerStackIsEmpty(wordReferences) == false){

                if(NDR_TrackerStackPeek(wordReferences)->reference->optionalPath == true){
                    while(follow->wordReference != NDR_TrackerStackPeek(wordReferences)->reference){
                        follow = follow->children[0];
                    }
                    NDR_TrackerStackPop(wordReferences);
                    break;
                }
                else if(NDR_TrackerStackPeek(wordReferences)->reference->orPath == true){
                    if(NDR_TrackerStackPeek(wordReferences)->reference->numberOfChildren - 1 <= NDR_TrackerStackPeek(wordReferences)->currentChild){
                        NDR_DestroyRegexTrackerStack(wordReferences);
                        free(wordReferences);
                        return false;
                    }
                    else{
                        follow = NDR_TrackerStackPeek(wordReferences)->reference->children[++(NDR_TrackerStackPeek(wordReferences)->currentChild)];
                    }
                    break;
                }
                else{
                    NDR_TrackerStackPop(wordReferences);
                    if((NDR_TrackerStackIsEmpty(wordReferences) == true)){
                        NDR_DestroyRegexTrackerStack(wordReferences);
                        free(wordReferences);
                        return false;
                    }
                }
            }
        }
        else if(follow->minMatches != 0 && NDR_TrackerStackIsEmpty(wordReferences) == true){
            NDR_DestroyRegexTrackerStack(wordReferences);
            free(wordReferences);
            return false;
        }
        follow = follow->children[0];
    }

    NDR_DestroyRegexTrackerStack(wordReferences);
    free(wordReferences);

    return true;
}

int checkAllButNewLine(char comp){
    char compare[] = {'\n'};
    int length = 1;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return -1;
        }
    }

    return 0;
}

int checkEverything(char comp){
    return 0;
}

int checkDecimalDigit(char comp){
    char compare[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int length = 10;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return 0;
        }
    }

    return -1;
}

int checkNotDecimalDigit(char comp){
    char compare[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int length = 10;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return -1;
        }
    }

    return 0;
}

int checkWhiteSpace(char comp){
    char compare[] = {' ', '\t'};
    int length = 2;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return 0;
        }
    }

    return -1;
}

int checkNotWhiteSpace(char comp){
    char compare[] = {' ', '\t'};
    int length = 2;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return -1;
        }
    }

    return 0;
}

int checkWordChar(char comp){
    char compare[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_'};
    int length = 63;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return 0;
        }
    }

    return -1;
}

int checkNotWordChar(char comp){
    char compare[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_'};
    int length = 63;

    for(int i = 0; i < length; i++){
        if(compare[i] == comp){
            return -1;
        }
    }

    return 0;
}

int HandleSpecialCharacters(NDR_RegexNode* node, char comp){

    // Below are the special characters for denoting specific classes of characters
    if(comp == 'N'){
        node->allButNewLine = true;
    }
    else if(comp == 'e'){
        node->everything = true;
    }
    else if(comp == 'E'){
        node->nothing = true;
    }
    else if(comp == 'd'){
        node->decimalDigit = true;
    }
    else if(comp == 'D'){
        node->notDecimalDigit = true;
    }
    else if(comp == 's'){
        node->whiteSpace = true;
    }
    else if(comp == 'S'){
        node->notWhiteSpace = true;
    }
    else if(comp == 'w'){
        node->wordChar = true;
    }
    else if(comp == 'W'){
        node->notWordChar = true;
    }
    // Below are the special characters for structural specification within the regex engine
    else if(comp == '['){
        NDR_AddRNodeChar(node, '[');
    }
    else if(comp == ']'){
        NDR_AddRNodeChar(node, ']');
    }
    else if(comp == '('){
        NDR_AddRNodeChar(node, '(');
    }
    else if(comp == ')'){
        NDR_AddRNodeChar(node, ')');
    }
    else if(comp == '{'){
        NDR_AddRNodeChar(node, '{');
    }
    else if(comp == '}'){
        NDR_AddRNodeChar(node, '}');
    }
    else if(comp == '|'){
        NDR_AddRNodeChar(node, '|');
    }
    else if(comp == '$'){
        NDR_AddRNodeChar(node, '$');
    }
    else if(comp == '%'){
        NDR_AddRNodeChar(node, '%');
    }
    else if(comp == '?'){
        NDR_AddRNodeChar(node, '?');
    }
    else if(comp == '*'){
        NDR_AddRNodeChar(node, '*');
    }
    else if(comp == '+'){
        NDR_AddRNodeChar(node, '+');
    }
    else if(comp == '.'){
        NDR_AddRNodeChar(node, '.');
    }
    else if(comp == '^'){
        NDR_AddRNodeChar(node, '^');
    }
    // Below are the special characters within the c language
    else if(comp == 'a'){
        NDR_AddRNodeChar(node, '\a');
    }
    else if(comp == 'b'){
        NDR_AddRNodeChar(node, '\b');
    }
    else if(comp == 'f'){
        NDR_AddRNodeChar(node, '\f');
    }
    else if(comp == 'n'){
        NDR_AddRNodeChar(node, '\n');
    }
    else if(comp == 'r'){
        NDR_AddRNodeChar(node, '\r');
    }
    else if(comp == 't'){
        NDR_AddRNodeChar(node, '\t');
    }
    else if(comp == 'v'){
        NDR_AddRNodeChar(node, '\v');
    }
    else if(comp == '\\'){
        NDR_AddRNodeChar(node, '\\');
    }
    else{
        return -1;
    }

    return 0;
}

bool IsCharacterAccepted(NDR_RegexNode* node, char comp){

    bool validChar = false;

    for(int x = 0; x < node->numberOfChars; x++){
        //printf("trying %c %c %i %i\n", comp, node->acceptChars[x], node->minMatches, node->maxMatches);
        if(comp == node->acceptChars[x]){
            //printf("matched %c %c\n", comp, node->acceptChars[x]);
            validChar = true;
            break;
        }
    }

    if(node->allButNewLine == true){//printf("trying allButNewLine %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkAllButNewLine(comp) == 0){//printf("matched allButNewLine %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->everything == true){//printf("trying everything %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkEverything(comp) == 0){//printf("matched everything %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->decimalDigit == true){//printf("trying decimalDigit %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkDecimalDigit(comp) == 0){//printf("matched decimalDigit %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
   if(node->notDecimalDigit == true){//printf("trying notDecimalDigit %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkNotDecimalDigit(comp) == 0){//printf("matched notDecimalDigit %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->whiteSpace == true){//printf("trying whiteSpace %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkWhiteSpace(comp) == 0){//printf("matched whiteSpace %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->notWhiteSpace == true){//printf("trying notWhiteSpace %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkNotWhiteSpace(comp) == 0){//printf("matched notWhiteSpace %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }
    if(node->wordChar == true){//printf("trying wordChar %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkWordChar(comp) == 0){//printf("matched wordChar %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }

    if(node->notWordChar == true){//printf("trying notWordChar %c %i %i\n", comp, node->minMatches, node->maxMatches);
        if(checkNotWordChar(comp) == 0){//printf("matched notWordChar %c\n", comp);
            validChar = true;
        }
        else
            validChar = false;
    }

    if(node->negatedClass){
        if(validChar == true)
            validChar = false;
        else
            validChar = true;
    }

    return validChar;
}


void NDR_InitRegex(NDR_Regex* cRegex){
    cRegex->initialized = false;
    cRegex->beginString = false;
    cRegex->endString = false;
    cRegex->isEmpty = false;
    strcpy(cRegex->errorMessage, "");
    cRegex->start = malloc(sizeof(NDR_RegexNode));
    NDR_InitRegexNode(cRegex->start);
    cRegex->start->start = true;
}

void NDR_DestroyRegex(NDR_Regex* graph){
    NDR_DestroyRegexGraph(graph);
}

void NDR_DestroyRegexGraph(NDR_Regex* head){

    if(head->initialized == true){
        if(head != NULL){
            size_t duplicatePostDepthTrackerCount = 0;
            size_t dupeTrackerMemoryAllocated = 50;

            NDR_RNodeStack* depthTracker = malloc(sizeof(NDR_RNodeStack));
            NDR_InitRNodeStack(depthTracker);
            NDR_RegexNode** duplicateTracker = malloc(sizeof(NDR_RegexNode*) * dupeTrackerMemoryAllocated);

            NDR_RegexNode* follow = head->start;

            NDR_RNodeStackPush(depthTracker, follow);

            size_t count;
            while(!NDR_RNodeStackIsEmpty(depthTracker)){
                count = 0;
                while(count < follow->numberOfChildren){
                    if(!NDR_RNodeDuplicate(follow->children[count], duplicateTracker, duplicatePostDepthTrackerCount)){
                        follow = follow->children[count];
                        NDR_RNodeStackPush(depthTracker, follow);
                        count = 0;
                        continue;
                    }
                    count++;
                }

                if(!NDR_RNodeDuplicate(follow, duplicateTracker, duplicatePostDepthTrackerCount)){
                    duplicateTracker[duplicatePostDepthTrackerCount++] = (follow);
                    if(duplicatePostDepthTrackerCount >= dupeTrackerMemoryAllocated - 2){
                        dupeTrackerMemoryAllocated = dupeTrackerMemoryAllocated * 2;
                        duplicateTracker = realloc(duplicateTracker, sizeof(NDR_RegexNode*) * dupeTrackerMemoryAllocated);
                    }
                }

                NDR_RNodeStackPop(depthTracker);

                if(!NDR_RNodeStackIsEmpty(depthTracker))
                    follow = NDR_RNodeStackPeek(depthTracker);
            }

            for(size_t x = 0; x < duplicatePostDepthTrackerCount; x++){
                NDR_DestroyRegexNode(duplicateTracker[x]);
                free(duplicateTracker[x]);
            }
            NDR_DestroyRegexStack(depthTracker);
            free(depthTracker);
            free(duplicateTracker);
        }
    }

}

bool NDR_Regex_IsCompiled(NDR_Regex* ndrregex){
    return ndrregex->initialized;
}

bool NDR_Regex_HasBeginFlag(NDR_Regex* ndrregex){
    return ndrregex->beginString;
}

bool NDR_Regex_HasEndFlag(NDR_Regex* ndrregex){
    return ndrregex->endString;
}

bool NDR_Regex_IsEmpty(NDR_Regex* ndrregex){
    return ndrregex->isEmpty;
}

char* NDR_Regex_GetErrorMessage(NDR_Regex* ndrregex){
    return ndrregex->errorMessage;
}

NDR_RegexNode*  NDR_Regex_GetStartNode(NDR_Regex* ndrregex){
    return ndrregex->start;
}
