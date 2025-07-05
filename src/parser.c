/**
 * @file parser.c
 * @author Nikita Vetluzhskikh (xvetlu00)
 * @author Murad Mikogaziev (xmikog00)
 * @brief syntax and semantic analysis
 */

#include "string.h"
#include "error.h"
#include "hash.h"
#include "parser.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "semantics.h"
#include "generator.h"



#define UNUSED(x) (void)(x)

#define GET_TOKEN() \
        if ((data->token_ptr = next_token(&(data->line_cnt), &ret_code, &(data->eol_flag),&data->current_char_pos,&data->token_start_pos)) == NULL) {\
            return ret_code;                                               \
        }           \

#define CHECK_RULE(_rule)           \
    if ((ret_code = _rule(data))) { \
        return ret_code;            \
    }                              \

char tmp_str[100];

#define VERIFY_TOKEN(t_token)  \
    GET_TOKEN()                \
    if (data->token_ptr->token_type != t_token) {print_syntax_error(data, t_token); return ER_SYNTAX;}\
                               \

#define INSERT_SYM() \
        bool internal_error; \
        if(table_count_elements_in_stack(data->table_stack) == 0)\
            PRINT_INTERNAL("Stack error");\
        data->id = insert_symbol(data->table_stack->top->table,data->token_ptr->attribute.string,&internal_error);\
        if(!data->id){\
            if(internal_error) PRINT_INTERNAL("Identifier in data is not NULL")\
        else PRINT_UNDEF_FUNC_OR_NOT_INIT_VARIABLE()\
}                       \

#ifdef RUNTIME_ALARM
void timeout_error(int sig) {
    UNUSED(sig);
    print_internal_error(0, "Timeout error");
    exit(ER_INTERNAL);
}
#endif


