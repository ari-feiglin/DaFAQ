#include "dafaq.h"

/** 
 * @brief: The interactive text interface that gets user input and calls create_table with it
 * @param[IN] table_name: The name of the file to create to hold the table
 * 
 * @return: On success the number of fields in the table, else -1
 * @notes: This function creates an array of field structures based off of user input and passes them to the
 *         create_table function. Input handling is done here. 
 */
int create_table_interface(IN char * table_name){
    int num_of_fields = 0;
    int difference = 0;
    int error_check = -1;
    int num_of_records = 0;
    int bytes_returned = 0;
    int row_len = 75;
    bool valid = false;
    char * field_name = NULL;
    char * field_data_type = NULL;
    char prompt[STRING_LEN] = {0};
    char * data_type_list = "char, int, string, boolean";
    char * input_mask = NULL;
    field_t new_field = {0};
    field_t * fields = NULL;

    print_indent_line(2, row_len, true, 0,200,255);
    print_color("`\n~", RESET, BG,0,0,0);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`\n", RESET);

    sprintf(prompt, "`  ~~CREATING TABLE %s~\n", table_name);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color(prompt, BG,0,0,0, BOLD, UNDERLINE, RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    while(true){
        valid = false;

        if(NULL != field_name){
            free(field_name);
            field_name = NULL;
        }
        sprintf(prompt, "`  ~Field number %d name:~\n", num_of_fields);
        print_indent_line(1, row_len, true, 0,200,255);
        print_color(prompt, BG,0,0,0, BOLD, RESET);
        print_indent_line(1, row_len, true, 0,200,255);
        print_color("`  ", BG,0,0,0);
        error_check = get_raw_input(NULL, &field_name);
        if(-1 == error_check){
            num_of_fields = -1;
            goto cleanup;
        }
        print_color("~", RESET);

        difference = strncmp(field_name, "quit", STRING_LEN);
        if(0 == difference){
            if(num_of_fields > 0){
                valid = true;
            }
            break;
        }
        
        fields = realloc(fields, (num_of_fields+1) * sizeof(field_t));
        if(NULL == fields){
            print_indent_line(1, row_len, true, 255,0,0);
            print_color("`  ~", BG,0,0,0, RED);
            fflush(stdout);
            perror("CREATE_TABLE_INTERFACE: Realloc error");
            print_color("~", RESET);
            num_of_fields = -1;
            goto cleanup;
        }
        memset(&fields[num_of_fields], 0, sizeof(field_t));

        if(NULL != field_data_type){
            free(field_data_type);
            field_data_type = NULL;
        }
        /*sprintf(prompt, "~`~Datatype: (%s):~\n", data_type_list);
        print_color(prompt, BG_WHITE, FG,0,0,0, BOLD, RESET);
        get_raw_input(NULL, &field_data_type);
        lower(field_data_type, 0);*/

        sprintf(prompt, "`  ~Datatype: (%s):~\n", data_type_list);
        print_indent_line(1, row_len, true, 0,200,255);
        print_color(prompt, BG,0,0,0, BOLD, RESET);
        print_indent_line(1, row_len, true, 0,200,255);
        print_color("`  ", BG,0,0,0);
        get_raw_input(NULL, &field_data_type);
        print_color("~", RESET);
        lower(field_data_type, 0);

        memset(new_field.name, 0, NAME_LEN);
        memset(new_field.input_mask, 0, NAME_LEN);
        strncpy(new_field.name, field_name, strnlen(field_name, NAME_LEN));

        difference = strncmp(field_data_type, "char", STRING_LEN);
        if(0 == difference){
            new_field.data_len = CHAR;
            valid = true;
        }

        difference = strncmp(field_data_type, "int", STRING_LEN);
        if(0 == difference){
            new_field.data_len = INT;
            valid = true;
        }

        difference = strncmp(field_data_type, "string", STRING_LEN);
        if(0 == difference){
            new_field.data_len = STRING_LEN;
            valid = true;

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  ~Input mask (null to skip):~\n", BG,0,0,0, BG,168,202,255, FG,0,0,255, BOLD, RESET, BG,0,0,0);
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  ", BG,0,0,0);
            bytes_returned = get_raw_input(NULL, &input_mask);
            print_color("~", RESET);

            difference = strncmp("null", input_mask, 4);
            if(0 != difference){
                valid = check_input_mask(input_mask);
                if(!valid){
                    print_indent_line(1, row_len, true, 255,0,0);
                    print_color("`  ~~INVALID INPUT MASK. NO INPUT MASK ADDED.~\n", BG,0,0,0, RED, BOLD, RESET);
                    valid = true;
                }
                else{
                    memcpy(new_field.input_mask, input_mask, bytes_returned);
                }
            }
        }

        difference = strncmp(field_data_type, "boolean", STRING_LEN);
        if(0 == difference){
            new_field.data_len = BOOLEAN;
            valid = true;
        }

        if(valid){
            fields[num_of_fields].data_len = new_field.data_len;
            memcpy(fields[num_of_fields].name, new_field.name, NAME_LEN);
            memcpy(fields[num_of_fields].input_mask, new_field.input_mask, NAME_LEN);
            num_of_fields++;
        }
        else{
            print_indent_line(1, row_len, true, 255,0,0);
            print_color("`  ~~INVALID REQUEST~\n", BG,0,0,0, RED, BOLD, RESET);
        }
    }

    error_check = create_table(table_name, num_of_fields, fields);
    if(-1 == error_check){
        num_of_fields = -1;
        goto cleanup;
    }


