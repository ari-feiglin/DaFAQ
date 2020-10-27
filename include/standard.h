#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE (1024)

#define IN
#define OUT

#define MAX(x, y) ((x > y) ? x : y)
#define MIN(x, y) ((x > y) ? y : x)

void swap(int * a, int * b);
int get_raw_input(char * prompt, char ** input);
int lower(char * string, int len);
int upper(char * string, int len);
int change_echo(bool on);
int center_text(char * text, char ** centered_text, int len);
int rect_text(char * text, char ** rectangled_text, int len);
int ntos(char * number_data, char *string, int num_of_numbers, int num_len);
void simple_rect_text(int len);
void simple_center_text(int string_len, int len);
