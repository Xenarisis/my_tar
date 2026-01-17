#include "my_tar.h"

/**
* @summary write the header and the content of the file that are in the tarball, in the archive.
* @param int fd_tarball: file descriptor of the tarfile to write the tarball.
*        t_tar* tarball: the tarball.
*        char* content_file: content of the file.
*        int content_size: size of the content of the file.
* @return return an int corresponding to the error.
*/

int write_tarball(int fd_tarball, t_tar* tarball, char* content_file, int content_size)
{
    unsigned int chcksum = checksum(tarball);
    convert(chcksum, 8, tarball->chksum, 7);

    if (secure_write(fd_tarball, tarball, 512) == -1)
    {
        return -1;
    }
    if (secure_write(fd_tarball, content_file, content_size) == -1)
    {
        return -1;
    }
    int padding = 512 - (content_size % 512);
    if (padding != 512)
    {
        char zero[512] = {0};
        if (secure_write(fd_tarball, zero, padding) == -1)
        {
            return -1;
        }
    }
    return 0;
}

/**
* @summary create the tarball that will be writen in the archive.
* @param t_tar_file* struct_tar_file: struct where all the filenames are.
*        int fd_tarball: file descriptor of the archive to write the tarball.
* @return return an int corresponding to the error.
*/

int create_tarball(t_tar_file* struct_tar_file, int fd_tarball)
{
    t_list* current = struct_tar_file->file_list;
    t_tar* tarball = secure_malloc(sizeof(t_tar));
    struct passwd* uname = secure_malloc(sizeof(struct passwd));
    struct group* gname = secure_malloc(sizeof(struct group));
    char* buff_content = NULL;
    int content_size = 0;
    if (tarball == NULL)
    {
        return -1;
    }
    struct stat* buff = (struct stat*) secure_malloc(sizeof(struct stat));
    if (buff == NULL)
    {
        return -1;
    }
    while (current != NULL)
    {
        int fd = secure_open(current->elem, O_RDWR);
        if (fd == -1)
        {
            return -1;
        }
        if (secure_fstat(fd, buff) == -1)
        {
            return -1;
        }
        memset(tarball, 0, sizeof(t_tar));
        strcpy(tarball->filename, current->elem);             // filename
        convert(buff->st_mode - 32768, 8, tarball->mode, 8);  // mode
        convert(buff->st_uid, 8, tarball->uid, 8);            // uid
        convert(buff->st_gid, 8, tarball->gid, 8);            // gid
        convert(buff->st_size, 8, tarball->size, 12);         // size
        convert(buff->st_mtime, 8, tarball->mtime, 12);       // mtime
        tarball->typeflag = '0';

        uname = getpwuid(buff->st_uid);
        gname = getgrgid(buff->st_gid);
        strcpy(tarball->uname, uname->pw_name);
        strcpy(tarball->gname, gname->gr_name);
        strcpy(tarball->magic, "ustar");
        tarball->version[0] = '0';
        tarball->version[1] = '0';
        content_size = buff->st_size;
        buff_content = secure_malloc(content_size);
        if (buff_content == NULL)
        {
            return -1;
        }
        if (read(fd, buff_content, content_size) == -1)
        {
            return -1;
        }
        if (write_tarball(fd_tarball, tarball, buff_content, content_size) == -1)
        {
            return -1;
        }
        free(buff_content);
        current = current->next;
    }
    char zero[512] = {0};
    write(fd_tarball, zero, 512);
    write(fd_tarball, zero, 512);

    free(tarball);
    free(buff);
    close(fd_tarball);
    return 0;
}

/**
* @summary opt -c creates the archive with the tarball given.
* @param t_tar_file* struct_tar_file: struct where all the filenames are.
* @return return an int corresponding to the error.
*/

int opt_create(t_tar_file* struct_tar_file)
{
    char* filename = struct_tar_file->file_name;
    int fd = secure_open(filename, O_RDWR | O_CREAT);
    if (fd == -1)
    {
        return -1;
    }
    chmod(filename, 0777);

    if (create_tarball(struct_tar_file, fd) == -1)
    {
        return -1;
    }

    close(fd);
    return 0;
}

/**
* @summary opt -r add a new tarball in the archive at the end.
* @param t_tar_file* struct_tar_file: struct where all the filenames are.
* @return return an int corresponding to the error.
*/

int opt_append(t_tar_file* struct_tar_file)
{
    int fd = secure_open(struct_tar_file->file_name, O_RDWR);
    if (fd == -1)
    {
        return -1;
    }
    lseek(fd, -1024, SEEK_END);
    if (create_tarball(struct_tar_file, fd) == -1)
    {
        return -1;
    }
    close(fd);
    return 0;
}

/**
* @summary opt -t print all the filename of the tarball that are in an archive.
* @param t_tar_file* struct_tar_file: struct where all the filenames are.
* @return return an int corresponding to the error.
*/