parser_data_t *init_data()
{

    parser_data_t *parser_data;
    // init parser data
    if((parser_data = (parser_data_t*) malloc(sizeof(parser_data_t))) == NULL) {
        return NULL;
    }
    parser_data->current_char_pos = 0;
    parser_data->token_start_pos = 0;


    t_table_stack *table_stack = table_stack_init();

    parser_data->table_stack = table_stack;

    // init global table
    hash_table *global_table = create_hash_table();
    if(global_table == NULL) {
        return NULL;
    }
    table_stack_push(parser_data->table_stack, global_table);
    parser_data->table_stack->top->next = NULL;

    parser_data->token_ptr = NULL;
    parser_data->id = NULL;
    parser_data->id_type = NULL;
    parser_data->exp_type = NULL;

    parser_data->is_in_function = false;
    parser_data->is_void_function = false;
    parser_data->is_in_declaration = false;
    parser_data->is_it_let_condition = false;
    parser_data->eol_flag = false;

    parser_data->param_index = 0;
    parser_data->line_cnt = 0;

    // predefined functions

    item_data *tmp;
    bool internal_error;


    if(table_count_elements_in_stack(parser_data->table_stack) == 1){
        hash_table *global_table = parser_data->table_stack->top->table;
        // readString() -> str?
        if ((tmp = insert_symbol(global_table, "readString", &internal_error)) == NULL) return NULL;
        tmp->defined = true;
        tmp->type = IT_STRING;
        tmp->nil_possibility = true;
        tmp->is_function = true;

        // readInt() -> int?
        tmp = insert_symbol(global_table, "readInt", &internal_error);
        tmp->defined = true;
        tmp->type = IT_INT;
        tmp->nil_possibility = true;
        tmp->is_function = true;

        // readDouble() -> double?
        tmp = insert_symbol(global_table, "readDouble", &internal_error);
        tmp->defined = true;
        tmp->type = IT_DOUBLE;
        tmp->nil_possibility = true;
        tmp->is_function = true;

        // write(...)
        tmp = insert_symbol(global_table, "write", &internal_error);
        tmp->defined = true;
        tmp->type = IT_ANY;
        tmp->nil_possibility = false;
        tmp->is_function = true;
        if (!string_append(tmp->params, 'a')) return NULL;

        // Int2Double(int) -> double
        tmp = insert_symbol(global_table, "Int2Double", &internal_error);
        tmp->defined = true;
        tmp->type = IT_DOUBLE;
        tmp->nil_possibility = false;
        tmp->is_function = true;
        if (!string_append(tmp->params, 'i')) {
            return NULL;
        }
        if ((tmp->id_names = (char**)realloc(tmp->id_names,1 * sizeof(char*)))==NULL)
            return NULL;
        if((tmp->id_names[0] = (char*)realloc(tmp->id_names[0],strlen("_"))) == NULL)
            return NULL;
        strcpy(tmp->id_names[0],"_");
        // Double2Int(double) -> int
        tmp = insert_symbol(global_table, "Double2Int", &internal_error);
        tmp->defined = true;
        tmp->type = IT_INT;
        tmp->nil_possibility = false;
        tmp->is_function = true;
        if (!string_append(tmp->params, 'd')) {
            return NULL;
        }
        if ((tmp->id_names = (char**)realloc(tmp->id_names,1 * sizeof(char*)))==NULL)
            return NULL;
        if((tmp->id_names[0] = (char*)realloc(tmp->id_names[0],strlen("_"))) == NULL)
            return NULL;
        strcpy(tmp->id_names[0],"_");
        // ord(str) -> int
        tmp = insert_symbol(global_table, "ord", &internal_error);
        tmp->defined = true;
        tmp->type = IT_INT;
        tmp->nil_possibility = false;
        tmp->is_function = true;

        if (!string_append(tmp->params, 's')) {
            return NULL;
        }
        if ((tmp->id_names = (char**)realloc(tmp->id_names,1 * sizeof(char*)))==NULL)
            return NULL;
        if((tmp->id_names[0] = (char*)realloc(tmp->id_names[0],strlen("_"))) == NULL)
            return NULL;
        strcpy(tmp->id_names[0],"_");
        // chr(int) -> str
        tmp = insert_symbol(global_table, "chr", &internal_error);
        tmp->defined = true;
        tmp->type = IT_STRING;
        tmp->nil_possibility = false;
        tmp->is_function = true;

        if (!string_append(tmp->params, 'i')) {
            return NULL;
        }
        if ((tmp->id_names = (char**)realloc(tmp->id_names,1 * sizeof(char*)))==NULL)
            return NULL;
        if((tmp->id_names[0] = (char*)realloc(tmp->id_names[0],strlen("_"))) == NULL)
            return NULL;
        strcpy(tmp->id_names[0],"_");

        // length(str) -> int
        tmp = insert_symbol(global_table, "length", &internal_error);
        tmp->defined = true;
        tmp->type = IT_INT;
        tmp->nil_possibility = false;
        tmp->is_function = true;

        if (!string_append(tmp->params, 's')) {
            return NULL;
        }
        if ((tmp->id_names = (char**)realloc(tmp->id_names,1 * sizeof(char*)))==NULL)
            return NULL;
        if((tmp->id_names[0] = (char*)realloc(tmp->id_names[0],strlen("_"))) == NULL)
            return NULL;
        strcpy(tmp->id_names[0],"_");
        // substring(str, int, int) -> str?
        tmp = insert_symbol(global_table, "substring", &internal_error);
        tmp->defined = true;
        tmp->type = IT_STRING;
        tmp->nil_possibility = true;
        tmp->is_function = true;

        if (!string_append(tmp->params, 's')) {
            return NULL;
        }
        if ((tmp->id_names = (char**)realloc(tmp->id_names,3 * sizeof(char*)))==NULL)
            return NULL;
        if((tmp->id_names[0] = (char*)realloc(tmp->id_names[0],strlen("of"))) == NULL)
            return NULL;
        strcpy(tmp->id_names[0],"of");
        if (!string_append(tmp->params, 'i')) {
            return NULL;
        }
        if((tmp->id_names[1] = (char*)realloc(tmp->id_names[1],strlen("startingAt"))) == NULL)
            return NULL;
        strcpy(tmp->id_names[1],"startingAt");
        if (!string_append(tmp->params, 'i')) {
            return NULL;
        }
        if((tmp->id_names[2] = (char*)realloc(tmp->id_names[2],strlen("endingBefore"))) == NULL)
            return NULL;
        strcpy(tmp->id_names[2],"endingBefore");
    }

    return parser_data;
}

void free_data(parser_data_t *parser_data) {
    table_stack_free(parser_data->table_stack);
}

