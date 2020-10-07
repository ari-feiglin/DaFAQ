#include "dafaq.h"

int switch_field(char * file_name, char * field_name, int data_size, char * input_mask, int field_num){
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
    int int_data = 0;
    char byte_data = 0;
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
        goto cleanup;
    }
    num_of_fields = get_fields(fd, &old_fields, false);
    if(-1 == num_of_fields){
        error_check = -1;
        goto cleanup;
    }

    fields = malloc(num_of_fields * sizeof(field));
    if(NULL == fields){
        perror("Malloc error");
        error_check = -1;
        goto cleanup;
    }
    memcpy(fields, old_fields, num_of_fields*sizeof(field));
    original_num_of_fields = num_of_fields;
    

    num_of_records = get_num_of_records(fd, num_of_fields, false);
    if(-1 == num_of_records){
        perror("Read error");
        error_check = -1;
        goto cleanup;
    }

    if(0 == data_size){     //An input of data_size = 0 means the user would like to retain the data size
        data_size = fields[field_num].data_len;
        copy = false;
    }
    if(-1 == field_num){        //An input of field_num = -1 means the user wants to add a new field
        field_num = num_of_fields;
        num_of_fields++;
        is_new_field = true;
        if(!copy){
            print_color("~~CANNOT ADD NEW FIELD WITH DATA SIZE 0~\n", B_RED, BOLD, RESET);
            goto cleanup;
        }
    }
    else if(NULL == field_name){    //An input of field_name = NULL means the user would like to retain the field name
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
    fields[field_num].data_len = data_size;


    //This is where the magic happens

    if(0 == num_of_records || !copy){        //If there are no records present or no data_sizes are to be changed, the file does not need to be rerwitten.
        offset = lseek(fd, magic_len, SEEK_SET);      //Seek to the integer that corresponds to the number of fields
        if(-1 == offset){
            perror("Lseek error");
            error_check = -1;
            goto cleanup;
        }

        error_check = write(fd, &num_of_fields, sizeof(num_of_fields));     //Rewrite the number of fields
        if(-1 == error_check){
            perror("Write error");
            goto cleanup;
        }

        error_check = write(fd, fields, num_of_fields*sizeof(field));   /*  Possibly not the best method for making a change to a large 
                                                                         *  numbers of fields,but the simplest when dealing with the 
                                                                         *  possibility of a rewrite.   
                                                                         */
        if(-1 == error_check){
            perror("Write error");
            goto cleanup;
        }

        if(field_num == num_of_fields){     //If field is at end
            error_check = write(fd, &num_of_records, sizeof(num_of_records));
            if(-1 == error_check){
                perror("Write error");
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
            goto cleanup;
        }

        new_fd = open(file_name, O_WRONLY | O_CREAT, 0666);     //Open new file
        if(-1 == new_fd){
            perror("Open error");
            error_check = -1;
            goto cleanup;
        }

        error_check = write(new_fd, magic, magic_len);      //Write the magic sequence to the new file
        if(-1 == error_check){
            perror("Write error");
            goto cleanup;
        }

        error_check = write(new_fd, &num_of_fields, sizeof(num_of_fields));     //Write the number of fields to the new file
        if(-1 == error_check){
            perror("Write error");
            goto cleanup;
        }

        error_check = write(new_fd, fields, num_of_fields * sizeof(field));     //Write the fields to the new file
        if(-1 == error_check){
            perror("Write error");
            goto cleanup;
        }

        error_check = write(new_fd, &num_of_records, sizeof(num_of_records));     //Write the number of records to the file ( * )
        if(-1 == error_check){
            perror("Write error");
            goto cleanup;
        }

        error_check = lseek(fd, magic_len+sizeof(num_of_fields)+original_num_of_fields*sizeof(field)+sizeof(num_of_records), SEEK_SET);    /*
                                                                        * Seek to records in original file */
        if(-1 == error_check){
            perror("Lseek error");
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
                        goto cleanup;
                    }
                }
                else{
                    error_check = read(fd, temp_data, old_fields[current_field].data_len);
                    if(-1 == error_check){
                        perror("Read error");
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
                                        error_check = -1;
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
                                print_color("~~ERROR: INVALID DATA LENGTH~\n", BOLD, B_RED, RESET);
                                goto cleanup;
                        }
                    }
                    else{
                        memset(record_data+record_offset, 0, data_size);
                    }
                }
                record_offset += fields[current_field].data_len;
            }
            error_check = write(new_fd, record_data, record_data_length);
            if(-1 == error_check){
                perror("Write error");
                goto cleanup;
            }
        }

        remove(del_name);
    }


cleanup:
    close(fd);
    close(new_fd);
    if(record_data)
        free(record_data);
    if(fields)
        free(fields);
    if(old_fields)
        free(old_fields);
    return error_check;
}

