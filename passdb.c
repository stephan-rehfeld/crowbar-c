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

#include "passdb.h"

#include <string.h>
#include <assert.h>

passdb load_passdb(FILE* file) {
    char buffer[100]; // Buffer to read a line. We assume max length of 100 per password.
    size_t num_passwords = 0; // Variable to count number of passwords.

    // Let's find out how many passwords are in the file.
    while(fgets(buffer, 100, file)) {
        ++num_passwords;
    }

    rewind(file); // Go back to the beginning of the file.

    // Allocate enough memory to store pointers to all passwords.
    char const * * passwords = (char const * *)malloc(sizeof(char const *) * num_passwords); 

    size_t i = 0; // Count loaded passwords.

    // No read all passwords.
    while(fgets(buffer, 100, file)) { // Read current line
        // Get length of the read line.
        size_t length = strlen(buffer);

        // fgets also puts new line into the string. We want to remove that.
        if(length > 0 && buffer[length-1] == '\n') {
            buffer[length-1] = 0;
        }

        // Allocate some memory to store read password.
        char* password = (char*)malloc(sizeof(char) * (length));
        // Copy password to allocated memory location.
        strcpy(password, buffer);

        // Store pointer to string into our lists of pointers.
        passwords[i] = password;

        // Increment i by 1.
        ++i;
    }
    
    assert(i == num_passwords);

    // Create struct.
    passdb db = { num_passwords, passwords };

    // Return the loaded password databse.
    return db;
}

void destroy_passdb(passdb * restrict db) {
    // Clean up memory of each password string.
    for(size_t i = 0; i < db->num_passwords; ++i) {
        free((void*)db->passwords[i]);
    }

    // Clean up memory where all pointers to all strings are stored.
    free((void*)db->passwords);
}