int analyse() {

    int ret_code = ER_NONE;
    bool flag = false;

    string_ptr string;
    if ((string = string_init()) == NULL)
        PRINT_INTERNAL_NULL_DATA("Internal error in String initialize");

    parser_data_t *data;
    if ((data = init_data()) == NULL)
    {
        string_free(string);
        PRINT_INTERNAL("Internal error in parser_data_init")
    }

    data->line_cnt = 1;

    generator_start();

#ifdef RUNTIME_ALARM
    signal(SIGALRM, timeout_error);
    alarm(10);
#endif


    if ((data->token_ptr = next_token(&(data->line_cnt), &ret_code, &flag,&data->current_char_pos, &data->token_start_pos)) != NULL)
    {
        ret_code = program(data);
    }
    else {
        print_lexical_error(data->line_cnt,data->token_start_pos,"Lex Error");
    }

    generator_end();
    //
    // if ( ret_code > 0) PRINT_UNRESOLVED(ret_code)

    string_free(string);
    free_data(data);

    return ret_code;
}

// <program> -> <stm> EOF
int program(parser_data_t *data) {
    int ret_code = ER_NONE;

    while (data->token_ptr->token_type != T_EOF) {
        CHECK_RULE(stm)
    }

    return ret_code;
}

int stm(parser_data_t *data) {
    int ret_code = ER_NONE;
    bool is_let = false;

    // <stm> -> var + let id : <var_type> = <expression> \n <stm>
    // <stm> -> var + let id : <var_type> \n <stm>
    // <stm> -> var + let id = <expression> \n <stm>
    if (data->token_ptr->token_type == T_KEYWORD && ( data->token_ptr->attribute.keyword == k_var || (is_let = data->token_ptr->attribute.keyword == k_let))) {
        data->is_in_declaration = true;
        VERIFY_TOKEN(T_ID)
        INSERT_SYM()
        if(table_count_elements_in_stack(data->table_stack) == 1)
            data->id->global = true;
        else
            data->id->global = false;
        data->id->defined = false;
        data->id->is_let = is_let;

        gen_define_var(data->id->id,!data->id->global);
        GET_TOKEN()
        if (data->token_ptr->token_type == T_COLON) {
            GET_TOKEN()
            CHECK_RULE(var_type)

            GET_TOKEN()
            if (data->token_ptr->token_type == T_ASSIGMENT) {
                GET_TOKEN()
                CHECK_RULE(expression)
                data->is_in_declaration = false;
                // todo: there may be a problem with EOL
                return stm(data);
            }
            else if (data->eol_flag) {
                data->is_in_declaration = false;
                if(data->id->nil_possibility) {
                    data->id->defined = true;
                }
                return stm(data);
            }
            else {
                PRINT_SYNTAX_ERROR("Waiting Assigment or EOL")
            }
        }
        else if (data->token_ptr->token_type == T_ASSIGMENT) {
            GET_TOKEN()
            CHECK_RULE(expression)

            data->is_in_declaration = false;
            if(data->eol_flag)
                return stm(data);
            VERIFY_TOKEN(T_EOF)
            return ER_NONE;
        }
        else PRINT_SYNTAX_ERROR("Waiting Colon or Assigment")

        GET_TOKEN()
    }

    // <stm> -> func_id( <func_params> ) \n <stm>
    // <stm> -> id = <expression> \n <stm>
    if (data->token_ptr->token_type == T_ID) {
        if(table_count_elements_in_stack(data->table_stack) == 0)
            PRINT_INTERNAL("Stack error in Parser")
        symbol *idFromTable = NULL;

        char *var_name = data->token_ptr->attribute.string;

        GET_TOKEN()
        if (data->token_ptr->token_type == T_BRACKET_OPEN) {

            if((idFromTable = find_symbol_global(data->table_stack, var_name,false)) == NULL)
                PRINT_UNDEF_OR_NOT_INIT_VAR(var_name,data->is_in_declaration)
            data->id = &(idFromTable->data);

            if(!data->id->is_function) {
                PRINT_UNDEF_FUNC_OR_NOT_INIT_VARIABLE()
            }

            data->id_type = data->id;
            data->param_index = 0;

            gen_function_before_params();

            CHECK_RULE(call_params)

            gen_function_pass_param_count(data->param_index+1);

            if (data->token_ptr->token_type != T_BRACKET_CLOSE) PRINT_SYNTAX_ERROR("Waiting close bracket")

            gen_function_call(data->id_type->id);

            GET_TOKEN()

            return stm(data);
        }
        else if (data->token_ptr->token_type == T_ASSIGMENT) {
            if((idFromTable = find_symbol_global(data->table_stack, var_name,false)) == NULL)
                PRINT_UNDEF_OR_NOT_INIT_VAR(var_name,data->is_in_declaration)
            data->id = &(idFromTable->data);

            GET_TOKEN()
            CHECK_RULE(expression)

            if (!data->eol_flag && data->token_ptr->token_type != T_EOF) PRINT_SYNTAX_ERROR("Waiting EOL")

            return stm(data);
        } else
            PRINT_SYNTAX_ERROR("Waiting assigment variable or function call")
    }

    // <stm> -> func func_id( <func_params> ) -> <var_type> { <stm> <return> } \n <stm>
    // <stm> -> func func_id( <func_params> ) { <stm> <return_void> } \n <stm>
    if (data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_func) {
        if(data->func_id != NULL)
            PRINT_UNRESOLVED(ER_OTHER_SEM)
        VERIFY_TOKEN(T_ID)
        data->is_in_declaration = true;

        symbol *idFromTable = NULL;

        if((idFromTable = find_symbol(data->table_stack->top->table, data->token_ptr->attribute.string)) != NULL){
            if(idFromTable->data.is_function)
                PRINT_UNRESOLVED(ER_OTHER_SEM)
        }

        INSERT_SYM()
        data->func_id = data->id;
        VERIFY_TOKEN(T_BRACKET_OPEN)
        data->is_in_params = true;
        data->param_index = 0;
        data->id->is_function = true;
        data->id->id_names = NULL;
        data->is_in_function = true;
        data->id->defined = true;
        hash_table *local_table = create_hash_table();
        table_stack_push(data->table_stack, local_table);
        CHECK_RULE(func_params)
        data->is_in_params = false;

        hash_table *local_table2 = create_hash_table();
        table_stack_push(data->table_stack,local_table2);

        if (data->token_ptr->token_type != T_BRACKET_CLOSE) PRINT_SYNTAX_ERROR("Waiting bracket close")

        GET_TOKEN()
        if (data->token_ptr->token_type == T_ARROW) {

            data->is_void_function = false;

            GET_TOKEN()
            CHECK_RULE(var_type)
            data->is_in_function = false;

            VERIFY_TOKEN(T_CURVED_BRACKET_OPEN)

            GET_TOKEN()
            CHECK_RULE(stm)

            if (data->token_ptr->token_type != T_CURVED_BRACKET_CLOSE) PRINT_SYNTAX_ERROR("Waiting Curved bracket close")
            table_stack_pop(data->table_stack);
            table_stack_pop(data->table_stack);

            GET_TOKEN()

            return stm(data);

        }
        else if (data->token_ptr->token_type == T_CURVED_BRACKET_OPEN) {
            data->is_void_function = true;
            data->is_in_function = false;

            GET_TOKEN()
            CHECK_RULE(stm)

            table_stack_pop(data->table_stack);
            table_stack_pop(data->table_stack);

            GET_TOKEN()

            return stm(data);
        }
        else PRINT_SYNTAX_ERROR("Waiting function body")
    }

    // <stm> -> if ( <condition> ) { <stm> } \n else { <stm> } \n <stm>
    if (data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_if) {


        GET_TOKEN()
        CHECK_RULE(condition)
        hash_table *local_table = create_hash_table();
        table_stack_push(data->table_stack,local_table);

        if (data->token_ptr->token_type != T_CURVED_BRACKET_OPEN) PRINT_SYNTAX_ERROR("Waiting curved bracket open")

        GET_TOKEN()
        CHECK_RULE(stm)

        if (data->token_ptr->token_type != T_CURVED_BRACKET_CLOSE) PRINT_SYNTAX_ERROR("Waiting curved bracket close")


        table_stack_pop(data->table_stack);


        GET_TOKEN()
        if (!(data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_else)) PRINT_SYNTAX_ERROR("Waiting condition bodym non a keyword or ELSE")

        local_table = create_hash_table();
        table_stack_push(data->table_stack,local_table);

        VERIFY_TOKEN(T_CURVED_BRACKET_OPEN)

        GET_TOKEN()
        CHECK_RULE(stm)
        data->is_it_let_condition = false;

        if (data->token_ptr->token_type != T_CURVED_BRACKET_CLOSE) PRINT_SYNTAX_ERROR("Waiting curved bracket close")

        table_stack_pop(data->table_stack);

        GET_TOKEN()

        return stm(data);

    }

    // <stm> -> while ( <condition> ) { <stm> } \n <stm>
    if (data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_while) {

        GET_TOKEN()
        CHECK_RULE(condition)

        hash_table *local_table = create_hash_table();
        table_stack_push(data->table_stack,local_table);

        if (data->token_ptr->token_type != T_CURVED_BRACKET_OPEN) PRINT_SYNTAX_ERROR("Waiting curved bracket open")

        GET_TOKEN()
        CHECK_RULE(stm)

        if (data->token_ptr->token_type != T_CURVED_BRACKET_CLOSE) PRINT_SYNTAX_ERROR("Waiting curved bracket close")

        table_stack_pop(data->table_stack);

        GET_TOKEN()

        return stm(data);
    }
    //return ...
    if (data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_return) {
        if (data->is_void_function) {
            CHECK_RULE(return_void_rule)
            if (data->token_ptr->token_type != T_CURVED_BRACKET_CLOSE) PRINT_SYNTAX_ERROR("Waiting curved bracket close")
        }
        else {
            CHECK_RULE(return_rule)
            // bug?
            if (data->token_ptr->token_type != T_CURVED_BRACKET_CLOSE) return stm(data);
        }
        data->func_id = NULL;
        return ER_NONE;
    }
    // <statement> -> ε

    return ret_code;
}

