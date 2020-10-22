#include "dafaq.h"

char * magic = "DaFAQ";
char * extension = ".dfq";
char * operations[] = {"==", "!=", ">", "<", ">=", "<="};
int num_of_operations = sizeof(operations) / sizeof(operations[0]);
int magic_len = 0;
int extension_len = 0;

int main(int argc, char ** argv){
    char * database_name = "DaFAQ Database";
    
    int error_check = 0;
    record_field ** record = NULL;
    int fd = 0;
    int sort_fd = 0;
    int record_numbers = 0;
    int num_of_fields = 0;
    int i = 0;
    int j = 1;
    char target[] = "zzzz";
    int itarget = 20;
    bool * map = NULL;

    magic_len = strnlen(magic, STRING_LEN);
    extension_len = strnlen(extension, STRING_LEN);

    if(2 == argc){
        database_name = argv[1];
    }

    print_menu();
#if 0
    create_database(database_name);
    create_table_interface("Table.dfq");
    error_check = switch_record_interface("Table.dfq", -1);
    switch_field_interface("Table.dfq");
    error_check = switch_record_interface("Table.dfq", -1);
    error_check = switch_record_interface("Table.dfq", -1);
    
    
    //poop("test.dfq", "table2.md", true);
    diarrhea(".", NULL);

    fd = open("Table.dfq", O_RDONLY);
    sort_fd = open("Table.dfq.srt", O_RDWR);
    if(-1 == sort_fd){
        perror("MAIN: Open error");
    }

/*    ftruncate(sort_fd, 0);
    write(sort_fd, (char *)&i, sizeof(int));
    i = 0;
    write(sort_fd, (char *)&i, sizeof(int));
    quicksort_record_fields(fd, sort_fd, 0, true);
    sleep(10);*/
    /*quicksort_record_fields(fd, sort_fd, 1, true);
    i = binary_search_sort_file(fd, sort_fd, target, 0, &map);
    printf("%i: %p\n", i, map);
    for(j=0; j<3; j++){
        printf("%i\n", map[j]);
    }*/
    query_interface("Table.dfq", "Table.dfq.srt");

    //get_record_field(fd, &record, 0, 1, false);
    //printf("\nDATA LEN: %i\nFIELD INDEX: %i\nFIELD OFFSET: %i\nRECORD NUM: %i\nDATA: %s\n", record.data_len, record.field_index, record.record_field_offset, record.record_num, record.data);
#endif
}
