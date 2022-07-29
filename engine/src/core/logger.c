#include "logger.h"

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
KAPI void log_output(log_level level, const char* message, ...) {
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
    // FIXME: b8 is_error = level < 2;

    // FIXME: Technically imposes a 32k character limit on a single log entry, but yk.
    // SILLY, DON'T DO THAT!!!!
    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    // Format the original message
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[32000];
    sprintf(out_message2, "%s%s\n", level_strings[level], out_message);

    // TODO: Temporary, will become platform specific.
    printf("%s", out_message2); // Print new formatted message with appended tag.
}