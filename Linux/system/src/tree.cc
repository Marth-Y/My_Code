#include "tree.h"

Tree::Tree():directories_(0), files_(0){}

void Tree::TreeLoop(char *path)
{
    printf("%s\n", getcwd(NULL, 0));
    DfsLoop(path, 4);
    printf("%d directories %d files\n", directories_, files_);
}
void Tree::DfsLoop(char *path, int width)
{
    DIR* dir = opendir(path);
    struct dirent* pdirent;
    while (pdirent = readdir(dir))
    {
        if ('.' == pdirent->d_name[0])
        {
            continue;
        }
        for (int i = 0; i < width - 1; ++i)
        {
            printf("-");
        }
        printf("%s\n", pdirent->d_name);
        if (pdirent->d_type == DT_DIR)
        {
            ++directories_;
            char name[1024];
            sprintf(name, "%s/%s", path, pdirent->d_name);
            DfsLoop(name, width + 4);
        }
        else
        {
            ++files_;
        }
    }
    closedir(dir);
}
