#include <stdio.h>
#include <error.h>

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
