#include "dafaq.h"

/**
 * @brief: Gets a field from a record
 * @param[IN] fd: The file descriptor of the table file
 * @param[OUT] target_record_field: A pointer to the record_field to fill
 * @param[IN] record_number: The number of the record from which to retrieve the field
 * @param[IN] field_num: The number/index of the field to retrieve
 * @param[IN] perserve_offset: If the function should preserve the offset of the file from before when
 *                             the function was run.
 * 
 * @return: ERROR_CODE_SUCCESS on succes, else an indicative error code of type error_code_t
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
    }

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
    if(NULL != fields)
        free(fields);

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

/**
 * @brief: Partitions the array record_fields using Hoare's algorithm (thanks Wikipedia)
 * @param[OUT] record_fields: The array of record_fields
 * @param[IN] low: Lowest index
 * @param[IN] high: Highest index
 * 
 * @return: The index of the new partition/pivot
 * @notes: As stated earlier, this partition function uses Hoare's partition scheme (translated to C from Wikipedia). 
 *         It works like so: you have two pointers (i and j) which start at the opposite ends of the array. They work
 *         towards the pivot until they find a pair of elements that are in opposite places relative to each other 
 *         (the one of the right is smaller than the pivot and the one on the left is larger than the pivot), then it
 *         swaps the two elements. Basically, what it does is it iterates through one end of the array until it hits
 *         an element that is out of place, then does the same on the other end. Once it finds these two elements, it
 *         swaps them.
 *         This is not included in the header file, as it should not be called by anything other than the function 
 *         quicksort_record_fields.
 */
int partition_record_fields(OUT record_field * record_fields, IN int low, IN int high){
    int i = low-1;
    int j = high+1;
    int value = 0;
    int pivot = 0;
    record_field intermediary;

    pivot = *((int *)(record_fields[(high + low)/2].data));

    while(true){
        do{
            i++;
        }while(*((int *)(record_fields[i].data)) < pivot);

        do{
            j--;
        }while(*((int *)(record_fields[j].data)) > pivot);

        if(i >= j){
            break;
        }

        intermediary = record_fields[j];
        record_fields[j] = record_fields[i];
        record_fields[i] = intermediary;
    }

    return j;
}

/**
 * @brief: Given an array of record fields (meant to be all from different records) it sorts them by the 
 *         value of their data and writes their indexes (sorted) to the file specified by sort_file_name.
 * @param[IN] table_fd: The file descriptor of the table file
 * @param[IN] sort_file_fd: The file descriptor of the file to write the sorted indexes to
 * @param[IN] field_index: The index of the field to sort
 * @param[IN] truncate: If the function should truncate files that aren't up to date (Redundant)
 * 
 * @returns: ERROR_CODE_SUCCESS on succes, else an indicative error of type error_code_t
 * @notes: To sort, it uses the quicksort algorithm, but instead of being recursive, it is iterative to 
 *         save memory.
 */
