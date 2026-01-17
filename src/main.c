#include "my_tar.h"

/**
* @summary main of the programm.
* @param  int ac: number of parameters given.
*         char av: all the parameters that are given.
* @return return an int corresponding to the error.
*/

int main(int ac, char** av)
{
    t_tar_file* struct_tar_file = init_tar_file(ac, av);
    if (struct_tar_file == NULL)
    {
        return 2;
    }
    int return_number = detect_options(struct_tar_file);
    if (return_number == -1)
    {
        write(2, "tar: Options '-Aru' are incompatible with '-f -'\n", my_strlen("tar: Options '-Aru' are incompatible with '-f -'\n"));
        write(2, "Try 'tar --help' or 'tar --usage' for more information.\n", my_strlen("Try 'tar --help' or 'tar --usage' for more information.\n"));
        return 2;
    }
    else if (return_number == -2)
    {
        write(2, "tar: invalid option -- 'e'\n", my_strlen("tar: invalid option -- 'e'\n"));
        write(2, "Try 'tar --help' or 'tar --usage' for more information.\n", my_strlen("Try 'tar --help' or 'tar --usage' for more information.\n"));
    }
    else if (return_number == 2)
    {
        return 2;
    }
    return 0;
}
