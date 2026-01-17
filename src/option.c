#include "my_tar.h"

/**
* @summary detect the option chosen by the user.
* @param char* flags: string containing all the options.
* @return return the options.
*/

int opt_flag(char* flags)
{
    int options = 0;
    t_opt opt_list[] = {
        {OPT_CREATE, 'c'},
        {OPT_APPEND, 'r'},
        {OPT_LIST, 't'},
        {OPT_UPDATE, 'u'},
        {OPT_EXTRACT, 'x'},
        {OPT_FILE, 'f'},
        {0,0}
    };

    while (*flags)
    {
        for (t_opt* tmp_opt_list = opt_list; tmp_opt_list->flag != 0; tmp_opt_list += 1)
        {
            if (*flags == tmp_opt_list->flag)
            {
                options += tmp_opt_list->opt;
            }
        }
        flags += 1;
    }
    return options;
}

/**
* @summary initialise the structure.
* @return return the structure initialised.
*/

t_tar_file* init_struct_tar() 
{
    t_tar_file* buffer = (t_tar_file*)secure_malloc(sizeof(t_tar_file));
    if (buffer == NULL)
    {
        return NULL;
    }
    return buffer;
}

/**
* @summary takes the filenames and put it in the structure.
* @param int ac: number of parameters given.
         char** av: all the characters that are given.
* @return return the structure with the filenames in it.
*/

t_tar_file* init_tar_file(int ac, char** av)
{
    int index = 1;
    t_tar_file* struct_tar_file = init_struct_tar();
    if (struct_tar_file == NULL)
    {
        return NULL;
    }
    while (index < ac)
    {
        if (av[index][0] == '-')
        {
            struct_tar_file->opt += opt_flag(av[index] + 1);
        }
        else
        {
            if (struct_tar_file->file_name == NULL)
            {
                struct_tar_file->file_name = av[index];
            }
            else
            {
                struct_tar_file->file_list = add_elem(struct_tar_file->file_list, av[index]);
                if (struct_tar_file->file_list == NULL)
                {
                    return NULL;
                }
            }
        }
        index += 1;
    }
    return struct_tar_file;
}

/**
* @summary verify if the option f is found.
* @param int flag: value of the flags that have been given.
* @return return a boolean to check if the flag f are present.
*/

int check_flag_f(int flag)
{
    if (flag & OPT_FILE)
    {
        return 1;
    }
    return 0;
}

/**
* @summary search if a given name is inside of linked list
* @param t_list head: linked list to go through
*        char* name : name to search for in the linked list
* @return return the name that was found, or NULL if it's not the case
*/

char* compare_name(t_list* head, char* name)
{
    t_list* current = head;
    while (current != NULL)
    {
        if(!strcmp(current->elem, name))
        {
            return current->elem;
        }
        current = current->next;
    }
    return NULL;
}

/**
* @summary creates files with the filename and the content by extracting the tarball of the archive.
* @param t_tar header: header of the tarball that contains the filename.
*        char *content_of_file: content of the file that will be put in the file.
* @return return an int corresponding to the error.
*/

int create_files_by_extract(t_tar header, char *content_of_file)
{
    if (!content_of_file || !header.filename[0]) {
        write(2, "Invalid parameters\n", my_strlen("Invalid parameters\n"));
        return 1;
    }

    char *filename = header.filename;
    int content_length = my_strlen(content_of_file);

    if (content_length > 0 && !(content_length == 1 && content_of_file[0] == '\n')) {
        write(1, content_of_file, content_length);
    }

    int fd = secure_open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    chmod(filename, 0777);
    if (fd == -1) {
        write(2, "Open error\n", my_strlen("Open error\n"));
        return 2;
    }

    int file_size = octal_to_decimal(header.size, 12);
    int write_size = (file_size > 0) ? file_size : content_length;

    lseek(fd, 0, SEEK_SET);
    ssize_t error_write = write(fd, content_of_file, write_size);
    if (error_write == -1) {
        write(2, "Write error\n", my_strlen("Write error\n"));
        close(fd);
        return 3;
    }

    close(fd);
    return 0;
}

/**
* @summary calculate the checksum of a string by reading character by character.
* @param const char* buff: buffer where we calculate the checksum.
* @return return the checksum.
*/

unsigned int calculating_checksum(const char* buff) 
{
    unsigned int chksum = 0;
    for(int index = 0; index < 512; index++) 
    {
        chksum += (unsigned char)buff[index];
    }
    return chksum;
}

/**
* @summary calculate the checksum of the header by using the function calculate_checksum.
* @param t_tar *tarball: tarball where we calculate the cheksum.
* @return return the checksum.
*/

unsigned int checksum(t_tar *tarball) 
{
    unsigned int chcksum = calculating_checksum((const char*)tarball);
    chcksum += ' ' * 8;

    return chcksum;
}

/**
* @summary detect what are the options.
* @param t_tar *tarball: tarball containing the options.
* @return return the checksum.
*/

int detect_options(t_tar_file* struct_tar_file)
{
    if (struct_tar_file->opt & OPT_CREATE)
    {
        if (!check_flag_f(struct_tar_file->opt))
        {
            write(2, "tar: Refusing to write archive contents to terminal (missing -f option?)\n", my_strlen("tar: Refusing to write archive contents to terminal (missing -f option?)\n"));
            write(2, "tar: Error is not recoverable: exiting now\n", my_strlen("tar: Error is not recoverable: exiting now\n"));
            return 2;
        }
        if (opt_create(struct_tar_file) == -1)
        {
            return 2;
        }
        return 0;
    }
    if (struct_tar_file->opt & OPT_APPEND)
    {
        if (!check_flag_f(struct_tar_file->opt))
        {
            return -1;
        }
        if (opt_append(struct_tar_file) == -1)
        {
            return 2;
        }
        return 0;

    }
    if (struct_tar_file->opt & OPT_LIST)
    {
        if (!check_flag_f(struct_tar_file->opt))
        {
            write(2, "tar: Refusing to read archive contents from terminal (missing -f option?)\n", my_strlen("tar: Refusing to read archive contents from terminal (missing -f option?)\n"));
            write(2, "tar: Error is not recoverable: exiting now\n", my_strlen("tar: Error is not recoverable: exiting now\n"));
            return 2;
        }
        if (opt_list(struct_tar_file) == -1)
        {
            return 2;
        }
        return 0;
    }
    if (struct_tar_file->opt & OPT_UPDATE)
    {
        if (!check_flag_f(struct_tar_file->opt))
        {
            return -1;
        }
        if (opt_update(struct_tar_file)== -1)
        {
            return 2;
        }
        return 0;
    }
    if (struct_tar_file->opt & OPT_EXTRACT)
    {
        if (!check_flag_f(struct_tar_file->opt))
        {
            write(2, "tar: Refusing to read archive contents from terminal (missing -f option?)\n", my_strlen("tar: Refusing to read archive contents from terminal (missing -f option?)\n"));
            write(2, "tar: Error is not recoverable: exiting now\n", my_strlen("tar: Error is not recoverable: exiting now\n"));
            return 2;
        }
        if (opt_extract(struct_tar_file) == -1)
        {
            return 2;
        }
        return 0;
    }
    return -2;
}
