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

    if(f->firstToken)
    {
        f->firstToken = 0;
        status = getline(&f->currentFileLine, &n, f->file);
        if(status == -1){
            return NULL;
        }

        token = strtok_r(f->currentFileLine, f->delims, &f->saveptr);
        if(!token){
            return NULL;
        }

        tokenCopy = (char*) malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(tokenCopy, token);
        return tokenCopy;
    }

    token = strtok_r(NULL, f->delims, &f->saveptr);

    // if no more tokens, try to get another line from the file
    if(!token)
    {
        while(1)
        {
            free(f->currentFileLine);
            f->currentFileLine = NULL;

            status = getline(&f->currentFileLine, &n, f->file);
            if(status == -1){
                return NULL;
            }

            token = strtok_r(f->currentFileLine, f->delims, &f->saveptr);
            if(!token){
                continue;
                printf("%s\n", "ff");
                return NULL;
            }

            break;
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

int main(int argc, char *argv[])
{
    ftokenizerIterator *f = ftokenizerCreate("onefile.txt", "\t\n\a\b\f\r\v\\\'\"\?`~!@#$%%^&*()-_=+{}|,.<>;: []");

    char *token = NULL;
    /*while((token = ftokenizerNextToken(f))){
        printf("%s\n", token);
        free(token);
    }*/

    ftokenizerDestroy(f);

    return EXIT_SUCCESS;
}
