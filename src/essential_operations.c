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
        perror("Lseek error");
        goto cleanup;
    }

    is_valid = check_magic(fd, false);
    if(!is_valid){
        goto cleanup;
    }

    error_check = read(fd, &num_of_fields, sizeof(num_of_fields));
    if(-1 == error_check){
        perror("read error");
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
 * @brief: Fills out an array with the fields in a table file
 * @param[IN] fd: The file descriptor of the table file.
 * @param[OUT] fields: A NULL pointer to an array of field structures.
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return: On success the number of fields in a file, else -1
 */
int get_fields(IN int fd, OUT field ** fields, IN bool preserve_offset){
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

    *fields = calloc(num_of_fields, sizeof(field));
    if(NULL == *fields){
        perror("Calloc error");
        num_of_fields = -1;
        goto cleanup;
    }

    error_check = read(fd, *fields, num_of_fields*sizeof(field));
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

    offset = lseek(fd, magic_len+ sizeof(num_of_fields)+ num_of_fields*sizeof(field), SEEK_SET);     //Seeks to num_of_records
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
    field * fields = NULL;

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
            goto cleanup;
        }
        else{
            is_valid = 0;
            goto cleanup;
        }
    }

cleanup:
    return is_valid;
}

/**
 * @brief: Gets a record_field
 * 
 * 
 * 
 * @notes: target_record_field must already be allocated
 */
