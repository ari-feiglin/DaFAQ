#include "standard.h"
#include <termios.h>
#include <unistd.h>
#include <math.h>

struct termios attributes;

//This file includes basic/standard functions (Here raw_input and lower)

int get_raw_input(char * prompt, char ** input){
    size_t size = 0;
    int bytes_read = 0;
    int i = 0;

    if(NULL != prompt){
        printf("%s", prompt);
    }

    bytes_read = getline(input, &size, stdin);
    if(-1 == bytes_read){
        perror("Getline error");
    }
    for(i=0; i<bytes_read; i++){
        if('\n' == (*input)[i]){
            (*input)[i] = 0;
        }
    }

    return bytes_read;
}

int lower(char * string, int len){
    int i = 0;

    if(1 != len){
        len = strnlen(string, BUFFER_SIZE);
        if(-1 == len){
            perror("Strnlen function error");
            goto cleanup;
        }
    }

    for(i=0; i<len; i++){
        if('A' <= string[i] && string[i] <= 'Z'){
            string[i] += 32;
        }
    }

cleanup:
    return len;
}

int upper(char * string, int len){
    int i = 0;

    if(1 != len){
        len = strnlen(string, BUFFER_SIZE);
        if(-1 == len){
            perror("Strnlen function error");
            goto cleanup;
        }
    }

    for(i=0; i<len; i++){
        if('a' <= string[i] && string[i] <= 'z'){
            string[i] -= 32;
        }
    }

cleanup:
    return len;
}

int change_echo(bool on){
    int error_check = 0;

    error_check = tcgetattr(STDIN_FILENO, &attributes);
    if(-1 == error_check){
        perror("Failed to retrieve terminal attributes");
        goto cleanup;
    }

    if(on){
        attributes.c_lflag |= (ECHO | ICANON);
    }
    else{
        attributes.c_lflag &= ~(ECHO | ICANON);
    }
    error_check = tcsetattr(STDIN_FILENO, TCSANOW, &attributes);
    if(-1 == error_check){
        perror("Failed to set terminal attributes");
        goto cleanup;
    }

cleanup:
    return error_check;
}

int center_text(char * text, char ** centered_text, int len){
    int return_val = -1;
    int text_len = 0;
    int delay_len = 0;

    if(NULL != *centered_text){
        free(*centered_text);
    }
    *centered_text = malloc(len+1);
    if(NULL == centered_text){
        perror("Malloc error");
        goto cleanup;
    }
    memset(*centered_text, ' ', len);
    (*centered_text)[len] = 0;

    text_len = strnlen(text, BUFFER_SIZE);
    if(-1 == text_len){
        perror("Strnlen function error");
        goto cleanup;
    }

    delay_len = (len-text_len)/2;

    memcpy(*centered_text+delay_len, text, MIN(text_len, len));

cleanup:
    return return_val;
}

int rect_text(char * text, char ** rectangled_text, int len){
    int return_val = -1;
    int text_len = 0;

    if(NULL != *rectangled_text){
        free(*rectangled_text);
    }
    *rectangled_text = malloc(len+1);
    if(NULL == rectangled_text){
        perror("Malloc error");
        goto cleanup;
    }
    memset(*rectangled_text, ' ', len);
    (*rectangled_text)[len] = 0;

    text_len = strnlen(text, BUFFER_SIZE);
    if(-1 == text_len){
        perror("Strnlen function error");
        goto cleanup;
    }

    memcpy(*rectangled_text, text, MIN(text_len, len));

cleanup:
    return return_val;
}

int ntos(char * number_data, char * string, int num_of_numbers, int num_len){
    char byte_num = 0;
    int int_num = 0;
    int error_check = 0;
    int string_len = 0;
    int i = 0;
    
    if(num_len != sizeof(int) && num_len != sizeof(char)){
        printf("NTOS: num_len must be of size 4 or 1 (int or char)\n");
        string_len = -1;
        goto cleanup;
    }

    for(i=0; i<num_of_numbers*num_len; i+=num_len){
        if(sizeof(int) == num_len){
            memcpy(&int_num, number_data+i, num_len);

            if(0 == int_num){
               string_len++;
            }
            else{
               string_len += floor(log10(byte_num)+1);
            }
            
            sprintf(string, "%s%i", string, int_num);
        }
        else{
            memcpy(&byte_num, number_data+i, num_len);

            if(0 == byte_num){
               string_len++;
            }
            else{
               string_len += floor(log10(byte_num)+1);
            }
            printf("%i\n", string_len);

            sprintf(string, "%s%i", string, byte_num);
        }
    }

cleanup:
    return string_len;
}
