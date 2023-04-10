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

#include "config.h"

#include <string.h>
#include <stdlib.h>

config parse_config(int argc, char * argv[argc+1]) {
    bool print_usage = false;
    bool verbose = false;
    int num_threads = 1;
    char * passdb_file = argv[argc-2];
    char * zip_file = argv[argc-1];

    for(int i = 0; i < argc; ++i) {
        if(!strcmp(argv[i], "--help")) {     
            print_usage = true;
        } else if(!strcmp(argv[i], "--verbose")) {
            verbose = true;
        } else if(!strcmp(argv[i], "--num-threads")) {
            num_threads = atoi(argv[i+1]);
        }
    }

    config config  = { print_usage, verbose, num_threads, passdb_file, zip_file };

    return config;
}


