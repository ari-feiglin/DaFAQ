# <ins>**USAGE OF THE DaFAQ DATABASE SYSTEM**</ins>
This is a description of the how to use the database system once the carpet code has been written (by carpte code, I mean the code that uses the framework provided hereby the functions I wrote), not a description of how to use each function. That will be given in some other place.

### **SIMPLE QUERIES:** <br />
**The process:**
Queries can be executed by calling the query_interface command or binary_search_sort_file (not recommended). query_interface is a mask for binary_search_sort_file, and it also deals with errors regarding to non-valid sort files. The steps for querying a table is as follows:
1. A sort file is created, if one does not exist already. A sort file contains a list of the sorted records in a table.
2. binary_search_sort_file is called and returns a byte map corresponding to the valid records
3. * If the function returns an error indicating that the desired field does not exist in the sort file, the desired field is sorted and the resulting sorted list is appended to the sort file (if the field had not existed in the sort file)
   * If the function returned an error indicating that the sort file is out of date, the sort file is truncated, and rewritten, appending the desired sorted field.
   And step 2 is repeated.

**Usage of query_interface:**
When running the query_interface function, you will have to input the following:
* The Field Name of the field you would like to put a filter on
* The Target Data, the data that you are basing you filter on
* The Operation, which is one of:
  * `==` - equal to
  * `!=` - not equal to
  * `>=` - equal to or greater than
  * `<=` - equal to or less than
  * `>` - greater than
  * `<` - less than  
  
  The condition is:  
  `$(Field Name) $(Operation) $(Target Data)`, so for instance, if:  
  `Field Name: ` Field 1  
  `Target Data: ` 314  
  `Operator: ` <=  
  Then the query will print every record where the field whose name is "Field 1" has a value of equal to or less than 314 (eg. 0, 300, 278).

### **INPUT MASKS:** <br />
An input mask is a series of characters (a string, if you will) that denotes how input must be given to a field. <br />
Say I had a field "Date" and I wanted it to be only have the format Mon day, year, like "Oct 07, 2020". I could create an input mask "LLLSNN,SNNNN". <br />
When a field is of data type STRING, the program will ask for an input mask. <br />
*This step can be skipped by typing "null" or any invalid input mask.* <br />

**Characters in an input mask:**
* c - OPTIONAL: any valid ASCII character: number, letter, or symbol.
* C - any valid ASCII character (save a space)
* n - OPTIONAL: any number
* N - any number
* l - OPTIONAL: any english letter
* L - any english letter
* \* - OPTIONAL: any symbol
* @ - any symbol (save a space)
* S - must be a space
* '/', '.', ',' - must be one of these characters
* e - end of input mask (any input past this will be considered valid input). Not including an e limits the size of input to the size given by the input mask.
    If the e isn't at the end of the input mask, all subsequent characters will be ignored.

Looking at our example input mask "LLLSNN,SNNNN", we can see that we must input three letters, then a space, then two numbers, a comma, a space, and then four numbers. <br />