int switch_record(char * name, int record_num){
    bool is_valid = false;
    int fd = -1;
    int num_of_fields = 0;
    int num_of_records = -1;
    int error_check = 0;
    int record_len = 0;
    int i = 0;
    int int_data = 0;
    int record_offset = 0;
    int difference = 0;
    int valid_input_mask = 0;
    bool input_mask = false;
    char byte_data = 0;
    off_t offset = 0;
    char prompt[STRING_LEN] = {0};
    char string_data[STRING_LEN] = {0};
    void * data = NULL;
    void * old_data = NULL;
    field * fields = NULL;

    fd = open(name, O_RDWR);
    if(-1 == fd){
        perror("Open error");
        goto cleanup;
    }

    is_valid = check_magic(fd, false);
    if(!is_valid){
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &fields, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    record_len = get_len_of_record(fd, false);
    if(-1 == record_len){
        num_of_records = -1;
        goto cleanup;
    }

    num_of_records = get_num_of_records(fd, num_of_fields, false);
    if(-1 == num_of_records){
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
        goto cleanup;
    }

    sprintf(prompt, "\n~~~RECORD NUM %i:~\n", record_num);
    print_color(prompt, BG_B_RED, B_WHITE, BOLD, RESET);
    for(i=0; i<num_of_fields; i++){
        input_mask = false;
        if(NULL != old_data){
            free(old_data);
            old_data = NULL;
        }
        old_data = malloc(fields[i].data_len+1);
        if(NULL == old_data){
            perror("SWITCH RECORD: Malloc error");
            num_of_records = -1;
            goto cleanup;
        }
        memset(old_data, 0, fields[i].data_len+1);

        if(record_num != num_of_records){
            error_check = read(fd, old_data, fields[i].data_len);
            if(-1 == error_check){
                perror("SWITCH RECORD: Read error");
                num_of_records = -1;
                goto cleanup;
            }

            switch(fields[i].data_len){
                case STRING:
                    sprintf(prompt, "~~%s (Old value: %s)", fields[i].name, (char *)old_data);
                    break;
                case INT:
                    memcpy(&int_data, old_data, sizeof(int_data));
                    sprintf(prompt, "~~%s (Old value: %i)", fields[i].name, int_data);
                    break;
                case CHAR:
                    memcpy(&byte_data, old_data, sizeof(byte_data));
                    sprintf(prompt, "~~%s (Old value: %i)", fields[i].name, byte_data);
                    break;
                default:
                    print_color("~~INVALID DATA TYPE~\n", B_RED, BOLD, RESET);
                    num_of_records = -1;
                    break;
            }
        }
        else{
            sprintf(prompt, "~~%s ", fields[i].name);
        }
        print_color(prompt, B_GREEN, BOLD);
        switch(fields[i].data_len){
            case STRING: 
                printf("(STRING)");
                if(fields[i].input_mask[0] != 0){
                    input_mask = true;
                    printf(" (INPUT MASK: %s)", fields[i].input_mask);
                }
                print_color(":~ ", RESET);
                break;
            case INT: print_color("(INT):~ ", RESET); break;
            case CHAR: print_color("(BYTE VALUE):~ ", RESET); break;
            default: print_color("\nINVALID DATATYPE~\n", RESET); num_of_records = -1; goto cleanup;
        }

        error_check = get_raw_input(NULL, (char **)&data);
        if(input_mask){
            valid_input_mask = valid_input(data, fields[i].input_mask);
            if(-1 == valid_input_mask){
                goto cleanup;
            }
            else if(0 == valid_input_mask){
                print_color("~~INVALID INPUT!~\n", B_RED, BOLD, RESET);
                i--;
                continue;
            }
        }
        memcpy(string_data, data, error_check);
        if(NULL != data){
            free(data);
            data = NULL;
        }

        switch(fields[i].data_len){
            case INT:
                int_data = (int)strtol(string_data, NULL, 10);
                data = (void *)&int_data;
                break;
            case STRING:
                data = (void *)string_data;
                break;
            case CHAR:
                difference = strncmp(string_data, "true", 4);
                if(0 == difference){
                    byte_data = 0;
                    data = (void *)&byte_data;
                    break;
                }
                difference = strncmp(string_data, "false", 5);
                if(0 == difference){
                    byte_data = 1;
                    data = (void *)&byte_data;
                    break;
                }
                byte_data = (char)strtol(string_data, NULL, 10);
                data = (void *)&byte_data;
                break;
            default:
                printf("SWITCH ERROR\n");
                num_of_records = -1;
                goto cleanup;
        }

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
        data = NULL;
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
    if(NULL != old_data)
        free(old_data);
    return num_of_records;
}
