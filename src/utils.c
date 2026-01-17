#include "my_tar.h"

/**
* @summary recreation of strlen.
* @param char* string: array of character.
* @return return the size.
*/

int my_strlen(char* string)
{
    int size = 0;
    while (string[size] != '\0')
    {
        size += 1;
    }
    return size;
}

/**
* @summary malloc the buffer and verify the result of the malloc.
* @param size_t size: size for the malloc.
* @return return the buffer that has been malloc.
*/

void* secure_malloc(size_t size)
{
    void* buffer = malloc(size);
    if (buffer == NULL)
    {
        write(2, "malloc failed\n", my_strlen("malloc failed\n"));
        return NULL;
    }
    memset(buffer, 0, size);
    return buffer;
}

/**
* @summary open the file and verify the result.
* @param char* pathname: name of the path of the file.
*        int flags: flags or option for the open.
* @return return an int corresponding to the error.
*/

int secure_open(char* pathname, int flags)
{
    int result = open(pathname, flags);
    if (result == -1)
    {
        write(2, "my_tar: ", my_strlen("my_tar: "));
        write(2, pathname, my_strlen(pathname));
        write(2, ": Cannot stat: No such file or directory\n", my_strlen(": Cannot stat: No such file or directory\n"));
        return -1;
    }
    return result;
}

/**
* @summary write something and verify the result.
* @param int fd: file descriptor.
*        const void* buff: array of character that will be writen.
*        size_t size: size of the array of character.
* @return return an int corresponding to the error.
*/

int secure_write(int fd, const void* buff, size_t size)
{
    int result = write(fd, buff, size);
    if (result == -1)
    {
        write(2, "Error, write failed\n", my_strlen("Error, write failed\n"));
        return -1;
    }
    return result;
}

/**
* @summary get the stat and verify the result.
* @param int fd: file descriptor.
*        struct stat* buff: variable that represent the struct stat.
* @return return an int corresponding to the error.
*/

int secure_fstat(int fd, struct stat* buff)
{
    int result = fstat(fd, buff);
    if (result == -1)
    {
        write(2, "Cannot stat : no such file or directory", my_strlen("Cannot stat : no such file or directory"));  
        return -1;
    }
    return result;
}

/**
* @summary get the link and verify the result.
* @param char* path: path of the file.
*        char* buff: buffer where the link will be.
*        size_t buff_size: size of the link.
* @return return an int corresponding to the error.
*/

int secure_readlink(char* path, char* buff, size_t buff_size)
{
    int result = readlink(path, buff, buff_size);
    if (result == -1)
    {
        write(2, "Error, readlink failed\n", my_strlen("Error, readlink failed\n"));
        return -1;
    }
    return result;
}

/**
* @summary copy src to dest.
* @param char* dest: destination of the copy.
*        char* src: source of the copy.
* @return return an int corresponding to the error.
*/

int my_strcpy(char* dest, char* src)
{
    while(src != NULL)
    {
        *dest = *src;
        dest += 1;
        src += 1;
    }
    return 0;
}

/**
* @summary convert a long to put it in an array of character at a certain base.
* @param long n: number to convert.
*        int base: base to convert.
*        char *result: array of character were the result will be.
*        int result_size: size of the array.
* @return return the size of the array.
*/

long convert(long n, int base, char *result, int result_size)
{
    const char *digits = "0123456789";
    char* convert = secure_malloc(sizeof(char) * my_strlen(result));
    if (convert == NULL)
    {
        return -1;
    }
    int index = 0;
    int i = 0;

    memset(result, '0', result_size - 1);
    while (n > 0)
    {
        convert[index++] = digits[n % base];
        n /= base;
    }

    // Reverse the string to get the correct order
    for (i = 0; i < index / 2; ++i)
    {
        char temp = convert[i];
        convert[i] = convert[index - i - 1];
        convert[index - i - 1] = temp;
    }

    int nb_char_null = my_strlen(result) - index;
    int jndex = 0;
    for (i = nb_char_null; i < my_strlen(result); i += 1)
    {
        result[i] = convert[jndex];
        jndex += 1;
    }
    result[i + 1] = '\0';
    return index;
}

/**
* @summary initialise the head of a linked list.
* @param char* elem: element to initialise the head with.
* @return return the initialised head.
*/

t_list* init_head(char* elem)
{
    t_list* new_head = (t_list*) secure_malloc(sizeof(t_list));
    if (new_head == NULL)
    {
        return NULL;
    }
    new_head->elem = elem;
    new_head->next = NULL;
    return new_head;
}

/**
* @summary insert an node at the end of a linked list.
* @param t_list* head of the linked list to modify.
*         char* elem: element to initialise the node with.
* @return return the modified linked list.
*/

t_list* insert_elem(t_list* head, char* elem)
{
    t_list* new_node = (t_list*) secure_malloc(sizeof(t_list));
    if (new_node == NULL)
    {
        return NULL;
    }
    new_node->elem = elem;
    t_list* current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = new_node;
    current->next->next = NULL;
    return head;
}

/**
* @summary insert a node at the end of a linked list or initialise the head if it's NULL.
* @param t_list* head of the linked list to modify.
*         char* elem: element to initialise the node with.
* @return return the modified linked list.
*/

t_list* add_elem(t_list* head, char* elem)
{
    if (head == NULL)
    {
        head = init_head(elem);
        if (head == NULL)
        {
            return NULL;
        }
    }
    else
    {
        head = insert_elem(head, elem);
        if (head == NULL)
        {
            return NULL;
        }
    }
    return head;
}

/**
* @summary print a given linked list.
* @param t_list* head of the linked list to print.
*/

void print_linked_list(t_list* head)
{
    t_list* current = head;
    while (current != NULL) 
    {
        printf("elem : %s\n", current->elem);
        current = current->next;
    }
}

/**
* @summary convert a string into an int.
* @param char* str: string to convert.
* @return return the converted int.
*/

int my_atoi(char *str)
{
    int negative = 0;
    int index = 0;
    int result = 0;

    if (str[index] == '-') {
        negative = 1;
        index++;  
    }

    while (str[index] != '\0') {
        if (str[index] >= '0' && str[index] <= '9')
        {
            result = result * 10 + (str[index] - '0');
        }
        index++;
    }

    if (negative == 1)
    {
        result *= -1;
    }

    return result;
}

/**
* @summary convert a number in base 8 to base 10.
* @param char* str: the number to convert.
*        size_t size: size of the number.
* @return return the converted number.
*/

int octal_to_decimal(char *str, size_t size) {
    int result = 0;
    for (size_t i = 0; i < size && str[i] >= '0' && str[i] <= '7'; i++)
    {
        result = result * 8 + (str[i] - '0');
    }
    return result;
}
