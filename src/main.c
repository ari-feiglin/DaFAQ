#include "dafaq.h"

int main(int argc, char ** argv){
    char * database_name = "DaFAQ Database";
    magic = "DaFAQ";
    extension = ".dfq";

    magic_len = strnlen(magic, STRING_LEN);
    extension_len = strnlen(extension, STRING_LEN);

    if(2 == argc){
        database_name = argv[1];
    }

    create_database(database_name);
    create_table_interface("Table.dfq");
    edit_record_interfaces("Table.dfq", -1);
    edit_record_interfaces("Table.dfq", -1);

    //poop("test.dfq", "table2.md", true);
    diarrhea(".", NULL);

}
