#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/*
** structs
*/
typedef struct s_list {
    char* elem;
    struct s_list* next;
} t_list;
/*
** utils.c
*/
int my_strlen(char*);
void* secure_malloc(size_t);
int secure_open(char*, int);
int secure_write(int, const void*, size_t);
int secure_fstat(int, struct stat*);
int secure_readlink(char*, char*, size_t);
int my_strcpy(char*, char*);
long convert(long, int, char*, int);
t_list* add_elem(t_list*, char*);
int check_flag_f(int);
void print_linked_list(t_list*);
int my_atoi(char*);
int octal_to_decimal(char*, size_t);
