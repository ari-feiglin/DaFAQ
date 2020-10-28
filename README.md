# <ins>**DaFAQ Database System**</ins>

The DaFAQ (**Da**tabase **F**ind **A**nd **Q**uery) database system is a work-in-progress system for managing DaFAQ databases. <br />
In the future there may also hopefully be a simple query language for it, too. But that's a big _maybe_. <br />
While I'm making all my wishes, I'll go one step further and wish that this program will be somewhat useful. But I doubt it. <br />
I have very limited knowledge on Databases and whatnot, so this system is probably extremely flawed (as in, not just the code, but the implementation, too).

**IMPORTANT NOTE:** The DaFAQ system is built for Linux, and it may not work for other kernels and OSs. Sorry for the inconvenience.

## <ins>**Version 0.4.0**</ins>
<ins>**v0.4 IS NOT COMPATIBLE WITH WITH TABLES MADE WITH v0.0**</ins><br />
Version 0.4.2 (v0.4.2) was released October 29, 2020

### <ins>**v0.4 CHANGES:**</ins>
A list of features added in version 0.4
* A bug massacre! 
  * Fixed a bug that messed up changes in datatypes of existing records when a field is changed
  * Fixed a bug where the name of fields can be messed up.
  * And a few more...
* Interfaces are now integrated into the "GUI" (Not all error messages will be printed properly. Only the ones that come from interfaces are. I'm uncertain if this will change. If I do change it, error messages will look weird for people calling the functions without interfaces.)
* Changed project structure
* Added usage on how to use the interfaces

### <ins>**USAGE:** </ins>

For information on how to use the interface, look at details/Usage. The following is quick descriptions of various functions in the program:

**Data Management Functions:**
* **create_database** - create a new database directory (a directory for holding tables) if one doesn't exist, and enter it.
* **create_table** - gets an array of fields and the number of fields as input and creates a table file. This will delete any other file with the same name.
* **switch_field** - edits a field in a table (changes name and/or data type). This should be a safe function, as it should update all of the records in the table too. An input of field_num = -1 adds appends a field, an input of data_size = 0 does keeps the size of the field that is being switched, an input of field_name = NULL preserves the field being switched's name.
* **switch_record** - writes a record to the table at the desired location. An input of record_num = -1 appends the record, and any attempt to switch a record past the end of the table appends the record, too. 
* **write_record_fields** - writes given record fields to a given file. It will be implemented more later.

**Table Navigation Functions:**
* **get_num_of_fields** - returns the number of fields in a table (mostly used by get_fields)
* **get_len_of_record** - returns the length of a record (the sum of all of the field's data lengths)
* **get_fields** - returns the number of fields in a table and writes them into a buffer of type `field * fields`.
* **get_num_of_records** - returns the number of records in a table.
* **check_extension** - checks if table file has a valid extension (.dfq)

**Interfaces:**
* **create_table_interface** - the interactive text interface for create_table.
* **switch_record_interface** - the interactive text interface for switch_record.
* **switch_field_interface** - the interactive text interface for switch_field.
* **query_interface** - an interactive text interface for executing queries. It also handles non-up-to-date sort files.

**Database Review Functions:**
* **poop** - prints every record in a table. If one is specified, print the output to a markdown file. (In order to not dump a table, dump_file should be NULL)
* **diahrrea** - poops every table in a database.

**Queries:**
* **get_record_field** - Gets the data from a field in a specified record. This data, and other meadata is stored in a struct of type `struct record_field`
* **get_record** - Gets a record and stores it as an array of record_fields.
* **get_all_records** - Gets very record in a table, and stores it as an array of arrays of record_fields (a record is an array of record_fields, hence an array of records is an array of arrays of record_fields)
* **quicksort_record_fields** - Sorts an input field of a table, and stores the sorted list in a sort file.
* **binary_search_sort_file** - Executes a query and returns a map of valid records.

**Menu:**
Only one of these functions is public. The rest are only utilized by this function.
* **print_menu** - Prints the GUI for interacting with the database system. I haven't vigorously tested this for leaks, so there may be a few.

**Other Functions:**
* **check_magic** - checks if a file has the proper magic sequence in order to be a valid DaFAQ file.
* **check_input_mask** - checks if input mask is valid. More on input masks in details/Usage
* **valid_input** - checks if input is valid according to the input mask

### <ins>**OTHER INFORMATION:**</ins>

The interactive text interfaces I so heavily used in this program rely heavily on the CColor library I wrote. I strongly suggest that you check it out. <br />
The standard.* files are comprised of functions I find useful in many of my projects. 

### <ins>**AN EXPLANATION OF THE LAYOUT OF THIS PROJECT:**</ins>
As you can see, this project is split into a couple of directories:
* In the main directory itself: the Makefile, executable, and README file (for easy access)
* **src** - the source code directory. It is split up into:
    * data_managment - All of the low level functions for editing tables
    * interfaces - Pretty printing fnctions (menu, etc.)
    * querying - Functions for accessing data in tables
    * main - the runner file (main.c) and standard.c
* **include** - the directory where all header files are stored
* **obj** - the directory created by the Makefile for the storage of object files created during the compilation process
* **details** - in the directory itself: details regarding the state and usage of the program. 
    * In the sub directory **archive** is an archive of all the previous READMEs and any other outdated files regarding the state and/or usage of the system for previous versions

### <ins>**The CColor Library:**</ins>  
In order to print colored text, I used the CColor library that I wrote. Here is a link to the remote github repository where its source code can be found, along with a shared object file: https://github.com/ari-feiglin/CColor  
A README containing all relevant information on its usage is provided there.

### <ins>**THE FUTURE/GOALS:** </ins>
Here are some of my ideas for the future:
* A querying language
* Fully comment the project
* More advanced queries
* An API for Java?

### <ins>**NOTES:**</ins>
None of what is in this current version (v0.4.2) is final. Hopefully the finished version will be drastically different than this one. My end goal is to create a semi-useful system for creating and managing databases. I have very little prior knowledge on databases, and chances are this program will be utterly useless. But we can all hope, can't we?

*** 

*If you find any bugs, or have any ideas for future features, remember to file an issue report an Github!*
### **THANKS FOR USING THE DaFAQ DATABASE SYSTEM!**
<small> <small> (If you can even call it a Database System.)
