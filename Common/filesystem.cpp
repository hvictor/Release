/*
 * filesystem.cpp
 *
 *  Created on: Jun 15, 2016
 *      Author: sled
 */

#include <sys/types.h>
#include <sys/stat.h>
#include "filesystem.h"


void filesystem_make_directory(char *dir_full_path)
{
	struct stat st = {0};

	if (stat(dir_full_path, &st) == -1) {
		mkdir(dir_full_path, 0700);
	}
}

