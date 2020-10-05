#include "dafaq.h"

int create_database(char * name){
    int error_check = 0;

    error_check = mkdir(name, 0);
    if(-1 == error_check){
        if(EEXIST == errno){
            printf("Directory already exists. Entering...\n");
            error_check = chdir(name);
            if(-1 == error_check){
                perror("Chdir error");
                goto cleanup;
            }
            goto cleanup;
        }
        else{
            perror("Database directory creation error");
            goto cleanup;
        }

    }
    error_check = chmod(name, 0775);
    if(-1 == error_check){
        perror("Database directory creation error");
        goto cleanup;
    }

    error_check = chdir(name);
    if(-1 == error_check){
        perror("Chdir error");
        goto cleanup;
    }

cleanup:
    return error_check;
}

int create_table(char * table_name){
    int num_of_fields = 0;
    int difference = 0;
    int error_check = -1;
    int fd = -1;
    int num_of_records = 0;
    bool valid = false;
    char * field_name = NULL;
    char * field_data_type = NULL;
    char prompt[STRING_LEN] = {0};
    char * data_type_list = "char, int, string, boolean";
    char * database_header_name = NULL;
    field new_field = {0};
    field * fields = NULL;


    sprintf(prompt, "\n~`~CREATING TABLE %s~\n\n", table_name);
    print_color(prompt, BG_WHITE, FG,0,0,150, BOLD, RESET);

    while(true){
        valid = false;

        sprintf(prompt, "~`~Field number %d:~\n", num_of_fields);
        print_color(prompt, BG_WHITE, FG,0,0,0, BOLD, RESET);
        get_raw_input(NULL, (char **)&field_name);

        difference = strncmp(field_name, "quit", STRING_LEN);
        if(0 == difference){
            if(num_of_fields > 0){
                valid = true;
            }
            break;
        }

        fields = realloc(fields, (num_of_fields+1) * sizeof(field));
        if(NULL == fields){
            perror("Realloc error");
            num_of_fields = -1;
            goto cleanup;
        }

        sprintf(prompt, "~`~Datatype: (%s):~\n", data_type_list);
        print_color(prompt, BG_WHITE, FG,0,0,0, BOLD, RESET);
        get_raw_input(NULL, &field_data_type);
        lower(field_data_type, 0);

        strncpy(new_field.name, field_name, STRING_LEN);

        difference = strncmp(field_data_type, "char", STRING_LEN);
        if(0 == difference){
            new_field.data_len = CHAR;
            valid = true;
        }

        difference = strncmp(field_data_type, "int", STRING_LEN);
        if(0 == difference){
            new_field.data_len = INT;
            valid = true;
        }

        difference = strncmp(field_data_type, "string", STRING_LEN);
        if(0 == difference){
            new_field.data_len = STRING_LEN;
            valid = true;
        }

        difference = strncmp(field_data_type, "boolean", STRING_LEN);
        if(0 == difference){
            new_field.data_len = BOOLEAN;
            valid = true;
        }

        if(valid){
            fields[num_of_fields].data_len = new_field.data_len;
            memcpy(&(fields[num_of_fields].name), &(new_field.name), strnlen(new_field.name, NAME_LEN));
            num_of_fields++;
        }
        else{
            print_color("~`~INVALID REQUEST~\n", BG_B_WHITE, FG,255,0,0, BOLD, RESET);
        }
    }

    remove(table_name);

    fd = open(table_name, O_WRONLY | O_CREAT, 0666);
    if(-1 == fd){
        perror("Open error");
        num_of_fields = -1;
        goto cleanup;
    }

    error_check = write(fd, magic, strnlen(magic, STRING_LEN));
    if(-1 == error_check){
        perror("Write error");
        num_of_fields = -1;
        goto cleanup;
    }
    
    error_check = write(fd, &num_of_fields, sizeof(num_of_fields));
    if(-1 == error_check){
        perror("Write error");
        num_of_fields = -1;
        goto cleanup;
    }

    error_check = write(fd, fields, num_of_fields * sizeof(field));
    if(-1 == error_check){
        perror("Write error");
        num_of_fields = -1;
        goto cleanup;
    }

    error_check = write(fd, &num_of_records, sizeof(num_of_records));
    if(-1 == error_check){
        perror("Write error");
        goto cleanup;
    }

cleanup:
    close(fd);
    if(field_name)
        free(field_name);
    if(field_data_type)
        free(field_data_type);
    if(database_header_name)
        free(database_header_name);
    if(fields)
        free(fields);
        
    return num_of_fields;
}