//<call_params> -> var_name : var_id <call_params_n>
//<call_params> -> var_id <call_params_n>
int  call_params(parser_data_t *data) {
    int ret_code = ER_NONE;

    if((ret_code = check_func_call(data,data->param_index))){
        // todo check it
        return ret_code;
    }

    CHECK_RULE(call_params_n)

    return ret_code;
}

//<call_params> -> var_name : var_id <call_params_n>
//<call_params> -> var_id <call_params_n>
int call_params_n(parser_data_t *data) {
    int ret_code;
    bool its_write = !strcmp(data->id_type->id,"write");

    if (data->token_ptr->token_type == T_BRACKET_CLOSE) {
        return ER_NONE;
    }

    GET_TOKEN()
    if (data->token_ptr->token_type == T_COMMA)
    {
        data->param_index++;

        CHECK_RULE(call_params)
    }
    else if (data->token_ptr->token_type == T_BRACKET_CLOSE) {
        if(data->param_index+1 != data->id_type->params->last_index && !its_write) {
            print_params_error_args_mismatch(data, data->param_index+1, data->id_type->params->last_index);
            PRINT_UNRESOLVED(ER_PARAMS)
        }
        return ER_NONE;
    }
    else {
        PRINT_SYNTAX_ERROR("Waiting Comma or Bracket close")
    }

    return ER_NONE;
}

