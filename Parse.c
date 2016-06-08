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
#include <string.h>
#include <dirent.h>
#include "SmartAlloc.h"
#include "Parse.h"

#define MAX_OPTS 5   /* maximum number of option flags */
#define MIN_ARGS 3   /* mim number of args needed to run program */
#define TAR_NDX 2    /* index of the tarfile */
#define BREAKPOINT 3 /* split between flags */

void Parse(Tar *tar, int argc, char **argv) {
   int ndx, valid;
   char *str, *temp;
   
   if (argc < MIN_ARGS)
      Usage();
   
   str = temp = calloc(MAX_OPTS + 1, sizeof(char));
   strcpy(str, argv[1]);   
   while (*str) {
      if (*str == 'c')
         tar->opts[C_FLAG] = 1;
      else if (*str == 't')
         tar->opts[T_FLAG] = 1;
      else if (*str == 'x')
         tar->opts[X_FLAG] = 1;
      else if (*str == 'v')
         tar->opts[V_FLAG] = 1;
      else if (*str == 'f')
         tar->opts[F_FLAG] = 1;
      else if (*str == 'z')
         tar->opts[Z_FLAG] = 1;
      else if (*str != '-')
         Usage();
      str++;
   }
   free(temp);
   
   if (tar->opts[F_FLAG]) {
      tar->tarfile = calloc(strlen(argv[TAR_NDX]) + 1, sizeof(char));
      strcpy(tar->tarfile, argv[TAR_NDX]);
   }

   if (tar->tarfile == NULL)
      Usage();
   
   for (ndx = 0, valid = 0; ndx < BREAKPOINT; ndx++)
      if (tar->opts[ndx])
         valid++;
   if (valid != 1)
      Usage();
   
   if (tar->opts[Z_FLAG] && !tar->opts[C_FLAG])
      Usage();
   
   if (tar->opts[C_FLAG]) {
      for (ndx = TAR_NDX + 1; ndx < argc; ndx++) {
         tar->files[tar->numFiles] = 
          calloc(strlen(argv[ndx]) + 1, sizeof(char));
         strcpy(tar->files[tar->numFiles++], argv[ndx]);
      }
   }
}
