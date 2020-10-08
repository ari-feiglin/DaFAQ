#include "dafaq.h"

int edit_record_interfaces(char * name, int record_num){
    int num_of_fields = 0;
    int num_of_records = -1;
    int error_check = 0;
    int fd = -1;
    int i = 0;
    int int_input = 0;
    int byte_input = 0;
    bool has_mask = false;
    bool valid_mask = false;
    field * fields = NULL;
    int * input_lens = NULL;
    char * input = NULL;
    char ** field_inputs = NULL;

    fd = open(name, O_RDWR);
    if(-1 == fd){
        perror("EDIT_RECORD_INTERFACE: Open error");
        goto cleanup;
    }

    num_of_fields = get_fields(fd, &fields, false);
    if(-1 == num_of_fields){
        goto cleanup;
    }

    field_inputs = calloc(num_of_fields, sizeof(char *));
    input_lens = calloc(num_of_fields, sizeof(int));

    print_color("\n~~~EDIT RECORD~\n\n", BG_MAGENTA, BOLD, B_GREEN, RESET);
    for(i=0; i<num_of_fields; i++){
        has_mask = false;
        print_color("~~", B_GREEN, BOLD);
        
        switch(fields[i].data_len){
            case STRING: 
                if(0 != fields[i].input_mask[0]){
                    has_mask = true;
                    printf("%s (STRING - INPUT MASK: %s): ", fields[i].name, fields[i].input_mask); 
                }
                else{
                    printf("%s (STRING): ", fields[i].name);
                }
                break;

            case INT: printf("%s (INT): ", fields[i].name); break;
            case CHAR: printf("%s (BYTE VALUE): ", fields[i].name); break;

            default:
                print_color("~~INVALID DATA TYPE!~\n", B_RED, BOLD, RESET);
                goto cleanup;
        }

        print_color("~", RESET);

        error_check = get_raw_input(NULL, &input);
        if(-1 == error_check){
            goto cleanup;
        }

        switch(fields[i].data_len){
            case STRING:
                if(0 != fields[i].input_mask[0]){
                    valid_mask = valid_input(input, fields[i].input_mask);
                    if(-1 == valid_mask){
                        goto cleanup;
                    }
                    if(0 == valid_mask){
                        print_color("~~INVALID INPUT~\n", B_RED, BOLD, RESET);
                        i--;
                        continue;
                    }
                }
                input_lens[i] = strnlen(input, STRING_LEN);
                field_inputs[i] = malloc(input_lens[i]);
                if(NULL == field_inputs[i]){
                    perror("EDIT_RECORD_INTERFACE: Malloc error");
                    goto cleanup;
                }
                memcpy(field_inputs[i], input, input_lens[i]);
                break;
            
            case INT:
                input_lens[i] = sizeof(int_input);
                field_inputs[i] = malloc(input_lens[i]);

                int_input = (int)strtol(input, NULL, 10);
                memcpy(field_inputs[i], &int_input, sizeof(int_input));
                break;
            
            case CHAR:
                input_lens[i] = sizeof(byte_input);
                field_inputs[i] = malloc(input_lens[i]);

                byte_input = (char)strtol(input, NULL, 10);
                memcpy(field_inputs[i], &byte_input, sizeof(byte_input));
                break;

            default:
                print_color("~~INVALID DATA TYPE~\n", B_RED, BOLD, RESET);
                goto cleanup;
                break;      //:( maybe one day...
        }
    }

    num_of_records = switch_record(fd, record_num, input_lens, field_inputs);
    if(-1 == num_of_records){
        goto cleanup;
    }

cleanup:
    if(NULL != field_inputs){
        for(i=0; i<num_of_fields; i++){
            if(NULL != field_inputs[i]){
                free(field_inputs[i]);
            }
        }
        free(field_inputs);
    }
    if(NULL != input_lens)
        free(input_lens);
    if(NULL != fields)
        free(fields);
    if(NULL != input)
        free(input);
    close(fd);

    return num_of_records;
}
