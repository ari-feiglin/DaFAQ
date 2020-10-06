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
    create_table("table2.dfq");
    add_record("table2.dfq");
    switch_field("table2.dfq", "NEW FIELD", STRING, "n@nn", -1);
    add_record("table2.dfq");

    poop("table2.dfq");
    diarrhea(".");

}
