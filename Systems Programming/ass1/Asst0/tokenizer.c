/*
 * tokenizer.c
 * @Authors: Jordano and Biggie
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "header.h"

/*
 * @Description
 * Struct to hold the input stream of characters to be tokenized.
 * 
 * @Properties
 * stream: the entire input stream
 * place: index of the current character in the input stream
 */
struct TokenizerT_ 
{
    int place; 
    char* stream;
};
typedef struct TokenizerT_ TokenizerT;

/*
 *@ Description
 *Enumerates all states contained in the Finite State Machine which tokenizes the characters.
 */
enum state_{START, COMMENT_DIV, COMMENT_INLINE, COMMENT_BLOCK1, COMMENT_BLOCK2,
 MATH_PLUS, EQ, MATH_MINUS, NEGATE, BIT_XOR, BIT_OR, BIT_AND, MOD_ARITH,
 MULT, GREATER_OR_LESSTHAN1, GREATER_OR_LESSTHAN2, DOUBLE_QUOTE, SINGLE_QUOTE,
 WORD, DECIMAL_FLOAT, FLOAT1, FLOAT2, FLOAT3, FLOAT4, FLOAT5, OCTAL, OCTAL2, HEX, HEX2, PEND_FLOAT};
typedef enum state_ state;

// List of keywords
const char* keywords[] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double",
 "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short",
 "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
 "while"};

void printToken(TokenizerT*, int, char*);

/*
 *This function returns a nonzero value if the word is a keyword in C and a zero otherwise.
 */
int isKeyword(char* token)
{
    int i = (sizeof(keywords) / sizeof(*keywords)) - 1;
    for(; i >= 0; i--)
    {
        if(!strcmp(token, keywords[i]))
        {
            return 1;
        }
    }

    return 0;
}

/*
 * TKCreate creates a new TokenizerT object which holds a copy of the given token stream.
 * If the function succeeds, it returns a non-NULL TokenizerT and NULL otherwise.
 */
TokenizerT *TKCreate( char * ts ) 
{
    TokenizerT* tk = (TokenizerT*) malloc(sizeof(TokenizerT));

    // set place
    tk->place = 0;

    // copy input stream into struct
    tk->stream = (char*) malloc( sizeof(char) * (strlen(ts) + 1) );
    strcpy(tk->stream, ts);

    return tk;
}

/*
 * TKDestroy destroys a TokenizerT object by freeing all allocated memory.
 */
void TKDestroy( TokenizerT * tk ) 
{
    free(tk->stream);
    free(tk);
}

/*
 * @Description
 * TKGetNextToken finds the next token, good or bad, from the token stream.
 * It then prints the token and its type as per the instructions.
 *
 * @Return
 * If the function succeeds, it returns a non-zero int and 0 otherwise
 */
