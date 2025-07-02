//
// Created by grusted on 7/2/25.
//

#include "error.h"

void print_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void print_lexical_error(const int line, const int char_pos, const char* message) {
    const error_t code = ER_LEX;
    print_error("{\n\t\"error_code\": %d,\n\t\"message\": \"Lexical error: %s\",\n\t\"line\": %d,\n\t\"char_pos\": %d}\n",
                code, message,line, char_pos);
}

void print_syntax_error(const parser_data_t* data,const enum token_type verified_token) {
    const error_t code = ER_SYNTAX;
    char token_content_buff[100] = {0};
    strcat(token_content_buff,(data->token_ptr->string == NULL || data->token_ptr->string->string == NULL ? "null" : data->token_ptr->string->string));
    print_error("{\n\t\"error_code\": %d,\n\t\"message\": \"Syntax error: didn't verify token: \",\n\t\"line\": %d,\n\t\"char_pos\": %d,\n\t\"token_type\":%d,\n\t\"token_string\":\"%s\",\n\t\"verified_token_type\":%d\n}\n",
            code,
            data->line_cnt,
            data->token_start_pos,
            data->token_ptr->token_type,
            token_content_buff,
            verified_token);
}

void print_syntax_error_message(const parser_data_t* data,const char* message) {
    const error_t code = ER_SYNTAX;
    char token_content_buff[100] = {0};
    strcat(
        token_content_buff,
        data->token_ptr->string == NULL || data->token_ptr->string->string == NULL ? "null" : data->token_ptr->string->string);
    char message_buff[100] = "Syntax error: ";
    strcat(message_buff,message);
    print_error("{\n\t\"error_code\": %d,\n\t\"message\": \"%s\",\n\t\"line\": %d,\n\t\"char_pos\": %d,\n\t\"token_type\":%d,\n\t\"token_string\":\"%s\"\n}\n",
            code,
            message_buff,
            data->line_cnt,
            data->token_start_pos,
            data->token_ptr->token_type,
            token_content_buff);
}