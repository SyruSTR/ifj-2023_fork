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

void print_undef_func_or_redef_var_error(const parser_data_t* data) {
    bool tmp_bool = data->id_type == NULL ? false : data->id_type->is_function;
    const error_t code = ER_UNDEF_FUNC_OR_REDEF_VAR;
    char token_content_buff[100] = {0};
    strcat(
        token_content_buff,
        data->token_ptr->string == NULL || data->token_ptr->string->string == NULL ? "null" : data->token_ptr->string->string);
    print_error("{\n\t\"error_code\": %d,\n\t\"line\": %d,\n\t\"char_pos\": %d,\n\t\"token_type\":%d,\n\t\"token_string\":\"%s\",\n\t\"is_function\":%b\n}\n",
            code,
            data->line_cnt,
            data->token_start_pos,
            data->token_ptr->token_type,
            token_content_buff,
            tmp_bool);
}

void print_params_error_type_mismatch(const parser_data_t* data, const item_type actual_type, const item_type expected_type) {
    const error_t code = ER_PARAMS_TYPE_MISMATCH;
    // print_error("{\n\t\"error_code\": %d,"
    //             "\n\t\"line\": %d,"
    //             "\n\t\"char_pos\": %d,"
    //             "\n\t\"function_name\": \"%s\","
    //             "\n\t\"expected_func_args_count\":%d,"
    //             "\n\t\"actual_func_args_count\":%d,"
    //             "\n\t\"expected_return_type\": \"%d\","
    //             "\n\t\"actual_return_type\": \"%d\","
    //             "\n\t\"actual_param_type\": \"%d\","
    //             "\n\t\"expected_param_type\": \"%d\","
    //             "}\n",
    //     code,
    //     data->line_cnt,
    //     data->token_start_pos,
    //     data->token_ptr->string,
    //     );
    print_error("{\n\t\"error_code\": %d,"
            "\n\t\"line\": %d,"
            "\n\t\"char_pos\": %d,"
            "\n\t\"function_name\": \"%s\","
            "\n\t\"actual_param_type\": \"%d\","
            "\n\t\"expected_param_type\": \"%d\""
            "\n}\n",
    code,
    data->line_cnt,
    data->token_start_pos,
    data->id_type->id,
    actual_type,
    expected_type
    );
}

void print_params_error_args_mismatch(const parser_data_t* data,int actual_args, const int expected_args) {
    const error_t code = ER_PARAMS_ARGS_MISMATCH;
    print_error("{\n\t\"error_code\": %d,"
        "\n\t\"line\": %d,"
        "\n\t\"char_pos\": %d,"
        "\n\t\"function_name\": \"%s\","
        "\n\t\"expected_func_args\":%d,"
        "\n\t\"actual_func_args\":%d"
        "\n}\n",
    code,
    data->line_cnt,
    data->token_start_pos,
    data->id_type->id,
    expected_args,
    actual_args// counting from 0
    );
}

void print_not_init_variable_error(const parser_data_t* data, char* var_name) {
    const int code = ER_UNDEF_VAR;
    print_error("{\n\t\"error_code\": %d,"
        "\n\t\"line\": %d,"
        "\n\t\"char_pos\": %d,"
        "\n\t\"variable_name\": \"%s\""
        "\n}\n",
    code,
    data->line_cnt,
    data->token_start_pos,
    var_name
    );
}