Iterate through every record and put every field in that record (record_field) into a struct:
```c
typedef struct record_field{
    off_t record_offset;
    off_t record_field_offset;
    char * data;
    int field_index;
}record_field;
```

Create an array for each field of type record_field * field_<name>, and then quicksort each one. 
To find a record of some type (like if record = "Some data") use a binary search to find the record_field that has that data (if more must be found, search for more) and the record_field into some array. Now all of the necessary data (the data, the record, the field) is there.
