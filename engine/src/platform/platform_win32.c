#include "platform/platform.h"

// WINDOWS PLATFORM LAYER!@!!, the penguin gets its own iceberg to stand on.
#if KPLATFORM_WINDOWS

#include "core/logger.h"

#include <windows.h>
#include <windowsx.h> // parameter input extraction(?)
#include <stdlib.h>

typedef struct internal_state {
    HINSTANCE h_instance; // windows specific
    HWND hwnd;
} internal_state;

// Clock
static f64 clock_frequency; // multiplier for clock cycle from OS and multiply to get real time.
static LARGE_INTEGER start_time; // starting time of application

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param); // windows specific stuff

b8 platform_startup(
    platform_state* plat_state,
    const char* application_name, // for windowed OS's
    i32 x,
    i32 y,
    i32 width,
    i32 height) {

    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state *state = (internal_state *)plat_state->internal_state;

    state->h_instance = GetModuleHandleA(0); // give me handle to application running this code.

    // create window class, then register it, if it fails, we need to figure out why, if not, we pass through info to create our window.
    // reference https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassa. we move away from this as much as we can.

    // let's do it
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION); // TODO: Needs to be changed later, obviously change icon here.
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS; // We want them double clicks baby.
    wc.lpfnWndProc = win32_process_message; // handles events in windows system, we need to handle those.
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance; // point to instance we created.
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // NULL; Manage the cursor manually.
    wc.hbrBackground = NULL;    // Transparent window
    wc.lpszClassName = "zag_window_class"; // Class name we register, remember this, silly goose.

    // registeration time!
    if(!RegisterClassA(&wc)) {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // With windows, there is a full box rectangle of the window, but also the client area, which is smaller than the entire window.
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;

    // Set main window size to client size for now.
    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    // information for windows styling, see https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    // Grab the size of the border.
    RECT border_rect = {0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    // Border rectangle will be negative.
    window_x += border_rect.left;
    window_y += border_rect.top;

    // Add the size by the OS border
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    // Actually create the window, pass required parameters we did above for window to be made.
    HWND handle = CreateWindowExA(
        window_ex_style, "zag_window_class", application_name,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, state->h_instance, 0);

    if(handle == 0) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        //KFATAL("Window creation failed!"); // Gives linking errors, am I missing something here?
        return FALSE;
    } else {
        state->hwnd = handle;
    }

    // Show the window
    // Windows requires flags to be passed to show the window, allowing for default state of how the window will be shown on the OS.
    b32 should_activate = 1; // TODO: If the window should not accept input, this should be false.
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE;
    ShowWindow(state->hwnd, show_window_command_flags);

    // Setup the Clock
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (f64)frequency.QuadPart; // convert to f64
    QueryPerformanceCounter(&start_time); // give us snapshot of current time when application starts.

    return TRUE; // If we get to this point, we are done here.
}

void platform_shutdown(platform_state* plat_state) {
    internal_state *state = (internal_state *)plat_state->internal_state;

    if(state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

b8 platform_pump_messages(platform_state* plat_state) {
    MSG message; // Why does windows make us create a message construct?, I don't know, Microsoft moment.
    while(PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) { // Fill message construct with info, Clear out message queueto ensure window doesn't become non-responsive.
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return TRUE;
}

void* platform_allocate(u64 size, b8 alligned) {
    return malloc(size); // TODO: temporary, we don't wanna be using the standard library.
}

void platform_free(void* block, b8 alligned) {
    free(block); // TODO: temporary again.
}

// platform_zero_memory is for convience, C does not have a zero memory function
void* platform_zero_memory(void* block, u64 size) {
    return memset(block, 0, size); 
}

void* platform_copy_memory(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size); // TODO: temporary
}

void* platform_set_memory(void* dest, i32 value, u64 size) {
    return memset(dest, value, size); // TODO: temporary
}

// We do this because this way allows colours and other cool stuff, so we may aswell utilize it.
void platform_console_write(const char* message, u8 colour) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8}; // Thanks Travis Vroman :)
    SetConsoleTextAttribute(console_handle, levels[colour]); // Documentation says it's outdated but CMD is default, so no. See: https://docs.microsoft.com/en-us/windows/console/setconsoletextattribute

    OutputDebugStringA(message); // Output to debug console and terminal.
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
}

void platform_console_write_error(const char* message, u8 colour) {
     HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8}; 
    SetConsoleTextAttribute(console_handle, levels[colour]);
    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0); // Write to different stream, utilize for errors.
}

f64 platform_get_absolute_time() {
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (f64)now_time.QuadPart * clock_frequency; // Get time in seconds.
}

void platform_sleep(u64 ms) {
    Sleep(ms); // Windows does something simple, for once.
}

// SEE https://docs.microsoft.com/en-us/windows/win32/winmsg/window-notifications, Windows makes me want to close my eyes sometimes.
LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
    switch(msg) {
        case WM_ERASEBKGND:
            // Notify the OS that erasing will be handled by the application to prevent flicker.
            return 1;
        case WM_CLOSE:
            // TODO: Fire an event for the application to quit.
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0); // Posts WM_QUIT
            return 0;
        case WM_SIZE: {
            // Get updated size of window
            //RECT r;
            //GetClientRect(hwnd, &r);
            //u32 width = r.right - r.left;
            //u32 height = r.bottom - r.top;

            // TODO: Fire an event for window resize.
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // Key pressed/released
            //b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            // TODO: Input processing
        } break;
        case WM_MOUSEMOVE: {
           // i32 x_position = GET_X_LPARAM(l_param);
           // i32 y_position = GET_YLPARAM(l_param);
            // TODO: Input processsing again.
        } break;
        case WM_MOUSEWHEEL: {
           // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
           // if(z_delta != 0) { // Mouse wheel data, Windows does this weird, we only need to know if it goes up or down, not anything special.
                // Flatten the input to OS independent (-1, 1).
           //     z_delta = (z_delta < 0) ? -1 : 1;
                // TODO: Input processing again.......
          //  }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            // b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            // TODO: You already know what's going here. (Input processing)
        } break;
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param); // Let the windows default procedure handle this.
}

#endif // KPLATFORM_WINDOWS