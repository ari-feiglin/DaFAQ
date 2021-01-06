#include "dafaq.h"
#include <dirent.h>

/**
 * @brief: Prints/writes a table file in table form
 * @param[IN] table_name: The name of the table file
 * @param[IN] dump_file: The name of the markdown file to dump the output table into
 * @param[IN] truncate: If a dump file was specified, if it should truncate it
 * 
 * @return: On success number of records in the table file, -1 on error
 * @notes: An input of dump_file = NULL will print the table to stdout and truncate will be ignored.
 */
int poop(IN char * table_name, IN char * dump_file, IN bool truncate){
    field_t * fields = NULL;
    int error_check = 0;
    int num_of_fields = 0;
    int num_of_records = -1;
    int fd = -1;
    int dump_fd = -1;
    int record = 0;
    int current_field = 0;
    int int_data = 0;
    int name_len = 0;
    int row_len = 75;
    bool is_valid = false;
    bool should_dump = true;
    char byte_data = 0;
    char string_data[STRING] = {0};
    char * print_text = NULL;
    off_t offset = 0;
    
    fd = open(table_name, O_RDONLY);        //Open table
    if(-1 == fd){
        perror("Open error");
        goto cleanup;
    }

    is_valid = check_magic(fd, false);      //Check if table has valid magic sequence
    if(!is_valid){
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &fields, false);     //Get the table's fields
    if(-1 == num_of_fields){
        goto cleanup;
    }

    num_of_records = get_num_of_records(fd, num_of_fields, false);      //Get the number of records
    if(-1 == num_of_records){
        goto cleanup;
    }

    print_text = malloc(strnlen(table_name, STRING_LEN)+12);        //Allocate memory for print_text
    if(NULL == print_text){
        perror("POOP: Malloc error");
        num_of_records = -1;
        goto cleanup;
    }

    if(NULL == dump_file){      //If user wants to print table to console
        should_dump = false;
    }
    else{
        if(truncate){       
            dump_fd = open(dump_file, O_WRONLY | O_TRUNC | O_CREAT, 0666);      //If the user wants to print table to markdown file and delete what was previosuly there
        }
        else{
            dump_fd = open(dump_file, O_WRONLY | O_CREAT, 0666);        //Don't truncate previous data
        }
        if(-1 == dump_fd){
            perror("POOP: Open error");
            num_of_records = -1;
            goto cleanup;
        }

        if(!truncate){
            offset = lseek(dump_fd, 0, SEEK_END);       //If the user doesn't want to truncate the markdown file, seek to the end as to not override the data
            if(-1 == offset){
                perror("POOP: Lseek error");
                num_of_records = -1;
                goto cleanup;
            }
        }
    }

    if(!should_dump){
        sprintf(print_text, "\n~`~%s:~\n\n", table_name);
        print_color(print_text, BG_B_WHITE, FG,0,0,255, BOLD, RESET);       //Print the table name
        for(current_field=0; current_field<num_of_fields; current_field++){     //Iterate over the table's fields
            rect_text(fields[current_field].name, (char **)&print_text, NAME_LEN);      //Rectangle-ify the table name
            print_color("~`~", BG_RED, FG,0,0,0, BOLD);
            printf("%s", print_text);       //Print the rectangle-ified table name
            print_color("~ ", RESET);
        }
    }
    else{
        sprintf(print_text, "### **%s:**\n", table_name);       //Make a bold header (3) with the table name
        name_len = strnlen(print_text, NAME_LEN+12);
        error_check = write(dump_fd, print_text, name_len);     //Write it to the markdown file
        if(-1 == error_check){
            perror("POOP: Write error");
            num_of_records = -1;
            goto cleanup;
        }

        name_len = 1;

        //Print Field Names
        for(current_field=0; current_field<num_of_fields; current_field++){     //Iterate over the fields
            name_len += strnlen(fields[current_field].name, NAME_LEN)+1;        //Increment name_len to reflect the new length of print_text

            print_text = realloc(print_text, name_len);
            if(NULL == print_text){
                perror("POOP: Realloc error");
                num_of_records = -1;
                goto cleanup;
            }
            if(0 == current_field){
                memset(print_text, 0, name_len);    /*  If this is the first iteration, set all of print_text to 0. (I had to do this, 
                                                     *  not sure why it works I only have to do this the first time, or why at all) */
            }

            sprintf(print_text, "%s%s|", print_text, fields[current_field].name);       //Add the new field name and a | to print_text
        }

        name_len++;     //Increment name_len in order to make room for a newline character and null terminater
        print_text = realloc(print_text, name_len);
        if(NULL == print_text){
            perror("POOP: Realloc error");
            num_of_records = -1;
            goto cleanup;
        }
        sprintf(print_text, "%s\n", print_text);        //Add a newline character to print_text

        error_check = write(dump_fd, print_text, name_len-1);     //Write print_text to the markdown file
        if(-1 == error_check){
            perror("POOP: Write error");
            num_of_records = -1;
            goto cleanup;
        }

        //Print Field Name delimiter (eg. :-:|:-:|:-:|)
        if(NULL != print_text){     //Reinitialize name_len and print_text
            free(print_text);
            print_text = NULL;
        }
        name_len = 4 * sizeof(char) * num_of_fields+2;
        print_text = malloc(name_len);      //Allocate memory to print_text (each field requires 4 characters: :-:|), a newline, and a NUL byte
        memset(print_text, 0, name_len);
        
        for(current_field=0; current_field<num_of_fields; current_field++){
            sprintf(print_text, "%s:-:|", print_text);
        }

        sprintf(print_text, "%s\n", print_text);    //Add newline character to print_text

        error_check = write(dump_fd, print_text, name_len-1);     //Write the Field name delimiter to the markdown file
        if(-1 == error_check){
            perror("POOP: Write error");
            num_of_records = -1;
            goto cleanup;
        }
    }

    //Print records
    for(record=0; record<num_of_records; record++){     //Iterate through every record
        name_len = 1;

        if(!should_dump){
            print_color("~\n", RESET);      //If not dumping table, print a newline and reset the color/emphasis
        }
        
        for(current_field=0; current_field<num_of_fields; current_field++){
            memset(string_data, 0, STRING_LEN);     //Reinitialize string_data

            if(INT == fields[current_field].data_len){      //If datatype is int
                error_check = read(fd, &int_data, sizeof(int_data));    //Read data into int_data
                if(-1 == error_check){
                    perror("Read error");
                    num_of_records = -1;
                    goto cleanup;
                }

                sprintf(string_data, "%i", int_data);       //Convert int_data into string and place it into string_data
            }
            else if(CHAR == fields[current_field].data_len){        //If data type is byte
                error_check = read(fd, &byte_data, sizeof(byte_data));      //Read data into byte_data
                if(-1 == error_check){
                    perror("Read error");
                    num_of_records = -1;
                    goto cleanup;
                }

                sprintf(string_data, "%i", byte_data);      //Convert byte_data into string and place it into string_data
            }
            else if(STRING == fields[current_field].data_len){      //If data type is string
                error_check = read(fd, string_data, STRING);    //Read directly into string_data
                if(-1 == error_check){
                    perror("Read error");
                    num_of_records = -1;
                    goto cleanup;
                }
            }
            else{
                print_color("~~INVALID DATA TYPE~\n", B_RED, BOLD, RESET);      //Invalid data type
                num_of_records = -1;
                goto cleanup;
            }

            if(!should_dump){       //If not dumping table
                rect_text(string_data, (char **)&print_text, NAME_LEN);     //Rectangle-ify string data and print
                print_color("~~~", BG_BLACK, B_WHITE, BOLD);
                printf("%s", print_text);
                print_color("~ ", RESET);
            }
            else{       //If dumping table
                name_len += strnlen(string_data, STRING_LEN)+1;    //Name len is length of string_data and 1
                print_text = realloc(print_text, name_len); 
                if(NULL == print_text){
                    perror("POOP: Realloc error");
                    num_of_records = -1;
                    goto cleanup;
                }
                if(0 == current_field){
                    memset(print_text, 0, name_len);
                }

                sprintf(print_text, "%s%s|", print_text, string_data);
            }
        }

        if(should_dump){
            print_text[name_len-1] = '\n';

            error_check = write(dump_fd, print_text, name_len);    //Write print_text to dump file
            if(-1 == error_check){
                perror("POOP: Write error");
                num_of_records = -1;
                goto cleanup;
            }
        }
        
    }
    if(should_dump){
        error_check = write(dump_fd, "<br />\n\n", strlen("<br />\n\n"));
        if(-1 == error_check){
            perror("POOP: Write error");
            num_of_records = -1;
            goto cleanup;
        }
    }
    else{
        printf("\n\n");
    }


cleanup:
    if(-1 != dump_fd)
        close(dump_fd);
        
    if(fields)
        free(fields);
    if(print_text)
        free(print_text);

    return num_of_records;
}

