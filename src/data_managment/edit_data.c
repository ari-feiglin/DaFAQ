#include "dafaq.h"

/**
 * @brief: Edits or appends a field to a table
 * @param[IN] file_name: The name of the table file
 * @param[IN] field_name: The new name of the field (or NULL to keep the name)
 * @param[IN] datatype: Data size of the field (or 0 to keep the datatype)
 * @param[IN] input_mask: The input mask of the field (NULL to keep the input mask). If the field is not a
 *            a string, it is ignored.
 * @param[IN] field_num: The index of the field to change.
 * 
 * @returns: ERROR_CODE_SUCCESS on succes, else an indicative error code
 */
error_code_t switch_field(IN char * file_name, IN char * field_name, IN int data_type, IN char * input_mask, IN int field_num){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int fd = -1;
    int new_fd = -1;
    int num_of_fields = 0;
    int num_of_records = 0;
    int original_num_of_fields = 0;
    int error_check = 0;
    int i = 0;
    int j = 0;
    int data_offset = 0;
    int temp_data = 0;
    bool valid = false;
    bool new_file = true;
    bool new_field = false;
    bool can_free = false;
    bool can_free_input_mask = false;
    char * del_name = "del_file";
    field * fields = NULL;
    field * old_fields = NULL;
    record_field old_record_field = {0};
    record_field new_record_field = {0};

    fd = open(file_name, O_RDWR);
    if(-1 == fd){
        perror("SWITCH_FIELD: Open error");
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }

    valid = check_magic(fd, false);
    if(!valid){
        print_color("~~INVALID FILE~\n", RED, BOLD, RESET);
        return_value = ERROR_CODE_INVALID_FILE;
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &old_fields, false);
    if(-1 == num_of_fields){
        return_value = ERROR_CODE_COULDNT_GET_FIELDS;
        goto cleanup;
    }

    original_num_of_fields = num_of_fields;

    num_of_records = get_num_of_records(fd, num_of_fields, false);
    if(-1 == num_of_records){
        return_value = ERROR_CODE_COULDNT_GET_NUM_OF_RECORDS;
        goto cleanup;
    }

    if(-1 == field_num || field_num >= num_of_fields){
        field_num = num_of_fields;
        num_of_fields++;
        new_field = true;

        if(0 == data_type || NULL == field_name){
            print_color("~~MUST SPECIFY FIELD METADATA FOR AN APPENDING FIELD~\n", B_RED, BOLD, RESET);
            return_value = ERROR_CODE_INVALID_INPUT;
            goto cleanup;
        }
    }
    if(0 == data_type){
        data_type = old_fields[field_num].data_len;
        new_file = false;
    }
    if(NULL == field_name){
        field_name = old_fields[field_num].name;
    }
    if(NULL == input_mask && STRING == data_type && !new_field){
        input_mask = old_fields[field_num].input_mask;
    }
    else{
        input_mask = malloc(NAME_LEN);
        if(NULL == input_mask){
            perror("SWITCH_FIELD: Malloc error");
            return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
            goto cleanup;
        }
        can_free_input_mask = true;
        memset(input_mask, 0, NAME_LEN);
        }

    fields = malloc(num_of_fields * sizeof(field));
    if(NULL == fields){
        perror("SWITCH_FIELD: Malloc error");
        return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
        goto cleanup;
    }

    memcpy(fields, old_fields, original_num_of_fields*sizeof(field));

    fields[field_num].data_len = data_type;
    memcpy(fields[field_num].name, field_name, NAME_LEN);
    memcpy(fields[field_num].input_mask, input_mask, NAME_LEN);



    if(0 == num_of_records){
        new_file = false;
    }

    if(!new_file){
        error_check = lseek(fd, magic_len, SEEK_SET);
        if(-1 == error_check){
            perror("SWITCH_FIELD: Lseek error");
            return_value = ERROR_CODE_COULDNT_LSEEK;
            goto cleanup;
        }

        error_check = write(fd, &num_of_fields, sizeof(num_of_fields));
        if(-1 == error_check){
            perror("SWITCH_FIELD: Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = write(fd, fields, num_of_fields * sizeof(field));
        if(-1 == error_check){
            perror("SWITCH_FIELD: Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        if(new_field){       //Last field
            error_check = write(fd, &num_of_records, sizeof(num_of_records));
            if(-1 == error_check){
                perror("Write error");
                return_value = ERROR_CODE_COULDNT_WRITE;
                goto cleanup;
            }
        }
    }
    else{
        error_check = rename(file_name, del_name);
        if(-1 == error_check){
            perror("SWITCH_FIELD: Rename error");
            return_value = ERROR_CODE_COULDNT_RENAME;
            goto cleanup;
        }

        new_fd = open(file_name, O_RDWR | O_CREAT, 0666);
        if(-1 == new_fd){
            perror("SWITCH FIELD: Open error");
            return_value = ERROR_CODE_COULDNT_OPEN;
            goto cleanup;
        }

        error_check = truncate(file_name, 0);
        if(-1 == error_check){
            perror("SWITCH_FIELD: Truncate error");
            return_value = ERROR_CODE_COULDNT_TRUNCATE;
            goto cleanup;
        }

        error_check = write(new_fd, magic, magic_len);
        if(-1 == error_check){
            perror("SWITCH_FIELD: Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = write(new_fd, &num_of_fields, sizeof(num_of_fields));
        if(-1 == error_check){
            perror("SWITCH_FIELD: Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = write(new_fd, fields, num_of_fields * sizeof(field));
        if(-1 == error_check){
            perror("SWITCH_FIELD: Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = write(new_fd, &num_of_records, sizeof(num_of_records));
        if(-1 == error_check){
            perror("SWITCH_FIELD: Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        for(i=0; i<num_of_records; i++){
            data_offset = 0;
            for(j=0; j<original_num_of_fields; j++){
                if(NULL != old_record_field.data){
                    free(old_record_field.data);
                    old_record_field.data = NULL;
                }
                error_check = get_record_field(fd, &old_record_field, i, j, false);
                if(ERROR_CODE_SUCCESS != error_check){
                    goto cleanup;
                }
                new_record_field.data_len = fields[j].data_len;
                new_record_field.field_index = j;
                new_record_field.record_num = i;
                if(can_free){
                    free(new_record_field.data);
                }
                new_record_field.data = NULL;
                can_free = false;

                if(j == field_num){
                    switch(fields[field_num].data_len){
                        case STRING:
                            if(INT == old_fields[field_num].data_len || CHAR == old_fields[field_num].data_len){
                                ntos(old_record_field.data, &(new_record_field.data), 1, old_fields[field_num].data_len);
                                can_free = true;
                            }
                            else{
                                new_record_field.data = old_record_field.data;
                            }
                            break;

                        case INT:
                            switch(old_fields[field_num].data_len){
                                case STRING:
                                    temp_data = (int)strtol(old_record_field.data, NULL, 10);
                                    new_record_field.data = (char *)&temp_data;
                                    break;
                                case INT:
                                    new_record_field.data = old_record_field.data;
                                    break;
                                case CHAR:
                                    new_record_field.data = (char *)old_record_field.data;
                                    break;
                            }
                            break;
                        
                        case CHAR:
                            switch(old_fields[field_num].data_len){
                                case STRING:
                                    temp_data = strtol(old_record_field.data, NULL, 10);
                                    new_record_field.data = (char *)&temp_data;
                                    break;
                                case INT:
                                    new_record_field.data = (char *)old_record_field.data;
                                    break;
                                case CHAR:
                                    new_record_field.data = old_record_field.data;
                                    break;
                            }
                            break;
                    }
                    
                }
                else{
                    new_record_field = old_record_field;
                }

                new_record_field.record_field_offset = data_offset;
                data_offset += new_record_field.data_len;
                
                write_record_fields(new_fd, 1, &new_record_field, false);
                //getchar();
            }

            if(new_field){
                new_record_field.record_field_offset = data_offset;
                new_record_field.data_len = fields[j].data_len;
                new_record_field.field_index = field_num;
                new_record_field.record_num = i;

                new_record_field.data = malloc(new_record_field.data_len);
                if(NULL == new_record_field.data){
                    perror("SWITCH_FIELD: Malloc error");
                    return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
                    goto cleanup;
                }
                memset(new_record_field.data, 0, new_record_field.data_len);
                can_free = true;

                write_record_fields(new_fd, 1, &new_record_field, false);
            }
        }

        remove(del_name);
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    if(NULL != fields){
        free(fields);
    }
    if(NULL != old_fields){
        free(old_fields);
    }
    if(NULL != new_record_field.data){
        free(new_record_field.data);
    }
    if(NULL != old_record_field.data){
        free(old_record_field.data);
    }
    if(can_free_input_mask){
        free(input_mask);
    }
    return return_value;
}

/** 
 * @brief: Replaces or appends a new record to a table file
 * @param[IN] fd: The file descriptor of the table file
 * @param[IN] record_num: The number of the record to append or replace
 * @param[IN] input_lens: The length of each element in record_input (so you don't need to pad input so it reaches STRING_LEN if 
 *                        its datatype is a string)
 * @param[IN] record_input: An array of array of bytes (so any datatype) that correspond to the fields in the record.
 * 
 * @return: The number of records in the table file on success, else -1
 * @notes: An input of record_num = -1 appends the record to the end of the record array. The same outcome will occur if
 *         record_num is greater than the number of records in the table file. Record_input have num_of_fields elements, or there 
 *         will be a large chance of a SEGFAULT. Every element in input_lens must be equal to or less than the the datatype of the
 *         field it corresponds to. This is to allow the user to not have to pad input strings to the required 256 bytes.
 *         This function also overwrites the number of records in the sort file to be 0, so the queryer knows to rewrite it.
 */
int switch_record(IN int fd, IN int record_num, IN int * input_lens, IN char ** record_input, int sort_file_fd){
    bool is_valid = false;
    int num_of_fields = 0;
    int num_of_records = -1;
    int error_check = 0;
    int record_len = 0;
    int i = 0;
    int record_offset = 0;
    off_t offset = 0;
    void * data = NULL;
    field * fields = NULL;

    is_valid = check_magic(fd, false);
    if(!is_valid){
        print_color("~~INVALID FILE TYPE~\n", RED, BOLD, RESET);
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &fields, false);
    if(-1 == num_of_fields){
        num_of_records = -1;
        goto cleanup;
    }

    record_len = get_len_of_record(fd, false);
    if(-1 == record_len){
        num_of_records = -1;
        goto cleanup;
    }

    num_of_records = get_num_of_records(fd, num_of_fields, false);
    if(-1 == num_of_records){
        print_color("~~COULDNT GET NUMBER OF RECORDS~\n", RED, BOLD, RESET);
        goto cleanup;
    }

    if(record_num > num_of_records - 1){
        record_num = num_of_records;
    }
    if(record_num <= -1){
        record_num = num_of_records;
    }

    offset = lseek(fd, record_num * record_len, SEEK_CUR);
    if(-1 == offset){
        perror("Lseek error");
        num_of_records = -1;
        goto cleanup;
    }
    for(i=0; i<num_of_fields; i++){
        if(NULL != data){
            free(data);
            data = NULL;
        }

        data = malloc(fields[i].data_len);
        if(NULL == data){
            perror("SWITCH_RECORD: Malloc error");
            num_of_records = -1;
            goto cleanup;
        }
        memset(data, 0, fields[i].data_len);
        
        memcpy(data, record_input[i], input_lens[i]);

        offset = lseek(fd, magic_len + sizeof(num_of_fields) + num_of_fields*sizeof(field) + sizeof(num_of_records) + record_num*record_len+record_offset, SEEK_SET);
        if(-1 == offset){
            perror("Lseek error");
            goto cleanup;
        }

        error_check = write(fd, (char *)data, fields[i].data_len);
        if(-1 == error_check){
            perror("Write error");
            goto cleanup;
        }

        record_offset += fields[i].data_len;
    }
    
    num_of_records = get_num_of_records(fd, num_of_fields, false);
    if(-1 == num_of_records){
        goto cleanup;
    }

    if(record_num == num_of_records){
        offset = lseek(fd, -1 * sizeof(num_of_records), SEEK_CUR);
        if(-1 == offset){
            perror("Lseek error");
            goto cleanup;
        }

        num_of_records++;
        error_check = write(fd, &num_of_records, sizeof(num_of_records));
        if(-1 == error_check){
            perror("Write error");
            num_of_records = -1;
            goto cleanup;
        }
    }

    error_check = lseek(sort_file_fd, 0, SEEK_SET);
    if(-1 == error_check){
        perror("SWITCH_RECORD: Lseek error");
        num_of_records = -1;
        goto cleanup;
    }

    error_check = 0;        /* I'm sorry! I'm sorry! I just need a variable to equal 0 so I can rewrite the number of records in
                             * the sort file to be 0, so the queryer will understand to rewrite it.
                             */

    error_check = write(sort_file_fd, &error_check, sizeof(error_check));
    if(-1 == error_check){
        perror("SWITCH_RECORD: Write error");
        num_of_records = -1;
        goto cleanup;
    }

cleanup:
    if(-1 != fd)
        close(fd);
    if(NULL != fields)
        free(fields);
    if(NULL != data)
        free(data);

    return num_of_records;
}
