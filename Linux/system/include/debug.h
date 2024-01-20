// C
#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
using std::string;

#define ERROR_CHECK(pointer, argv, funcname) \
  do                                         \
  {                                          \
    if ((pointer) == (argv)) {               \
      perror((funcname));                    \
    }                                        \
  } while (false)


#define ARGS_CHECK(argc, number)        \
  do                                    \
  {                                     \
    if ((argc != (number)))             \
    {                                   \
      printf("%d is false!\n", (argc)); \
    }                                   \
  } while (false)
