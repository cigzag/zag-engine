#include <core/logger.h>
#include <core/asserts.h>

// TODO: Test
#include <platform/platform.h>

int main(void) {
    KFATAL("This is a fatal error message: %f\n", 3.14f);
    KERROR("This is a error message: %f\n", 3.14f);
    KWARN("This is a warning message: %f\n", 3.14f);
    KINFO("This is a info message: %f\n", 3.14f);
    KDEBUG("This is a debug message: %f\n", 3.14f);
    KTRACE("This is a trace message: %f\n", 3.14f);

    platform_state state;
    if(platform_startup(&state, "Zag Engine Testbed", 100, 100, 1280, 720)) {
        while(TRUE) {
            platform_pump_messages(&state);
        }
    }
    platform_shutdown(&state);

    return 0;
}