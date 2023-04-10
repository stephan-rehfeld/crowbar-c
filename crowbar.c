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
#include <threads.h>

#include "passdb.h"
#include "config.h"
#include "zip_pass.h"

void print_usage();

typedef struct crack_task crack_task;
struct crack_task {
    size_t begin_index;
    size_t num_passwords;
    passdb passdb;
    char const * zip_file;
    char const * * result;
};

int crack_function(void* data);

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
    if(config.verbose) printf("Using %i threads.\n", config.num_threads);

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

    zip_close(zip_file);

    /* HERE, the interesting stuff happens */

    char const * password = NULL;

    time_t start_time = time(NULL);

    crack_task task = { 0, passdb.num_passwords, passdb, config.zip_file, &password };

    thrd_t thread_handle;

    thrd_create(&thread_handle, crack_function, &task);

    int return_code = 0;

    thrd_join(thread_handle, &return_code);

    double exec_time = difftime(time(NULL), start_time);

    /* HERE, the intersting stuff ends*/

    if(config.verbose) printf("Total time for cracking: %f\n", exec_time);

    if(password) {
        puts(password);
    }
    
    destroy_passdb(&passdb);

    if(password) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

int crack_function(void* data) {
    crack_task * task = (crack_task*)data;
    
    int error_code = 0;

    zip_t * zip_file = zip_open(task->zip_file, ZIP_RDONLY, &error_code );

    if(!zip_file) {
        fprintf(stderr, "Failed to load ZIPFILE file %s.\n", task->zip_file);
        return EXIT_FAILURE; 
    }
    
    char const * * passwords = task->passdb.passwords + task->begin_index;

    for(size_t i = 0; i < task->num_passwords; ++i) {
        if(try_password(passwords[i], zip_file)) {
            *(task->result) = passwords[i];
            zip_close(zip_file);
            return EXIT_SUCCESS;
        }
    }

    zip_close(zip_file);
    return EXIT_FAILURE;
}

void print_usage() {
    puts("Usage: crowbar [OPTION]... PASSDB ZIPFILE");
    puts("Tries to crack a password protected zip files by trying the passwords from PASSDB");
    puts("Options:");
    puts("\t--help\t\t\tDisplay this information");
    puts("\t--verbose\t\tPrint more information during execution");
    puts("\t--num-threads <n>\tUser <n> threads. Default: 1");
    puts("");
    puts("PASSDB: Textfile that contains passwords to try. One password per line.");
    puts("");
    puts("ZIPFILE: Zip file to crack");
}