/**
 * @brief: Print/writes every table in a database directory
 * @param[IN] database_name: The name of the database directory
 * @param[IN] dump_name: The name of the markdown file to dump the tables into
 * 
 * @return: On success the number of tables printed/written, else -1
 * @notes: This function iterates over the contents of the directory, checks if they are a valid, and
 *         if they are, it poops them.
 */
int diarrhea(IN char * database_name, IN char * dump_name){
    int num_of_tables = 0;
    int error_check = 0;
    int difference = 0;
    bool is_valid = false;
    bool truncate = true;
    DIR * directory = NULL;
    struct dirent * entry = NULL;

    directory = opendir(database_name);
    if(NULL == directory){
        perror("DIARRHEA: Opendir error");
        num_of_tables = -1;
        goto cleanup;
    }
    while(true){
        errno = 0;
        entry = readdir(directory);
        if(NULL == entry){
            if(0 != errno){
                perror("DIARHEA: Readdir error");
                num_of_tables = -1;
                goto cleanup;
            }
            else{
                break;
            }
        }

        difference = strncmp(".", entry->d_name, STRING_LEN);   //Check if current directory
        if(0 == difference){
            continue;
        }
        difference = strncmp("..", entry->d_name, STRING_LEN);      //Check if parent directory
        if(0 == difference){
            continue;
        }

        is_valid = check_extension(entry->d_name);      //Check if file is valid table file (extension-wise)
        if(!is_valid){
            continue;
        }

        error_check = poop(entry->d_name, dump_name, truncate);
        if(truncate){
            truncate = false;
        }
        if(-1 == error_check && 0 != errno){
            num_of_tables = -1;
            goto cleanup;
        }

        num_of_tables++;
    }

cleanup:
    if(NULL != directory){
        free(directory);
    }
    return num_of_tables;
}

