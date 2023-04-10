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

#include "zip_pass.h"

#include <stdlib.h>
#include <zlib.h>

bool try_password(char const * password, zip_t * zip_file) {
    // Struct that holds information about a file in the zip file.
    zip_stat_t stat;

    // Let's load the information about the first file in the zip file.
    zip_stat_index(zip_file, 0, 0, &stat);

    // Let's try to open this file using the passed password.
    zip_file_t * file_in_zip = zip_fopen_index_encrypted(zip_file, 0, 0, password);

    // Okay here it becomes a bit complicated. Due to the algorithm ZIP
    // uses, most of the times the wrong password simply leads to the result
    // that nor data can be restores. But in some cases, data of the correct
    // length can be read using the wrong password. In this case, the CRC
    // of the read data needs to be compared to the CRC for the files we
    // just read from the meta information.

    bool correct = false; // We assume that our password was not correct.

    if(file_in_zip) {
        // Allocate anough memory to store the whole uncompressed and decrypted file.
        // In read world we would not do this for every attempt, but just create this buffer once.
        char unsigned * buffer = (char unsigned *)malloc(sizeof(char unsigned) * stat.size);

        // Decompress and decrypt the file using the password.
        zip_int64_t bytes_read = zip_fread(file_in_zip, buffer, stat.size);

        // Close our file handle.
        zip_fclose(file_in_zip);

        // Did we read any bytes?
        if(bytes_read != -1) {
            // Calculate CRC32 of the data we decompressed and decrypted.
            uint32_t crc = crc32(0, buffer, stat.size);
            // Does CRC23 we calcualted match the CRC32 from the meta information?
            if(crc == stat.crc) {
                // We found the correct password!
                correct = true;
            }
        }

        // Free the memory of our buffer.
        free((void*)buffer);
    }

    return correct;
}
