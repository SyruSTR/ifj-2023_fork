/**
 * @file string.c
 * @author Oleg Borshch (xborsh00)
 * @author Murad Mikogaziev (xmikog00)
 * @brief syntax and semantic analysis
 */

#include "string.h"

string_ptr string_init()
{
    //create a new string
    string_ptr string;
    if((string = (string_ptr) malloc(sizeof(struct string_t))) == NULL){
        //alloc failed
        return NULL;
    }

    string->last_index = 0;
    string->mem_allocated = 0;
    string->string = NULL;

    return string;
}

void string_free(string_ptr string)
{
    if(string != NULL){
        free(string->string);
        string->string = NULL;
        free(string);
        string = NULL;
    }
}

void string_clear(string_ptr str)
{
    str->mem_allocated = 0;
    memset(str->string, 0, str->last_index);
}

bool string_append(string_ptr string, char c)
{
    if((string->last_index + 1 == string->mem_allocated) || string->last_index == 0){
        if(string->mem_allocated == 0){
            string->mem_allocated = BYTES_TO_ALLOC;
        }
        else{
            string->mem_allocated *= 2;
        }
        
        if((string->string = (char *) realloc(string->string, string->mem_allocated)) == NULL){
            //alloc failed
            return false;
        }
    }

    string->string[string->last_index] = c;
    string->last_index++;
    string->string[string->last_index] = '\0';

    return true;
}

bool string_concat(string_ptr string, const char* src)
{
    if (src == NULL)
        return false;
    int srclen = strlen(src);
    while (string->last_index + srclen > string->mem_allocated - 1)
    {
        if(string->mem_allocated == 0){
            string->mem_allocated = BYTES_TO_ALLOC;
        }
        else{
            string->mem_allocated *= 2;
        }

        if (!(string->string = realloc(string->string, string->mem_allocated)))
            return false;
        memset(string->string, 0, string->mem_allocated);
    }

    strcat(string->string, src);
    string->last_index += srclen;
    string->string[string->last_index] = '\0';

    return true;
}
