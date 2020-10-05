#include "dafaq.h"

int poop(char * file_name){
    field * fields = NULL;
    int error_check = 0;
    int num_of_fields = 0;
    int num_of_records = -1;
    int fd = 0;
    int record = 0;
    int current_field = 0;
    int int_data = 0;
    bool is_valid = false;
    char byte_data = 0;
    char string_data[STRING] = {0};
    char * print_text = NULL;
    off_t offset = 0;
    
    fd = open(file_name, O_RDONLY);
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

    num_of_records = get_num_of_records(fd, num_of_fields, false);
    if(-1 == num_of_records){
        goto cleanup;
    }
    
    for(current_field=0; current_field<num_of_fields; current_field++){
        if(NULL != print_text){
            free(print_text);
        }
        rect_text(fields[current_field].name, (char **)&print_text, NAME_LEN);
        print_color("~`~", BG_RED, FG,0,0,0, BOLD);
        printf("%s", print_text);
        print_color("~ ", RESET);
    }

    for(record=0; record<num_of_records; record++){
        print_color("~\n", RESET);
        for(current_field=0; current_field<num_of_fields; current_field++){
            memset(string_data, 0, STRING_LEN);

            if(INT == fields[current_field].data_len){
                error_check = read(fd, &int_data, sizeof(int_data));
                if(-1 == error_check){
                    perror("Read error");
                    num_of_records = -1;
                    goto cleanup;
                }
                sprintf(string_data, "%i", int_data);
            }
            else if(CHAR == fields[current_field].data_len){
                error_check = read(fd, &byte_data, sizeof(byte_data));
                if(-1 == error_check){
                    perror("Read error");
                    num_of_records = -1;
                    goto cleanup;
                }
                sprintf(string_data, "%i", byte_data);
            }
            else if(STRING == fields[current_field].data_len){
                error_check = read(fd, string_data, STRING);
                if(-1 == error_check){
                    perror("Read error");
                    num_of_records = -1;
                    goto cleanup;
                }
            }
            else{
                print_color("~~INVALID DATA TYPE~\n", B_RED, BOLD, RESET);
                num_of_records = -1;
                goto cleanup;
            }
            rect_text(string_data, (char **)&print_text, NAME_LEN);
            print_color("~~~", BG_BLACK, B_WHITE, BOLD);
            printf("%s", print_text);
            print_color("~ ", RESET);
        }
    }
    printf("\n");


cleanup:
    if(fields)
        free(fields);
    if(print_text)
        free(print_text);

    return num_of_records;
}
