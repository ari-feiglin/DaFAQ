#include "standard.h"
#include <termios.h>
#include <unistd.h>
#include <math.h>

struct termios attributes;

//This file includes basic/standard functions (Here raw_input and lower)

void swap(int * a, int * b){
    int temp = *a;

    *a = *b;
    *b = temp;
}

/**
 * @brief: Gets raw input
 * @param[IN] prompt: The prompt to print to indicate that it is waiting for user input.
 * @param[OUT] input: A buffer to fill with user input
 * 
 * @return: The number of bytes that the user input
 * @notes: Since this is _raw_ input, the input will be a string (or array of characters). It is up to 
 *         the caller to change the input acccording to whatever datatype is needed
 */
int get_raw_input(IN char * prompt, OUT char ** input){
    size_t size = 0;
    int bytes_read = 0;
    int i = 0;

    if(NULL != prompt){
        printf("%s", prompt);
    }
    if(NULL != *input){
        free(*input);
        *input = NULL;
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

/**
 * @brief: Makes all english letters in a string lowercase
 * @param[OUT] string: The string to operate on (cannot be NULL for obvious reasons)
 * @param[IN] len: The length of the string (only matters if len is 1, denoting a character)
 * 
 * @return: The length of the string
 * @notes: Any input of len other than 1 will not matter, and will be discarded by the function
 */
int lower(OUT char * string, IN int len){
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

/**
 * @brief: Makes all english letters in a string uppercase
 * @param[OUT] string: The string to operate on (cannot be NULL for obvious reasons)
 * @param[IN] len: The length of the string (only matters if len is 1, denoting a character)
 * 
 * @return: The length of the string
 * @notes: Any input of len other than 1 will not matter, and will be discarded by the function
 */
int upper(OUT char * string, IN int len){
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

/**
 * @brief: Turns on or off ECHO in a terminal
 * @param[IN] on: True if ECHO should be turned on, false for off
 * 
 * @return: 0 on success, else -1
 */
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

/**
 * @brief: Creates a new string of spaces and places the old string in the middle
 * @param[IN] text: The old string to center
 * @param[OUT] centered_text: A pointer to a string (preferrably a NULL pointer) that will house
 *                            the centered text.
 * @param[IN] len: The length of the centered text
 * 
 * @returns: 0 on success, else -1
 * @notes: If len is less than the string length of text, text will be cut off
 */
int center_text(IN char * text, OUT char ** centered_text, IN int len){
    int return_val = -1;
    int text_len = 0;
    int delay_len = 0;

    if(NULL != *centered_text){
        free(*centered_text);
        *centered_text = NULL;
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
    return_val = 0;

cleanup:
    return return_val;
}

/**
 * @brief: Creates a new string of spaces and places the old string at the beginning
 * @param[IN] text: The old string to rectangle-ify
 * @param[OUT] rectangled_text: A pointer to a string (preferrably a NULL pointer) that will house
 *                              the rectangled text.
 * @param[IN] len: The length of the rectangled text
 * 
 * @returns: 0 on success, else -1
 * @notes: If len is less than the string length of text, text will be cut off
 */
int rect_text(char * text, char ** rectangled_text, int len){
    int return_val = -1;
    int text_len = 0;

    if(NULL != *rectangled_text){
        free(*rectangled_text);
        *rectangled_text = NULL;
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

/**
 * @brief: Converts a byte array of numbers to a string
 * @param[IN] number_data: The byte array housing the numbers
 * @param[OUT] string: A pointer to the string that will house the text-form numbers
 * @param[IN] num_of_numbers: The number of numbers in the byte array
 * @param[IN] num_len: The number of bytes each number consists of (4 for int, 1 for char)
 * 
 * @return: The length of the new string
 * @notes: For some reason I didn't make it so that string can be NULL, but I might change that later
 */
int ntos(IN char * number_data, OUT char ** string, IN int num_of_numbers, IN int num_len){
    char byte_num = 0;
    int int_num = 0;
    int error_check = 0;
    int string_len = 0;
    int i = 0;
    char temp_string[BUFFER_SIZE] = {0};
    
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
               string_len += floor(log10(int_num)+1);
            }
            sprintf(temp_string, "%s%i", temp_string, int_num);
        }
        else{
            memcpy(&byte_num, number_data+i, num_len);

            if(0 == byte_num){
               string_len++;
            }
            else{
               string_len += floor(log10(byte_num)+1);
            }

            sprintf(temp_string, "%s%i", temp_string, byte_num);
        }
    }

    if(NULL != *string){
        free(*string);
    }
    *string = malloc(string_len+1);
    if(NULL == *string){
        perror("NTOS: Malloc error");
        string_len = -1;
        goto cleanup;
    }
    
    memcpy(*string, temp_string, string_len);
    (*string)[string_len] = 0;

cleanup:
    return string_len;
}

/**
 * @brief: Prints a line of spaces and returns to the beginning of the line
 * @param[IN] len: The length of the rectangle
 */
void simple_rect_text(IN int len){
    int i = 0;

    for(i=0; i<len; i++){
        printf(" ");
    }
    
    printf("\r");
}

/**
 * @brief: Prints a line of spaces and goes to a place such that a string of string_len will be centered
 * @param[IN] string_len: The length of the string that will be printed
 * @param[IN] len: The length of the line
 */
void simple_center_text(IN int string_len, IN int len){
    int i = 0;

    for(i=0; i<len; i++){
        printf(" ");
    }
    printf("\r\033[%dC", (len - string_len)/2);
}
