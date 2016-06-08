/* Erik Phillips
 * CPE-357 - Systems Programming
 * California Polytechnic State Unviversity
 * San Luis Obispo, California
 *
 * Final Project: MyTar - tape archive
 * Presented: 07 June 2016
 */
 
#ifndef TAR_H
#define TAR_H

#define MAX_FILES 100   /* maximum number of files */
#define NUM_OPTS 6      /* number of options that is implemented */
#define C_FLAG 0        /* specific flags c, t, x, v, f, z */
#define T_FLAG 1
#define X_FLAG 2
#define V_FLAG 3
#define F_FLAG 4
#define Z_FLAG 5

#define BUF_SIZE 512    /* block size */
#define OCT 8           /* octal number encoding */
#define MAGIC "ustar"   /* POSIX-specified USTAR archive format */

#define CMD_SIZE 512    /* command string max size */
#define COMPRESSION "gzip"    /* compression algorithm */

/* size definitions for the header's fields */
#define H_NAME 255
#define H_MODE 8
#define H_UID 8
#define H_GID 8
#define H_SIZE 12
#define H_MTIME 12
#define H_CHKSUM 8
#define H_TYPEFLAG 1
#define H_LINKNAME 100
#define H_MAGIC 6
#define H_VERSION 2
#define H_UNAME 32
#define H_GNAME 32
#define H_DEVMAJOR 8
#define H_DEVMINOR 8

/* holds the important file metadata for each entry into the tarfile */
typedef struct Header {
   char name[H_NAME];
   char mode[H_MODE];
   char uid[H_UID];
   char gid[H_GID];
   char size[H_SIZE];
   char mtime[H_MTIME];
   char chksum[H_CHKSUM];
   char typeflag[H_TYPEFLAG];
   char linkname[H_LINKNAME];
   char magic[H_MAGIC];
   char version[H_VERSION];
   char uname[H_UNAME];
   char gname[H_GNAME];
   char devmajor[H_DEVMAJOR];
   char devminor[H_DEVMINOR];
} Header;

typedef struct Tar {
   char *files[MAX_FILES];    /* list of files from the commandline */
   Header *headers[MAX_FILES];   /* list of headers for each file */
   int numFiles;              /* number of files */
   int numHeaders;            /* number of headers */
   char *tarfile;             /* name of the tarfile from the commandline */
   int opts[NUM_OPTS];        /* options array taken from the cmdline flags */
   int verbose;               /* signals an increased verbosity when true */
} Tar;

/* Report error when opening a file */
void OpenError(char *file, int fd);

/* Report a usage error to the user */
void Usage();

/* Terminate the program, report an error to user */
void Terminate(Tar *tar);

#endif
