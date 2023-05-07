#ifndef NDRPARSER_H
#define NDRPARSER_H

int NDR_Configure_Parser(char* fileName);
int NDR_Parse();

void NDR_PrintParseTable();
void NDR_PrintModifiedTokenTable();

#endif
