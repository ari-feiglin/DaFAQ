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