cleanup:
    if(NULL != field_name)
        free(field_name);
    if(NULL != field_data_type)
        free(field_data_type);
    if(NULL != fields)
        free(fields);
    if(NULL != input_mask)
        free(input_mask);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("~\n", RESET);
  
    return num_of_fields;
}

/** 
 * @brief: An interactive text interface that gets user input and calls switch_record with it
 * @param[IN] table_name: The name of the table file to operate on
 * @param[IN] record_num: The number of the target record to change. An input of record_num=-2
 *                           will allow for user input of the record_num
 * @return: On success the number of records in the file, else -1
 * @notes: An input of record_num = -1 will append the record to the table. The function allows for
 *         caller input so the implementer can allow the user to append records.
 */
int switch_record_interface(IN char * table_name, int record_num){
    int num_of_fields = 0;
    int num_of_records = -1;
    int error_check = 0;
    int fd = -1;
    int sort_fd = -1;
    int i = 0;
    int int_input = 0;
    int byte_input = 0;
    int row_len = 75;
    bool has_mask = false;
    bool valid_mask = false;
    field_t * fields = NULL;
    int * input_lens = NULL;
    char * input = NULL;
    char ** field_inputs = NULL;
    char * sort_file_name = NULL;

    print_indent_line(2, row_len, true, 0,200,255);
    print_color("`\n~", RESET, BG,0,0,0);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`\n", RESET);

    fd = open(table_name, O_RDWR);
    if(-1 == fd){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~~", BG,0,0,0, RED, BOLD);
        fflush(stdout);
        perror("EDIT_RECORD_INTERFACE: Open error");
        print_color("~", RESET);
        
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &fields, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    field_inputs = calloc(num_of_fields, sizeof(char *));
    input_lens = calloc(num_of_fields, sizeof(int));

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~~EDIT RECORD~\n", BG,0,0,0, BOLD, UNDERLINE, RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    printf("\n");

    if(-2 == record_num){
        print_indent_line(1, row_len, true, 0,200,255);
        print_color("`  ~Record Number:~\n", BG,0,0,0, BOLD, RESET);
        print_indent_line(1, row_len, true, 0,200,255);
        print_color("`  ", BG,0,0,0);
        error_check = get_raw_input(NULL, &input);
        print_color("~", RESET);
        if(-1 == error_check){
            goto cleanup;
        }
        record_num = (int)strtol(input, NULL, 10);
    }

    for(i=0; i<num_of_fields; i++){
        has_mask = false;

        print_color("~", RESET);
        print_indent_line(1, row_len, true, 0,200,255);
        print_color("`  ~", BG,0,0,0, BOLD);
        
        switch(fields[i].data_len){
            case STRING: 
                if(0 != fields[i].input_mask[0]){
                    has_mask = true;
                    printf("%s (STRING - INPUT MASK: %s): ", fields[i].name, fields[i].input_mask); 
                }
                else{
                    printf("%s (STRING): ", fields[i].name);
                }
                break;

            case INT: printf("%s (INT): ", fields[i].name); break;
            case CHAR: printf("%s (BYTE VALUE): ", fields[i].name); break;

            default:
                print_color("~~INVALID DATA TYPE!~\n", B_RED, BOLD, RESET);
                goto cleanup;
        }

        print_color("~`", RESET, BG,0,0,0);

        error_check = get_raw_input(NULL, &input);
        if(-1 == error_check){
            goto cleanup;
        }

        switch(fields[i].data_len){
            case STRING:
                if(0 != fields[i].input_mask[0]){
                    valid_mask = valid_input(input, fields[i].input_mask);
                    if(-1 == valid_mask){
                        num_of_records = -1;
                        goto cleanup;
                    }
                    if(0 == valid_mask){
                        print_indent_line(1, row_len, true, 255,0,0);
                        print_color("`  ~~INVALID INPUT~\n", BG,0,0,0, RED, BOLD, RESET);
                        i--;
                        continue;
                    }
                }
                input_lens[i] = strnlen(input, STRING_LEN);
                field_inputs[i] = malloc(input_lens[i]);
                if(NULL == field_inputs[i]){
                    print_indent_line(1, row_len, true, 255,0,0);
                    print_color("`  ~~", BG,0,0,0, RED, BOLD);
                    fflush(stdout);
                    perror("EDIT_RECORD_INTERFACE: Malloc error");
                    print_color("~", RESET);
                    goto cleanup;
                }
                memcpy(field_inputs[i], input, input_lens[i]);
                break;
            
            case INT:
                input_lens[i] = sizeof(int_input);
                field_inputs[i] = malloc(input_lens[i]);

                int_input = (int)strtol(input, NULL, 10);
                memcpy(field_inputs[i], &int_input, sizeof(int_input));
                break;
            
            case CHAR:
                input_lens[i] = sizeof(byte_input);
                field_inputs[i] = malloc(input_lens[i]);

                byte_input = (char)strtol(input, NULL, 10);
                memcpy(field_inputs[i], &byte_input, sizeof(byte_input));
                break;

            default:
                print_indent_line(1, row_len, true, 255,0,0);
                print_color("`  ~~INVALID DATA TYPE~\n", BG,0,0,0, RED, BOLD, RESET);
                goto cleanup;
                break;      //:( maybe one day...
        }
    }

    sort_file_name = malloc(strnlen(table_name, STRING_LEN) + extension_len + 1);
    sprintf(sort_file_name, "%s.srt", table_name);

    sort_fd = open(sort_file_name, O_RDWR);

    num_of_records = switch_record(fd, record_num, input_lens, field_inputs, sort_fd);
    if(-1 == num_of_records){
        goto cleanup;
    }


