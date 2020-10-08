#include "dafaq.h"
#include <dirent.h>

int poop(char * table_name, char * dump_file, bool truncate){
    field * fields = NULL;
    int error_check = 0;
    int num_of_fields = 0;
    int num_of_records = -1;
    int fd = 0;
    int dump_fd = 0;
    int record = 0;
    int current_field = 0;
    int int_data = 0;
    int name_len = 0;
    bool is_valid = false;
    bool print_table = true;
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
        print_table = false;
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

    if(!print_table){
        sprintf(print_text, "\n~`~%s:~\n\n", table_name);
        print_color(print_text, BG_B_WHITE, FG,0,255,0, BOLD, RESET);       //Print the table name
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

        if(!print_table){
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

            if(!print_table){       //If not dumping table
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

        if(print_table){
            print_text[name_len-1] = '\n';

            error_check = write(dump_fd, print_text, name_len);    //Write print_text to dump file
            if(-1 == error_check){
                perror("POOP: Write error");
                num_of_records = -1;
                goto cleanup;
            }
        }
        
    }
    if(print_table){
        error_check = write(dump_fd, "<br />\n\n", strlen("<br />\n\n"));
        if(-1 == error_check){
            perror("POOP: Write error");
            num_of_records = -1;
            goto cleanup;
        }
    }
    else{
        printf("\n");
    }


cleanup:
    close(dump_fd);
    if(fields)
        free(fields);
    if(print_text)
        free(print_text);

    return num_of_records;
}

int diarrhea(char * database_name, char * dump_name){
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

int execute_query(query input_query){
    int i = 0;
    int j = 0;
    int num_of_fields = 0;
    int fds[MAX_CALLS] = {0};
    int * get_fields_indexes[MAX_CALLS] = {0};
    field * fields = NULL;

    if(FROM != input_query.queryers[0] || GET != input_query.queryers[1]){
        print_color("~~ERROR: INVALID QUERY FORMAT~\n", B_RED, BOLD, RESET);
        goto cleanup;
    }

    for(i=0; i<MAX_CALLS; i++){
        if(0 != input_query.tables[i][0]){      //Check that the table file path is initialized
            fds[i] = open(input_query.tables[i], O_RDONLY);     //Open table file
            if(-1 == fds[i]){
                perror("EXECUTE_QUERY: Open error");
                goto cleanup;
            }
        }
    }

cleanup:
    return 0;
}
