
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

#ifndef NDRFILEPROCESSOR_H
#define NDRFILEPROCESSOR_H

#include <stdbool.h>


//typedef struct NDR_LineInformation NDR_LineInformation;

//typedef struct NDR_FileInformation NDR_FileInformation;

/**
* \struct NDR_LineInformation
* \brief provides information about a single line (ended by a newline character) within a file
*/
typedef struct NDR_LineInformation {
    char** tokens;
    char* originalLine;
    size_t numberOfTokens;
} NDR_LineInformation;
/**
* \struct NDR_FileInformation
* \brief provides information about the contents of an entire file
*/
typedef struct NDR_FileInformation {
    NDR_LineInformation** lines;
    size_t numberOfLines;
    size_t longestLineLength;
    size_t longestTokenAmount;
    bool fileError;
} NDR_FileInformation;
/** @brief A destructor style function for freeing the memory used within the NDR_FileInformation structure
* 
* @param fileInfo is a NDR_FileInformation* that has been previously used with NDR_InitializeFileInformation
*/
void NDR_FreeFileInformation(NDR_FileInformation* fileInfo);
/** @brief A provided file will be processed and the information about the file will be stored in a NDR_FileInformation structure
* 
* @param fileName is the name of a text file for processing
* @param fileInfo is a structure that already has memory allocated previously used with NDR_InitializeFileInformation
* @param separator is the string by which each line should be split into tokens
* @return An int signaling the result of the function. 0 is success and 1 is failure
*/
int NDR_ProcessFile(char* fileName, NDR_FileInformation* fileInfo, char* separator);
/** @brief A provided NDR_FileInformation structure will have all file tokens printed for visual viewing
* 
* @param fileInfo is a structure that has been previously used with NDR_ProcessFile
*/
void NDR_PrintFileInformation(NDR_FileInformation* fileInfo);

/** @brief The number of lines found in the associated file will be returned
* 
* @param fileInfo is a structure that has been previously used with NDR_ProcessFile
* @return the number of lines in the file
*/
size_t NDR_GetNumberOfLines(NDR_FileInformation* fileInfo);
/** @brief Gets the number of tokens from the line with the most tokens* \param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* 
* @param fileInfo is a structure that has been previously used with NDR_ProcessFile
* @return the number of tokens found in the line in the file with the most tokens
*/
size_t NDR_GetLongestTokenAmount(NDR_FileInformation* fileInfo);
/** @brief Gets the length of the longest line found in the file
* 
* @param fileInfo is a structure that has been previously used with NDR_ProcessFile
* @return the length of the longest line found in the file
*/
size_t NDR_GetLongestLineLength(NDR_FileInformation* fileInfo);
/** @brief Allows users to tell if there was an error in the file during processing
*
* @param fileInfo is a structure that has been previously used with NDR_ProcessFile
* @return whether an error was found during processing
*/
bool NDR_ErrorDuringProcessing(NDR_FileInformation* fileInfo);
/** @brief Gets a line from the associated file
*
* @param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* @param index is a reference to the line wanted from within the file
* @return information about a specific line within the file
*/
NDR_LineInformation* NDR_GetLine(NDR_FileInformation* fileInfo, int index);

/** @brief Gets all of the tokens associated with a single line
*
* @param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* @return an array of the tokens within a single file line
*/
char** NDR_GetTokens(NDR_LineInformation* lineInfo);
/** @brief Gets a single token from a single line
*
* @param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* @param index is a reference to the token wanted from within the line
* @return one of the tokens found within a file line
*/
char* NDR_GetToken(NDR_LineInformation* lineInfo, int index);
/** @brief Gets the original line seen in the file
* 
* @param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* @return the original line from the file
*/
char* NDR_GetOriginalLine(NDR_LineInformation* lineInfo);
/** @brief Gets the number of tokens associated with the given line
*
* @param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* @return the number of tokens found in a file line
*/
size_t NDR_GetNumberOfTokens(NDR_LineInformation* lineInfo);


#endif