error_code_t get_record_field(IN int fd, OUT record_field * target_record_field, IN int record_number, IN int field_num, IN bool preserve_offset){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int num_of_fields = 0;
    int num_of_records = 0;
    int record_len = 0;
    int error_check = 0;
    int i = 0;
    off_t offset = 0;
    off_t old_offset = 0;
    field * fields = NULL;

    if(NULL == target_record_field){
        print_color("~~Target record_field must already be allocated before being passed to get_record_field!~", RED, BOLD, RESET);
        return_value = ERROR_CODE_INVALID_INPUT;
        goto cleanup;
    }

    old_offset = lseek(fd, 0, SEEK_CUR);
    if(-1 == old_offset){
        perror("GET_RECORD: Lseek error");
        return_value = ERROR_CODE_COULDNT_LSEEK;
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &fields, false);
    if(-1 == num_of_fields){
        return_value = ERROR_CODE_COULDNT_GET_FIELDS;
        goto cleanup;
    }

    record_len = get_len_of_record(fd, false);
    if(-1 == record_len){
        return_value = ERROR_CODE_COULDNT_GET_LEN_OF_RECORD;
        goto cleanup;
    }

    target_record_field->record_num = record_number;
    target_record_field->record_field_offset = 0;
    target_record_field->field_index = field_num;
    target_record_field->data_len = fields[field_num].data_len;
    target_record_field->data = malloc(target_record_field->data_len);

    for(i=0; i<field_num; i++){
        target_record_field->record_field_offset += fields[i].data_len;
    }

    offset = magic_len + sizeof(num_of_fields) + num_of_fields*sizeof(field) + sizeof(num_of_records) + record_number*record_len + target_record_field->record_field_offset;
    offset = lseek(fd, offset, SEEK_SET);
    if(-1 == offset){
        perror("GET_RECORD_FIELD: Lseek error");
        return_value = ERROR_CODE_COULDNT_LSEEK;
        goto cleanup;
    }
    error_check = read(fd, target_record_field->data, target_record_field->data_len);
    if(-1 == error_check){
        perror("GET_RECORD_FIELD: Read error");
        return_value = ERROR_CODE_COULDNT_READ;
        goto cleanup;
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    if(NULL != fields)
        free(fields);
    return return_value;
}

/**
 * @brief: Reads a record into the input record_field ** record (which is a pointer to the record)
 * @param[IN] fd: The file descriptor of the table file
 * @param[OUT] record: A pointer to the record (which is an array of record_fields)
 * @param[IN] record_number: The target index of the record that the caller wants
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return: ERROR_CODE_SUCCESS on succes, else an indicative error code of type error_code_t
 * @notes: A record is a collection of record_field structs (see dafaq.h for the struct)
 */
error_code_t get_record(IN int fd, OUT record_field ** record, IN int record_number, IN bool preserve_offset){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int number_of_fields = 0;
    int number_of_records = 0;
    int record_len = 0;
    int record_field_offset = 0;
    int error_check = 0;
    int i = 0;
    off_t offset = 0;
    off_t old_offset = 0;
    field * fields = NULL;

    old_offset = lseek(fd, 0, SEEK_CUR);
    if(-1 == old_offset){
        perror("GET_RECORD: Lseek error");
        return_value = ERROR_CODE_COULDNT_LSEEK;
        goto cleanup;
    }

    number_of_fields = get_fields(fd, &fields, false);
    if(-1 == number_of_fields){
        return_value = ERROR_CODE_COULDNT_GET_FIELDS;
        goto cleanup;
    }

    for(i=0; i<number_of_fields; i++){
        record_len += fields[i].data_len;
    }

    number_of_records = get_num_of_records(fd, number_of_fields, false);
    if(-1 == number_of_records){
        return_value = ERROR_CODE_COULDNT_GET_NUM_OF_RECORDS;
        goto cleanup;
    }

    if(record_number > number_of_records){
        print_color("~~RECORD NUMBER OUT OF BOUNDS~\n", RED, BOLD, RESET);
        record_number = ERROR_CODE_INDEX_OUT_OF_BOUNDS;
        goto cleanup;
    }

    *record = calloc(number_of_fields, sizeof(record_field));
    if(NULL == *record){
        perror("GET_RECORD: Calloc error");
        return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
        goto cleanup;
    }

    for(i=0; i<number_of_fields; i++){
        return_value = get_record_field(fd, &(*record)[i], record_number, i, false);
        if(ERROR_CODE_SUCCESS != return_value){
            goto cleanup;
        }
        printf("DATA OUTSIDE: %s\n", (&(*record)[i])->data);
    }

/*
    offset = lseek(fd, record_number * record_len, SEEK_CUR);
    if(-1 == offset){
        perror("GET_RECORD: Lseek error");
        return_value = ERROR_CODE_COULDNT_LSEEK;
        goto cleanup;
    }
    for(i=0; i<number_of_fields; i++){
        (*record)[i].data = malloc(fields[i].data_len);
        (*record)[i].data_len = fields[i].data_len;
        (*record)[i].field_index = i;
        (*record)[i].record_num = record_number;
        (*record)[i].record_field_offset = record_field_offset;
        if(NULL == (*record)[i].data){
            perror("GET_RECORD: Malloc error");
            return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
            goto cleanup;
        }

        error_check = read(fd, (*record)[i].data, fields[i].data_len);
        if(-1 == error_check){
            perror("GET_RECORD: Read error");
            return_value = ERROR_CODE_COULDNT_READ;
            goto cleanup;
        }

        record_field_offset += fields[i].data_len;
    }
*/

    return_value = ERROR_CODE_SUCCESS;
    
cleanup:
    if(ERROR_CODE_SUCCESS != return_value){
        for(i=0; i<number_of_fields; i++){
            if(NULL != (*record)[i].data)
                free((*record)[i].data);
        }
        if(NULL != record)
            free(record);
    }

    return return_value;
}

/**
 * @brief: Reads all records in a table file into an array of records (records)
 * @param[IN] fd: The file descriptor of the table file
 * @param[OUT] records: An array of records that the caller wants to fill out with the records stored 
 *                      in the table file
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return: The number of records in the table file on success, else -1
 * @notes: records is a pointer to an array of records, and records are arrays of record_fields, so
 *         records is a pointer to an array of arrays of record_fields (record_field ***). Kind of
 *         confusing, I know.
 * 
 */
int get_all_records(IN int fd, OUT record_field *** records, IN bool preserve_offset){
    int num_of_fields = 0;
    int num_of_records = -1;
    int i = 0;
    error_code_t error_check = 0;

    num_of_fields = get_num_of_fields(fd, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    num_of_records = get_num_of_records(fd, num_of_fields, false);
    if(-1 == num_of_records){
        goto cleanup;
    }

    *records = calloc(num_of_records, sizeof(record_field *));
    if(NULL == *records){
        perror("GET_ALL_RECORDS: Calloc error");
        num_of_records = -1;
        goto cleanup;
    }

    for(i=0; i<num_of_records; i++){
        error_check = get_record(fd, &((*records)[i]), i, false);
        if(ERROR_CODE_SUCCESS != error_check){
            num_of_records = -1;
            goto cleanup;
        }
    }

cleanup:
    if(-1 == num_of_records){
        if(NULL != *records){
            for(i=0; i<num_of_records; i++){
                if(NULL != (*records)[i]){
                    free((*records)[i]);
                }
            }
            free(*records);
        }
    }    
    return num_of_records;
}

int partition_record_fields(record_field * record_fields, int low, int high){
    int i = -1;
    int j = 0;
    int value = 0;
    int partition = 0;
    record_field intermediary;

    partition = *((int *)record_fields[high].data);

    for(j=low; j<high; j++){
        value = *((int *)record_fields[j].data);

        if(value <= partition){
            i++;
            intermediary = record_fields[i];
            record_fields[i] = record_fields[j];
            record_fields[j] = intermediary;
        }
    }

    i++;
    intermediary = record_fields[i];
    record_fields[i] = record_fields[j];
    record_fields[j] = intermediary;

    return i;
}
/**
 * @brief: Given an array of record fields (meant to be all from different records) it sorts
 *         them by the value of their data and writes them (sorted) 
 */
int quicksort_record_fields(int table_fd, char * sort_file_name, int field_index){
    int error_check = 0;
    int sort_file_fd = -1;
    int num_of_records = 0;
    int num_of_fields = 0;
    int i = 0;
    int high = 0;
    int low = 0;
    int partition_index = 0;
    int total_data_len = 0;
    int sp = 0;     //Stack pointer
    int * stack = NULL;
    record_field * record_fields = NULL;

    num_of_fields = get_num_of_fields(table_fd, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    num_of_records = get_num_of_records(table_fd, num_of_fields, false);
    if(-1 == num_of_records){
        goto cleanup;
    }

    record_fields = calloc(num_of_records, sizeof(record_field));
    if(NULL == record_fields){
        perror("QUICKSORT_RECORD_FIELDS: Calloc error");
        goto cleanup;
    }

    for(i=0; i<num_of_records; i++){
        error_check = get_record_field(table_fd, &record_fields[i], i, field_index, false);
        if(ERROR_CODE_SUCCESS != error_check){
            goto cleanup;
        }
    }

    //Iterative Quicksort. Definitley not copied from GeeksForGeeks
    high = num_of_records - 1;
    low = 0;
    stack = calloc(high - low + 1, sizeof(int));

    //Pushing low and high onto the stack
    stack[sp] = low;
    sp++;
    stack[sp] = high;
    
    while(sp >= 0){
        //Popping high and low off of the stack
        high = stack[sp];
        sp--;
        low = stack[sp];
        sp--;

        partition_index = partition_record_fields(record_fields, low, high);

        /**If more than one element exists before the partition (one element will be ordered), push
         * The new high and low onto the stack.
         */
        if(partition_index > low + 1){
           sp++;
           stack[sp] = low;
           sp++;
           stack[sp] = partition_index - 1;
        }

        /**If more than one element exists after the partition (one element will be ordered), push
         * The new high and low onto the stack.
         */
        if(partition_index < high - 1){
            sp++;
            stack[sp] = partition_index + 1;
            sp++;
            stack[sp] = high;
        }
    }

    sort_file_fd = open(sort_file_name, O_RDWR | O_TRUNC | O_CREAT, 0666);
    if(-1 == sort_file_fd){
        perror("QUICKSORT_RECORD_FIELDS: Open error");
        goto cleanup;
    }

    for(i=0; i<num_of_records; i++){
        error_check = write(sort_file_fd, &(record_fields[i].record_num), sizeof(record_fields[i].record_num));
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Write error");
            close(sort_file_fd);
            sort_file_fd = -1;
            goto cleanup;
        }
    }
    
cleanup:
    return sort_file_fd;
}
