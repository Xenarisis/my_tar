#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include "utils.h"
#define BYTE_OFFSET 512
/*
** structs
*/
typedef struct s_opt
{
#define OPT_CREATE (1<<0)
#define OPT_APPEND (1<<1)
#define OPT_LIST (1<<2)
#define OPT_UPDATE (1<<3)
#define OPT_EXTRACT (1<<4)
#define OPT_FILE (1<<5)
  int opt;
  char flag;
} t_opt;

typedef struct s_tar_file
{
  int opt;
  char* file_name;
  t_list* file_list;
} t_tar_file;

typedef struct s_tar
{                               /* byte offset */
  char filename[100];           /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
  char padding[12];             /* 500 */
                                /* 512 */
} t_tar;
/*
** main.c
*/
int main(int, char**);
/*
** option.c
*/
t_tar_file* init_tar_file(int, char**);
char* compare_name(t_list*, char*);
int create_files_by_extract(t_tar, char *);
char* compare_name(t_list*, char*);
int detect_options(t_tar_file*);
unsigned int calculating_checksum(const char *); 
unsigned int checksum(t_tar *); 
/*
** tar.c
*/
int opt_create(t_tar_file*);
int opt_append(t_tar_file*);
int opt_list(t_tar_file*);
int opt_update(t_tar_file*);
int opt_extract(t_tar_file*);