//<condition> -> let id
//<condition> -> <expression>
int condition(parser_data_t *data) {
    int ret_code = ER_NONE;

    if (data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_let) {
        VERIFY_TOKEN(T_ID)
        if(table_count_elements_in_stack(data->table_stack) == 0)
            PRINT_INTERNAL("Stack error")
        if (!find_symbol(data->table_stack->top->table, data->token_ptr->attribute.string)) {
            PRINT_UNDEF_OR_NOT_INIT_VAR(data->token_ptr->attribute.string,data->is_in_declaration);
        }
        GET_TOKEN()
        data->is_it_let_condition = true;
        return ret_code;
    }
    item_data tmp_data = create_default_item();
    tmp_data.type = IT_BOOL;
    data->id = &tmp_data;
    CHECK_RULE(expression)

    if (data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_let) {
        VERIFY_TOKEN(T_ID)
    }

    return ret_code;
}

//<func_params> -> ε
//<func_params> -> var_name var_id: <var_type> <func_params_not_null>
//<func_params> -> _ var_id: <var_type> <func_params_not_null>
int func_params(parser_data_t *data) {
    int ret_code = ER_NONE;


    if ((data->id->id_names = (char**)realloc(data->id->id_names,(data->param_index+1) * sizeof(char*)))==NULL)
        PRINT_INTERNAL("Allocation memory error")
    GET_TOKEN()


    if(data->token_ptr->token_type == T_UNDERLINE || data->token_ptr->token_type == T_ID){
        if((data->id->id_names[data->param_index] = (char*)realloc(data->id->id_names[data->param_index],strlen(data->token_ptr->attribute.string))) == NULL)
            PRINT_INTERNAL("Allocation memory error")
        if(data->token_ptr->token_type == T_UNDERLINE)
            strcpy(data->id->id_names[data->param_index],"_");
        else
            strcpy(data->id->id_names[data->param_index],data->token_ptr->attribute.string);

        GET_TOKEN()
        if(data->token_ptr->token_type == T_ID){


            if(!strcmp(data->id->id_names[data->param_index],data->token_ptr->attribute.string)){
                PRINT_UNRESOLVED(ER_OTHER_SEM)
            }
            // if there is function named as parameter
            if(table_count_elements_in_stack(data->table_stack) != 2)
                PRINT_INTERNAL("Stack error")
            if (find_symbol(data->table_stack->top->table, data->token_ptr->attribute.string))
                PRINT_UNDEF_OR_NOT_INIT_VAR(data->token_ptr->attribute.string,data->is_in_declaration);

            // if we are in definition, we need to add_LitInt_LitInt parameters to the local symtable
            bool internal_error;
            if(table_count_elements_in_stack(data->table_stack) == 0)
                PRINT_INTERNAL("Stack error")
            if (!(data->exp_type = insert_symbol(data->table_stack->top->table, data->token_ptr->attribute.string,
                                             &internal_error))) {
                if (internal_error) PRINT_INTERNAL("Internal error")
                else PRINT_UNDEF_OR_NOT_INIT_VAR(data->token_ptr->attribute.string,data->is_in_declaration)
            }
            data->exp_type->defined = true;
        }
        else if(data->token_ptr->token_type == T_UNDERLINE){
            item_data tmp_item = create_default_item();

            if((data->exp_type = (item_data *) malloc(sizeof(item_data))) == NULL) {
                PRINT_INTERNAL("Allocate memory error")
            }

            *(data->exp_type) = tmp_item;
        } else
            PRINT_SYNTAX_ERROR("waiting ID or Underline")

        VERIFY_TOKEN(T_COLON)

        GET_TOKEN()
        CHECK_RULE(var_type)


#ifdef PARS_DEBUG
    for(int i = 0; i < data->id->params->last_index;i++){
            printf("string in data->id->id_names %s, position: %d\n",data->id->id_names[i],i);
        }
#endif

        CHECK_RULE(func_params_not_null)
    }
    else {
        return ER_NONE;
    }

    // <func_params> -> ε

    return ER_NONE;
}

