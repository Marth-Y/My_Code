#include "chmod.h"


Chmod::Chmod(const char* mode, char* pathname) {
    sscanf(mode, "%o", &mode_);
    pathname_ = pathname;
}
void Chmod::Done() {
    int retval = chmod(pathname_, mode_);
    ERROR_CHECK(retval, -1, "chmod");
}