#include <unistd.h>
#include "my_tar.h"

int my_strlen(char* string)
{
    int size = 0;
    while (string[size] != '\0')
    {
        size += 1;
    }
    return size;
}

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

int my_strcpy(char* dest, char* src)
{
    while(src)
    {
        *dest = *src;
        dest += 1;
        src += 1;
    }
    return 0;
}

long convert(long n, int base, char *result)
{
    const char *digits = "0123456789";
    int index = 0;
    int negative = 0;

    if(n < 0)
    {
        negative = 1;
        n = -n;
    }
    if (n == 0)
    {

        result[0] = '0';
        result[1] = '\0';
        return 1;
    }

    while (n > 0)
    {
        result[index++] = digits[n % base];
        n /= base;
    }
    result[index] = '\0';

    // Reverse the string to get the correct order
    for (int i = 0; i < index / 2; ++i)
    {
        char temp = result[i];
        result[i] = result[index - i - 1];
        result[index - i - 1] = temp;
    }

    if (negative)
    {
        for (int i = index; i >= 0; --i)
        {  
            result[i + 1] = result[i];
        }
        result[0] = '-';
        index++;
    }
    return index;
}

t_list* init_head(char* elem)
{
    t_list* new_head = (t_list*)secure_malloc(sizeof(t_list));
    if (new_head == NULL)
    {
        return NULL;
    }
    new_head->elem = elem;
    new_head->next = NULL;
    return new_head;
}

t_list* insert_elem(t_list* head, char* elem)
{
    t_list* new_node = (t_list*)secure_malloc(sizeof(t_list));
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

void print_linked_list(t_list* head)
{
    t_list* current = head;
    while (current != NULL) 
    {
        printf("elem : %s\n", current->elem);
        current = current->next;
    }
}

int octal_to_decimal(char *str, size_t size) {
    int result = 0;
    for (size_t i = 0; i < size && str[i] >= '0' && str[i] <= '7'; i++) {
        result = result * 8 + (str[i] - '0');
    }
    return result;
}

t_tar *read_header(int fd, t_tar *header) 
{
    read(fd, header->filename, 100);
    lseek(fd, 21, SEEK_CUR);
    read(fd, header->size, 12);
    lseek(fd, 376, SEEK_CUR);

    return header;
}

int create_files_by_extract(t_tar header, char *content_of_file)
{
    int write_error = 0;

    char *filename = header.filename;
    write(1, content_of_file, my_strlen(content_of_file));

    int fd = secure_open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    chmod(filename, 0777);

    write_error = write(fd, content_of_file, my_strlen(content_of_file));
    if(write_error == -1) {
        write(2, "error write\n", my_strlen("error write\n"));
        return 2;
    }

    close(fd);

    return 0;
}