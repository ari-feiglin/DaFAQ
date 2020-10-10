#include "dafaq.h"

/**
 * @brief: Creates a database
 * @param[IN] name: Name of the database directory to create
 * 
 * @return: ERROR_CODE_SUCCESS on succes, else an indicative error of error_code_t 
 * @notes: Creates a directory whose name is input in the variable name and changes the working directory to it.
 *         This is an important function for all successive function calls. (Save diahrrea.)
 */
error_code_t create_database(IN char * name){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int error_check = 0;

    error_check = mkdir(name, 0);       //Make new Database directory
    if(-1 == error_check){
        if(EEXIST == errno){        //If directory already exists, don't fail
            print_color("~`~Directory already exists. Entering...~\n", BG_BLACK, FG,0,255,0, BOLD, RESET);
            error_check = chdir(name);      //Go into already existing directory
            if(-1 == error_check){
                perror("Chdir error");
                goto cleanup;
            }
            goto cleanup;
        }
        else{       //Other error, 
            perror("Database directory creation error");
            goto cleanup;
        }

    }
    error_check = chmod(name, 0775);
    if(-1 == error_check){
        perror("Database directory creation error");
        return_value = ERROR_CODE_COULDNT_CHMOD;
        goto cleanup;
    }

    error_check = chdir(name);
    if(-1 == error_check){
        perror("Chdir error");
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
 * @return: ERROR_CODE_SUCCESS on success, else an indicative error code of error_code_t
 * @notes: This function will remove any other file with the same name as table_name in the working directory. 
 */
error_code_t create_table(IN char * table_name, IN int num_of_fields, IN field * fields){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    int fd = -1;
    int error_check = 0;
    int num_of_records = 0;

    remove(table_name);

    fd = open(table_name, O_WRONLY | O_CREAT, 0666);
    if(-1 == fd){
        perror("Open error");
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }

    error_check = write(fd, magic, strnlen(magic, STRING_LEN));
    if(-1 == error_check){
        perror("Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }
    
    error_check = write(fd, &num_of_fields, sizeof(num_of_fields));
    if(-1 == error_check){
        perror("Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    error_check = write(fd, fields, num_of_fields * sizeof(field));
    if(-1 == error_check){
        perror("Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    error_check = write(fd, &num_of_records, sizeof(num_of_records));
    if(-1 == error_check){
        perror("Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    return_value = ERROR_CODE_SUCCESS;
cleanup:
    return return_value;
}
