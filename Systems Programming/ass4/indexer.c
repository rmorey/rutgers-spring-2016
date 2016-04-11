/**************************************************************
 *indexer.c
 *
 *Program to index files in a given directory.
 *
 *Authors: BIggie Emmanuel and Jordan Lyon
***************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<limits.h>
#include<dirent.h>

typedef struct FileOccurence FileOccurence;
typedef struct LetterNode LetterNode;
typedef struct WordTrie WordTrie;

FileOccurence* addFileOccurence(char *filename, FileOccurence *list);
int hasFileOccurence(const char *filename, FileOccurence *list);
FileOccurence* sortFileOccurences(FileOccurence *list);
LetterNode* newLetterNode(char data);
WordTrie* newWordTrie();
void writeJSON(char* filename);
int fileExists(char *filename);
void writeJSONBody(LetterNode *curr, char *wordBuffer, FILE *json, int wordNum);
char *appendChar(char *dest, char src);
char *removeChar(char *str);
void traverse(char *dirName);
char** tokenizer(char *fileName);
int validPathSize(char* dirname, char* name);

/******
 *MAIN
******/

WordTrie *trie = NULL;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Invalid number of arguments");
        return EXIT_FAILURE;
    }

    char* dir = argv[1];
    traverse(dir);

    return 0;
}

/************************
 *FileOccurence Struct/Methods
************************/

/*
 * Holds the name of a file and the number of times a word occurs in it.
 * The word comes from WordTrie, and is not directly stored here.
 *
 * count: number of occurences of a word
 *
 * filename: name of the file
 *
 * next: pointer to next FileOccurence
 */
typedef struct FileOccurence
{
    int count;
    char *filename;
    struct FileOccurence *next;
} FileOccurence;

/*
 *Creates a new FileOccurence, adds it to the front of list, sorts the list (descending),
 *and then returns a ptr to the new list.
 *
 * params:
 * filename: name of the file
 * list: list of FileOccurences
 *
 * returns: pointer to new FileOccurence
 */
FileOccurence* addFileOccurence(char *filename, FileOccurence *list)
{
    FileOccurence* f = (FileOccurence*) malloc(sizeof(FileOccurence));
    f->count = 0;
    f->filename = filename;
    f->next = list;

    return sortFileOccurences(f);
}

/*
 * Check to see if a file is in a given list of FileOccurences
 *
 * params:
 * filename: name of file to search for
 * list: current list of FileOccurences
 *
 * returns:
 * 0: Not found
 * 1: Found
 */
int hasFileOccurence(const char *filename, FileOccurence *list)
{
    while(list){
        if(strcmp(list->filename, filename) == 0){
            return 1; // found file
        }

        list = list->next;
    }

    return 0;
}

/*
 * Sort FileOccurences in descending order. BUBBLE-SORT FTW
 */
FileOccurence* sortFileOccurences(FileOccurence *list)
{
    int madeSwap = 1;
    FileOccurence *prev, *curr, *next;
    prev = curr = next = NULL;

    curr = list;
    if(!curr){
        return curr;
    }

    do
    {
        next = curr->next;
        if(next)
        {
            // make swap if curr file has lower occurence
            // or curr and next have the same occurence, but next's filename
            // is alphanumerically before curr
            if((curr->count < next->count) ||
                ((curr->count == next->count) && (strcmp(curr->filename, next->filename) > 0)))
            {
                madeSwap = 1;
                if(prev)
                {
                    prev->next = next;
                    curr->next = next->next;
                    next->next = curr;
                }
                else{
                    list = next;
                    curr->next = next->next;
                    next->next = curr;
                }
            }
        }

        prev = curr;
        curr = next;
    } while(curr && madeSwap);

    return list;
}

/************************
 *LetterNode Struct/Methods
************************/

/*
 * A single node in a WordTrie
 *
 * data: the letter associated with that node
 *
 * children: array for 26 possible child LetterNodes (1 per letter)
 *
 * fileOccurences: list of # of occurences of the word in a given file
 *
 * childrenLength: size of array of children
 */
typedef struct LetterNode
{
    char data;
    struct  LetterNode **children;
    FileOccurence *fileOccurences;
    int childrenLength;
} LetterNode;

/*
 * Creates a new LetterNode
 *
 * params:
 * data: character for the LetterNode
 *
 * returns: pointer to new LetterNode
 */
LetterNode* newLetterNode(char data)
{
    LetterNode *l = (LetterNode*) malloc(sizeof(LetterNode));
    l->data = data;
    l->children = (LetterNode**) calloc(122, sizeof(LetterNode*));
    l->fileOccurences = NULL;
    l->childrenLength = 122;

    return l;
}

/************************
 *WordTrie Struct/Methods
************************/

/*
 * Uses a trie to keep track of words
 */
typedef struct WordTrie
{
    LetterNode *head;
    int numWords;
} WordTrie;

/*
 * Creates a new WordTrie
 *
 * params: N\A
 *
 * returns: pointer to new WordTrie
 */
WordTrie* newWordTrie()
{
    WordTrie *w = (WordTrie*) malloc(sizeof(WordTrie));
    w->head = newLetterNode('\0');
    w->numWords = 0;

    return w;
}

