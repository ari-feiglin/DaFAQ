#include "dafaq.h"

/** 
 * @brief: Replaces or appends a new field to the table file
 * @param[IN] file_name: The name of the table file
 * @param[IN] field_name: The name of the field to add or append
 * @param[IN] data_type: The data type of the (new) field
 * @param[IN] input_mask: A string that denotes the input mask of the field (see details/Usage.md for more information)
 * @param[IN] field_num: The index of the field
 * 
 * @return: ERROR_CODE_SUCCESS on succes, else an indicative error code of type error_code_t
 * @notes: An input of field_num = -1 will append the field instead of switching an existing one. An input of 
 *         field_name = NULL will retain the field's old name, an input of data_type = 0 will retain the field's
 *         old data type, and an input of input_mask = NULL will retain the field's old input_mask. These cannot be
 *         input as such if field_num = -1.
 */
error_code_t switch_field(IN char * file_name, IN char * field_name, IN int data_type, IN char * input_mask, IN int field_num){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int error_check = -1;
    int i = 0;
    int record = 0;     //For iterating over records
    int current_field = 0;      //For iterating over fields in a record
    int name_len = 0;
    int fd = -1;
    int new_fd = -1;
    int num_of_fields = 0;
    int num_of_records = 0;
    int record_data_length = 0;
    int original_num_of_fields = 0;
    int record_offset = 0;
    bool copy = true;
    bool is_new_field = false;
    off_t offset = 0;
    field new_field = {0};
    char * del_name = "del_file.dfq";
    char * record_data = NULL;
    char temp_data[STRING_LEN] = {0};    //For record data where the field has changed
    field * fields = NULL;
    field * old_fields = NULL;

    fd = open(file_name, O_RDWR);
    if(-1 == fd){
        perror("Open error");
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }
    num_of_fields = get_fields(fd, &old_fields, false);
    if(-1 == num_of_fields){
        return_value = ERROR_CODE_COULDNT_GET_FIELDS;
        goto cleanup;
    }

    fields = malloc(num_of_fields * sizeof(field));
    if(NULL == fields){
        perror("Malloc error");
        return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
        goto cleanup;
    }
    memcpy(fields, old_fields, num_of_fields*sizeof(field));
    original_num_of_fields = num_of_fields;
    
    num_of_records = get_num_of_records(fd, num_of_fields, false);
    if(-1 == num_of_records){
        print_color("~~COULDNT GET NUMBER OF RECRODS~\n", RED, BOLD, RESET);
        return_value = ERROR_CODE_COULDNT_NUM_OF_RECORDS;
        goto cleanup;
    }

    if(-1 == field_num || field_num >= num_of_fields){        //An input of field_num = -1 means the user wants to append a new field
        field_num = num_of_fields;
        num_of_fields++;
        is_new_field = true;
        if(0 == data_type || NULL == file_name){
            print_color("~~MUST SPECIFY FIELD METADATA FOR AN APPENDING FIELD~\n", B_RED, BOLD, RESET);
            return_value = ERROR_CODE_INVALID_INPUT;
            goto cleanup;
        }
    }
    if(0 == data_type){     //An input of data_type = 0 means the user would like to retain the data size
        data_type = fields[field_num].data_len;
        copy = false;
    }
    if(NULL == field_name){    //An input of field_name = NULL means the user would like to retain the field name
        field_name = fields[field_num].name;
    }
    if(NULL == input_mask){
        if(-1 != field_num){
            input_mask = fields[field_num].input_mask;
        }
    }

    fields = realloc(fields, num_of_fields*sizeof(field));
    memset(fields[field_num].name, 0, NAME_LEN);
    memcpy(fields[field_num].name, field_name, strnlen(field_name, NAME_LEN));
    if(NULL != input_mask){
        memcpy(fields[field_num].input_mask, input_mask, strnlen(input_mask, NAME_LEN));
    }
    else{
        memset(fields[field_num].input_mask, 0, NAME_LEN);
    }
    fields[field_num].data_len = data_type;


    //This is where the magic happens

    if(0 == num_of_records || !copy){        //If there are no records present or no data_types are to be changed, the file does not need to be rerwitten.
        offset = lseek(fd, magic_len, SEEK_SET);      //Seek to the integer that corresponds to the number of fields
        if(-1 == offset){
            perror("Lseek error");
            return_value = ERROR_CODE_COULDNT_LSEEK;
            goto cleanup;
        }

        error_check = write(fd, &num_of_fields, sizeof(num_of_fields));     //Rewrite the number of fields
        if(-1 == error_check){
            perror("Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = write(fd, fields, num_of_fields*sizeof(field));   /*  Possibly not the best method for making a change to a large 
                                                                         *  numbers of fields,but the simplest when dealing with the 
                                                                         *  possibility of a rewrite.   
                                                                         */
        if(-1 == error_check){
            perror("Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        if(field_num == num_of_fields-1){     //If field is at end
            error_check = write(fd, &num_of_records, sizeof(num_of_records));
            if(-1 == error_check){
                perror("Write error");
                return_value = ERROR_CODE_COULDNT_WRITE;
                goto cleanup;
            }
        }
    }
    else{       //Must rewrite file
        for(i=0; i<num_of_fields; i++){     //Get the length each record by finding the sum of the data_len fields in the field structures
            record_data_length += fields[i].data_len;
        }
        record_data = malloc(record_data_length);
        
        error_check = rename(file_name, del_name);      /*  Rename current file in order to create new one of the original name.
                                                         *  This file (the original) will be deleted in the end. */
        if(-1 == error_check){
            perror("Rename error");
            return_value = ERROR_CODE_COULDNT_RENAME;
            goto cleanup;
        }

        new_fd = open(file_name, O_WRONLY | O_CREAT, 0666);     //Open new file
        if(-1 == new_fd){
            perror("Open error");
            return_value = ERROR_CODE_COULDNT_OPEN;
            goto cleanup;
        }

        error_check = write(new_fd, magic, magic_len);      //Write the magic sequence to the new file
        if(-1 == error_check){
            perror("Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = write(new_fd, &num_of_fields, sizeof(num_of_fields));     //Write the number of fields to the new file
        if(-1 == error_check){
            perror("Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = write(new_fd, fields, num_of_fields * sizeof(field));     //Write the fields to the new file
        if(-1 == error_check){
            perror("Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = write(new_fd, &num_of_records, sizeof(num_of_records));     //Write the number of records to the file ( * )
        if(-1 == error_check){
            perror("Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }

        error_check = lseek(fd, magic_len+sizeof(num_of_fields)+original_num_of_fields*sizeof(field)+sizeof(num_of_records), SEEK_SET);    /*
                                                                        * Seek to records in original file */
        if(-1 == error_check){
            perror("Lseek error");
            return_value = ERROR_CODE_COULDNT_LSEEK;
            goto cleanup;
        }

        for(record=0; record<num_of_records; record++){
            memset(record_data, 0, record_data_length);
            for(current_field=0; current_field<num_of_fields; current_field++){
                memset(temp_data, 0, STRING_LEN);

                if(current_field != field_num){
                    error_check = read(fd, record_data+record_offset, fields[current_field].data_len);
                    if(-1 == error_check){
                        perror("Read error");
                        return_value = ERROR_CODE_COULDNT_READ;
                        goto cleanup;
                    }
                }
                else{
                    error_check = read(fd, temp_data, old_fields[current_field].data_len);
                    if(-1 == error_check){
                        perror("Read error");
                        return_value = ERROR_CODE_COULDNT_READ;
                        goto cleanup;
                    }

                    if(!is_new_field){
                        switch(fields[current_field].data_len){
                            case STRING:
                                switch(old_fields[current_field].data_len){     
                                    case STRING:
                                        memcpy(record_data+record_offset, temp_data, STRING_LEN);
                                        break;
                                    case INT:
                                        error_check = ntos((char *)temp_data, record_data+record_offset, 1, INT);
                                        if(-1 == error_check){
                                            goto cleanup;
                                        }
                                        memset(record_data+record_offset+INT, 0, STRING-INT);
                                        break;
                                    case CHAR:
                                        error_check = ntos((char *)temp_data, record_data+record_offset, 1, CHAR);
                                        if(-1 == error_check){
                                            goto cleanup;
                                        }
                                        memset(record_data+record_offset+CHAR, 0, STRING-CHAR);
                                        break;
                                    default:
                                        print_color("~~INVALID DATA TYPE~\n", B_RED, BOLD, RESET);
                                        return_value = ERROR_CODE_INVALID_DATATYPE;
                                        goto cleanup;
                                }

                                break;
                            case INT:
                                i = (int)strtol(temp_data, NULL, 10);
                                memcpy(record_data+record_offset, &i, sizeof(i));
                                break;
                            case 1:
                                i = temp_data[0];
                                memcpy(record_data+record_offset, &i, 1);   //This will not work for BIG-ENDIAN systems
                                break;
                            default:
                                print_color("~~ERROR: INVALID DATA TYPE~\n", BOLD, B_RED, RESET);
                                return_value = ERROR_CODE_INVALID_DATATYPE;
                                goto cleanup;
                        }
                    }
                    else{
                        memset(record_data+record_offset, 0, data_type);
                    }
                }
                record_offset += fields[current_field].data_len;
            }
            error_check = write(new_fd, record_data, record_data_length);
            if(-1 == error_check){
                perror("Write error");
                return_value = ERROR_CODE_COULDNT_WRITE;
                goto cleanup;
            }
        }

        remove(del_name);
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    close(fd);
    close(new_fd);
    if(record_data)
        free(record_data);
    if(fields)
        free(fields);
    if(old_fields)
        free(old_fields);
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
 */
int switch_record(IN int fd, IN int record_num, IN int * input_lens, IN char ** record_input){
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
    if(-1 == record_num){
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

cleanup:
    close(fd);
    if(NULL != fields)
        free(fields);
    if(NULL != data)
        free(data);

    return num_of_records;
}
