/**************************************************************
 *ftokenizer.c
 *
 *implementation of ftokenizer.h
 *
 *Authors: biggs and jordano
***************************************************************/

#include "ftokenizer.h"

ftokenizerIterator* ftokenizerCreate(char * filename, char* delims)
{
    ftokenizerIterator *f = (ftokenizerIterator*) malloc(sizeof(ftokenizerIterator));
    f->delims = (char*) malloc(sizeof(char) * (strlen(delims) + 1));
    strcpy(f->delims, delims);
    f->filename = (char*) malloc(sizeof(char) * (strlen(filename) + 1));
    strcpy(f->filename, filename);
    f->saveptr = NULL;
    f->firstToken = 1;
    f->file = fopen(f->filename, "r");
    f->currentFileLine = NULL;

    return f;
}

char* ftokenizerNextToken(ftokenizerIterator* f)
{
    int status = 0;
    size_t n = 0;
    char *token = NULL;
    char *tokenCopy = NULL;

    if(!f->currentFileLine){
        status = getline(&f->currentFileLine, &n, f->file);
        if(status == -1){
            return NULL;
        }
    }

    while(1)
    {
        if(f->firstToken){
            f->firstToken = 0;
            token = strtok_r(f->currentFileLine, f->delims, &f->saveptr);
        }
        else{
            token = strtok_r(NULL, f->delims, &f->saveptr);
        }

        if(token)
        {
            break;
        }
        else
        {
            free(f->currentFileLine);
            f->currentFileLine = NULL;
            status = getline(&f->currentFileLine, &n, f->file);
            if(status == -1){
                return NULL;
            }
            f->firstToken = 1;
        }
    }

    tokenCopy = (char*) malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(tokenCopy, token);
    return tokenCopy;
}

void ftokenizerDestroy(ftokenizerIterator* f)
{
    free(f->filename);
    free(f->delims);
    if(f->currentFileLine){
        free(f->currentFileLine);
    }
    fclose(f->file);
    free(f);


    return;
}

/*int main(int argc, char *argv[])
{
    ftokenizerIterator *f = ftokenizerCreate("onefile.txt", "\t\n\a\b\f\r\v\\\'\"\?`~!@#$%%^&*()-_=+{}|,.<>;: []");

    char *token = NULL;

    ftokenizerDestroy(f);

    return EXIT_SUCCESS;
}*/