/*************
 *Misc Methods
*************/

/*
 * Write contents of trie in json format to a file (1)
 */
void writeJSON(char* filename)
{
    // if file exists, ask user to overwrite
    if(fileExists(filename))
    {
        char response[50];
        printf("File already exists, overwrite? (Enter yes or no): ");
        scanf("%s", response);
        while((strcmp(response, "no") != 0) && (strcmp(response, "yes") != 0)){
            printf("%s", "\nInvalid response, try again: ");
        }
        if(strcmp(response, "no") == 0)
        {
            return;
        }
        else{
            remove(filename);
        }
    }

    // write first line of file
    FILE *json = fopen(filename, "r");
    fprintf(json, "%s", "{\"list\" : [\n");

    // write body of file
    int i = 0;
    for(; i < trie->head->childrenLength; i++)
    {
        if(trie->head->children[i])
        {
            writeJSONBody(trie->head->children[i], NULL, json, 0);
        }
    }

    // write last line of file
    fprintf(json, "%s", "]}");


    return;
}

/*
 * Write contents of trie in json format to a file (2)
 * writes the word, along with occurences in files, to the specified file
 */
void writeJSONBody(LetterNode *curr, char *wordBuffer, FILE *json, int wordNum)
{
    wordBuffer = appendChar(wordBuffer, curr->data);

    // this letter is the end of a word, so add to json
    if(curr->fileOccurences)
    {
        wordNum++;

        fprintf(json, "\t{\"%s\" : [\n", wordBuffer);

        FileOccurence *f = curr->fileOccurences;
        while(f){
            fprintf(json, "\t\t\{\"%s\" : %d%s\n", f->filename, f->count,
                (f->next) ? "}," : "}");
        }

        fprintf(json, "%s", (wordNum < trie->numWords) ? "]}," : "]}");
    }

    int i = 0;
    for(; i < curr->childrenLength; i++)
    {
        if(curr->children[i]){
            writeJSONBody(curr->children[i], wordBuffer, json, wordNum);
        }
    }

    removeChar(wordBuffer);

    return;
}

/*
 * appends a char to a string (string does not need space for it beforehand)
 */
char *appendChar(char *dest, char src)
{
    if(dest) // string not null
    {
        dest = (char*) realloc(dest, (strlen(dest) + 2) * sizeof(char));
        strcat(dest, &src);
    }
    else // null string
    {
        dest = (char*) malloc(sizeof(char) * 2);
        dest[0] = src;
        dest[1] = '\0';
    }

    return dest;
}

/*
 * remove char from string. return pointer to it (deallocates space for the char as well)
 */
char *removeChar(char *str)
{
    if(!str){
        return NULL;
    }

    char *newStr = (char*) calloc(strlen(str), sizeof(char));
    strncpy(newStr, str, strlen(str) - 1);

    return newStr;
}

/*
 * Check if file exists
 *
 * param: name of file
 *
 * returns:
 * 0: does not exit
 * 1: does exist
 */
int fileExists(char *filename)
{
    struct stat st;
    return (stat(filename, &st) == 0);
}

/*char** tokenizer(char *fileName)
{
   FILE *fp = fopen(fileName, "r");

   if(fp == NULL) //file not found
   {
       printf("error - File does not exist\n");
       return 0;
   }
   //------empty file check--------
   fseek(fp, 0, SEEK_END);
   if(ftell(fp) == 0)
   {
        printf("0\n");
        return 0;
   }
   fseek(fp, 0, SEEK_SET);
   //------------------------------

   char *line;
   int i = 0;
   if(fgets(line, strlen(line), fp) != NULL)
   {
       char **arr = (char**)malloc(10 * sizeof(char*));
       arr[i] = strtok(line, " ");
       i++;
   }

   return arr;
}*/

/* Trarverses the directory, and adds all words in all files to trie */
void traverse(char *dirName)
{
    DIR *dir;
    if((dir = opendir(dirName)) == NULL)
    {
        printf("Cannot open directory\n");
        exit(EXIT_FAILURE);
    }

    struct dirent *dptr;
    while((dptr = readdir(dir)) != NULL)
    {
        char *name = dptr->d_name;

        if(dptr->d_type == DT_DIR) // its a directory
        {
            // can't be current directory or parent directory
            if(strcmp(name, "..") && strcmp(name, "."))
            {
                char path[PATH_MAX]; //holds full path
                if(validPathSize(dirName, name))
                {
                    snprintf(path, PATH_MAX, "%s/%s", dirName, name);
                    printf("%s\n", path);
                    traverse(path);
                }
                else
                {
                    printf("path too long");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else //dptr->d_type != DT_DIR (its a file)
        {
            printf("%s/%s\n", dirName, name);
        }
    }
    closedir(dir);

    return;
}

/*
 * params:
 * name: current directory or file name
 * dirname: name of parent directory of name
 *
 * returns:
 * nonzero int: full path to name is less than PATH_MAX (max pathname length)
 * 0: path name too big
 */
int validPathSize(char* dirname, char* name)
{
    return ((strlen(dirname) + strlen(name)) < PATH_MAX);
}
