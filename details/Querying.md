## <ins>**Sort Files**</ins>
Sort files are files that consist of the sorted indexes of records based off of the value of the data in one of their fields. This allows the queryer to quickly find values that it is looking for, making the process much faster. The structure of the sort file is:

< Number of Records > < Number of fields written > < Section 1 > < Section 2 > < Section 3 > ...

The structure of a section in a sort file is as follows (each X is a byte, ignore spaces):

```
X X X X  X X X X  X X X X  X X X X  ...  
|_____|  |_____|  |_____|  |_____| 
   |        |________|________|  
   |                 ⎣_________________Record indexes (A list of Number of records members, each one is 4 bytes)  
   |        
   ⎣_____________________________________________Field index (4 bytes)
```

In order to ensure that the sort file stays usable and up to date (at least when necessary), whenever a record is changed or appended, the Number of records at the beginning of the file is overwritten with NUL bytes. If the queryer detects this, it will rewrite the sort file.
The binary search algorithm returns a byte array of the indexes of the records whose data in some field is equal to some input data. The reason it does this instead of returning an array of record_fields is beacuse the array of indexes it returns has a number of record elements, and this can potentially be very data consuming for large databases. The reason for such a large array is to allow easy sorting. A 1 is inserted into the byte array in the index of the valid record (ie. if record 8 is valid, index 8 will have a 1) and the rest of the array (that corresponds to records that don't have the target data) have a value of 0. This allows the queryer to quickly tell if multiple conditions were met (ie. given tthese two arrays of valid records that passed two conditions: {0, 1, 0, 0, 1, 1} and {1, 1, 0, 1, 1, 0} it can be quickly determined that only records 1 and 4 are valid)