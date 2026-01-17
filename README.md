# Welcome to My Tar
***

## Task
The task is to recreate the command 'tar' present in bash. Its goal is to compress multiples files into a single one, named archive. This archive contains multiples tarball, which are each associated with a file.
Each tarball contains 2 fields of information. The first one is called a header and has informations such as the filename, the modification date and the permission. The other one is the content of the file,
which is directly transfered from the original file. 

This project has multiples parameters that it can be executed with. They are most commonly named flags. The flags that needed to be recreated were the flags -c, -r, -t, -u, -x and -f.
The flag -c, also named create, is used to create an archive. Each files given are compressed into their corresponding tarballs. The flag -r, or append, creates a tarball and add it at the end of an existing archive.
The flag -t, or list, search for every name of the files present in an archive, then prints them in the terminal. The flag -u, or update, compares the modification date of a given file with the one of a file
already present in an archive. If the modification date of the given file is superior, the file is appended to the archive. The flag -x, or extract, takes an archive and extracts every tarball, to recreate the files
associated with. The flag -f, or file, tells the computer that the parameters are files. It is mandatory everytime another flag is used.

## Description
Firstly, to solve this project, a bit field was implemented to detect which flags are given when using the command. Then a structure containing every field in the header was put in place,
in order to make this project easier.

When the command is executed, the structure is initialised as empty, in order to avoid any bugs. Then the flags are detected using the bit field and a loop, that goes through every argument. In the meantime, the others
arguments are saved. The first one that is not a flag is indentified as the filename of the archive. The others are written inside a linked list, as the files that will be used by the program. Then the function corresponding
to the flag that has been recognised is called.

## Installation
A Makefile was created in order to compile the code more easily.

## Usage
In order to use this project, you need to execute this command in the terminal :
```
./my_tar tarball flags files
```

An exemple could be :
```
./my_tar test.tar -cf path/toto.txt path/tata.txt
```

Don't forget to use the command 'make' in order to update any issue that might have happened

### The Core Team

<span><i>Made at <a href='https://qwasar.io'>Qwasar SV -- Software Engineering School</a></i></span>
<span><img alt='Qwasar SV -- Software Engineering School's Logo' src='https://storage.googleapis.com/qwasar-public/qwasar-logo_50x50.png' width='20px' /></span>
