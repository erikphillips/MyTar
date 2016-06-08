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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>
#include <string.h>
#include "Tar.h"
#include "Extract.h"
#include "SmartAlloc.h"

#define DIRECTORY '5'   /* directory type */

void Extract(Tar *tar) {
   Header header;
   struct utimbuf modtime;
   int fdTar, fdFile, size, fileSize, mode;
   char *buf, *end;
   
   fdTar = open(tar->tarfile, O_RDONLY);
   if (fdTar < 0) {
      OpenError(tar->tarfile, fdTar);
      exit(EXIT_FAILURE);
   }
   
   while ((size = read(fdTar, &header, sizeof(Header))) != 0) {
      if (strcmp(MAGIC, header.magic)) {
         fprintf(stderr, "Header reading error: '%s'\n", MAGIC);
         Terminate(tar);
      }
      
      if (*(header.typeflag) == DIRECTORY) {
         mode = strtol(header.mode, &end, OCT);
         mkdir(header.name, mode);
         
         if (tar->opts[V_FLAG])
            printf("Created directory '%s'\n", header.name);
         
         continue;
      }
      
      fileSize = strtol(header.size, &end, OCT);
      buf = calloc(fileSize, sizeof(char));
      
      size = read(fdTar, buf, fileSize);
      
      mode = strtol(header.mode, &end, OCT);      
      fdFile = open(header.name, O_CREAT | O_TRUNC | O_WRONLY, mode);
      if (fdFile < 0) {
         OpenError(header.name, fdFile);
         exit(EXIT_FAILURE);
      }
      
      size = write(fdFile, buf, fileSize);
      if (size != fileSize)
         fprintf(stderr, "file writing error\n");
      
      if (tar->opts[V_FLAG])
         printf("Extracted '%s'\n", header.name);
      
      free(buf);
      if (close(fdFile)) {
         perror("closing error\n");
         exit(EXIT_FAILURE);
      }
      
      modtime.modtime = strtol(header.mtime, &end, OCT);
      modtime.actime = strtol(header.mtime, &end, OCT);
      if (utime(header.name, &modtime))
         fprintf(stderr, "mod time change error for '%s'\n", header.name);
   }
   
   close(fdTar);
}
