#include "dafaq.h"

/**
 * @brief: Creates a database
 * @param[IN] name: Name of the database directory to create
 * 
 * @return: ERROR_CODE_SUCCESS on succes, else an indicative error code of type error_code_t 
 * @notes: Creates a directory whose name is input in the variable name and changes the working directory to it.
 *         This is an important function for all successive function calls. (Save diahrrea.)
 */
error_code_t create_database(IN char * name){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int error_check = 0;
    int row_len = 75;

    error_check = mkdir(name, 0);       //Make new Database directory
    if(-1 == error_check){
        if(EEXIST == errno){        //If directory already exists, don't fail
            print_indent_line(1, row_len, true, 0,200,255);
            print_color("`  `~Directory already exists. Entering...~\n", BG,0,0,0, FG,255,100,0, BOLD, RESET);
            error_check = chdir(name);      //Go into already existing directory
            if(-1 == error_check){
                print_indent_line(1, row_len, true, 255,0,0);
                print_color("`  ~~", BG,0,0,0, RED, BOLD);
                fflush(stdout);
                perror("CREATE_DATABASE: Chdir error");
                print_color("~", RESET);
                return_value = ERROR_CODE_COULDNT_OPEN;
                goto cleanup;
            }
            return_value = ERROR_CODE_SUCCESS;
            goto cleanup;
        }
        else{       //Other error, 
            print_indent_line(1, row_len, true, 255,0,0);
            print_color("`  ~", BG,0,0,0, RED);
            fflush(stdout);
            perror("CREATE_DATABASE: Mkdir error");
            print_color("~", RESET);
            goto cleanup;
        }

    }
    error_check = chmod(name, 0775);
    if(-1 == error_check){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_DATABASE: Chmod error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_CHMOD;
        goto cleanup;
    }

    error_check = chdir(name);
    if(-1 == error_check){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_DATABASE: Chdir error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_CHANGE_DIR;
        goto cleanup;
    }

    return_value = ERROR_CODE_SUCCESS;
cleanup:
    return return_value;
}

/** 
 * @bried: Creates a table inside of the working directory
 * @param[IN] table_nam: Name of the table file to create
 * @param[IN] num_of_fields: The number of fields to add to the table file.
 * @param[IN] fields: An array of fields to write to the table file
 * 
 * @return: ERROR_CODE_SUCCESS on success, else an indicative error code of type error_code_t
 * @notes: This function will remove any other file with the same name as table_name in the working directory. 
 */
error_code_t create_table(IN char * table_name, IN int num_of_fields, IN field_t * fields){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int fd = -1;
    int error_check = 0;
    int num_of_records = 0;
    int sort_fd = 0;
    int zero = 0;
    int i = 0;
    int row_len = 75;
    char * sort_file_name = NULL;

    remove(table_name);

    fd = open(table_name, O_WRONLY | O_CREAT, 0666);
    if(-1 == fd){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_TABLE: Open error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }

    error_check = write(fd, magic, strnlen(magic, STRING_LEN));
    if(-1 == error_check){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_TABLE: Write error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }
    
    error_check = write(fd, &num_of_fields, sizeof(num_of_fields));
    if(-1 == error_check){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_TABLE: Write error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    error_check = write(fd, fields, num_of_fields * sizeof(field_t));
    if(-1 == error_check){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_TABLE: Write error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    error_check = write(fd, &num_of_records, sizeof(num_of_records));
    if(-1 == error_check){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_TABLE: Write error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    sort_file_name = malloc(strnlen(table_name, STRING_LEN) + extension_len + 1);
    sprintf(sort_file_name, "%s.srt", table_name);

    remove(sort_file_name);
    sort_fd = creat(sort_file_name, 0666);
    if(-1 == sort_fd){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_TABLE: Open error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }

    error_check = write(sort_fd, &zero, sizeof(zero));      //Write number of fields written to the sort file (0)
    if(-1 == error_check){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_TABLE: Open error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    error_check = write(sort_fd, &zero, sizeof(zero));      //Write number of records in the table (0)
    if(-1 == error_check){
        print_indent_line(1, row_len, true, 255,0,0);
        print_color("`  ~", BG,0,0,0, RED);
        fflush(stdout);
        perror("CREATE_TABLE: Write error");
        print_color("~", RESET);
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    if(-1 != sort_fd)
        close(sort_fd);
    if(NULL != sort_file_name){
        free(sort_file_name);
    }
    return return_value;
}
