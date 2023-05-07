
/*********************************************************************************
*                                 NDR File Processor                             *
**********************************************************************************/

/* ndrfileprocessor is part of the set of tools used within the ndr_lap compilation tools library
                             Written by Neil Runcie - 2023
-----------------------------------------------------------------------------
*/

#ifndef NDRFILEPROCESSOR_H
#define NDRFILEPROCESSOR_H

#include <stdbool.h>

/*
* \struct NDR_LineInformation
* \brief provides information about a single line (ended by a newline character) within a file
*/
//typedef struct NDR_LineInformation NDR_LineInformation;
/*
* \struct NDR_FileInformation
* \brief provides information about the contents of an entire file
*/
//typedef struct NDR_FileInformation NDR_FileInformation;
typedef struct NDR_LineInformation {
    char** tokens;
    char* originalLine;
    size_t numberOfTokens;
} NDR_LineInformation;

typedef struct NDR_FileInformation {
    NDR_LineInformation** lines;
    size_t numberOfLines;
    size_t longestLineLength;
    size_t longestTokenAmount;
    bool fileError;
} NDR_FileInformation;
/*
* \fn NDR_FreeLineInformation
* \brief A destructor style function for freeing the memory used within the NDR_FileInformation structure
* \param fileInfo is a NDR_FileInformation* that has been previously used with NDR_InitializeFileInformation
*/
void NDR_FreeFileInformation(NDR_FileInformation* fileInfo);
/*
* \fn NDR_ProcessFile
* \brief A provided file will be processed and the information about the file will be stored in a NDR_FileInformation structure
* \param fileName is the name of a text file for processing
* \param fileInfo is a structure that already has memory allocated previously used with NDR_InitializeFileInformation
* \param separator is the string by which each line should be split into tokens
* \return An int signaling the result of the function. 0 is success and 1 is failure
*/
int NDR_ProcessFile(char* fileName, NDR_FileInformation* fileInfo, char* separator);
/*
* \fn NDR_PrintFileInformation
* \brief A provided NDR_FileInformation structure will have all file tokens printed for visual viewing
* \param fileInfo is a structure that has been previously used with NDR_ProcessFile
*/
void NDR_PrintFileInformation(NDR_FileInformation* fileInfo);

/*
* \fn NDR_GetNumberOfLines
* \brief The number of lines found in the associated file will be returned
* \param fileInfo is a structure that has been previously used with NDR_ProcessFile
* \return the number of lines in the file
*/
size_t NDR_GetNumberOfLines(NDR_FileInformation* fileInfo);
/*
* \fn NDR_GetLongestTokenAmount
* \brief Gets the number of tokens from the line with the most tokens* \param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* \param fileInfo is a structure that has been previously used with NDR_ProcessFile
* \return the number of tokens found in the line in the file with the most tokens
*/
size_t NDR_GetLongestTokenAmount(NDR_FileInformation* fileInfo);
/*
* \fn NDR_GetLongestLineLength
* \brief Gets the length of the longest line found in the file
* \param fileInfo is a structure that has been previously used with NDR_ProcessFile
* \return the length of the longest line found in the file
*/
size_t NDR_GetLongestLineLength(NDR_FileInformation* fileInfo);
/*
* \fn NDR_ErrorDuringProcessing
* \brief Allows users to tell if there was an error in the file during processing
* \param fileInfo is a structure that has been previously used with NDR_ProcessFile
* \return whether an error was found during processing
*/
bool NDR_ErrorDuringProcessing(NDR_FileInformation* fileInfo);
/*
* \fn NDR_GetLine
* \brief Gets a line from the associated file
* \param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* \param index is a reference to the line wanted from within the file
* \return information about a specific line within the file
*/
NDR_LineInformation* NDR_GetLine(NDR_FileInformation* fileInfo, int index);

/*
* \fn NDR_GetTokens
* \brief Gets all of the tokens associated with a single line
* \param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* \return an array of the tokens within a single file line
*/
char** NDR_GetTokens(NDR_LineInformation* lineInfo);
/*
* \fn NDR_GetToken
* \brief Gets a single token from a single line
* \param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* \param index is a reference to the token wanted from within the line
* \return one of the tokens found within a file line
*/
char* NDR_GetToken(NDR_LineInformation* lineInfo, int index);
/*
* \fn NDR_GetOriginalLine
* \brief Gets the original line seen in the file
* \param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* \return the original line from the file
*/
char* NDR_GetOriginalLine(NDR_LineInformation* lineInfo);
/*
* \fn NDR_GetNumberOfTokens
* \brief Gets the number of tokens associated with the given line
* \param lineInfo is a structure that is part of a NDR_FileInformation structure that has been previously used with NDR_ProcessFile
* \return the number of tokens found in a file line
*/
size_t NDR_GetNumberOfTokens(NDR_LineInformation* lineInfo);


#endif
