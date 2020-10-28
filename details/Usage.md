# <u>**USAGE OF THE DaFAQ DATABASE SYSTEM**</u>
This is how to navigate the DaFAQ menu and interact with databases.

## **WHAT IS DaFAQ?**
DaFAQ is an open-source Nearly Useless Database Engine (NUDE). It was created because some dumb kid wanted to make a database system to show to his teachers in the hopes of (possibly) not having to take a class on databases. Hasn't worked yet. 

## **WHAT IS A DaFAQ DATABASE?**
A DaFAQ Database is nothing more than a directory in your file system. In this directory are two types of files: table files and sort files. These store data on tables that you create. 

## <u>**THE MENU:**</u>
When running DaFAQ, if no changes have been made to make a different (better) interface or whatever, you will be met with a menu. The following is alist of every option in the menu, and a detailed documentation on how to use it.  
When you select an option (with enter) you can go back to the menu without completing the option by typing in "quit" (no quotations). After the option finishes (either by error, typing quit, or just finishing), you will be asked to type q to return to the menu. This is so that you can view messages given by the system. If an error occurred, another message will pop up (in red), telling you an error had occurred. Review previous error messages and determine if you would like to continue or terminate the process. If you would like to terminate, enter q. 

* ### **CREATE DATABASE:**
  This is the first option, and you guessed it, it's for creating databases. Like I said before, DaFAQ databases are nothing more than directories. So this will create a new directory with the given name and enter it. If the directory already exists, it will not be removed, it will just be entered and a message will pop up telling you that no directory was created. If you would like to navigate to a directory, just enter the path.

* ### **CREATE NEW TABLE**
  This allows you to create a new table. Any existing table will be removed. As I explained earlier, a table is comprised of two files: a table files, and a sort file. This option creates and initializes both. We'll get to sort files later. A table file contains all the data held in a table. When you create one with this option, it will ask you for a table name. In order for <u>Diarrhea</u> to work on the table, give the table the extension .dfq , but this is not necessary. to see more details on table files, view details/structure. 
  After you give the system the name of the table file you want to create, you will need to give metadata on the fields of the table. The metadata consits of:
  * A field name : The name of the field
  * The datatype of the field: Either "string", "int", "char", or "boolean". (Boolean is redundant and doesn't provide anything different from a char datatype)
  * An input mask: **Only if the datatype is a string** this will allow you to filter input. View "INPUT MASKS" later on in this documentation.
  If you want to stop inputting fields, input "quit" when it asks for field name. All input can be changed later, don't worry.

* ### **EDIT TABLE:**
  This allows you to edit fields in an already existing table. This will do its best to keep the old data, in its new form, but will not always succeed. For instance, converting from a string with letters or too large of a value (for instance "hello78", "12345678912345412345673456234561356784") will result in some other value. Same goes with conversion from an int to a char (given that the int is too large). 
  You will be told to give the following:
  * The name of the table file
  * Field number: The index of the field you want to change
  * A field name : The name of the new field
  * The datatype of the field: Either "string", "int", "char", or "boolean". (Boolean is redundant and doesn't provide anything different from a char datatype)
  * An input mask: **Only if the datatype is a string** this will allow you to filter input. View "INPUT MASKS" later on in this documentation.
  
* ### **CREATE NEW RECORD:**
  This creates a new record based on the tables field. You will be asked to give the table file name and input for every field.

* ### **EDIT RECORD:**
  This allows you to edit an already existing record. It asks for the record index and data on every field.

* ### **POOP TABLE:**  
  This prints out a table as, you guessed it, a table! All you need to input is the table file name.

* ### **DIARRHEA:**
  This poops every table in the current database/directory.

* ### **EXECUTE QUERY:**
  This allows you to filter values in a table. View "Simple Queries" for more detail on how it works and usage.

And that's it. The following are more in-depth explanantions on various things.

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
