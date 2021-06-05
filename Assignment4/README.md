# Assignment 4
## Myls - custom implementation of linux ls command

Description:
This program takes command line input and functions like linux command line
options. It checks the input and displays the information of files or directory
depending on the input. The valid options are ilR. See the description of each 
option as following:

        -i   print the index number of each file.
    	-l   List in  long  format,  giving  mode,  ACL  indication,
             number  of links, owner, group, size in bytes, and time
             of last modification for each file.
        -R   Prints directories and subdirectories recursively.

How to run:
  To run this program, either type the excutatble name followed the names
of directories or files that you want information for. Or no argument which
will then take the current directory as the input.
  For example:
```sh  	
	./myls -i
	./myls -l directoryname
	./myls -ilR filename1 filename 2
```
