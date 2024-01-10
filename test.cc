#include "ls.h"


void TestLs(char* path) {
  Ls ls(path);
  ls.LsLoop();
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: ./ls {path}\n");
        return -1;
    }
    TestLs(argv[1]);
}