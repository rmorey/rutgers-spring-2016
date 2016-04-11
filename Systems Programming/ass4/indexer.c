/**************************************************************
 *indexer.c
 *
 *Program to index files in a given directory.
 *
 *Authors: BIggie Emmanuel and Jordano Lyono
***************************************************************/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct FileOccurence;
struct LetterNode;
struct WordTrie;

FileOccurence* addFileOccurence(const char *filename, FileOccurence *list);
int hasFileOccurence(const char *filename, FileOccurence *list);
FileOccurence* sortFileOccurences(FileOccurence *list);
LetterNode* newLetterNode(char data);
WordTrie* newWordTrie();

/******
 *MAIN
******/

int main(int argc, char const *argv[])
{




    return EXIT_SUCCESS;
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
    const char *filename;
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
FileOccurence* addFileOccurence(const char *filename, FileOccurence *list)
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
 * Sort FileOccurences. BUBBLE-SORT FTW
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
            if(curr->count < next->count)
            { // make a swap
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
    l->children = (LetterNode**) calloc(122, sizeof(LetterNode*));
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

/*************
 *Misc Methods
*************/

void writeJSON(WordTrie* trie)
{

}
