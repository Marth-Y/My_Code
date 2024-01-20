#include "debug.h"

class Tree {
public:
    Tree();
    void TreeLoop(char* path);
private:
    void DfsLoop(char* path, int width);
    int directories_;
    int files_;
};