#include <core/logger.h>
#include <core/asserts.h>

int main(void) {
    KFATAL("This is a fatal error message: %f", 3.14f);
    KERROR("This is a error message: %f", 3.14f);
    KWARN("This is a warning message: %f", 3.14f);
    KINFO("This is a info message: %f", 3.14f);
    KDEBUG("This is a debug message: %f", 3.14f);
    KTRACE("This is a trace message: %f", 3.14f);

    KASSERT(1 == 0);

    return 0;
}