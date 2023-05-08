
/*********************************************************************************
*                                 NDR File Processor                             *
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
#include "ndr_fileprocessor.h"



/*
* \fn NDR_InitializeFileInformation
* \brief A constructor style function for initializing the FileInformation structure
* \param fileInfo is a NDR_FileInformation* with memory allocated to it
*/
static void NDR_InitializeFileInformation(NDR_FileInformation* fileInfo);
/*
* \fn NDR_FreeLineInformation
* \brief A destructor style function for freeing the memory used within the NDR_LineInformation structure
* \param lineInfo is a NDR_LineInformation* that is part of the NDR_FileInformation structure previously used with NDR_InitializeFileInformation
*/
static void NDR_FreeLineInformation(NDR_LineInformation* lineInfo);
/*
* \fn NDR_AddLine
* \brief Handles the addition of a new line from the file to the NDR_FileInformation structure
* \param fileInfo is the NDR_FileInformation structure that the new line will be added to
* \param line is the new line to be added
* \param separator is the string by which each line should be split into tokens
*/
static void NDR_AddLine(NDR_FileInformation* fileInfo, char* line, char* separator);
/*
* \fn NDR_AddToken
* \brief Handles the addition of a new token from the file line to the NDR_LineInformation structure
* \param lineInfo is a NDR_LineInformation* that is part of the NDR_FileInformation structure previously used with NDR_InitializeFileInformation
* \param token is the new token to be added to the line
*/
static void NDR_AddToken(NDR_LineInformation* lineInfo, char* token);


int NDR_ProcessFile(char* fileName, NDR_FileInformation* fileInfo, char* separator){

    NDR_InitializeFileInformation(fileInfo);
    if(fileName == NULL || strcmp(fileName, "") == 0){
        fileInfo->fileError = true;
        return 1;
    }

    FILE* file = fopen(fileName, "r");
    if(file == NULL){
        fileInfo->fileError = true;
        return 1;
    }

    int fileLinePosition;
    int ch;
    int maxLineLength = 50;
    char* fileLine = malloc(maxLineLength);

    while((ch = fgetc(file)) != EOF){
        if(ch == '\n'){
            fileLine[0] = '\n';
            fileLine[1] = '\0';
        }
        else{
            fileLinePosition = 0;
            do{
                if(ch != EOF){
                    fileLine[fileLinePosition] = (char) ch;
                }
                else
                    break;
                fileLinePosition++;
                if(fileLinePosition > maxLineLength - 5){
                    maxLineLength = maxLineLength * 2;
                    fileLine = realloc(fileLine, maxLineLength);
                }
                fileLine[fileLinePosition] = '\0';
            }while((ch = fgetc(file)) != '\n');

        }

        NDR_AddLine(fileInfo, fileLine, separator);
    }

    free(fileLine);

    return 0;
}

void NDR_InitializeFileInformation(NDR_FileInformation* fileInfo){
    fileInfo->fileError = false;
    fileInfo->lines = malloc(sizeof(NDR_LineInformation*));
    fileInfo->numberOfLines = 0;
    fileInfo->longestTokenAmount = 0;
    fileInfo->longestLineLength = 0;
}

void NDR_FreeLineInformation(NDR_LineInformation* lineInfo){
    for(size_t x = 0; x < lineInfo->numberOfTokens; x++){
        free(lineInfo->tokens[x]);
    }
    free(lineInfo->tokens);
    free(lineInfo->originalLine);

}

void NDR_FreeFileInformation(NDR_FileInformation* fileInfo){
    for(size_t x = 0; x < fileInfo->numberOfLines; x++){
        NDR_FreeLineInformation(fileInfo->lines[x]);
        free(fileInfo->lines[x]);
    }
    free(fileInfo->lines);
}

void NDR_AddLine(NDR_FileInformation* fileInfo, char* line, char* separator){

    fileInfo->lines = realloc(fileInfo->lines, sizeof(NDR_LineInformation*) * (fileInfo->numberOfLines + 1));
    fileInfo->lines[fileInfo->numberOfLines] = malloc(sizeof(NDR_LineInformation));

    fileInfo->lines[fileInfo->numberOfLines]->originalLine = malloc(strlen(line) + 1);
    strcpy(fileInfo->lines[fileInfo->numberOfLines]->originalLine, line);

    if(strlen(line) > fileInfo->longestLineLength)
        fileInfo->longestLineLength = strlen(line);

    fileInfo->lines[fileInfo->numberOfLines]->tokens = malloc(sizeof(char*));
    fileInfo->lines[fileInfo->numberOfLines]->numberOfTokens = 0;

    size_t numberOfTokens = 0;
    char* temp = strtok(line, separator);
    while(temp != NULL){
        numberOfTokens++;
        NDR_AddToken(fileInfo->lines[fileInfo->numberOfLines], temp);
        temp = strtok(NULL, separator);
    }

    if(numberOfTokens > fileInfo->longestTokenAmount)
        fileInfo->longestTokenAmount = numberOfTokens;

    fileInfo->numberOfLines++;
}

void NDR_AddToken(NDR_LineInformation* lineInfo, char* token){
    lineInfo->tokens[lineInfo->numberOfTokens] = malloc(strlen(token)+1);
    strcpy(lineInfo->tokens[lineInfo->numberOfTokens], token);
    lineInfo->numberOfTokens++;
    lineInfo->tokens = realloc(lineInfo->tokens, sizeof(char*) * (lineInfo->numberOfTokens + 1));
}


void NDR_PrintFileInformation(NDR_FileInformation* fileInfo){
    for(size_t i = 0; i < fileInfo->numberOfLines; i++){
        for(size_t x = 0; x < fileInfo->lines[i]->numberOfTokens; x++){
            printf("%s ", fileInfo->lines[i]->tokens[x]);
        }
        printf("\n");
    }
}


size_t NDR_GetNumberOfLines(NDR_FileInformation* fileInfo){
    return fileInfo->numberOfLines;
}
size_t NDR_GetLongestTokenAmount(NDR_FileInformation* fileInfo){
    return fileInfo->longestTokenAmount;
}
size_t NDR_GetLongestLineLength(NDR_FileInformation* fileInfo){
    return fileInfo->longestLineLength;
}
bool NDR_ErrorDuringProcessing(NDR_FileInformation* fileInfo){
    return fileInfo->longestLineLength;
}
NDR_LineInformation* NDR_GetLine(NDR_FileInformation* fileInfo, int index){
    if(index >= 0 && index < fileInfo->numberOfLines)
        return fileInfo->lines[index];
    else
        return NULL;
}

char** NDR_GetTokens(NDR_LineInformation* lineInfo){
    return lineInfo->tokens;
}
char* NDR_GetToken(NDR_LineInformation* lineInfo, int index){
    if(index >= 0 && index < lineInfo->numberOfTokens)
        return lineInfo->tokens[index];
    else
        return NULL;
}
char* NDR_GetOriginalLine(NDR_LineInformation* lineInfo){
    return lineInfo->originalLine;
}
size_t NDR_GetNumberOfTokens(NDR_LineInformation* lineInfo){
    return lineInfo->numberOfTokens;
}