int TKGetNextToken( TokenizerT * tk ) 
{ 
    state s;

    // set initial state of finite state machine
    s = START;
    
    // Characters are put through the FSM
    int i;
    for(i = tk->place; i <= strlen(tk->stream);i++)
    {
        // grab next char
        char next_char = tk->stream[i];
        
        switch(s)
        {
            case START:
                if(isspace(next_char))
                {
                    tk->place++;
                }
                else if(next_char == '/')
                {
                    s = COMMENT_DIV;         
                }
                else if(next_char == '+')
                {
                    s = MATH_PLUS;
                }
                else if(next_char == '-')
                {
                    s = MATH_MINUS;
                }
                else if(next_char == '=')
                {
                    s = EQ;
                }
                else if(next_char == '.')
                {
                    printToken(tk, i, "dot-operator");
                    return 1;
                }
                else if(next_char == '~')
                {
                    printToken(tk, i, "one's-compliment");
                    return 1;
                }
                else if(next_char == ':')
                {
                    printToken(tk, i, "colon");
                    return 1;
                }
                else if(next_char == ',')
                {
                    printToken(tk, i, "comma");
                    return 1;
                }
                else if(next_char == '?')
                {
                    printToken(tk, i, "question-mark");
                    return 1;
                }
                else if(next_char == '!')
                {
                    s = NEGATE;    
                }
                else if(next_char == '^')
                {
                    s = BIT_XOR;
                }
                else if(next_char == '|')
                {
                    s = BIT_OR;
                }
                else if(next_char == '&')
                {
                    s = BIT_AND;
                }
                else if(next_char == '(')
                {
                    printToken(tk, i, "open-parenthesis");
                }
                else if(next_char == ')')
                {
                    printToken(tk, i, "close-parenthesis");
                }
                else if(next_char == '[')
                {
                    printToken(tk, i, "open-bracket");
                }
                else if(next_char == ']')
                {
                    printToken(tk, i, "close-bracket");
                }
                else if(next_char == '{')
                {
                    printToken(tk, i, "cpen-curly-brace");
                }
                else if(next_char == '}')
                {
                    printToken(tk, i, "close-curly-brace");
                }
                else if(next_char == '%')
                {
                    s = MOD_ARITH;
                }
                else if(next_char == '*')
                {
                    s = MULT;
                }
                else if( (next_char == '<') || (next_char == '>') )
                {
                    s = GREATER_OR_LESSTHAN1;
                }
                else if( (next_char == '\"') || (next_char == '\'') )
                {
                    s = (next_char == '\"') ? DOUBLE_QUOTE : SINGLE_QUOTE;
                }
                else if(isalpha(next_char))
                {
                    s = WORD;
                }
                else if(isdigit(next_char) && (next_char != '0'))
                {
                    s = DECIMAL_FLOAT;
                }
                else if(next_char == '0')
                {
                    s = OCTAL;
                }
                else if(next_char == ';')
                {
                    printToken(tk, i, "semi-colon");
                    return 1;
                }
                else if(next_char != '\0') // If its \0 we ignore it, but anything else is probably a bad token like '@'
                {
                    printToken(tk, i, "mal");
                    return 1;
                }

                break;         

            case DECIMAL_FLOAT:
                if(next_char == '.')
                {
                    s = FLOAT1;
                }
                else if(!isdigit(next_char))
                {
                    printToken(tk, (i-1), "decimal");
                    return 1;
                }
                
                break;

            case FLOAT1:
                if(isdigit(next_char))
                {
                    s = FLOAT2;
                }
                else
                {
                    printToken(tk, (i-1), "mal");
                    return 1;
                }

                break;

            case FLOAT2:
                if(isdigit(next_char))
                {
                    break;
                }
                else if(tolower(next_char) == 'e')
                {
                    s = FLOAT3;
                }
                else
                {
                    printToken(tk, (i-1), "float");
                    return 1;
                }
                
                break;

            case FLOAT3:
                if( (next_char == '-') || (next_char == '+') )
                {
                    s = FLOAT4;
                }
                else if(isdigit(next_char))
                {
                    s = FLOAT5;
                }
                else
                {
                    printToken(tk, (i-1), "mal");
                    return 1;
                }

                break;

            case FLOAT4:
                if(isdigit(next_char))
                {
                    s = FLOAT5;
                }
                else
                {
                    printToken(tk, (i-1), "mal");
                    return 1;
                }

                break;

            case FLOAT5:
                if(!isdigit(next_char))
                {
                    printToken(tk, (i-1), "float");
                    return 1;
                }
                
                break;

            case WORD:
                if( !(isdigit(next_char) || isalpha(next_char)) ) 
                {
                    printToken(tk, (i-1), "word");
                    return 1;
                }
                
                break;
            
            case COMMENT_DIV:
                if(next_char == '*')
                {
                    s = COMMENT_BLOCK1;    
                }
                else if(next_char == '/')
                {
                    s = COMMENT_INLINE;    
                }
                else
                {
                    printToken(tk, (i-1), "division-operator");
                    return 1;
                }
                break;

            case COMMENT_INLINE:
                if(next_char == '\n' || next_char == '\0')
                {
                    printToken(tk, i, "inline-comment");
                    return 1;
                }
                break;

            case COMMENT_BLOCK1:
                if(next_char == '*')
                {
                    s = COMMENT_BLOCK2;    
                }
                else if(next_char == '\0')
                {
                    printToken(tk, i, "mal");
                    return 0;
                }
                break;

            case COMMENT_BLOCK2:
                if(next_char == '/')
                {
                    printToken(tk, i, "block-comment");
                    return 1;
                }
                else if(next_char == '\0')
                {
                    printToken(tk, i, "mal");
                    return 0;
                }
                else
                {
                    s = COMMENT_BLOCK1;    
                }
                break;

            case MATH_PLUS:
                if(next_char == '+')
                {
                   printToken(tk, i, "double-plus");
                }
                else if (next_char == '=')
                {
                    printToken(tk, i, "plus-equals");
                }
                else
                {
                    printToken(tk, (i-1), "plus-perator");   
                }

                return 1;
                break;

            case MATH_MINUS:
                if(next_char == '-')
                {
                    printToken(tk, i, "double-minus");    
                }
                else if(next_char == '=')
                {
                    printToken(tk, i, "minus-equals"); 
                }
                else if(next_char == '>')
                {
                    printToken(tk, i, "struct-pointer");    
                }
                else
                {
                    printToken(tk, (i-1), "minus-operator");        
                }

                return 1;
                break;

            case EQ:
                if(next_char == '=')
                {
                    printToken(tk, i, "double-equals");    
                }
                else
                {
                    printToken(tk, (i-1), "equals");    
                }

                return 1;
                break;

            case NEGATE:
                if(next_char == '=')
                {
                    printToken(tk, i, "not-equals");
                }
                else
                {
                    printToken(tk, (i-1), "negate");    
                }

                return 1;
                break;

            case BIT_XOR:
                if(next_char == '=')
                {
                    printToken(tk, i, "bitwise-exclusive-OR-equals");
                }
                else
                {
                    printToken(tk, (i-1), "bitwise-exclusive-OR");
                }
                
                return 1;
                break;

            case BIT_OR:
                if(next_char == '|')
                {
                    printToken(tk, i, "logical-OR");
                }
                else if(next_char == '=')
                {
                    printToken(tk, i, "bitwise-OR-equals");
                }
                else
                {
                    printToken(tk, (i-1), "bitwise-OR");    
                }

                return 1;
                break;

            case BIT_AND:
                if(next_char == '&')
                {
                    printToken(tk, i, "logical-AND");
                }
                if(next_char == '=')
                {
                    printToken(tk, i, "bitwise-AND-equals");
                }
                else
                {
                    printToken(tk, (i-1), "bitwise-AND");
                }

                return 1;
                break;

            case MOD_ARITH:
                if(next_char == '=')
                {
                    printToken(tk, i, "modulus-equals");
                }
                else
                {
                    printToken(tk, (i-1), "modulus");
                }

                return 1;
                break;

            case MULT:
                if(next_char == '=')
                {
                    printToken(tk, i, "multiply-equals");
                }
                else
                {
                    printToken(tk, (i-1), "multiply-operator/indirect");
                }

                return 1;
                break;

            case GREATER_OR_LESSTHAN1:
                if(next_char == '=')
                {
                    printToken( tk, i, ((tk->stream[i-1] == '<') ? "less-than-or-equal-to" : "greater-than-or-equal-to") );
                    return 1;
                }
                else if(next_char == tk->stream[i-1])
                {
                    s = GREATER_OR_LESSTHAN2;
                }
                else
                {
                    printToken( tk, (i-1), ((tk->stream[i-1] == '<') ? "lessthan" : "greater-than") );
                    return 1;
                }

                break;

            case GREATER_OR_LESSTHAN2:
                if(next_char == '=')
                {
                    printToken( tk, i, ((tk->stream[i-1] == '<') ? "left-bit-shift-equals" : "right-bit-shift-equals") );
                }
                else
                {
                    printToken( tk, (i-1), ((tk->stream[i-1] == '<') ? "left-bit-shift" : "right bit-shift") );    
                }

                return 1;
                break; 

            case DOUBLE_QUOTE:
                if(next_char == '\"')
                {
                    printToken(tk, i, "double-quotes");
                    return 1;
                }

                break;

            case SINGLE_QUOTE:
                if(next_char == '\'')
                {
                    printToken(tk, i, "single-quotes");
                    return 1;    
                }

                break;

            case OCTAL:
                if(isdigit(next_char) && (next_char < '8'))
                {
                    s = OCTAL2;
                }
                else if(next_char == 'x')
                {
                    s = HEX;
                }
                else if(next_char == '.')
                {
                    s = PEND_FLOAT;
                }
                else
                {
                    printToken(tk, (i-1), "mal");
                    return 1;
                }

                break;

            case PEND_FLOAT:
                if(isdigit(next_char))
                {
                    s = FLOAT2;
                }
                else
                {
                    printToken(tk, (i-1), "mal");
                    return 1;
                }

                break;

            case OCTAL2:
                if(!(isdigit(next_char) && (next_char < '8')))
                {
                    printToken(tk, (i-1), "octal");
                    return 1;
                }

                break;

            case HEX:
                if(isdigit(next_char) || (tolower(next_char) >= 'a' && tolower(next_char) <= 'g'))
                {
                    s = HEX2;
                }
                else
                {
                    printToken(tk, (i-1), "mal");
                    return 1;
                }

                break;

            case HEX2:
                if(!(isdigit(next_char) || (tolower(next_char) >= 'a' && tolower(next_char) <= 'g')))
                {
                    printToken(tk, (i-1), "hexidecimal");
                    return 1;
                }

                break;

            default:
                printf("Not in a state");
                exit(EXIT_FAILURE);

                break;
        }
        
    }

    return 0;
}

