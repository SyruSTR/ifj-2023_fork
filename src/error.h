/**
 * @file error.h
 * @author Nikita Vetluzhskikh (xvetlu00)
 * @brief error codes list
 */

#ifndef ERROR_HEADER_G
#define ERROR_HEADER_G

#define PRINT_MESSAGE_AND_EXIT_FULL(message, error_code, line, char, token_type, token_string) {fprintf(stderr, "{\n\terror_code : %d,\n\tmessage : \"%s\",\n\tline : %d,char_pos : %d,\n\ttoken_type : %d,\n\ttoken_string : \"%s\"\n}",error_code,message,line,char,token_type,token_string ); \
    return error_code;}

#define PRINT_MESSAGE_AND_EXIT_SHORT(message,error_code) PRINT_MESSAGE_AND_EXIT_FULL(message,error_code,-1,-1,0,"")

/**
 * @brief enum for readable code errors
 */
typedef enum error{
    ER_NONE          = 0,
    ER_LEX           = 1,
    ER_SYNTAX        = 2,
    ER_UNDEF_FUNC_OR_REDEF_VAR         = 3,
    ER_PARAMS        = 4,
    ER_UNDEF_VAR_OR_NOTINIT_VAR     = 5,
    ER_FUNC_RETURN   = 6,
    ER_TYPE_COMP     = 7,
    ER_INFERENCE     = 8,
    ER_OTHER_SEM       = 9,
    ER_INTERNAL      = 99
} error_t;

#endif