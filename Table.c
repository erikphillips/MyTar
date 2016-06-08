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
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "Tar.h"
#include "Table.h"
#include "SmartAlloc.h"

#define MODE_SIZE 10

typedef struct {
   int flag;
   int index;
   int val;
} ModeChar;

static ModeChar MODECHRS[] = {
   {S_IRUSR, 1, 'r'}, {S_IWUSR, 2, 'w'}, {S_IXUSR, 3, 'x'},
   {S_IRGRP, 4, 'r'}, {S_IWGRP, 5, 'w'}, {S_IXGRP, 6, 'x'},
   {S_IROTH, 7, 'r'}, {S_IWOTH, 8, 'w'}, {S_IXOTH, 9, 'x'},
   {S_ISUID, 3, 's'}, {S_ISGID, 6, 's'}, {S_IFDIR, 0, 'd'},
   {0, 0, 0}
};

void Table(Tar *tar) {
   Header header;
   ModeChar *modeChr;
   int fdTar, filesize, size, ndx, mode;
   char *end, modeStr[MODE_SIZE + 1], *timeStr;
   time_t mtime;
   
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
      
      filesize = strtol(header.size, &end, OCT);
      lseek(fdTar, filesize, SEEK_CUR);
      
      if (tar->opts[V_FLAG]) {
         mode = strtol(header.mode, &end, OCT);
                  
         for (ndx = 0; ndx < MODE_SIZE; ndx++)
            modeStr[ndx] = '-';
         modeStr[ndx] = 0;
         
         for (modeChr = MODECHRS; modeChr->flag; modeChr++)
            if (mode & modeChr->flag)
               modeStr[modeChr->index] = modeChr->val;
         
         mtime = strtol(header.mtime, &end, OCT);
         timeStr = asctime(gmtime(&mtime));
         timeStr[strlen(timeStr) - 1] = 0;
      
         printf("%s %s %s %9u %s ", 
          modeStr, header.uname, header.gname, filesize, timeStr);
      }
     
      printf("%s\n", header.name);
   }
   
}
