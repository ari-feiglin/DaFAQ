#include "dafaq.h"

char * magic = "DaFAQ";
char * extension = ".dfq";
char * operations[] = {"==", "!=", ">", "<", ">=", "<="};
int num_of_operations = sizeof(operations) / sizeof(operations[0]);
int magic_len = 0;
int extension_len = 0;

int main(int argc, char ** argv){
    magic_len = strnlen(magic, STRING_LEN);
    extension_len = strnlen(extension, STRING_LEN);

    print_menu();
}
