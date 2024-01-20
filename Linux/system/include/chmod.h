#include "debug.h"

class Chmod {
public:
    Chmod(const char* mode, char* pathname);
    void Done();
private:
    mode_t mode_;
    char* pathname_;
};