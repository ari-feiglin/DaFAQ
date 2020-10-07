# <ins>**USAGE OF THE DaFAQ DATABASE SYSTEM**</ins>
This is a description of the how to use the database system once the carpet code has been written (by carpte code, I mean the code that uses the framework provided hereby the functions I wrote), not a description of how to use each function. That will be given in some other place.

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
