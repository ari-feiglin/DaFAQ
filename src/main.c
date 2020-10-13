#include "dafaq.h"

int main(int argc, char ** argv){
    char * database_name = "DaFAQ Database";
    magic = "DaFAQ";
    extension = ".dfq";
    int error_check = 0;
    record_field ** record = NULL;
    int fd = 0;
    int record_numbers = 0;
    int num_of_fields = 0;
    int i = 0;
    int j = 0;

    magic_len = strnlen(magic, STRING_LEN);
    extension_len = strnlen(extension, STRING_LEN);

    if(2 == argc){
        database_name = argv[1];
    }

    create_database(database_name);
/*    create_table_interface("Table.dfq");
    switch_field_interface("Table.dfq");
    error_check = switch_record_interface("Table.dfq", -1);
    error_check = switch_record_interface("Table.dfq", -1);
    error_check = switch_record_interface("Table.dfq", -1);*/
    
    //poop("test.dfq", "table2.md", true);
    diarrhea(".", NULL);

    fd = open("Table.dfq", O_RDONLY);
/*    num_of_fields = get_num_of_fields(fd, false);
    record_numbers = get_all_records(fd, &record, false);
    printf("%i\n", record_numbers);
    for(i=0; i<record_numbers; i++){
        for(j=0; j<num_of_fields; j++){
            printf("\nDATA LEN: %i\nFIELD INDEX: %i\nFIELD OFFSET: %i\nRECORD NUM: %i\nDATA: %s\n", record[i][j].data_len, record[i][j].field_index, record[i][j].record_field_offset, record[i][j].record_num, record[i][j].data);
        }
    }*/

    quicksort_record_fields(fd, "Hallo", 1);

    //get_record_field(fd, &record, 0, 1, false);
    //printf("\nDATA LEN: %i\nFIELD INDEX: %i\nFIELD OFFSET: %i\nRECORD NUM: %i\nDATA: %s\n", record.data_len, record.field_index, record.record_field_offset, record.record_num, record.data);
}