int opt_list(t_tar_file* struct_tar_file)
{
    char* filename = struct_tar_file->file_name;
    int fd = secure_open(filename, O_RDONLY);

    t_tar header;
    int nb_read = -1;
    int index = 0;
    int is_null = 0;

    while (is_null != 2) 
    {
        nb_read = read(fd, &header, 512);
        if (nb_read == -1)
        {
            write(2, "Read error\n", my_strlen("Read error\n"));
            return 1;
        }

        if(nb_read != 512)
        {
            is_null++;
        }
        else
        {
            is_null = 0;
        }

        if (header.filename[0] != '\0')
        {
            write(1, header.filename, my_strlen(header.filename));
            write(1, "\n", 1);
        }

        int file_size = octal_to_decimal(header.size, 12);

        for(int i = 0; i < file_size; i++)
        {
            index++;
            if(index == 512)
            {
                lseek(fd, 512, SEEK_CUR);
                index = 0;
            }
        }
        index = 0;
        lseek(fd, 512, SEEK_CUR);
    }

    close(fd);
    return 0;
}

/**
* @summary opt -u updates the tarball if the modification is newer than the one in the archive.
* @param t_tar_file* struct_tar_file: struct where all the filenames are.
* @return return an int corresponding to the error.
*/

int opt_update(t_tar_file* struct_tar_file)
{
    char* filename = struct_tar_file->file_name;
    int fd = secure_open(filename, O_RDWR);
    if (fd == -1)
    {
        return -1;
    }
    t_tar header;
    int nb_read = -1;
    int is_null = 0;
    int found = 0;
    int index = 0;

    t_list* current = struct_tar_file->file_list;
    while (current) 
    {
        int updated = 0;
        struct stat st;
        if (stat(current->elem, &st) == -1)
            return -1;

        lseek(fd, 0, SEEK_SET);
        found = 0;

        while (nb_read != 0)
        {
            nb_read = read(fd, &header, 512);
            if (nb_read == -1)
            {
                write(2, "Read error\n", my_strlen("Read error\n"));
                return 1;
            }
            if (nb_read != 512)
            {
                is_null++;
            }
            else
            {
                is_null = 0;
            }
            if (is_null == 2) 
            {
                close(fd);
                return -1;
            }

            int file_size = octal_to_decimal(header.size, 12);
            if (strcmp(header.filename, current->elem) == 0)
            {
                found = 1;
                time_t mtime_tar = octal_to_decimal(header.mtime, 12);
                if (st.st_mtime > mtime_tar)
                {
                    updated = 1;
                }

                if (st.st_mtime == mtime_tar)
                {
                    updated = 0;
                }
            }
            
            for(int i = 0; i < file_size; i++)
            {
                index++;
                if(index == 512)
                {
                    lseek(fd, 512, SEEK_CUR);
                    index = 0;
                }
            }
            index = 0;
            lseek(fd, 512, SEEK_CUR);
        }
    
        if(updated == 1|| found == 0)
        {
            t_list *new_list = secure_malloc(sizeof(t_list));
            if(new_list == NULL) 
            {
                return -1;
            }
            new_list->elem  = current->elem;
            t_tar_file* test = struct_tar_file;
            test->file_list = new_list;

            opt_append(test);
        }
        current = current->next;
    }

    close(fd);
    return 0;
}

/**
* @summary opt -x extracts one by one the tarball that are in the archive.
* @param t_tar_file* struct_tar_file: struct where all the filenames are.
* @return return an int corresponding to the error.
*/

int opt_extract(t_tar_file* struct_tar_file)
{
    char* filename = struct_tar_file->file_name;
    int fd = secure_open(filename, O_RDWR);
    t_tar header;
    int nb_read = -1;
    int index = 0;
    int is_null = 0;
    int size_for_str = 0;
    char *str;
    while (nb_read != 0) 
    {
        nb_read = read(fd, &header, 512);
        if (nb_read == -1)
        {
            write(2, "Read error\n", my_strlen("Read error\n"));
            return 1;
        }

        if(nb_read != 512)
        {
            is_null++;
        }
        else
        {
            is_null = 0;
        }

        if(is_null == 2) 
        {
            close(fd);
            return 0;
        }

        int file_size = octal_to_decimal(header.size, 12);

        for(int i = 0; i < file_size; i++)
        {
            index++;
            if(index == 512)
            {
                size_for_str += 512;
                index = 0;
            }

        }
        index = 0;
        size_for_str += 512;

        if (header.filename[0] != '\0') 
        {
            str = secure_malloc(sizeof(char) * size_for_str);
            read(fd, str, size_for_str);
            create_files_by_extract(header, str);
        }
    }

    free(str);
    close(fd);
    return 0;
}
