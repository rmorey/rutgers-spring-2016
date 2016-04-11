/**************************************************************
 *indexer.c
 *
 *Program to index files in a given directory.
 *
 *Authors: BIggie Emmanuel and Jordano Lyono
***************************************************************/

#include<stdlib.h>
#include<stdio.h>

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
    const char *filename;
    struct FileOccurence *next;
} FileOccurence;

/*
 *Creates a new FileOccurence and adds it to the front of list
 *
 * params:
 * filename: name of the file
 * list: list of FileOccurences
 *
 * returns: pointer to new FileOccurence
 */
FileOccurence* addFileOccurence(const char *filename, FileOccurence *list)
{
    FileOccurence* f = (FileOccurence*) malloc(sizeof(FileOccurence));
    f->count = 0;
    f->filename = filename;
    f->next = list;

    return f;
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
 */
typedef struct LetterNode
{
    char data;
    struct  LetterNode **children;
    FileOccurence *fileOccurences;
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
    l->children = (LetterNode**) calloc(26, sizeof(LetterNode*));
    l->fileOccurences = NULL;

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

    return w;
}

int main(int argc, char const *argv[])
{




    return EXIT_SUCCESS;
}