error_code_t quicksort_record_fields(IN int table_fd, int sort_file_fd, int field_index, bool truncate){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int error_check = 0;
    int num_of_records = 0;
    int num_of_fields = 0;
    int sort_num_of_records = 0;
    int sort_num_of_fields = 0;
    int current_field_index = 0;
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
        return_value = ERROR_CODE_COULDNT_GET_NUM_OF_FIELDS;
        goto cleanup;
    }

    num_of_records = get_num_of_records(table_fd, num_of_fields, false);
    if(-1 == num_of_records){
        return_value = ERROR_CODE_COULDNT_GET_NUM_OF_FIELDS;
        goto cleanup;
    }

    error_check = lseek(sort_file_fd, 0, SEEK_SET);
    if(-1 == error_check){
        perror("QUICKSORT_RECORD_FIELDS: Lseek error");
        return_value = ERROR_CODE_COULDNT_LSEEK;
        goto cleanup;
    }

    error_check = read(sort_file_fd, &sort_num_of_records, sizeof(sort_num_of_fields));
    if(-1 == error_check){
        perror("QUICKSORT_RECORD_FIELDS: Read error");
        return_value = ERROR_CODE_COULDNT_READ;
        goto cleanup;
    }
    
    if(0 == sort_num_of_records){
        error_check = lseek(sort_file_fd, 0, SEEK_SET);
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Lseek error");
            return_value = ERROR_CODE_COULDNT_LSEEK;
            goto cleanup;
        }

        sort_num_of_records = num_of_records;
        error_check = write(sort_file_fd, &sort_num_of_records, sizeof(sort_num_of_fields));
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Write error");
            return_value = ERROR_CODE_COULDNT_READ;
            goto cleanup;
        }
    }
    if(sort_num_of_records != num_of_records){
        print_color("~~SORT FILE NOT UP TO DATE~\n", RED, BOLD, RESET);
        return_value = ERROR_CODE_INVALID_INPUT;
        goto cleanup;
    }

    record_fields = calloc(num_of_records, sizeof(record_field));
    if(NULL == record_fields){
        perror("QUICKSORT_RECORD_FIELDS: Calloc error");
        return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
        goto cleanup;
    }

    for(i=0; i<num_of_records; i++){
        error_check = get_record_field(table_fd, &record_fields[i], i, field_index, false);
        if(ERROR_CODE_SUCCESS != error_check){
            return_value = ERROR_CODE_COULDNT_GET_RECORD;
            goto cleanup;
        }
    }

    //Iterative Quicksort. Definitley not copied from GeeksForGeeks
    high = num_of_records - 1;
    low = 0;
    stack = calloc(high - low + 1, sizeof(int));
    if(NULL == stack){
        perror("QUICKSORT_RECORD_FIELDS: Calloc error");
        return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
        goto cleanup;
    }

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

        //If the pivot is not at the lowest index, push the new high and low onto the stack.
        if(partition_index > low){
           sp++;
           stack[sp] = low;
           sp++;
           stack[sp] = partition_index;
        }

        //If the pivot is not at the highest index, push the new high and low onto the stack.
        if(partition_index < high){
            sp++;
            stack[sp] = partition_index + 1;
            sp++;
            stack[sp] = high;
        }
    }

    error_check = read(sort_file_fd, &sort_num_of_fields, sizeof(sort_num_of_fields));
    if(-1 == error_check){
        perror("QUICKSORT_RECORD_FIELDS: Read error");
        return_value = ERROR_CODE_COULDNT_READ;
        goto cleanup;
    }

    for(i=0; i<sort_num_of_records && sort_num_of_fields != 0; i++){
        error_check = read(sort_file_fd, &current_field_index, sizeof(current_field_index));
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Read error");
            return_value = ERROR_CODE_COULDNT_READ;
            goto cleanup;
        }

        if(current_field_index == field_index){
            i = -1;
            error_check = lseek(sort_file_fd, -1 * sizeof(current_field_index), SEEK_CUR);
            if(-1 == error_check){
                perror("QUICKSORT_RECORD_FIELDS: Lseek error");
                return_value = ERROR_CODE_COULDNT_LSEEK;
                goto cleanup;
            }
            break;
        }

        error_check = lseek(sort_file_fd, sort_num_of_records * sizeof(int), SEEK_CUR);
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Lseek error");
            return_value = ERROR_CODE_COULDNT_LSEEK;
            goto cleanup;
        }

    }
    
    if(i != -1){
        sort_num_of_fields++;
        error_check = lseek(sort_file_fd, sizeof(sort_num_of_records), SEEK_SET);
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Lseek error");
            return_value = ERROR_CODE_COULDNT_LSEEK;
            goto cleanup;
        }

        error_check = write(sort_file_fd, &sort_num_of_fields, sizeof(sort_num_of_fields));
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Lseek error");
            return_value = ERROR_CODE_COULDNT_LSEEK;
            goto cleanup;
        }

        error_check = lseek(sort_file_fd, 0, SEEK_END);
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Lseek error");
            return_value = ERROR_CODE_COULDNT_LSEEK;
            goto cleanup;
        }
    }

    error_check = write(sort_file_fd, &field_index, sizeof(field_index));
    if(-1 == error_check){
        perror("QUICKSORT_RECORD_FIELDS: Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    for(i=0; i<num_of_records; i++){
        error_check = write(sort_file_fd, &(record_fields[i].record_num), sizeof(record_fields[i].record_num));
        if(-1 == error_check){
            perror("QUICKSORT_RECORD_FIELDS: Write error");
            return_value = ERROR_CODE_COULDNT_WRITE;
            goto cleanup;
        }
    }

    return_value = ERROR_CODE_SUCCESS;
    
cleanup:
    if(NULL != stack)
        free(stack);
    for(i=0; i<num_of_records; i++){
        if(NULL != record_fields[i].data)
            free(record_fields[i].data);
    }
    if(NULL != record_fields)
        free(record_fields);
    return return_value;
}

