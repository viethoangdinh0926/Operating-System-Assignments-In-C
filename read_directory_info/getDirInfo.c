#include "header.h"

int main(int argc, char **argv) {
	int i;
	permission[3] = ' ';
	permission[7] = ' ';
	file_info = (struct stat *)malloc(sizeof(struct stat));
	if(argc == 1){////process all elements of current directory
		pDir = opendir (".");
		if (pDir == NULL) {
			printf ("Cannot open current directory");
			exit(1);
		}

		while ((pDirent = readdir(pDir)) != NULL) {
			printInfo(pDirent->d_name);
		}

		closedir (pDir);
		return 0;
	}////END of process all elements of current directory
	else{////process SOME elements of current directory
		for(i = 1; i < argc; i++){
			printInfo(argv[i]);
		}
	}////END of process SOME elements of current directory
}

void printInfo(char *filePath){
	if(lstat(filePath, file_info) == -1){
		printf("%s\n",strerror(errno));
		exit(2);
	}

    	printf("FILENAME:\t\t\t %s\n", filePath);
	printf("FILE_TYPE:\t\t\t ");
	switch(file_info->st_mode & S_IFMT){
		case S_IFLNK:
			printf("symbolic link\n");
			isDev = 0;
			break;
		case S_IFREG:
			printf("ordinary\n");
			isDev = 0;
			break;
		case S_IFDIR:
			printf("directory\n");
			isDev = 0;
			break;
		case DT_BLK:
			printf("bock device\n");
			isDev = 1;
			break;
		case DT_CHR:
			printf("character device\n");
			isDev = 1;
			break;
	}

	permission[0] = (file_info->st_mode & S_IRUSR) != 0 ? 'r':'-';
	permission[1] = (file_info->st_mode & S_IWUSR) != 0 ? 'w':'-';
	permission[2] = (file_info->st_mode & S_IXUSR) != 0 ? 'x':'-';
	permission[4] = (file_info->st_mode & S_IRGRP) != 0 ? 'r':'-';
	permission[5] = (file_info->st_mode & S_IWGRP) != 0 ? 'w':'-';
	permission[6] = (file_info->st_mode & S_IXGRP) != 0 ? 'x':'-';
	permission[8] = (file_info->st_mode & S_IROTH) != 0 ? 'r':'-';
	permission[9] = (file_info->st_mode & S_IWOTH) != 0 ? 'w':'-';
	permission[10] = (file_info->st_mode & S_IXOTH) != 0 ? 'x':'-';
	printf("PERMISSIONS:\t\t\t %s\n", permission);
	printf("OWNER_NAME:\t\t\t %s\n", getpwuid(file_info->st_uid)->pw_name);
	printf("GROUP_NAME:\t\t\t %s\n", getgrgid(file_info->st_gid)->gr_name);
	printf("DATE_OF_LAST_MODIFICATION:\t %s", ctime(&(file_info->st_mtime)));
	printf("LINK_COUNT:\t\t\t %lu\n", (unsigned long)(file_info->st_nlink));
	if(isDev == 0){
		printf("SIZE_IN_BYTES:\t\t\t %lu\n", (unsigned long)(file_info->st_size));
	}
	else{
		printf("DEVICE_INFO:\t\t\t %d, %d\n", major(file_info->st_rdev), minor(file_info->st_rdev));
	}
	printf("INODE_NUMBER:\t\t\t %lu\n\n\n", file_info->st_ino);
}
