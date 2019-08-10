#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define DOG_MAGIC 0x84838281
#define DOG_VERSION 0x00001

char* boot = "";
char* src = "";
char* out = "disk.img";

DIR* srcd;
FILE* bootf;
FILE* tempf;
FILE* outf;

unsigned long blockSize = 512;
unsigned long long size = 4096;

typedef struct {
	unsigned int magic;
	unsigned short version;
	
	unsigned short header_size;
	unsigned long long size;
	unsigned long blocksize;
	
	unsigned long long first_file;
} DogFsHeader;

typedef struct {
	char name[128];
	unsigned char flags;
	unsigned long long size;
	unsigned long long prev;
	unsigned long long next;
	unsigned long create_time;
	unsigned long modify_time;
	unsigned long access_time;
	char* data;
	char file_flag;
	unsigned long long pointer;
} Block;

void createDir (char* name) {
	//printf ("%s \n", name + strlen(src));
}
void createFile (){
	
}

void write_block (Block* block) {
	
}

void tree (char *path, unsigned int level) {	
    struct dirent *dp;
	char temp[1000];
	
	DIR* dir;
	dir = opendir (path);
	
	if (!dir)
		return;
	
	while ((dp = readdir (dir)) != NULL)
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
			strcpy (temp, path);
			for (int i = 0; i < level; i++)
				printf ("| ");
			printf("|-- %s\n", dp->d_name);
			if (dp->d_type == DT_DIR) {
				strcat (temp, "/");
				strcat (temp, dp->d_name);
				createDir(&temp);
				tree (temp, level + 1);
			} else
				createFile(&temp);
		}
	closedir (dir);
}

void closeAll () {
	if (strcmp (boot, "") != 0)
		fclose (bootf);
	//if (strcmp (temp, "") != 0)
	//	fclose (tempf);
	if (strcmp (out, "") != 0)
		fclose (outf);
}

void getArgs (int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		if (!strncmp (argv [i], "boot=", 5))
			boot = argv [i] + 5;
		else if (!strncmp (argv [i], "src=", 4))
			src = argv [i] + 4;
		else if (!strncmp (argv [i], "out=", 4))
			out = argv [i] + 4;
		else if (!strncmp (argv [i], "bs=", 3))
			blockSize = atoi (argv [i] + 3);
		else if (!strncmp (argv [i], "size=", 5))
			size = atoi (argv [i] + 5);
		else {
			printf ("Unknown arg: %s\n", argv [i]);
			exit (1);
		}
	}
}

void checkArgs () {
	if (size < 4096) {
		printf ("Too low size: %lli!\n", size);
		exit (1);
	} else if (blockSize < 512) {
		printf ("Too low block size: %li\n", blockSize);
		exit (1);
	}
	
	if (strcmp (out, "") == 0) {
		printf ("Out is not specified so out is disk.img\n");
		out = "disk.img";
	}
	
	if (strcmp (src, "") == 0)
		printf ("Source directory isn't specified no files to write. (src=*DIRECTORY_NAME*)\n");
}

int main (int argc, char** argv) {
	printf ("############################################\n");
	printf ("##              Making dogfs              ##\n");
	printf ("############################################\n");
	
	getArgs (argc, argv);
	checkArgs ();
	
	if (remove (out) != -1)
		printf ("Removed output file to recreate again!\n");
	
	if ( (outf = fopen (out, "w+b")) == NULL) {
			printf ("Cannot open output file (%s).\n", out);
			exit (1);
	}
	
	printf ("Output: \"%s\"\n\n", out);
	
	/*Writing boot if specified*/
	
	if (strcmp (boot, "") != 0) {
		if ( (bootf = fopen (boot, "rb")) == NULL) {
			printf ("Cannot open boot file (%s).\n", boot);
			exit (1);
		}
		fseek (bootf, 0L, SEEK_END);
		long filelen;
		if ( (filelen = ftell (bootf)) != 512) {
			printf ("Boot must be 512 bytes, %li found!\n", ftell (bootf));
			exit (1);
		}
		rewind (bootf);
		
		char buffer [filelen];
		fread (buffer, filelen, 1, bootf);
		
		if ( (unsigned char)buffer [filelen-2] != 0x55 || (unsigned char)buffer [filelen-1] != 0xAA) {
			printf ("Your boot is not signed right! (0x55, 0xAA): 0x%02X, 0x%02X\n", (unsigned char)buffer [filelen-2], (unsigned char)buffer [filelen-1]);
			exit (1);
		}
	
		printf ("Boot: \"%s\"\nSize: %li byte (s)\nBoot signature right: 0x55, 0xAA\n", boot, filelen);
		for (int i = 0; i < filelen; i++)
			if (i > 509)
				printf ("\033[0;32m%02X\033[0m", (unsigned char)buffer [i]);
			else if ( (unsigned char)buffer [i] == 0x00)
				printf ("\033[1;31m%02X\033[0m", (unsigned char)buffer [i]);
			else
				printf ("%02X", (unsigned char)buffer [i]);
		printf ("\n\n");
		
		if (fwrite (buffer, 512, 1, outf) != 1) {
			printf ("Can't write boot to output file!\n");
			exit (1);
		}
		
		printf ("Writed boot successfully!\n\n");
	}
	
	fseek (outf, 0L, 512);
	char buffer[sizeof (DogFsHeader)];
	
	if (strcmp (src, "") != 0) {
		if ( (srcd = opendir (src)) == NULL) {
			printf ("Can't open source directory! %s\n", src);
			exit (1);
		}
		
		DogFsHeader header = {DOG_MAGIC, DOG_VERSION, sizeof (DogFsHeader), size, blockSize, 512 + sizeof (DogFsHeader)};
		
		if (fwrite (&header, sizeof (DogFsHeader), 1, outf) != 1) {
			printf ("Can't write file system to output file!\n");
			exit (1);
		}
		
		printf (".\n");
		tree(src, 0);
	} else {
		DogFsHeader header = {DOG_MAGIC, DOG_VERSION, sizeof (DogFsHeader), size, blockSize, -1};
		
		if (fwrite (&header, sizeof (DogFsHeader), 1, outf) != 1) {
			printf ("Can't write file system to output file!\n");
			exit (1);
		}
	}
	
	closeAll ();
}