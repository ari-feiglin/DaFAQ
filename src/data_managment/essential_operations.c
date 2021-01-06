#include "dafaq.h"

/**
 * @brief: Checks if a file has the valid magic sequence to denote it as a valid DaFAQ table file
 * @param[IN] fd: The file descriptor of the table file.
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return: If valid true, else false (no error returning)
 */
bool check_magic(IN int fd, IN bool preserve_offset){
    bool is_valid = false;
    int error_check = 0;
    int difference = 0;
    off_t offset = 0;
    off_t old_offset = 0;
    char * magic_check = NULL;

    magic_check = malloc(magic_len);
    if(NULL == magic_check){
        perror("Malloc error");
        goto cleanup;
    }

    old_offset = lseek(fd, 0, SEEK_CUR);
    if(-1 == old_offset){
        perror("Lseek error");
        goto cleanup;
    }

    offset = lseek(fd, 0, SEEK_SET);
    if(-1 == old_offset){
        perror("Lseek error");
        goto cleanup;
    }

    error_check = read(fd, magic_check, magic_len);
    if(-1 == error_check){
        perror("Read error");
        goto cleanup;
    }

    difference = strncmp(magic, magic_check, magic_len);
    if(0 == difference){
        is_valid = true;
    }
    else{
        print_color("~~INVALID FILE FORMAT~\n", B_RED, BOLD, RESET);
    }

    if(preserve_offset){
        offset = lseek(fd, old_offset, SEEK_SET);
        if(-1 == offset){
            perror("Lseek error");
            is_valid = false;
            goto cleanup;
        }
    }

cleanup:
    if(NULL != magic_check)
        free(magic_check);
    return is_valid;
}

/**
 * @brief: Returns the number of fields in a table file
 * @param[IN] fd: The file descriptor of the table file.
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return: The number of fields in a table file
 */
int get_num_of_fields(IN int fd, IN bool preserve_offset){
    bool is_valid = false;
    int num_of_fields = -1;
    int error_check = 0;
    off_t old_offset = 0;

    old_offset = lseek(fd, 0, SEEK_CUR);
    if(-1 == old_offset){
        perror("GET_NUM_OF_FIELDS: Lseek error");
        goto cleanup;
    }

    is_valid = check_magic(fd, false);
    if(!is_valid){
        goto cleanup;
    }

    error_check = read(fd, &num_of_fields, sizeof(num_of_fields));
    if(-1 == error_check){
        perror("GET_NUM_OF_FIELDS: Read error");
        num_of_fields = -1;
        goto cleanup;
    }

    if(preserve_offset){
        old_offset = lseek(fd, old_offset, SEEK_SET);
        if(-1 == old_offset){
            perror("GET_NUM_OF_FIELDS: Lseek error");
            num_of_fields = -1;
            goto cleanup;
        }
    }

cleanup:
    return num_of_fields;
}

/**
 * @brief: Gets a field from a table
 * @param[IN] fd: The file descriptor of the table file
 * @param[IN] target_field: The field to fill out
 * @param[IN] field_index: The index of the field to get
 * 
 * @returns: SUCCESS on success, else an indicative error code
 */
