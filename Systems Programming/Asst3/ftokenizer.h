/**************************************************************
 *ftokenizer.h
 *
 *tokenizes a text file and returns an iterator for all tokens
 *
 *Authors: biggs and jordano
***************************************************************/

#ifndef ftokenizer_h_
#define ftokenizer_h_

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef struct ftokenizerIterator{
    char *filename;
    char *delims;
    char *saveptr;
    int firstToken;
    FILE* file;
    char* currentFileLine;
} ftokenizerIterator;

ftokenizerIterator* ftokenizerCreate(char * filename, char* delims);

char* ftokenizerNextToken(ftokenizerIterator* f);

void ftokenizerDestroy(ftokenizerIterator* f);

#endif
