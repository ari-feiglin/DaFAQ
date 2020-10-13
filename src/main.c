#include "dafaq.h"

int main(int argc, char ** argv){
    char * database_name = "DaFAQ Database";
    magic = "DaFAQ";
    extension = ".dfq";
    int error_check = 0;
    record_field ** records = NULL;

    magic_len = strnlen(magic, STRING_LEN);
    extension_len = strnlen(extension, STRING_LEN);

    if(2 == argc){
        database_name = argv[1];
    }

    create_database(database_name);
    create_table_interface("Table.dfq");
    switch_field_interface("Table.dfq");
    error_check = switch_record_interface("Table.dfq", -1);
    error_check = switch_record_interface("Table.dfq", -2);
    
    //poop("test.dfq", "table2.md", true);
    diarrhea(".", NULL);
}
