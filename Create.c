/* Erik Phillips
 * CPE-357 - Systems Programming
 * California Polytechnic State Unviversity
 * San Luis Obispo, California
 *
 * Final Project: MyTar - tape archive
 * Presented: 07 June 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include "SmartAlloc.h"
#include "Create.h"

#define PERM 0600       /* default permissions for the tarfile */
#define PERM_MASK 0x1FF /* mask to get the correct mode numebr */
#define TYPE_DIR 5      /* specific number for directories */
#define TYPE_FILE 0     /* specific number for file types */

/* Creates a directory if one is passed on the commandline
 * or if there is a sub-directory found within a directory */
void CreateDirectory(Tar *tar, int fdTar, char *name, DIR *dir);
 
/* Creates a file if one is passed on the commandline
 * or if there is a file within a directory that is included */
void CreateFile(Tar *tar, int fdTar, char *name);
 
/* Creates a Header for eahc of the files that is added to the tarfile */
void CreateHeader(Tar *tar, struct stat *buf, char *name, char type);

void Create(Tar *tar) {
   int fdTar, ndx, pid;   
   DIR *dir;
   char *buf;
   
   fdTar = open(tar->tarfile, O_CREAT | O_TRUNC | O_WRONLY, PERM);
   if (fdTar < 0) {
      OpenError(tar->tarfile, fdTar);
      exit(EXIT_FAILURE);
   }
   
   for (ndx = 0; ndx < tar->numFiles; ndx++) {
      dir = opendir(tar->files[ndx]);
      if (dir != NULL) {
         buf = calloc(strlen(tar->files[ndx]) + 2, sizeof(char));

         if (tar->files[ndx][strlen(tar->files[ndx]) - 1] != '/') 
            sprintf(buf, "%s/", tar->files[ndx]);
         else 
            strcpy(buf, tar->files[ndx]);
         
         CreateDirectory(tar, fdTar, buf, dir);
         closedir(dir);
         free(buf);
      }
      else {
         CreateFile(tar, fdTar, tar->files[ndx]);
      }
   }
   
   close(fdTar);
   
   if (tar->opts[Z_FLAG]) {
      
      pid = fork();
      
      if (pid == 0) {
         if (tar->opts[V_FLAG])
            printf("Compressing '%s'\n", tar->tarfile);
         
         execlp(COMPRESSION, COMPRESSION, "-9", tar->tarfile, NULL);
         perror("exec failed");
         exit(EXIT_FAILURE);
      }
         
      wait(NULL);
   }
}

void CreateDirectory(Tar *tar, int fdTar, char *name, DIR *dir) {
   struct dirent *ent;
   DIR *newDir;
   struct stat statbuf[sizeof(stat) + 1];
   int fdDir = dirfd(dir);
   char *buf;
   
   if (fstat(fdDir, statbuf) != 0) {
      perror("stat error");
      exit(EXIT_FAILURE);
   }
   
   if (tar->numHeaders >= MAX_FILES) {
      printf("maximm number of files reached\n");
      exit(EXIT_SUCCESS);
   }
   
   CreateHeader(tar, statbuf, name, TYPE_DIR);
   write(fdTar, tar->headers[tar->numHeaders], sizeof(Header));
   tar->numHeaders++;
   
   if (tar->opts[V_FLAG]) 
      printf("Added directory '%s'\n", name);
   
   while ((ent = readdir(dir)) != NULL) {
      if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
         if (strlen(ent->d_name) >= H_NAME) {
            fprintf(stderr, "file name/path cannot exceed %d, given %u\n", 
             H_NAME, strlen(ent->d_name));
            continue;
         }
         
         buf = calloc(H_NAME, sizeof(char));
         sprintf(buf, "%s%s", name, ent->d_name);
         
         newDir = opendir(buf);
         if (newDir != NULL) {
            sprintf(buf, "%s%c", buf, '/');
            CreateDirectory(tar, fdTar, buf, newDir);
            closedir(newDir);
         }
         else {
            CreateFile(tar, fdTar, buf);
         }
         
         free(buf);
      }
   }
}

void CreateFile(Tar *tar, int fdTar, char *name) {
   int fdFile, size;
   struct stat statbuf[sizeof(stat) + 1];
   char buf[BUF_SIZE];
   
   if (tar->numHeaders >= MAX_FILES) {
      printf("maximm number of files reached\n");
      exit(EXIT_SUCCESS);
   }
   
   if (strlen(name) >= H_NAME) {
      fprintf(stderr, "file name/path cannot exceed %d, given %u\n", 
       H_NAME, strlen(name));
      return;
   }
   
   fdFile = open(name, O_RDONLY);
      
   if (fdFile < 0) {
      OpenError(name, fdFile);
      close(fdFile);
      return;
   }
   
   if (fstat(fdFile, statbuf) != 0) {
      perror("stat error");
      exit(EXIT_FAILURE);
   }
   
   CreateHeader(tar, statbuf, name, TYPE_FILE);
   write(fdTar, tar->headers[tar->numHeaders], sizeof(Header));
   tar->numHeaders++;
   
   while ((size = read(fdFile, buf, BUF_SIZE)) != 0)
      write(fdTar, buf, size);
   
   if (tar->opts[V_FLAG])
      printf("Added file '%s'\n", name);
   
   close(fdFile);
      
}

void CreateHeader(Tar *tar, struct stat *buf, char *name, char type) {
   Header *header = tar->headers[tar->numHeaders] = calloc(1, sizeof(Header));
   struct passwd *usr;
   struct group *grp;
   
   strncpy(header->name, name, H_NAME);
   sprintf(header->mode, "%06ho", (unsigned short) (buf->st_mode & PERM_MASK));
   sprintf(header->uid, "%06o", (int) buf->st_uid);
   sprintf(header->gid, "%06o", (int) buf->st_gid);
   
   if (type == TYPE_DIR) 
      sprintf(header->size, "%011lo", (unsigned long) 0);
   else if (type == TYPE_FILE)
      sprintf(header->size, "%011lo", (long) buf->st_size);
   
   sprintf(header->mtime, "%lo", (long) buf->st_mtime);
  
   if (type == TYPE_DIR)
      *(header->typeflag) = '5'; /* '0' for regular file */
   else if (type == TYPE_FILE)
      *(header->typeflag) = '0';
   
   strcpy(header->magic, MAGIC);
   *(header->version) = '0';
   *(header->version + 1) = '0';
   
   usr = getpwuid(getuid());
   strcpy(header->uname, usr->pw_name);
   
   grp = getgrgid(getgid());
   strcpy(header->gname, grp->gr_name);
   
   // missing chksum
   // missing linkname
   // missing devmajor/devminor
}
