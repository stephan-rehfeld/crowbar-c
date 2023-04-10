/*
MIT License

Copyright (c) 2023 Stephan Rehfeld

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <time.h>

#include "passdb.h"
#include "config.h"
#include "zip_pass.h"

void print_usage();

int main(int argc, char * argv[argc+1]) {
    if(argc < 3) {
        print_usage();
        return EXIT_FAILURE;
    } 
    
    config config = parse_config(argc, argv);

    if(config.print_usage) {
        print_usage();
        return EXIT_SUCCESS;
    }

    if(config.verbose) printf("Trying to open %s with passwords from %s.\n", config.zip_file, config.passdb_file);

    FILE* passdb_file = fopen(config.passdb_file, "r");

    if(!passdb_file) {
        fprintf(stderr, "Failed to load PASSWD file %s.\n", config.passdb_file);
        return EXIT_FAILURE;
    }

    if(config.verbose)  printf("Parsing passworda from file %s.\n", config.passdb_file);

    passdb passdb = load_passdb(passdb_file);

    if(config.verbose) printf("Loaded %li passwords from %s.\n", passdb.num_passwords, config.passdb_file);

    fclose(passdb_file);    

    int error_code = 0;

    zip_t * zip_file = zip_open(config.zip_file, ZIP_RDONLY, &error_code );

    if(!zip_file) {
        fprintf(stderr, "Failed to load ZIPFILE file %s.\n", config.zip_file);
        destroy_passdb(&passdb);
        return EXIT_FAILURE; 
    }

    /* HERE, the interesting stuff happens */

    char const * password = NULL;

    time_t start_time = time(NULL);

    for(size_t i = 0; i < passdb.num_passwords; ++i) {
        if(try_password(passdb.passwords[i], zip_file)) {
            password = passdb.passwords[i];
            break;
        }
    }

    double exec_time = difftime(time(NULL), start_time);

    /* HERE, the intersting stuff ends*/

    if(config.verbose) printf("Total time for cracking: %f\n", exec_time);

    if(password) {
        puts(password);
    }
    
    destroy_passdb(&passdb);
    zip_close(zip_file);

    if(password) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

void print_usage() {
    puts("Usage: crowbar [OPTION]... PASSDB ZIPFILE");
    puts("Tries to crack a password protected zip files by trying the passwords from PASSDB");
    puts("Options:");
    puts("\t--help\t\tDisplay this information");
    puts("\t--verbose\tPrint more information during execution");
    puts("");
    puts("PASSDB: Textfile that contains passwords to try. One password per line.");
    puts("");
    puts("ZIPFILE: Zip file to crack");
}
