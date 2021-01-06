#include "dafaq.h"

char * magic = "DaFAQ";
char * extension = ".dfq";
char * operations[] = {"==", "!=", ">", "<", ">=", "<="};
int num_of_operations = sizeof(operations) / sizeof(operations[0]);
int magic_len = 0;
int extension_len = 0;

int main(int argc, char ** argv){
    int error_check = 0;
    query_t query = {0};

    if(argc > 1){
        error_check = mkdir(argv[1], 0);
        if(-1 == error_check){
            if(EEXIST == errno){
                error_check = chdir(argv[1]);
                if(-1 == error_check){
                    perror("MAIN: Chdir error");
                    error_check = errno;
                    goto cleanup;
                }
            }
            else{
                perror("MAIN: Mkdir error");
                error_check = errno;
                goto cleanup;
            }
        }
        else{
            error_check = chmod(argv[1], 0775);
            if(-1 == error_check){
                perror("MAIN: Chmod error");
                error_check = errno;
                goto cleanup;
            }

            error_check = chdir(argv[1]);
            if(-1 == error_check){
                perror("MAIN: Chdir error");
                error_check = errno;
                goto cleanup;
            }
        }
    }
    magic_len = strnlen(magic, STRING_LEN);
    extension_len = strnlen(extension, STRING_LEN);

    print_menu();

cleanup:
    exit(error_check);
}
