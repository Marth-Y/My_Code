#include "debug.h"


class Ls
{
public:
    Ls(const string &path)
    {
        path_ = path;
        pdir_ = opendir(path.c_str());
        ERROR_CHECK(pdir_, nullptr, "opendir");
        errno = 0;
    }
    ~Ls()
    {
        if (errno != 0) {
            perror("readdir");
            exit(1);
        }
        closedir(pdir_);
    }

    void LsLoop();

private:
    void PrintType();
    void PrintMode();
    void PrintUserName();
    void PrintGroupName();
    void PrintLinkNumber();
    void PrintSize();
    void PrintTime();
    void PrintFileName();
    void InitStatBuf();

private:
    DIR *pdir_;
    struct dirent *pdirent_;
    struct stat statbuf_;
    string path_;
    char pathname_[1024];
};