/*
 * @Description
 * This function prints a token by printing all characters from the the current position in tk up to where the FSM halted.
 * It also prints out the type of token as supplied by the user and updates the current position in the struct.
 * NOTE: If it's a word token then it is checked if it is keyword in C first and if so the token_type is changed.
 *
 * @Params
 * tk: struct with input stream and current position in stream
 * end: position in stream where FSM halted, the current position in the struct gets updated to this
 * type: the type of token
 */
void printToken(TokenizerT* tk, int end, char* token_type)
{
    int token_length = end - tk->place + 1;

    // create holder for token
    char* token = (char*) malloc(sizeof(char) * (token_length + 1));

    // store characters from stream in token holder
    int i;
    for(i = 0; i < token_length; i++)
    {
        token[i] = tk->stream[tk->place++];
    }
    token[i] = '\0';
    
    // Check if word token is a keyword
    if(!strcmp(token_type,"word"))
    {
        token_type = (isKeyword(token)) ? "keyword" : "word";
    }

    printf("%s: \"%s\"\n", token_type, token);

    free(token);
}

/*
 * main will have a string argument (in argv[1]) containing the tokens.
 * Tokens printed in left-to-right order on seperate lines.
 */
int main(int argc, char **argv) 
{
    // validate args (Just checking the # of args. Input is assumed good if its there.)
    if(argc != 2)
    {
        printf("For some reason there are %d args\n", argc);
        return EXIT_FAILURE; 
    }

    TokenizerT* tk = TKCreate(argv[1]);

    // Get tokens until there are none left
    while(TKGetNextToken(tk));

    TKDestroy(tk);

    return EXIT_SUCCESS;
}
