There will be a struct:
```c
typedef struct query{
    queryers queryers[NUM_OF_QUERYERS];
    functions functions[MAX_CALLS];
    char tables[MAX_CALLS][STRING_LEN];
    char get_fields[MAX_CALLS][STRING_LEN];
    char at_fields[MAX_CALLS][STRING_LEN];
    char conditions[MAX_CALLS][STRING_LEN];
};
```
The queryers and functions are defined as:
```c
typedef enum queryers {FROM, GET, MERGE, AT, WHERE}queryers;
typedef enum functions {FIELD, AVERAGE, SUM, AMOUNT}functions;
```

For a query, the interpreter parses the query and creates a query struct for it. The list of queryers is filled out. For example:
```DaFAQ
FROM table1.dfq
GET AVERAGE(field1) field2
WHERE field1 > field2
```
The structure would be:
```c
struct query{
    queryers = {FROM, GET, WHERE};
    functions = {AVERAGE, FIELD};
    tables = {"table1.dfq"};
    get_fields = {"field1", "field2"};
    at_fields = {};
    conditions = {"field1>field2"};
}
```
To disassemble the query, the program would do the following steps:
1. If the queryer is a FROM, then it looks at the first table in tables and opens it.
2. If the queryer is a GET, it looks at the functions array and iterates over it, if the current element is FIELD, then it knows to get the field in get_fields at the same index. If it is something else, it knows to perform that function on the field at that index.
3. If the queryer is a MERGE, then it will open the next table in the tables array (if I can, I'll try and implement a way to merge multiple tables)
4. If the queryer is an AT, it'll know to get the fields in the at_fields and will use them when looking through a database to find data that corresponds to the same data in the field of some record.
5. If the queryer is a WHERE, it'll pass every field that passes the rest through a condition. (Or maybe first, I don't know)