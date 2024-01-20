#include "ls.h"
#include "tree.h"
#include "chmod.h"

void TestLs(char* path) {
  Ls ls(path);
  ls.LsLoop();
}

void TestTree(char* path) {
  Tree tree;
  tree.TreeLoop(path);
}
 void TestChmod(const char* mode, char* pathname) {
  Chmod chmode(mode, pathname);
  chmode.Done();
 }

int main(int argc, char *argv[])
{
    // TestLs(argv[1]);
    // TestTree(argv[1]);
    TestChmod(argv[1], argv[2]);
}