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
    bool valid = false;
    char * field_name = NULL;
    char * field_data_type = NULL;
    char prompt[STRING_LEN] = {0};
    char * data_type_list = "char, int, string, boolean";
    char * input_mask = NULL;
    field new_field = {0};
    field * fields = NULL;


    sprintf(prompt, "\n~`~CREATING TABLE %s~\n\n", table_name);
    print_color(prompt, BG_WHITE, FG,0,150,0, BOLD, RESET);

    while(true){
        valid = false;

        if(NULL != field_name){
            free(field_name);
            field_name = NULL;
        }
        sprintf(prompt, "~`~Field number %d name:~\n", num_of_fields);
        print_color(prompt, BG_WHITE, FG,0,0,0, BOLD, RESET);
        get_raw_input(NULL, &field_name);

        difference = strncmp(field_name, "quit", STRING_LEN);
        if(0 == difference){
            if(num_of_fields > 0){
                valid = true;
            }
            break;
        }
        
        fields = realloc(fields, (num_of_fields+1) * sizeof(field));
        if(NULL == fields){
            perror("Realloc error");
            num_of_fields = -1;
            goto cleanup;
        }

        if(NULL != field_data_type){
            free(field_data_type);
            field_data_type = NULL;
        }
        sprintf(prompt, "~`~Datatype: (%s):~\n", data_type_list);
        print_color(prompt, BG_WHITE, FG,0,0,0, BOLD, RESET);
        get_raw_input(NULL, &field_data_type);
        lower(field_data_type, 0);

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

            print_color("``~Input mask (null to skip):~ ", BG,168,202,255, FG,0,0,255, BOLD, RESET);
            bytes_returned = get_raw_input(NULL, &input_mask);
            difference = strncmp("null", input_mask, 4);
            if(0 != difference){
                valid = check_input_mask(input_mask);
                if(!valid){
                    print_color("~~INVALID INPUT MASK. NO INPUT MASK ADDED.~\n", B_RED, BOLD, RESET);
                    valid = true;
                }
                else{
                    memcpy(fields[num_of_fields].input_mask, input_mask, bytes_returned);
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
            num_of_fields++;
        }
        else{
            print_color("~`~INVALID REQUEST~\n", BG_B_WHITE, FG,255,0,0, BOLD, RESET);
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
    int sort_fd = 0;
    int i = 0;
    int int_input = 0;
    int byte_input = 0;
    bool has_mask = false;
    bool valid_mask = false;
    field * fields = NULL;
    int * input_lens = NULL;
    char * input = NULL;
    char ** field_inputs = NULL;
    char * sort_file_name = NULL;

    fd = open(table_name, O_RDWR);
    if(-1 == fd){
        perror("EDIT_RECORD_INTERFACE: Open error");
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &fields, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    field_inputs = calloc(num_of_fields, sizeof(char *));
    input_lens = calloc(num_of_fields, sizeof(int));

    print_color("\n~~~EDIT RECORD~\n\n", BG_WHITE, BOLD, MAGENTA, RESET);

    if(-2 == record_num){
        print_color("~~RECORD NUMBER:~\n", B_GREEN, BOLD, RESET);
        error_check = get_raw_input(NULL, &input);
        if(-1 == error_check){
            goto cleanup;
        }
        record_num = (int)strtol(input, NULL, 10);
    }

    for(i=0; i<num_of_fields; i++){
        has_mask = false;
        print_color("~~", B_GREEN, BOLD);
        
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

        print_color("~", RESET);

        error_check = get_raw_input(NULL, &input);
        if(-1 == error_check){
            goto cleanup;
        }

        switch(fields[i].data_len){
            case STRING:
                if(0 != fields[i].input_mask[0]){
                    valid_mask = valid_input(input, fields[i].input_mask);
                    if(-1 == valid_mask){
                        goto cleanup;
                    }
                    if(0 == valid_mask){
                        print_color("~~INVALID INPUT~\n", B_RED, BOLD, RESET);
                        i--;
                        continue;
                    }
                }
                input_lens[i] = strnlen(input, STRING_LEN);
                field_inputs[i] = malloc(input_lens[i]);
                if(NULL == field_inputs[i]){
                    perror("EDIT_RECORD_INTERFACE: Malloc error");
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
                print_color("~~INVALID DATA TYPE~\n", B_RED, BOLD, RESET);
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

    return num_of_records;
}

/**
 * @brief: Aninteractive text interface that gets user input and calls switch_field with it
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
    bool valid = false;
    char * input = NULL;
    char * field_name = NULL;
    char * input_mask = NULL;
    char * data_type_list = "char, int, string, boolean";
    
    print_color("\n~`~EDITING FIELD~\n\n", BG_WHITE, FG,0,100,0, BOLD, RESET);

    print_color("~`~Field number:~\n", BG_WHITE, FG,0,0,0, BOLD, RESET);
    error_check = get_raw_input(NULL, &input);
    if(-1 == error_check){
        return_value = ERROR_CODE_COULDNT_GET_INPUT;
        goto cleanup;
    }
    field_num = (int)strtol(input, NULL, 10);

    print_color("~`~Field name:~\n", BG_WHITE, FG,0,0,0, BOLD, RESET);
    error_check = get_raw_input(NULL, &field_name);
    if(-1 == error_check){
        return_value = ERROR_CODE_COULDNT_GET_INPUT;
        goto cleanup;
    }

    while(!valid){
        print_color("~`~", BG_WHITE, FG,0,0,0, BOLD);
        printf("Datatype: (%s)", data_type_list);
        print_color("~\n", RESET);
        error_check = get_raw_input(NULL, &input);
        if(-1 == error_check){
            return_value = ERROR_CODE_COULDNT_GET_INPUT;
            goto cleanup;
        }
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

        print_color("~~INVALID DATATYPE~\n", B_RED, BOLD, RESET);
    }
    input_mask = NULL;
    if(STRING == datatype){
        while(!valid){
            print_color("``~Input mask (null to skip):~ ", BG,168,202,255, FG,0,0,255, BOLD, RESET);
            error_check = get_raw_input(NULL, &input_mask);
            if(-1 == error_check){
                return_value = ERROR_CODE_COULDNT_GET_INPUT;
                goto cleanup;
            }

            difference = strncmp("null", input_mask, error_check);
            if(0 == difference){
                free(input_mask);
                input_mask = NULL;
                break;
            }

            valid = check_input_mask(input_mask);
            if(!valid){
                print_color("~~INVALID INPUT MASK~\n", BOLD, B_RED, RESET);
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

    return return_value;
}

error_code_t query_interface(char * table_name, char * sort_file_name){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int error_check = 0;
    int table_fd = -1;
    int sort_fd = -1;
    int num_of_fields = 0;
    int num_of_records = 0;
    int field_index = -1;
    int data = 0;
    int i = 0;
    int j = 0;
    int difference = 0;
    bool continue_loop = true;
    bool can_free_target_data = true;
    bool can_free_record = false;
    operators operator = -1;
    char * input = NULL;
    char * field_name = NULL;
    char * target_data = NULL;
    char input_print[STRING_LEN+1] = {0};
    char * print_text = NULL;
    field * fields = NULL;
    bool * valid_record_map = NULL;
    record_field * record = NULL;

    table_fd = open(table_name, O_RDWR);
    if(-1 == table_fd){
        perror("QUERY_INTERFACE: Open error");
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }

    sort_fd = open(sort_file_name, O_RDWR);
    if(-1 == sort_fd){
        perror("QUERY_INTERFACE: Open error");
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }

    num_of_fields = get_fields(table_fd, &fields, false);
    if(-1 == num_of_fields){
        return_value = ERROR_CODE_COULDNT_GET_NUM_OF_FIELDS;
        goto cleanup;
    }

    num_of_records = get_num_of_records(table_fd, num_of_fields, false);
    if(-1 == num_of_records){
        return_value = ERROR_CODE_COULDNT_GET_NUM_OF_RECORDS;
        goto cleanup;
    }

    print_color("\n~`~CREATING QUERY~\n\n", BG_WHITE, FG,200,100,0, BOLD, RESET);
    
    while(continue_loop){
        print_color("~~Field Name:~ ", B_GREEN, BOLD, RESET);
        error_check = get_raw_input(NULL, &field_name);
        if(-1 == error_check){
            return_value = ERROR_CODE_COULDNT_GET_INPUT;
            goto cleanup;
        }
        
        difference = strncmp(field_name, "quit", NAME_LEN);
        if(0 == difference){
            return_value = ERROR_CODE_SUCCESS;
            goto cleanup;
        }

        for(i=0; i<num_of_fields; i++){
            difference = strncmp(field_name, fields[i].name, NAME_LEN);
            if(0 == difference){
                field_index = i;
                continue_loop = false;
                break;
            }
        }
        if(-1 == field_index){
            print_color("~~INVALID FIELD NAME~\n", RED, BOLD, RESET);
        }
    }

    print_color("~~Target Data:~ ", B_GREEN, BOLD, RESET);
    error_check = get_raw_input(NULL, &target_data);
    if(-1 == error_check){
        return_value = ERROR_CODE_COULDNT_GET_INPUT;
        goto cleanup;
    }

    switch(fields[field_index].data_len){
        case STRING:
            break;

        case INT:
            data = (int)strtol(target_data, NULL, 10);
            free(target_data);
            can_free_target_data = false;
            target_data = (char *)&data;
            break;

        case CHAR:
            data = (char)strtol(target_data, NULL, 10);
            free(target_data);
            can_free_target_data = false;
            target_data = (char *)&data;
            break;

        default:
            print_color("~~INVALID DATATYPE~\n", RED, BOLD, RESET);
            return_value = ERROR_CODE_INVALID_DATATYPE;
            goto cleanup;
    }

    continue_loop = true;
    while(continue_loop){
        print_color("~~Operation:~ ", B_GREEN, BOLD, RESET);
        error_check = get_raw_input(NULL, &input);
        if(-1 == error_check){
            return_value = ERROR_CODE_COULDNT_GET_INPUT;
            goto cleanup;
        }
        
        difference = strncmp(input, "quit", NAME_LEN);
        if(0 == difference){
            return_value = ERROR_CODE_SUCCESS;
            goto cleanup;
        }

        for(i=0; i<num_of_operations; i++){
            difference = strncmp(input, operations[i], NAME_LEN);
            if(0 == difference){
                operator = i;
                continue_loop = false;
                break;
            }
        }
        if(-1 == operator){
            print_color("~~INVALID OPERATOR~\n", RED, BOLD, RESET);
        }
    }

    return_value = get_valid_record_map(table_fd, sort_fd, target_data, operator, field_index, num_of_records, &valid_record_map);
    if(ERROR_CODE_SUCCESS != return_value){
        goto cleanup;
    }

    for(i=0; i<num_of_fields; i++){
        rect_text(fields[i].name, &print_text, NAME_LEN);
        print_color("~`~", BG_RED, FG,0,0,0, BOLD);
        printf("%s", print_text);
        print_color("~ ", RESET);
    }
    printf("\n");
    for(i=0; i<num_of_records; i++){
        if(valid_record_map[i]){
            if(NULL != record){
                for(j=0; j<num_of_fields; j++){     //I accidentally used 
                    if(NULL != record[j].data){
                        free(record[j].data);
                    }
                }
                free(record);
            }

            return_value = get_record(table_fd, &record, i, false);
            if(ERROR_CODE_SUCCESS != return_value){
                goto cleanup;
            }

            for(j=0; j<num_of_fields; j++){ 
                switch(record[j].data_len){
                    case STRING:
                        sprintf(input_print, "%s", record[j].data);
                        break;
                    case INT:
                        sprintf(input_print, "%i", *((int *)record[j].data));
                        break;
                    case CHAR:
                        sprintf(input_print, "%i", *((char *)record[j].data));
                        break;
                    default:
                        print_color("~~INVALID DATATYPE~\n", RED, BOLD, RESET);
                        return_value = ERROR_CODE_INVALID_DATATYPE;
                        goto cleanup;
                }

                rect_text(input_print, &print_text, NAME_LEN);
                print_color("~`~", BG_BLACK, FG,255,255,255, BOLD);
                printf("%s", print_text);
                print_color("~ ", RESET);
            }
            printf("\n");
        }
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    if(NULL != fields)
        free(fields);
    if(NULL != input)
        free(input);
    if(NULL != field_name)
        free(field_name);
    if(NULL != target_data && can_free_target_data)
        free(target_data);
    if(NULL != print_text)
        free(print_text);
    if(NULL != valid_record_map)
        free(valid_record_map);
        
    if(NULL != record){
        for(i=0; i<num_of_fields; i++){
            if(NULL != record[i].data)
                free(record[i].data);
        }
        free(record);
    }

    if(-1 != table_fd)
        close(table_fd);
    if(-1 != sort_fd)
        close(sort_fd);

    return return_value;
}