error_code_t get_field(IN int fd, field_t * target_field, int field_index){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int error_check = 0;

    error_check = lseek(fd, magic_len + sizeof(int) + field_index * sizeof(field_t), SEEK_SET);
    if(-1 == error_check){
        perror("GET_FIELD: Lseek error");
        return_value = ERROR_CODE_COULDNT_LSEEK;
        goto cleanup;
    }

    error_check = read(fd, target_field, sizeof(field_t));
    if(-1 == error_check){
        perror("GET_FIELD: Read error");
        return_value = ERROR_CODE_COULDNT_READ;
        goto cleanup;
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    return return_value;
}

/**
 * @brief: Fills out an array with the fields in a table file
 * @param[IN] fd: The file descriptor of the table file.
 * @param[OUT] fields: A NULL pointer to an array of field structures.
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return: On success the number of fields in a file, else -1
 */
int get_fields(IN int fd, OUT field_t ** fields, IN bool preserve_offset){
    int num_of_fields = -1;
    int error_check = 0;
    off_t old_offset = 0;

    old_offset = lseek(fd, 0, SEEK_CUR);
    if(-1 == old_offset){
        perror("Lseek error");
        goto cleanup;
    }

    num_of_fields = get_num_of_fields(fd, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    *fields = calloc(num_of_fields, sizeof(field_t));
    if(NULL == *fields){
        perror("Calloc error");
        num_of_fields = -1;
        goto cleanup;
    }

    error_check = read(fd, *fields, num_of_fields*sizeof(field_t));
    if(-1 == error_check){
        perror("Read error");
        num_of_fields = -1;
        goto cleanup;
    }

    if(preserve_offset){
        old_offset = lseek(fd, old_offset, SEEK_SET);
        if(-1 == old_offset){
            perror("Lseek error");
            num_of_fields = -1;
            goto cleanup;
        }
    }

cleanup:
    return num_of_fields;
}

/**
 * @brief: Returns the number of records in a table file
 * @param[IN] fd: The file descriptor of the table file.
 * @param[IN] num_of_fields: The number of fields in the table file
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return:On success the number of records in the table file, else -1
 * @notes: It is assumed that the user already called get_fields, so get_num_of_records doesn't get the
 *         number of fields itself.
 */
int get_num_of_records(IN int fd, IN int num_of_fields, IN bool preserve_offset){
    int num_of_records = -1;
    int error_check = 0;
    bool is_valid = 0;
    off_t old_offset = 0;
    off_t offset = 0;

    old_offset = lseek(fd, 0, SEEK_CUR);
    if(-1 == old_offset){
        perror("Lseek error");
        goto cleanup;
    }

    is_valid = check_magic(fd, false);
    if(!is_valid){
        print_color("~~INVALID FILE TYPE~\n", RED, BOLD, RESET);
        goto cleanup;
    }

    offset = lseek(fd, magic_len+ sizeof(num_of_fields)+ num_of_fields*sizeof(field_t), SEEK_SET);     //Seeks to num_of_records
    if(-1 == offset){
        perror("Lseek error");
        goto cleanup;
    }

    error_check = read(fd, &num_of_records, sizeof(num_of_records));
    if(-1 == error_check){
        perror("Read error");
        num_of_records = -1;
        goto cleanup;
    }

    if(preserve_offset){
        offset = lseek(fd, old_offset, SEEK_SET);
        if(-1 == offset){
            perror("GET_NUM_OF_RECORDS: Lseek error");
            num_of_records = -1;
            goto cleanup;
        }
    }


cleanup:
    return num_of_records;
}

/**
 * @brief: Returns the length of a record in a table file
 * @param[IN] fd: The file descriptor of the table file.
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return: On success the length of a record, else -1
 * @notes: For some reason, get_len_of_records utilizes get_fields instead of having an array of fields
 *         as input. I can't remember why. I'll look into this later maybe.
 */
int get_len_of_record(IN int fd, IN bool preserve_offset){
    int len_of_records = 0;
    int num_of_fields = 0;
    int i = 0;
    off_t offset = 0;
    field_t * fields = NULL;

    offset = lseek(fd, 0, SEEK_CUR);
    if(-1 == offset){
        perror("GET_LEN_OF_RECORD: Lseek error");
        len_of_records = -1;
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &fields, false);
    if(-1 == num_of_fields){
        len_of_records = -1;
        goto cleanup;
    }

    for(i=0; i<num_of_fields; i++){
        len_of_records += fields[i].data_len;
    }

    if(preserve_offset){
        offset = lseek(SEEK_SET, offset, SEEK_SET);
        if(-1 == offset){
            perror("GET_LEN_OF_RECORD: Lseek error");
            len_of_records = -1;
            goto cleanup;
        }
    }

cleanup:
    if(NULL != fields)
        free(fields);
    return len_of_records;
}

/**
 * @brief: Checks if an input file has a valid .dfq extension
 * @param[IN] table_name: The name of the table file
 * 
 * @return: If valid true, else false (it is not expected that this will fail, unless you really try)
 */
bool check_extension(IN char * table_name){
    bool is_valid = false;
    int i = 0;
    int name_len = 0;
    int difference = 0;
    char * extension_check = NULL;

    name_len = strnlen(table_name, STRING_LEN);
    if(-1 == name_len){
        perror("CHECK EXTENSION: strnlen error");
        goto cleanup;
    }

    for(i=0; i<name_len && table_name[i] != '.'; i++);  //Get to . in table_name
    if(i == name_len){
        is_valid = false;
        goto cleanup;
    }

    extension_check = malloc(name_len - i);     //Allocate a buffer whose size is the length of the extension of the table file

    memcpy(extension_check, table_name+i, name_len-i);      //Copy extension to extension_check

    difference = strncmp(extension, extension_check, name_len-i);       //Check if extension is valid
    if(0 == difference){
        is_valid = true;
    }
    else{
        goto cleanup;
    }

cleanup:
    if(NULL != extension_check){
        free(extension_check);
    }

    return is_valid;
}

/**
 * @brief: Returns if an if an input input mask is a valid one or not
 * @param[IN] input_mask: The input mask to validate
 * 
 * @return: If valid true, else false (This function can only fail with a SEGFAULT)
 */
bool check_input_mask(IN char * input_mask){
    bool is_valid = true;
    int i = 0;

    for(i=0; input_mask[i] != 0; i++){
        if( ('C' != input_mask[i]) &&
        ('c' != input_mask[i]) &&
        ('N' != input_mask[i]) &&
        ('n' != input_mask[i]) &&
        ('L' != input_mask[i]) &&
        ('l' != input_mask[i]) &&
        ('@' != input_mask[i]) &&
        ('*' != input_mask[i]) &&
        ('S' != input_mask[i]) &&
        (',' != input_mask[i]) &&
        ('.' != input_mask[i]) &&
        ('/' != input_mask[i]) &&
        ('e' != input_mask[i])){
            is_valid = false;
            goto cleanup;
        }
    }

cleanup:
    return is_valid;
}

/**
 * @brief: Returns if input is valid according to an input mask
 * @param[IN] input: The input to validate against:
 * @param[IN] input_mask: The input mask
 * 
 * @return: If valid 1, if invalid 0, on error -1
 */
int valid_input(IN char * input, IN char * input_mask){
    int is_valid = 1;
    int input_len = 0;
    int input_mask_len = 0;
    int i = 0;
    bool size_valid = true;

    input_len = strnlen(input, STRING_LEN);
    if(-1 == input_len){
        perror("VALID_INPUT: Strnlen error");
        is_valid = -1;
        goto cleanup;
    }

    input_mask_len = strnlen(input_mask, STRING_LEN);
    if(-1 == input_len){
        perror("VALID_INPUT: Strnlen error");
        is_valid = -1;
        goto cleanup;
    }

    if(input_len != input_mask_len){
        size_valid = false;
    }

    for(i=0; i<input_len; i++){
        if('C' == input_mask[i]){
            if(' ' == input[i]){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('n' == input_mask[i]){
            if( (input[i] < '0' || input[i] > '9') && ' ' != input[i]){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('N' == input_mask[i]){
            if(input[i] < '0' || input[i] > '9'){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('l' == input_mask[i]){
            if( (input[i] < 'A' || ( ('Z' < input[i]) && (input[i] < 'a') ) || input[i] > 'z') && ' ' != input[i]){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('L' == input_mask[i]){
            if(input[i] < 'A' || ( ('Z' < input[i]) && (input[i] < 'a') ) || input[i] > 'z'){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('@' == input_mask[i]){
            if( (('0' <= input[i] && input[i] <= '9') ||
            ('A' <= input[i] && input[i] <= 'Z') ||
            ('a' <= input[i] && input[i] <= 'z')) &&
            ' ' != input[i] ){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('*' == input_mask[i]){
            if( ('0' <= input[i] && input[i] <= '9') ||
            ('A' <= input[i] && input[i] <= 'Z') ||
            ('a' <= input[i] && input[i] <= 'z') ){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('S' == input_mask[i]){
            if(' ' != input[i]){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if(',' == input_mask[i] || '.' == input_mask[i] || '/' == input_mask[i]){
            if(input_mask[i] != input[i]){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('e' == input_mask[i]){
            is_valid = 1;
            size_valid = true;
            goto cleanup;
        }
        else if(0 == input_mask[i]){
            is_valid = 0;
            goto cleanup;
        }
        else{
            is_valid = -1;
            print_color("~~INVALID INPUT MASK~\n", RED, BOLD, RESET);
            goto cleanup;
        }
    }

    if(!size_valid){
        is_valid = 0;
    }

cleanup:
    return is_valid;
}

int write_record_fields(int fd, int num_of_record_fields, record_field_t * record_fields, bool preserver_offset){
    int num_of_fields = 0;
    int record_len = 0;
    int bytes_written = 0;
    int total_bytes_written = -1;
    int i = 0;
    int error_check = 0;
    off_t record_start = 0;

    num_of_fields = get_num_of_fields(fd, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    record_len = get_len_of_record(fd, false);
    if(-1 == record_len){
        goto cleanup;
    }

    record_start = lseek(fd, sizeof(int), SEEK_CUR);
    if(-1 == record_start){
        goto cleanup;
    }

    for(i=0; i<num_of_record_fields; i++){
        error_check = lseek(fd, record_start + record_fields[i].record_num*record_len + record_fields[i].record_field_offset, SEEK_SET);
        if(-1 == error_check){
            total_bytes_written = -1;
            goto cleanup;
        }

        bytes_written = write(fd, record_fields[i].data, record_fields[i].data_len);
        if(-1 == bytes_written){
            total_bytes_written = -1;
            goto cleanup;
        }

        total_bytes_written += bytes_written;
    }

cleanup:
    return total_bytes_written;
}
