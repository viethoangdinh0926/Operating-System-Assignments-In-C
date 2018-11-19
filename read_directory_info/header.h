#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h> // file type/prot macros
#include<sys/sysmacros.h> // major/minor macros
#include<stdio.h>
#include<stdlib.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<string.h>
#include<errno.h>

struct dirent *pDirent;
DIR *pDir;
struct stat *file_info;
char permission[11];
int isDev;

void printInfo(char *filePath);
