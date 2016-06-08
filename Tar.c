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
#include "SmartAlloc.h"
#include "Tar.h"
#include "Parse.h"
#include "Create.h"
#include "Extract.h"
#include "Table.h"

/* Prints out the tar structure for debugging */
void Print(Tar *tar);

/* Frees all alloc'd data created in each process */
void Free(Tar *tar);

int main(int argc, char **argv) {
   Tar *tar = calloc(1, sizeof(Tar));
   
   Parse(tar, argc, argv);
   
   if (tar->opts[C_FLAG])
      Create(tar);
   if (tar->opts[X_FLAG])
      Extract(tar);
   if (tar->opts[T_FLAG])
      Table(tar);
   
   // Print(tar);
   
   Free(tar);
   
   exit(EXIT_SUCCESS);
}

void Free(Tar *tar) {
   int ndx, vflag;
   
   vflag = tar->opts[V_FLAG];
   
   if (tar->tarfile)
      free(tar->tarfile);
   
   for (ndx = 0; ndx < tar->numFiles; ndx++)
      if (tar->files[ndx])
         free(tar->files[ndx]);
   tar->numFiles = 0;
   
   for (ndx = 0; ndx < tar->numHeaders; ndx++)
      if (tar->headers[ndx])
         free(tar->headers[ndx]);
   tar->numHeaders = 0;
   
   if (tar)
      free(tar);
   tar = NULL;

   /* prints of the non-freed space in the heap */
   // if (vflag) 
   //   printf("Final space used: %lu\n", report_space());
   
}

void OpenError(char *file, int fd) {
   fprintf(stderr, "Error opening file: %s -> %d\n", file, fd);
}

void Usage() {
   printf("Usage: mytar -[ctxv]f tarfile [path [...] ]\n");
   exit(EXIT_FAILURE);
}

void Terminate(Tar *tar) {
   fprintf(stderr, "Error reading from '%s'\n", tar->tarfile);
   Free(tar);
   exit(EXIT_FAILURE);
}

void Print(Tar *tar) {
   int ndx;
   
   printf("TAR STRUCTURE:\n");
   printf("\topts c: %d, t: %d, x: %d\n", 
    tar->opts[C_FLAG], tar->opts[T_FLAG], tar->opts[X_FLAG]);
   printf("\ttarfile: %s\n", tar->tarfile);
   printf("\tnumFiles: %d\n", tar->numFiles);
   printf("\tFiles: \n");
   for (ndx = 0; ndx < tar->numFiles; ndx++) 
      printf("\t\t%s\n", tar->files[ndx]);   
}