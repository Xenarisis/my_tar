#include "my_tar.h"

int write_tarball(int fd_tarball, t_tar* tarball, char* content_file)
{
    if (secure_write(fd_tarball, &tarball->filename, 100) == -1)  /* 100 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->mode, 8) == -1)        /* 108 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->uid, 8) == -1)         /* 116 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->gid, 8) == -1)         /* 124 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->size, 12) == -1)       /* 136 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->mtime, 12) == -1)      /* 148 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->chksum, 8) == -1)      /* 156 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->typeflag, 1) == -1)    /* 157 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->linkname, 100) == -1)  /* 257 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->magic, 6) == -1)       /* 263 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->version, 2) == -1)     /* 265 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->uname, 32) == -1)      /* 297 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->gname, 32) == -1)      /* 329 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->devmajor, 8) == -1)    /* 337 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->devminor, 8) == -1)    /* 345 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->prefix, 155) == -1)    /* 500 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, &tarball->padding, 12) == -1)    /* 512 */
    {
        return -1;
    }
    if (secure_write(fd_tarball, "\n", 1) == -1)
    {
        return -1;
    }
    if (secure_write(fd_tarball, content_file, my_strlen(content_file)) == -1)
    {
        return -1;
    }
    for (int i=0; i<512; i+=1)
    {
        if (secure_write(fd_tarball, "\0", 1) == -1)
        {
            return -1;
        }
    }
    return 0;
}

int create_tarball(t_tar_file* struct_tar_file, int fd_tarball)
{
    t_list* current = struct_tar_file->file_list;
    t_tar* tarball = (t_tar*) secure_malloc(sizeof(t_tar));
    char* buff_char = secure_malloc(sizeof(char) * 100);
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
        strcpy(tarball->filename, current->elem);           // filename
        if (secure_fstat(fd, buff) == -1)
        {
            return -1;
        }
        convert(buff->st_mode, 10, tarball->mode);          // mode
        memset(buff_char, 0, sizeof(char) * 100);
        convert(buff->st_uid, 10, tarball->uid);            // uid
        memset(buff_char, 0, sizeof(char) * 100);
        convert(buff->st_gid, 10, tarball->gid);            // gid
        memset(buff_char, 0, sizeof(char) * 100);
        convert(buff->st_size, 10, tarball->size);          // size
        memset(buff_char, 0, sizeof(char) * 100);
        convert(buff->st_mtime, 10, tarball->mtime);        // mtime
        memset(buff_char, 0, sizeof(char) * 100);
        convert(512, 10, tarball->chksum);                  // chksum
        memset(&tarball->typeflag, 0, sizeof(char));        // typeflag
        memset(&tarball->linkname, 0, sizeof(char) * 100);  // linkname
        memset(&tarball->magic, 0, sizeof(char) * 6);       // magic
        memset(&tarball->uname, 0, sizeof(char) * 32);      // uname
        memset(&tarball->gname, 0, sizeof(char) * 32);      // gname
        memset(&tarball->devmajor, 0, sizeof(char) * 8);    // devmajor
        memset(&tarball->devminor, 0, sizeof(char) * 8);    // devminor
        memset(&tarball->prefix, 0, sizeof(char) * 155);    // prefix
        memset(&tarball->padding, 0, sizeof(char) * 12);    // padding
        content_size = buff->st_size;
        buff_content = secure_malloc(sizeof(char) * content_size);
        if (buff_content == NULL)
        {
            return -1;
        }
        if (read(fd, buff_content, content_size) == -1)
        {
            return -1;
        }
        if (write_tarball(fd_tarball, tarball, buff_content) == -1)
        {
            return -1;
        }
        if (current->next != NULL)
        {
            if (secure_write(fd_tarball, "\n", 1) == -1)
            {
                return -1;
            }
        }
        current = current->next;
        memset(tarball, 0, sizeof(t_tar));
    }
    close(fd_tarball);
    return 0;
}

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

int opt_append(t_tar_file* struct_tar_file)
{
    int fd = secure_open(struct_tar_file->file_name, O_RDWR | O_APPEND);
    if (fd == -1)
    {
        return -1;
    }
    if (secure_write(fd, "\n", 1) == -1)
    {
        return -1;
    }
    if (create_tarball(struct_tar_file, fd) == -1)
    {
        return -1;
    }
    close(fd);
    return 0;
}

int opt_list(t_tar_file* struct_tar_file)
{
    char* filename = struct_tar_file->file_name;
    int fd = secure_open(filename, O_RDONLY);

    t_tar header;
    int nb_read = -1;
    int index = 0;
    int is_null = 0;

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
        
        // if (nb_read != 512)
        // {
        //     write(2, "Incomplete header\n", my_strlen("Incomplete header\n"));
        //     return 1;
        // }
        // printf("toto\n");

        if (header.filename[0] != '\0')
        {
            write(1, header.filename, my_strlen(header.filename));
            write(1, "\n", 1);

            header.filename[my_strlen(header.filename)-1] = 'h';
            write(1, header.filename, my_strlen(header.filename));
            write(1, "\n", 1);
        }
        // printf("toto\n");

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
    int to_take = 0;

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

        to_take = size_for_str;

        if (header.filename[0] != '\0') 
        {
            str = secure_malloc(sizeof(char) * (to_take + 1));
            read(fd, str, to_take);
            str[to_take] = '\0';
            create_files_by_extract(header, str);
        }
    }

    close(fd);
    return 0;
}