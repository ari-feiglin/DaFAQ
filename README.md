# <ins>**DaFAQ Database System**</ins>

The DaFAQ (**Da**tabase **F**ind **A**nd **Q**uery) database system is a work-in-progress system for managing DaFAQ databases. <br />
In the future there will also hopefully be a simple query language for it, too. But that's a big _maybe_.

**IMPORTANT NOTE:** The DaFAQ system is built for Linux, and it may not work for other kernels and OSs. Sorry for the inconvenience.

## <ins>**Version 0.0.0**</ins>
Version 0.0.0 (v0.0.0) was released October 5, 2020. 

### <ins>**USAGE:** </ins>

**Data Management Functions:**
* **create_database** - create a new database directory (a directory for holding tables) if one doesn't exist, and enter it.
* **create_table** - an interactive text interface for creating a table and adding fields to it. This will delete any other table that had the same name.
* **switch_field** - edits a field in a table (changes name and/or data type). This should be a safe function, as it should update all of the records in the table too. An input of field_num = -1 adds appends a field, an input of data_size = 0 does keeps the size of the field that is being switched, and an input of field_name = NULL preserves the field being switched's name.
* **add record** - prints an interactive text interface and appends a record to the table (there is no way to switch a record, I may have forgotten about that). 

**Table Navigation Functions:**
* **check_magic** - checks if a file has the proper magic sequence in order to be a valid DaFAQ file.
*  **get_num_of_fields** - returns the number of fields in a table (mostly used by get_fields)
* **get_fields** - returns the number of fields in a table and writes them into a buffer of type field * fields.
* **get_num_of_records** - returns the number of records in a table.

**Database Review Functions:**
* **poop** - prints every record in a table. 

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
    * In the sub directory **archive** (to be added as of October 5, 2020) is an archive of all the previous READMEs and any other outdated files regarding the state and/or usage of the system for previous versions


### <ins>**NOTES:**</ins>
None of what is in this current version (v0.0.0) is final. Hopefully the finished version will be drastically different than this one. My end goal is to create a semi-useful system for creating and managing databases. I have very little prior knowledge on databases, and chances are this program will be utterly useless. But we can all hope, can't we?
