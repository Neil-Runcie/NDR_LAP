#ifndef NDRLEXER_H
#define NDRLEXER_H

int NDR_Configure_Lexer(char* fileName);
int NDR_Lex(char* fileName);

void NDR_PrintSymbolTable();
void NDR_PrintTokenTable();
void NDR_PrintTokenTableLocations();

#endif