cleanup:
    if(NULL != field_inputs){
        for(i=0; i<num_of_fields; i++){
            if(NULL != field_inputs[i]){
                free(field_inputs[i]);
            }
        }
        free(field_inputs);
    }
    if(NULL != input_lens)
        free(input_lens);
    if(NULL != fields)
        free(fields);
    if(NULL != input)
        free(input);
    if(NULL != sort_file_name)
        free(sort_file_name);
    if(-1 != fd)
        close(fd);
    if(-1 != sort_fd)
        close(sort_fd);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    return num_of_records;
}

/**
 * @brief: An interactive text interface that gets user input and calls switch_field with it
 * @param[IN] table_name: The name of the table file to edit
 * 
 * @return: ERROR_CODE_SUCCESS on success, else an indicative error code of type error_code_t
 * @notes: The function doesn't allow for caller input of field_num because it is assumed that if
 *         the implementer wanted to change a field, they would use switch_field.
 */
error_code_t switch_field_interface(IN char * table_name){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int error_check = 0;
    int datatype = 0;
    int field_num = 0;
    int difference = 0;
    int row_len = 75;
    bool valid = false;
    char * input = NULL;
    char * field_name = NULL;
    char * input_mask = NULL;
    char * data_type_list = "char, int, string, boolean";

    print_indent_line(2, row_len, true, 0,200,255);
    print_color("`\n~", RESET, BG,0,0,0);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`\n", RESET);
    
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~~EDITING FIELD~\n", BG,0,0,0, BOLD, UNDERLINE, RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    printf("\n");

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~Field number:~\n", BG,0,0,0, BOLD, RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ", BG,0,0,0);
    error_check = get_raw_input(NULL, &input);
    if(-1 == error_check){
        return_value = ERROR_CODE_COULDNT_GET_INPUT;
        goto cleanup;
    }
    field_num = (int)strtol(input, NULL, 10);
    print_color("~", RESET);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ~Field name:~\n", BG,0,0,0, BOLD, RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("`  ", BG,0,0,0);
    error_check = get_raw_input(NULL, &field_name);
    if(-1 == error_check){
        return_value = ERROR_CODE_COULDNT_GET_INPUT;
        goto cleanup;
    }
    print_color("~", RESET);

    while(!valid){
        print_indent_line(1, row_len, true, 0,200,255);
        print_color("`  ~", BG,0,0,0, BOLD);
        printf("Datatype: (%s):", data_type_list);
        print_color("~\n", RESET);
        print_indent_line(1, row_len, true, 0,200,255);
        print_color("`  ", BG,0,0,0);
        error_check = get_raw_input(NULL, &input);
        if(-1 == error_check){
            return_value = ERROR_CODE_COULDNT_GET_INPUT;
            goto cleanup;
        }
        print_color("~", RESET);

        difference = strncmp("int", input, error_check);
        if(0 == difference){
            datatype = INT;
            break;
        }
        difference = strncmp("char", input, error_check);
        if(0 == difference){
            datatype = CHAR;
            break;
        }
        difference = strncmp("boolean", input, error_check);
        if(0 == difference){
            datatype = BOOLEAN;
            break;
        }
        difference = strncmp("string", input, error_check);
        if(0 == difference){
            datatype = STRING;
            break;
        }

        print_indent_line(1, row_len, true, 200,0,0);
        print_color("`  ~~INVALID DATATYPE~\n", BG,0,0,0, RED, BOLD, RESET);
    }
    input_mask = NULL;
    if(STRING == datatype){
        while(!valid){
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  ~Input mask (null to skip):~\n", BG,0,0,0, BOLD, RESET);

            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  ", BG,0,0,0);
            error_check = get_raw_input(NULL, &input_mask);
            if(-1 == error_check){
                return_value = ERROR_CODE_COULDNT_GET_INPUT;
                goto cleanup;
            }
            print_color("~", RESET);

            difference = strncmp("null", input_mask, error_check);
            if(0 == difference){
                free(input_mask);
                input_mask = NULL;
                break;
            }

            valid = check_input_mask(input_mask);
            if(!valid){
                print_indent_line(1, row_len, true, 200,0,0);
                print_color("`  ~~INVALID INPUT MASK~\n", BG,0,0,0, BOLD, RED, RESET);
            }
        }
    }

    return_value = switch_field(table_name, field_name, datatype, input_mask, field_num);
    if(ERROR_CODE_SUCCESS != return_value){
        goto cleanup;
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    if(NULL != input)
        free(input);
    if(NULL != field_name)
        free(field_name);
    if(NULL != input_mask)
        free(input_mask);

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    return return_value;
}

/**
 * @brief: An interactive text interface that gets user input and calls binary_search_sot_file (and quicksort_field
 *         if needed) with it, and formats and manages the output.
 * @param[IN] table_name: The name of the table file to query
 * @param[IN] sort_file_name: The name of the table file's corresponding sort_file
 * 
 * @returns: ERROR_CODE_SUCCESS on success, else an indicative error of type error_code_t
 */
error_code_t query_interface(char * table_name, char * sort_file_name){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int row_len = 75;
    int num_of_fields = 0;
    int num_of_records = 0;
    int i = 0;
    int j = 0;
    int difference = 0;
    int field_index = 0;
    bool continue_loop = true;
    bool inputting_conditions = true;
    char * input = NULL;
    int temp_data[MAX_CALLS] = {0};
    int freeable_targets[MAX_CALLS] = {0};
    query_t query = {0};
    field_t * fields = NULL;
    char * query_file_name = NULL;

    print_indent_line(2, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    query.table_fd = open(table_name, O_RDONLY);
    if(-1 == query.table_fd){
        perror("QUERY_INTERFACE: Open error");
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }

    query.sort_fd = open(sort_file_name, O_RDWR);
    if(-1 == query.sort_fd){
        perror("QUERY_INTERFACE: Open error");
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }

    num_of_fields = get_fields(query.table_fd, &fields, false);
    if(-1 == num_of_fields){
        return_value = ERROR_CODE_COULDNT_GET_NUM_OF_FIELDS;
        goto cleanup;
    }

    num_of_records = get_num_of_records(query.table_fd, num_of_fields, false);
    if(-1 == num_of_records){
        return_value = ERROR_CODE_COULDNT_GET_NUM_OF_RECORDS;
        goto cleanup;
    }

    for(i=0; i<MAX_CALLS && inputting_conditions; i++){
        continue_loop = true;
        while(continue_loop){
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~~  ~SELECT~ Field Name:~~ ", BG_BLACK, BOLD, BLUE, WHITE, RESET, BG_BLACK);
            get_raw_input(NULL, &input);

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                inputting_conditions = false;
                break;
            }

            difference = strncmp(input, "*", NAME_LEN);
            if(0 == difference){
                for(j=0; j<num_of_fields; j++){
                    query.selected_field_map[j] = true;
                }
                inputting_conditions = false;
                break;
            }

            field_index = -1;
            for(j=0; j<num_of_fields; j++){
                difference = strncmp(fields[j].name, input, STRING_LEN);
                if(0 == difference){
                    field_index = j;
                    break;
                }
            }

            if(-1 != field_index){
                if(!query.selected_field_map[field_index]){
                    query.selected_field_map[field_index] = true;
                }

                continue_loop = false;
            }
            else{
                print_indent_line(1, row_len, true, 255,0,0);
                print_color("~~~  INVALID FIELD NAME!~\n", BG_BLACK, RED, BOLD, RESET);
            }
        }
    }

    inputting_conditions = true;

    for(i=0; i<MAX_CALLS && inputting_conditions; i++){
        continue_loop = true;
        while(continue_loop){
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("~~  ~WHERE~ Field Name:~~ ", BG_BLACK, BOLD, GREEN, WHITE, RESET, BG_BLACK);
            get_raw_input(NULL, &input);

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                inputting_conditions = false;
                break;
            }

            field_index = -1;
            for(j=0; j<num_of_fields; j++){
                difference = strncmp(fields[j].name, input, STRING_LEN);
                if(0 == difference){
                    field_index = j;
                    break;
                }
            }

            if(-1 != field_index){
                query.conditions[i].field_index = field_index;
                query.num_of_conditions++;

                if(!query.selected_field_map[field_index]){
                    query.num_of_fields++;
                }

                continue_loop = false;
            }
            else{
                print_indent_line(1, row_len, true, 255,0,0);
                print_color("~~~  INVALID FIELD NAME!~\n", BG_BLACK, RED, BOLD, RESET);
            }
        }

        if(!inputting_conditions){
            break;
        }

        continue_loop = true;
        while(continue_loop){
            print_indent_line(1, row_len, true, 0, 200, 255);
            print_color("~~  Operator:~~ ", BG_BLACK, BOLD, RESET, BG_BLACK);
            get_raw_input(NULL, &input);

            query.conditions[i].operator = -1;
            for(j=0; j<num_of_operations; j++){
                difference = strncmp(input, operations[j], NAME_LEN);
                if(0 == difference){
                    query.conditions[i].operator = j;
                    continue_loop = false;
                    break;
                }
            }
            if(-1 == query.conditions[i].operator){
                print_indent_line(1, row_len, true, 255,0,0);
                print_color("~~~  INVALID OPERATOR!~\n", BG_BLACK, RED, BOLD, RESET);
            }
        }

        print_indent_line(1, row_len, true, 0, 200, 255);
        print_color("~~  Value? ([y]/n)~~ ", BG_BLACK, BOLD, RESET, BG_BLACK);
        get_raw_input(NULL, &input);

        difference = strncmp(input, "n", NAME_LEN);
        if(0 != difference){
            query.conditions[i].target_id = INTEGRAL;

            print_indent_line(1, row_len, true, 0, 200, 255);
            print_color("~~  Target Value:~~ ", BG_BLACK, BOLD, RESET, BG_BLACK);
            get_raw_input(NULL, &input);

            switch(fields[query.conditions[i].field_index].data_len){
                case STRING:
                    query.conditions[i].target.target_data = malloc(strnlen(input, STRING_LEN) + 1);
                    if(NULL == query.conditions[i].target.target_data){
                        perror("QUERY_INTERFACE: Malloc error");
                        return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
                        goto cleanup;
                    }
                    memcpy(query.conditions[i].target.target_data, input, strnlen(input, STRING_LEN) + 1);
                    freeable_targets[i] = true;
                    break;

                case INT:
                    temp_data[i] = (int)strtol(input, NULL, 10);
                    query.conditions[i].target.target_data = (char *)&(temp_data[i]);
                    break;

                case CHAR:
                    temp_data[i] = (char)strtol(input, NULL, 10);
                    query.conditions[i].target.target_data = (char *)&(temp_data[i]);
                    break;

                default:
                    print_indent_line(1, row_len, true, 255,0,0);
                    print_color("~~~  INVALID DATATYPE!~\n", BG_BLACK, RED, BOLD, RESET);
                    return_value = ERROR_CODE_INVALID_DATATYPE;
                    goto cleanup;
                    break;          //      :)
            }
        }
        else{
            query.conditions[i].target_id = FIELD;

            continue_loop = true;
            while(continue_loop){
                print_indent_line(1, row_len, true, 0,200,255);
                print_color("~~  Target Field:~~ ", BG_BLACK, BOLD, RESET, BG_BLACK);
                get_raw_input(NULL, &input);

                difference = strncmp(input, "quit", NAME_LEN);
                if(0 == difference){
                    inputting_conditions = false;
                    break;
                }

                field_index = -1;
                for(j=0; j<num_of_fields; j++){
                    difference = strncmp(fields[j].name, input, STRING_LEN);
                    if(0 == difference){
                        field_index = j;
                        break;
                    }
                }

                if(-1 != field_index){
                    query.conditions[i].target.target_field_index = field_index;

                    continue_loop = false;
                }
                else{
                    print_indent_line(1, row_len, true, 255,0,0);
                    print_color("~~~  INVALID FIELD NAME!~\n", BG_BLACK, RED, BOLD, RESET);
                }
            }
        }

        continue_loop = true;
        while(continue_loop){
            print_indent_line(1, row_len, true, 0, 200, 255);
            print_color("~~  Logical Operator:~~ ", BG_BLACK, BOLD, RESET, BG_BLACK);
            get_raw_input(NULL, &input);

            difference = strncmp(input, "quit", NAME_LEN);
            if(0 == difference){
                inputting_conditions = false;
                break;
            }

            difference = strncmp(input, "||", NAME_LEN);
            if(0 == difference){
                query.conditions[i].logical_operator = OR;
                break;
            }

            difference = strncmp(input, "&&", NAME_LEN);
            if(0 == difference){
                query.conditions[i].logical_operator = AND;
                break;
            }

            print_indent_line(1, row_len, true, 255,0,0);
            print_color("~~~  INVALID LOGICAL OPERATOR!~\n", BG_BLACK, RED, BOLD, RESET);
        }
    }

    print_indent_line(1, row_len, true, 0, 200, 255);
    puts("");
    print_indent_line(1, row_len, true, 0, 200, 255);
    print_color("~~  Query File Name:~~ ", BG_BLACK, BOLD, RESET, BG_BLACK);
    get_raw_input(NULL, &query_file_name);

    difference = strncmp(query_file_name, "quit", NAME_LEN);
    if(0 == difference){
        free(query_file_name);
        query_file_name = NULL;
    }

    print_indent_line(1, row_len, true, 0,200,255);
    puts("");
    print_indent_line(1, row_len, true, 0,200,255);
    puts("");
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~  ~~YOUR QUERY:~\n", BG_BLACK, UNDERLINE, BOLD, RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    puts("");
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~~~  SELECT~~ ", BG_BLACK, BLUE, BOLD, RESET, BG_BLACK);

    for(i=0; i<num_of_fields; i++){
        if(query.selected_field_map[i]){
            printf("%s, ", fields[i].name);
        }
    }

    print_color("~\n", RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~~  ~FROM~~ ", BG_BLACK, YELLOW, BOLD, RESET, BG_BLACK);
    printf("%s", table_name);

    print_color("~\n", RESET);
    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~~~  WHERE~~ ", BG_BLACK, GREEN, BOLD, RESET, BG_BLACK);

    for(i=0; i<query.num_of_conditions; i++){
        printf("%s ", fields[query.conditions[i].field_index].name);
        printf("%s ", operations[query.conditions[i].operator]);

        if(INTEGRAL == query.conditions[i].target_id){
            switch(fields[query.conditions[i].field_index].data_len){
                case STRING: printf("%s ", query.conditions[i].target.target_data); break;
                case INT: printf("%i ", *((int *)query.conditions[i].target.target_data)); break;
                case CHAR: printf("%i ", *((char *)query.conditions[i].target.target_data)); break;
            }
        }
        else if(FIELD == query.conditions[i].target_id){
            printf("[%s] ", fields[query.conditions[i].target.target_field_index].name);
        }
        
        if(i != query.num_of_conditions - 1){
            switch(query.conditions[i].operator){
                case AND: printf("&& "); break;
                case OR: printf("|| "); break;
            }
        }
    }
    puts("");
    print_indent_line(1, row_len, true, 0,200,255);
    puts("");

    while(true){
        return_value = execute_query(query, query_file_name);
        if(ERROR_CODE_EXISTS == return_value){
            print_indent_line(1, row_len, true, 0, 200, 255);
            print_color("~~  ~ERROR: FILE EXISTS~ Query File Name:~~ ", BG_BLACK, BOLD, RED, WHITE, RESET, BG_BLACK);
            get_raw_input(NULL, &query_file_name);
            continue;
        }
        break;
    }

cleanup:
    if(NULL != fields){
        free(fields);
    }
    if(NULL != input){
        free(input);
    }

    for(i=0; i<MAX_CALLS; i++){
        if(freeable_targets[i]){
            free(query.conditions[i].target.target_data);
        }
    }

    print_indent_line(1, row_len, true, 0,200,255);
    print_color("~\n", RESET);
    print_indent_line(2, row_len, true, 0,200,255);
    print_color("~\n", RESET);

    return return_value;
}