// <func_params_not_null> -> , <func_params>
// <func_params_not_null> -> ε
int func_params_not_null(parser_data_t *data) {
    int ret_code;

    GET_TOKEN()
    if (data->token_ptr->token_type == T_COMMA)
    {
        data->param_index++;

        CHECK_RULE(func_params)
    }
    else if (data->token_ptr->token_type == T_BRACKET_CLOSE) {
        return ER_NONE;
    }
    else {
        PRINT_SYNTAX_ERROR("Waiting next function params or Bracket close")
    }

    return ER_NONE;
}

// <nil_flag> -> ! + ε
int nil_flag(parser_data_t *data) {
    if (data->token_ptr->token_type == T_EXCLAMATION_MARK) {
        return ER_NONE;
    }
    else if (data->token_ptr->token_type == T_CURVED_BRACKET_CLOSE) {     // TODO bug?
        return ER_NONE;
    }
    PRINT_SYNTAX_ERROR("Waiting ! or ")
}

// <return> -> return <expression> <nil_flag>
int return_rule(parser_data_t *data) {
    int ret_code = ER_NONE;

    if (!(data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_return))
        PRINT_SYNTAX_ERROR("Waiting non a Keyword or return")
    GET_TOKEN()
    if(data->func_id == NULL)
        PRINT_SYNTAX_ERROR("Waiting return is placed other of body")
    data->id = data->func_id;

    if (data->token_ptr->token_type == T_CURVED_BRACKET_CLOSE) PRINT_FUNC_RETURN(data->id->id);
    CHECK_RULE(expression)

    return ret_code;
}

