#include "my_tar.h"

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

t_tar_file* init_struct_tar() 
{
    t_tar_file* buffer = (t_tar_file*)secure_malloc(sizeof(t_tar_file));
    if (buffer == NULL)
    {
        return NULL;
    }
    return buffer;
}

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

int check_flag_f(int flag)
{
    if (flag & OPT_FILE)
    {
        return 1;
    }
    return 0;
}

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