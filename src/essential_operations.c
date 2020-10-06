#include "dafaq.h"

bool check_magic(int fd, bool preserve_offset){
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
    if(magic_check)
        free(magic_check);
    return is_valid;
}

int get_num_of_fields(int fd, bool preserve_offset){
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

int get_fields(int fd, field ** fields, bool preserve_offset){
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

int get_num_of_records(int fd, int num_of_fields, bool preserve_offset){
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

cleanup:
    return num_of_records;
}

int get_len_of_record(int fd, bool preserve_offset){
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

}

bool check_extension(char * table_name){
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

bool check_input_mask(char * input_mask){
    bool is_valid = true;
    int i = 0;

    for(i=0; input_mask[i] != 0; i++){
        if( ('C' != input_mask[i]) &&
        ('c' != input_mask[i]) &&
        ('n' != input_mask[i]) &&
        ('L' != input_mask[i]) &&
        ('l' != input_mask[i]) &&
        ('@' != input_mask[i]) ){
            is_valid = false;
            goto cleanup;
        }
    }

cleanup:
    return is_valid;
}

int valid_input(char * input, char * input_mask){
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

    if(input_mask_len != input_len){
        is_valid = 0;
        goto cleanup;
    }

    for(i=0; i<input_len; i++){
        if('C' == input_mask[i]){
            upper(input+i, 1);
        }
        else if('n' == input_mask[i]){
            if(input[i] < '0' || input[i] > '9'){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('l' == input_mask[i]){
            if(input[i] < 'A' || ( ('Z' < input[i]) && (input[i] < 'A') ) || input[i] > 'z'){
                is_valid = 0;
                goto cleanup;
            }
        }
        else if('L' == input_mask[i]){
            if(input[i] < 'A' || ( ('Z' < input[i]) && (input[i] < 'A') ) || input[i] > 'z'){
                is_valid = 0;
                goto cleanup;
            }
            else{
                upper(input+i, 1);
            }
        }
        else if('@' == input_mask[i]){
            if( ('0' <= input[i] && input[i] <= '9') ||
            ('A' <= input[i] && input[i] <= 'Z') ||
            ('a' <= input[i] && input[i] <= 'z') ){
                is_valid = 0;
                goto cleanup;
            }
        }
    }

cleanup:
    return is_valid;
}