// <return_void> -> return
// <return_void> -> ε
int return_void_rule(parser_data_t *data) {
    int ret_code;
    if (data->token_ptr->token_type == T_KEYWORD && data->token_ptr->attribute.keyword == k_return) {
        GET_TOKEN()
        return ER_NONE;
    }
    // eps
    else if (data->eol_flag) {
        GET_TOKEN()
        return ER_NONE;
    }
    else {
        PRINT_SYNTAX_ERROR("Waiting 'nothing' or return")
    }
}

item_type get_type(struct token* token, parser_data_t * data, item_data* item){
    symbol* symbol;
    *item = create_default_item();

    switch(token -> token_type){
        case T_ID:
            if(table_count_elements_in_stack(data->table_stack) == 0)
                return IT_UNDEF;
            bool is_overlap;
            if(data->id->id == NULL)
                is_overlap = false;
            else
                is_overlap = !data->id->is_function && !strcmp(data->id->id,data->token_ptr->attribute.string);
            symbol = find_symbol_global(data->table_stack, token->attribute.string,is_overlap);
            if (symbol == NULL){
                item->nil_possibility = false;
                item->defined = false;
                item->is_function = false;
                return IT_UNDEF;
            }
            data->id_type = &(symbol->data);
            if(data->is_it_let_condition)
                item->nil_possibility = false;
            else
                item->nil_possibility = symbol->data.nil_possibility;
            item->defined = symbol->data.defined;
            item->is_function = symbol->data.is_function;
            item->is_let = symbol->data.is_let;
            item->global = symbol->data.global;
            return symbol->data.type;
        case T_INT:
            item->defined = false;
            return IT_INT;
        case T_STRING:
            item->defined = false;
            return IT_STRING;
        case T_DECIMAL:
            item->defined = false;
            return IT_DOUBLE;
        default:
            switch (token->attribute.keyword) {
                case k_int:
                case k_qmark_Int:
                    return IT_INT;
                case k_string:
                case k_qmark_String:
                    return IT_STRING;
                case k_double:
                case k_qmark_Double:
                    return IT_DOUBLE;
                case k_nil:
                    item->nil_possibility = true;
                    return IT_NIL;
                default:
                    return IT_UNDEF;
            }
            return IT_UNDEF;
    }
}

int insert_data_type(parser_data_t *data){
    item_data tmp_item = create_default_item();
    item_type type;
    type = get_type(data->token_ptr,data,&tmp_item);

    //var declaration
    if(data->is_in_declaration && !data->is_in_function && !data->is_in_params){
        data->id->type = type;
        switch (data->token_ptr->token_type) {
            case T_KEYWORD_NIL_POSSIBILITY:
                data->id->nil_possibility = true;
            default:
                break;
        }
    }
    //return func type
    else if(data->is_in_declaration && data->is_in_function && !data->is_in_params){
        data->id->type = type;
        if(data->token_ptr->token_type == T_KEYWORD_NIL_POSSIBILITY)
            data->id->nil_possibility = true;
    }
    //func params
    else if(data->is_in_declaration && data->is_in_function && data->is_in_params) {
        char data_type_for_func_params = 'a';
        switch (type) {
            case IT_INT:
                data_type_for_func_params = 'i' - ((data->token_ptr->token_type == T_KEYWORD_NIL_POSSIBILITY) * 32);
                break;
            case IT_STRING:
                data_type_for_func_params = 's' - ((data->token_ptr->token_type == T_KEYWORD_NIL_POSSIBILITY) * 32);
                break;
            case IT_DOUBLE:
                data_type_for_func_params = 'd' - ((data->token_ptr->token_type == T_KEYWORD_NIL_POSSIBILITY) * 32);
                break;
            default:
                PRINT_INTERNAL("Internal error")
        }
        if (!string_append(data->id->params, data_type_for_func_params)) {
            PRINT_INTERNAL("Internal error in String append")
        }
        data->exp_type->type = type;
    }

    return ER_NONE;
}

int var_type(parser_data_t* data) {
    if (data->token_ptr->token_type == T_KEYWORD || data->token_ptr->token_type == T_KEYWORD_NIL_POSSIBILITY)
    {
        insert_data_type(data);
    }
    else {
        PRINT_SYNTAX_ERROR("Waiting keyword or variable doesnt have nil_flag")
    }
    return ER_NONE;
}
