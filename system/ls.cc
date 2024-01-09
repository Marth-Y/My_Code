#include "ls.h"

void Ls::PrintType() {
   switch (statbuf_.st_mode & S_IFMT)
    {
    case S_IFREG:
      printf("-");
      break;
    case S_IFDIR:
      printf("d");
      break;
    case S_IFBLK:
      printf("b");
      break;
    case S_IFCHR:
      printf("c");
      break;
    case S_IFIFO:
      printf("p");
      break;
    case S_IFLNK:
      printf("l");
      break;
    case S_IFSOCK:
      printf("s");
      break;
    default:
      printf("?");
      break;
    }
}
void Ls::PrintMode()
{
  for (int i = 0; i < 3; ++i)
  {
    int m = (statbuf_.st_mode >> 3 * (2 - i)) & 0x7;
    switch (m)
    {
    case 0:
      printf("---");
      break;
    case 1:
      printf("--x");
      break;
    case 2:
      printf("-w-");
      break;
    case 3:
      printf("-wx");
      break;
    case 4:
      printf("r--");
      break;
    case 5:
      printf("r-x");
      break;
    case 6:
      printf("rw-");
      break;
    case 7:
      printf("rwx");
      break;
    default:
      break;
    }
  }
}
void Ls::PrintUserName() {
    struct passwd *pwd = getpwuid(statbuf_.st_uid);
    ERROR_CHECK(pwd, nullptr, "getpwuid");
    printf(" %s ", pwd->pw_name);
}
void Ls::PrintGroupName() {
    struct group *grp = getgrgid(statbuf_.st_gid);
    ERROR_CHECK(grp, nullptr, "getgrgid");
    printf("%s ", grp->gr_name);
}
void Ls::PrintLinkNumber() {
    printf("%3ld ", statbuf_.st_nlink);

}
void Ls::PrintSize() {
    printf("%8ld ", statbuf_.st_size);
}
void Ls::PrintTime() {
    struct tm *ptime = localtime(&statbuf_.st_atime);
    ERROR_CHECK(ptime, nullptr, "localtime");
    printf("%d/%.2d/%.2d %.2d:%.2d:%.2d ",
           ptime->tm_year + 1900,
           ptime->tm_mon + 1,
           ptime->tm_mday,
           ptime->tm_hour,
           ptime->tm_min,
           ptime->tm_sec);
}
void Ls::PrintFileName() {
    puts(pdirent_->d_name);
}
void Ls::InitStatBuf() {
  int retval = stat(pathname_, &statbuf_);
  ERROR_CHECK(retval, -1, "stat");
}
void Ls::LsLoop() {
  while((pdirent_ = readdir(pdir_)) != nullptr) {
    sprintf(pathname_, "%s/%s", path_.c_str(), pdirent_->d_name);
    InitStatBuf();
    PrintType();
    PrintMode();
    PrintLinkNumber();
    PrintUserName();
    PrintGroupName();
    PrintSize();
    PrintTime();
    PrintFileName();
  }
}

int main(int argc, char *argv[])
{
  Ls ls(argv[1]);
  ls.LsLoop();
}