/**
 * @brief: Writes the output of a query to a table file
 * @param[IN] table_fd: The file descriptor of the table file that the query was performed on
 * @param[IN] file_name: The name of the file to create
 * @param[IN] num_of_fields: The number of files in the table (of table_fd)
 * @param[IN] field_map: A map of all of the valid/SELECTed fields
 * @param[IN] num_of_records: The number of records in the table (of table_fd)
 * @param[IN] record_map: A map of all of the records that were validated by the query
 * 
 * @returns: ERROR_CODE_SUCCESS on success, else an indicative error code
 */
error_code_t write_table(IN int table_fd, IN char * file_name, IN int num_of_fields, IN bool * field_map, IN int num_of_records, IN bool * record_map){
    error_code_t return_value = ERROR_CODE_UNINTIALIZED;
    field_t * fields = NULL;
    record_field_t * record = NULL;
    int error_check = 0;
    int num_of_valid_fields = 0;
    int num_of_valid_records = 0;
    int record_len = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int new_table_fd = 0;

    for(i=0; i<num_of_fields; i++){
        if(field_map[i]){
            num_of_valid_fields++;
        }
    }

    for(i=0; i<num_of_records; i++){
        if(record_map[i]){
            num_of_valid_records++;
        }
    }

    fields = calloc(num_of_valid_fields, sizeof(field_t));
    if(NULL == fields){
        perror("WRITE_TABLE: Calloc error");
        return_value = ERROR_CODE_COULDNT_ALLOCATE_MEMORY;
        goto cleanup;
    }
    memset(fields, 0, num_of_valid_fields * sizeof(field_t));

    for(i=0; i<num_of_fields; i++){
        if(field_map[i]){
            return_value = get_field(table_fd, &(fields[j]), i);
            if(ERROR_CODE_SUCCESS != return_value){
                goto cleanup;
            }
            j++;
        }
    }

    errno = 0;
    new_table_fd = open(file_name, O_RDWR | O_TRUNC | O_EXCL | O_CREAT, 0666);
    if(-1 == new_table_fd && EEXIST != errno){
        perror("WRITE_TABLE: Open error");
        return_value = ERROR_CODE_COULDNT_OPEN;
        goto cleanup;
    }
    else if(EEXIST == errno){
        return_value = ERROR_CODE_EXISTS;
        goto cleanup;
    }

    error_check = write(new_table_fd, magic, magic_len);
    if(-1 == error_check){
        perror("WRITE_TABLE: Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    error_check = write(new_table_fd, &num_of_valid_fields, sizeof(num_of_valid_fields));
    if(-1 == error_check){
        perror("WRITE_TABLE: Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    error_check = write(new_table_fd, fields, num_of_valid_fields * sizeof(field_t));
    if(-1 == error_check){
        perror("WRITE_TABLE: Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    error_check = write(new_table_fd, &num_of_valid_records, sizeof(num_of_valid_records));
    if(-1 == error_check){
        perror("WRITE_TABLE: Write error");
        return_value = ERROR_CODE_COULDNT_WRITE;
        goto cleanup;
    }

    record_len = get_len_of_record(table_fd, false);
    if(-1 == record_len){
        return_value = ERROR_CODE_COULDNT_GET_LEN_OF_RECORD;
        goto cleanup;
    }

    for(i=0, j=0; i<num_of_records; i++){
        if(record_map[i]){
            return_value = get_record(table_fd, &record, i, false);
            if(ERROR_CODE_SUCCESS != return_value){
                goto cleanup;
            }

            record->record_num = j;
            j++;

            error_check = write_record_fields(new_table_fd, num_of_fields, record, false);
            if(-1 == error_check){
                perror("WRITE_TABLE: Write error");
                return_value = ERROR_CODE_COULDNT_WRITE;
                goto cleanup;
            }

            for(k=0; k<num_of_fields; k++){
                free(record[k].data);
            }

            free(record);
        }
    }

    return_value = ERROR_CODE_SUCCESS;

cleanup:
    if(NULL != fields){
        free(fields);
    }

    return return_value;
}
