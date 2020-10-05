# **STRUCTURE**  <br />
A Database of type Database Find and Query (DaFAQ) language is split into files of tables. These tables follow a format of the follwing:
### MAGIC <br />
At the beginning of each file is a magic sequence denoting it as a valid DaFAQ file <br />
The magic sequence is DaFAQ (0x48 0x44 0x61 0x46 0x41 0x51 0x6c)

### HEADER <br />
The header of a DaFAQ file consists of the following: <br />
* **NUMBER OF FIELDS**: An integer denoting the number of fields in the field array
* **FIELD ARRAY**: An array of fields, whose elements are field structures. Field structures defined as: <br />
```c
typedef struct field{
    char field_name[256];
    int data_len;
} 
```
### BODY <br />
The body of the file consists of the following:
* **NUMBER OF RECORDS**: The number of records in the body
* **RECORD ARRAY**: The body of the file is an array of records. <br />
