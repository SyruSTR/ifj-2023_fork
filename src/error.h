/**
 * @file error.h
 * @author Nikita Vetluzhskikh (xvetlu00)
 * @brief error codes list
 */

#ifndef ERROR_HEADER_G
#define ERROR_HEADER_G

#include <stdarg.h>
#include <stdio.h>
#include "parser.h"

#define PRINT_UNDEF_OR_NOT_INIT_VAR(var_name) {print_undef_or_not_init_variable_error(data,var_name); return ER_UNDEF_VAR_OR_NOTINIT_VAR;}

#define PRINT_FUNC_RETURN(func_name) {print_func_return_error(func_name); return ER_FUNC_RETURN;}

#define PRINT_TYPE_COMP(actual,expected) {print_type_comp_error(data,actual,expected); return ER_TYPE_COMP;}

#define PRINT_TYPE_COMP_NIL() {print_type_comp_nil_error(data); return ER_TYPE_COMP;}

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
    ER_PARAMS_ARGS_MISMATCH = 40, // for LSP
    ER_PARAMS_TYPE_MISMATCH = 41, // for LSP
    ER_INTERNAL      = 99
} error_t;

void print_error(const char* format, ...);

void print_lexical_error(const int line, const int char_pos, const char* message);

void print_syntax_error(const parser_data_t* data,const enum token_type verified_token);

void print_syntax_error_message(const parser_data_t* data,const char* message);

void print_undef_func_or_redef_var_error(const parser_data_t* data);

void print_params_error_type_mismatch(const parser_data_t* data,item_type actual_type, item_type expected_type);

void print_params_error_args_mismatch(const parser_data_t* data,int actual_args, int expected_args);

void print_undef_or_not_init_variable_error(const parser_data_t* data, char* var_name);

void print_func_return_error(const char* function_name);

void print_type_comp_error(const parser_data_t* data,item_type actual_type,item_type expected_type);

void print_type_comp_nil_error(const parser_data_t* data);

#endif