# <ins>**DaFAQ Database System**</ins>

The DaFAQ (**Da**tabase **F**ind **A**nd **Q**uery) database system is a work-in-progress system for managing DaFAQ databases. <br />
In the future there may also hopefully be a simple query language for it, too. But that's a big _maybe_. <br />
While I'm making all my wishes, I'll go one step further and wish that this program will be somewhat useful. But I doubt it. <br />
I have very limited knowledge on Databases and whatnot, so this system is probably extremely flawed (as in, not just the code, but the implementation, too).

**IMPORTANT NOTE:** The DaFAQ system is built for Linux, and it may not work for other kernels and OSs. Sorry for the inconvenience.

## <ins>**Version 0.1.0**</ins>
<ins>**v0.1.0 IS NOT COMPATIBLE WITH WITH TABLES MADE WITH v0.0**</ins><br />
Version 0.1.0 (v0.1.0) was released October 6, 2020

### <ins>**v0.1.0 CHANGES:**</ins>
A list of features added in version 0.1.0
* Killed a few bugs that I missed in v0.0 (There seems to be no shortage of them)
* Feature tweaks (not necessarily bug fixes)
* The "diarrhea" function was added
* Input masks were added

### <ins>**USAGE:** </ins>

**Data Management Functions:**
* **create_database** - create a new database directory (a directory for holding tables) if one doesn't exist, and enter it.
* **create_table** - an interactive text interface for creating a table and adding fields to it. This will delete any other table that had the same name.
* **switch_field** - edits a field in a table (changes name and/or data type). This should be a safe function, as it should update all of the records in the table too. An input of field_num = -1 adds appends a field, an input of data_size = 0 does keeps the size of the field that is being switched, an input of field_name = NULL preserves the field being switched's name.
* **switch_record** - prints an interactive text interface and writes a record to the table at the desired location. An input of record_num = -1 appends the record, 
and any attempt to switch a record past the end of the table appends the record, too. 

**Table Navigation Functions:**
* **check_magic** - checks if a file has the proper magic sequence in order to be a valid DaFAQ file.
*  **get_num_of_fields** - returns the number of fields in a table (mostly used by get_fields)
* **get_fields** - returns the number of fields in a table and writes them into a buffer of type field * fields.
* **get_num_of_records** - returns the number of records in a table.
* **check_extension** - checks if table file has a valid extension (.dfq)

**Database Review Functions:**
* **poop** - prints every record in a table. 
* **diahrrea** - poops every table in a database.

**Other Functions:**
* **check_input_mask** - checks if input mask is valid. More on input masks in details/Usage
* **valid_input** - checks if input is valid according to the input mask

### <ins>**OTHER INFORMATION:**</ins>

The interactive text interfaces I so heavily used in this program rely heavily on the CColor library I wrote. I strongly suggest that you check it out. <br />
The standard.* files are comprised of functions I find useful in many of my projects. 

### <ins>**AN EXPLANATION OF THE STRUCTURE OF THIS PROJECT:**</ins>
As you can see, this project is split into a couple of directories:
* In the main directory itself: the Makefile, executable, and README file (for easy access)
* **src** - the directory where the source code is stored
* **include** - the directory where all header files are stored
* **obj** - the directory created by the Makefile for the storage of object files created during the compilation process
* **details** - in the directory itself: details regarding the state and usage of the program. 
    * In the sub directory **archive** is an archive of all the previous READMEs and any other outdated files regarding the state and/or usage of the system for previous versions

### <ins>**THE FUTURE/GOALS:** </ins>
Here are some of my ideas for the future:
* A querying language
* The ability to query files and write results into markdown files (this feature may come soon...)

### <ins>**NOTES:**</ins>
None of what is in this current version (v0.1.0) is final. Hopefully the finished version will be drastically different than this one. My end goal is to create a semi-useful system for creating and managing databases. I have very little prior knowledge on databases, and chances are this program will be utterly useless. But we can all hope, can't we?

*** 

*If you find any bugs, or have any ideas for future features, remember to file an issue report an Github!*
### **THANKS FOR USING THE DaFAQ DATABASE SYSTEM!**
<small> <small> (If you can even call it a Database System.)