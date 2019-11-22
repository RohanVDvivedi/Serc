#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include<stdio.h>
#include<unistd.h>

#include<hashmap.h>
#include<dstring.h>

#include<strhsh.h>

#define SERC_ROOT_PATH "./serc_root"

// this method returns -1, if the file is absent, else 0 if no error
int read_file_in_dstring(dstring* file_contents_result, dstring* file_path);

// extension_result is appended with the extension from the file_path
void get_extension_from_file_path(dstring* extension_result, dstring* file_path);

#endif