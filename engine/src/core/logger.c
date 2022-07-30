#include "logger.h"
#include "asserts.h"
#include "platform/platform.h"

// TODO: temporary
#include <stdio.h>
#include <string.h>
#include <stdarg.h> // Work with variadic arguments.

// TODO: Do logging in multithreaded batches, faster than disk writing & keep IO support.

b8 initialize_logging() {
    // TODO: Create log file

    return TRUE;
}

void shutdown_logging() {
    // TODO: Cleanup logging/write queued entries.


}

// Not to be used directly, for other logging functions.
void log_output(log_level level, const char* message, ...) {
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
    b8 is_error = level < LOG_LEVEL_WARN;
    // FIXME: b8 is_error = level < 2;

    // FIXME: Technically imposes a 32k character limit on a single log entry, but yk.
    // SILLY, DON'T DO THAT!!!!
    const i32 msg_length = 32000;
    char out_message[msg_length];
    memset(out_message, 0, sizeof(out_message));

    // Format the original message
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[32000];
    sprintf(out_message2, "%s%s\n", level_strings[level], out_message);

    // Platform specific output.
    if(is_error) {
        platform_console_write_error(out_message, level);
    } else {
        platform_console_write(out_message, level);
    }
}

// Not every header file needs a C file, especially for only one method.
void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}