/**
 * @brief: Finds records that have the target data relative to the operation
 * @param[IN] table_fd: The file descriptor of the table file
 * @param[IN] sort_fd: The file descriptor of the sort file
 * @param[IN] target_data: The data that the caller wants to locate
 * @param[IN] operator: The inequality operation to perform in order to locate valid records
 * @param[IN] field_index: The idex of the field in which to find corresponding records
 * @param[OUT] valid_record_indexes: An array to hold a mapping of which records are valid
 * 
 * @returns: ERROR_CODE_SUCCESS on success, else an indicative error code of type error_code_t
 * @notes: The valid operations are defined as follows:
 *              + == - equal to
 *              + != - not equal to
 *              + >= - greater than or equal to
 *              + <= - less than or equal to
 *              + > - greater than
 *               + < - less than
 *         These are relative to the data in the record's field, then the target (so data < target, not
 *         target < data)
 */
error_code_t binary_search_sort_file(int table_fd, int sort_fd, char * target_data, operators operator, int field_index, bool ** valid_record_indexes){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int first_matching_index = 0;
    int current_index = 0;
    int table_num_of_fields = 0;
    int num_of_fields = 0;
    int error_check = 0;
    int num_of_records = 0;
    int current_field_index = -1;
    int i = 0;
    int high = 0;
    int low = 0;
    int high_data = 0;
    int low_data = 0;
    int difference = 0;
    record_field field = {0};
    int * record_nums = NULL;

    table_num_of_fields = get_num_of_fields(table_fd, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    error_check = lseek(sort_fd, 0, SEEK_SET);
    if(-1 == error_check){
        perror("BINARY_SEARCH_SORT_FILE: Lseek error");
        goto cleanup;
    }

    error_check = read(sort_fd, &num_of_records, sizeof(num_of_records));
    if(-1 == error_check){
        perror("BINARY_SEARCH_SORT_FILE: Read error");
        goto cleanup;
    }

    error_check = read(sort_fd, &num_of_fields, sizeof(num_of_fields));
    if(-1 == error_check){
        perror("BINARY_SEARCH_SORT_FILE: Read error");
        goto cleanup;
    }

    error_check = get_num_of_records(table_fd, table_num_of_fields, false);
    if(-1 == error_check){
        goto cleanup;
    }

    if(error_check != num_of_records){
        return_value = ERROR_CODE_OUT_OF_DATE;
        goto cleanup;
    }

    for(i=0; current_field_index != field_index; i++){
        if(i > num_of_fields){
            return_value = ERROR_CODE_FIELD_DOESNT_EXIST;
            goto cleanup;
        }
        error_check = read(sort_fd, &current_field_index, sizeof(current_field_index));
        if(-1 == error_check){
            perror("BINARY_SEARCH_SORT_FILE: Read error");
            goto cleanup;
        }

        if(current_field_index != field_index){
            error_check = lseek(sort_fd, num_of_records * sizeof(int), SEEK_CUR);
            if(-1 == error_check){
                perror("BINARY_SEARCH_SORT_FILE: Lseek error");
                goto cleanup;
            }
        }
    }

    record_nums = calloc(num_of_records, sizeof(int));
    if(NULL == record_nums){
        perror("BINARY_SEARCH_SORT_FILE: Calloc error");
        goto cleanup;
    }

    error_check = read(sort_fd, record_nums, num_of_records * sizeof(int));
    if(-1 == error_check){
        perror("BINARY_SEARCH_SORT_FILE: Read error");
        goto cleanup;
    }

    high = num_of_records - 1;
    low = 0;
    current_index = 0;

    //If target data does not exist, and is less than the smallest element...
    if(NULL != field.data){
        free(field.data);
    }
    error_check = get_record_field(table_fd, &field, record_nums[low], field_index, false);
    if(ERROR_CODE_SUCCESS != error_check){
        goto cleanup;
    }
    if(*((int *)target_data) < *((int *)field.data)){
        current_index = low-1;
        low = high + 1;
        high = -1;
    }

    //...or larger than the biggest element
    if(NULL != field.data){
        free(field.data);
    }
    error_check = get_record_field(table_fd, &field, record_nums[high], field_index, false);
    if(ERROR_CODE_SUCCESS != error_check){
        goto cleanup;
    }
    if(*((int *)target_data) > *((int *)field.data)){
        current_index = high+1;
        low = high + 1;
        high = -1;
    }
    
    while(high >= low){     //The actual binary search
        current_index = (high+low)/2;

        if(NULL != field.data){
            free(field.data);
        }
        error_check = get_record_field(table_fd, &field, record_nums[current_index], field_index, false);
        if(ERROR_CODE_SUCCESS != error_check){
            goto cleanup;
        }

        if(*((int *)target_data) == *((int *)field.data)){
            high = -1;
            break;
        }
        if(*((int *)target_data) > *((int *)field.data)){
            low = current_index + 1;
        }
        else{
            high = current_index - 1;
        }
    }       /** 
             * This search may not give us the first time the value exists in the array. In fact, it may give us 
             * the wrong value altogether! (Due to how I checked if it was right), so we need another method to
             * get to the first value. Then we can iterate forwards through the array and get every record_field
             * that matches.
             * 
             * NOTE: After this, low = high + 1
             */
    //If element did not exist in list.
    if(-1 != high){
        if(NULL != field.data){
            free(field.data);
        }
        error_check = get_record_field(table_fd, &field, record_nums[high], field_index, false);
        if(ERROR_CODE_SUCCESS != error_check){
            goto cleanup;
        }
        high_data = *((int *)field.data);

        if(NULL != field.data){
            free(field.data);
        }
        error_check = get_record_field(table_fd, &field, record_nums[low], field_index, false);
        if(ERROR_CODE_SUCCESS != error_check){
            goto cleanup;
        }
        low_data = *((int *)field.data);

        if(low_data - *((int *)target_data) > *((int *)target_data) - high_data){
            current_index = high;
        }
        else{
            current_index = low;
        }
    }
    if(NULL != field.data){
        free(field.data);
    }
    error_check = get_record_field(table_fd, &field, record_nums[current_index], field_index, false);
    if(ERROR_CODE_SUCCESS != error_check){
        goto cleanup;
    }
    
    while(*((int *)target_data) == *((int *)field.data)){       //This is a loop to get to the first instance of the target data
        current_index--;
        if(current_index <= 0){
            break;
        }

        if(NULL != field.data){
            free(field.data);
        }
        return_value = get_record_field(table_fd, &field, record_nums[current_index], field_index, false);
        if(ERROR_CODE_SUCCESS != return_value){
            goto cleanup;
        }
    }

    current_index++;
    if(NULL != field.data){
        free(field.data);
    }
    return_value = get_record_field(table_fd, &field, record_nums[current_index], field_index, false);
    if(ERROR_CODE_SUCCESS != return_value){
        goto cleanup;
    }
    if(*((int *)target_data) != *((int *)field.data)){
        current_index--;
    }
    first_matching_index = current_index;

    if(NULL != field.data){
        free(field.data);
    }
    return_value = get_record_field(table_fd, &field, record_nums[current_index], field_index, false);

    *valid_record_indexes = calloc(num_of_records, sizeof(bool));    //The byte map for the valid records
    if(NULL == *valid_record_indexes){
        perror("BINARY_SEARCH_SORT_FILE: Calloc error");
        return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
        goto cleanup;
    }
    if(NOT_EQUALS == operator){
        memset(*valid_record_indexes, true, num_of_records);
    }
    else{
        memset(*valid_record_indexes, false, num_of_records);
    }

    while(*((int *)target_data) == *((int *)field.data)){
        difference = strncmp(field.data, target_data, field.data_len);
        if(0 == difference){
            if(NOT_EQUALS == operator){
                (*valid_record_indexes)[field.record_num] = false;
            }
            else if(EQUALS == operator){
                (*valid_record_indexes)[field.record_num] = true;
            }
        }

        if(GREATER_OR_EQUALS == operator || LESS_OR_EQUALS == operator){
            (*valid_record_indexes)[field.record_num] = true;
        }

        if(current_index >= num_of_records-1){
            break;
        }
        current_index++;

        if(NULL != field.data){
            free(field.data);
        }
        error_check = get_record_field(table_fd, &field, record_nums[current_index], field_index, false);
        if(ERROR_CODE_SUCCESS != error_check){
            goto cleanup;
        }
    }

    if(GREATER_OR_EQUALS == operator || GREATER == operator){
        for(current_index; current_index<num_of_records; current_index++){
            if(NULL != field.data){
                free(field.data);
            }
            error_check = get_record_field(table_fd, &field, record_nums[current_index], field_index, false);
            if(ERROR_CODE_SUCCESS != error_check){
                goto cleanup;
            }
            if(*((int *)field.data) > *((int *)target_data)){
                (*valid_record_indexes)[field.record_num] = true;
            }
        }
    }
    else if(LESS_OR_EQUALS == operator || LESS == operator){
        for(current_index; current_index>=0; current_index--){
            if(NULL != field.data){
                free(field.data);
            }
            error_check = get_record_field(table_fd, &field, record_nums[current_index], field_index, false);
            if(ERROR_CODE_SUCCESS != error_check){
                goto cleanup;
            }
            if(*((int *)field.data) < *((int *)target_data)){
                (*valid_record_indexes)[field.record_num] = true;
            }
        }
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    if(NULL != field.data)
        free(field.data);

    if(NULL != record_nums)
        free(record_nums);
    return return_value;
}
