#include "dafaq.h"

int COUNT(int fd, int num_of_records, bool * valid_record_map){
    int count = -1;
    int num_of_fields = 0;
    int i = 0;
    
    if(-1 == num_of_records){
        num_of_fields = get_num_of_fields(fd, true);
        if(-1 == num_of_fields){
            goto cleanup;
        }

        num_of_records = get_num_of_records(fd, num_of_fields, true);
        if(-1 == num_of_records){
            goto cleanup;
        }
    }

    count = 0;
    for(i=0; i<num_of_records; i++){
        if(valid_record_map[i]){
            count++;
        }
    }

cleanup:
    return count;
}

int SUM(int fd, int field_index, int num_of_records, bool * valid_record_map){
    int sum = -1;
    int i = 0;
    int num_of_fields = 0;
    record_field curr_record_field = {0};
    int error_check = 0;

    if(-1 == num_of_records){
        num_of_fields = get_num_of_fields(fd, true);
        if(-1 == num_of_fields){
            goto cleanup;
        }

        num_of_records = get_num_of_records(fd, num_of_fields, true);
        if(-1 == num_of_records){
            goto cleanup;
        }
    }

    sum = 0;
    for(i=0; i<num_of_records; i++){
        if(valid_record_map[i]){
            if(NULL != curr_record_field.data){
                free(curr_record_field.data);
            }
            error_check = get_record_field(fd, &curr_record_field, i, field_index, true);
            if(-1 == error_check){
                sum = -1;
                goto cleanup;
            }

            sum += *((int *)curr_record_field.data);
        }
    }

cleanup:
    if(NULL != curr_record_field.data)
        free(curr_record_field.data);
    return sum;
}

double AVG(int fd, int field_index, int num_of_records, bool * valid_record_map){
    double average = 0;
    int num_of_valid_records = 0;

    average = (double)SUM(fd, field_index, num_of_records, valid_record_map);
    if(-1 == average){
        goto cleanup;
    }

    num_of_valid_records = COUNT(fd, num_of_records, valid_record_map);
    if(-1 == num_of_valid_records){
        goto cleanup;
    }

    average /= num_of_valid_records;

cleanup:
    return average;
}

