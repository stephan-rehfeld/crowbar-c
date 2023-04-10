# Crowbar - Educational password crack tool

Crowbar is a small password crack tool for encrypted ZIP files, written in C.
It implements a simple dictionary attack, by loading passwords from a text file
and trying all of the loaded passwords, until one decrypts the first file from
the ZIP file.

In case crowbar finds the correct password, it prints the password to STDOUT and
returns 0. Otherwise, it prints nothing and returns a non-success code.

This tool is only for educational use and written as foundation for a laboratory
exercise at a university, to pratice multithreading in c. Hence, the code is
written with the goal of readability, not performance. Hence, many aspects can
be improved for real-world usage.

## Compile

Crowbar requires libzip and zlib. On Debian (and derivates) simply install them
via:

    sudo apt install zlib1g-dev libzip-dev

Afterwards, simply compile crowbar using the following command.

    cc -Wall -Wextra -Werror -pedantic -std=c17 -O3 crowbar.c passdb.c zip_pass.c config.c -lzip -lz -o crowbar

## Using

Usage: crowbar [OPTION]... PASSDB ZIPFILE
Tries to crack a password protected zip files by trying the passwords from PASSDB
Options:
        --help          Display this information
        --verbose       Print more information during execution

PASSDB: Textfile that contains passwords to try. One password per line.

ZIPFILE: Zip file to crack

## License

Code is distributed under MIT License.
Password lists in folder passdbs are copied from Daniel Miessler's [SecList](https://github.com/danielmiessler/SecLists)
project, which is also distributed under MIT License.

