/******************************************************
PARAMETERS
******************************************************/
static const float MOUSE_SPEED = 10.0f;
static const bool REVERSE_MOUSE_BUTTONS = true;
static const bool ALLOW_ENABLE_VIA_GAMEPAD = true;
/******************************************************/

#include <Windows.h>
#include <iostream>
#include <thread>       
#include <chrono>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <thread>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct mouse_state
{
    bool left_button_pressed;
    bool right_button_pressed;
};

struct keyboard_state
{
    bool up_arrow_pressed;
    bool down_arrow_pressed;
    bool left_arrow_pressed;
    bool right_arrow_pressed;
    bool space_pressed;
    bool enter_pressed;
    bool win_pressed;
};

struct virtual_state
{
    bool minimize_pressed;
    bool maximize_pressed;
    bool select_all_pressed;
    bool copy_pressed;
    bool paste_pressed;
};

static bool g_should_quit = false;
static bool g_is_enabled = false;
static bool g_js_connected = false;
static int g_js_id = -1;
static mouse_state g_prev_ms_state;
static keyboard_state g_prev_kb_state;
static virtual_state g_prev_vir_state;

using namespace std;

void error_callback(int error, const char* description)
{
    fprintf(stderr, "GL ERROR: %s\n", description);
}

bool check_joystick(int jid)
{
    if (glfwJoystickIsGamepad(jid))
    {
        auto name = string(glfwGetGamepadName(jid));
        transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return tolower(c); });
        auto found = name.find("gamepad");
        if (found == string::npos) return false;
        cout << "Gamepad connected [id=" << jid << ", name=" << glfwGetGamepadName(jid) << "]" << endl;
        g_js_connected = true;
        g_js_id = jid;
        return true;
    } else {
        return false;
    }
}

void check_joysticks()
{
    for (int i = 0; i < 5; i++) {
        if (check_joystick(i)) return;
    }
}

void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED) {
        if (!check_joystick(jid))
            cout << "Joystick connected [id=" << jid << ", name=" << glfwGetGamepadName(jid) << "]" << endl;
    } else if (event == GLFW_DISCONNECTED) {
        if (g_js_connected && g_js_id == jid) {
            cout << "Gamepad disconnected [id=" << jid << "]" << endl;
            g_js_connected = false;
            g_js_id = -1;
        } else {
            cout << "Joystick disconnected [id=" << jid << "]" << endl;
        }
    }
}

void left_mouse_button_pressed()
{
    INPUT mouse[1] = { 0 };
    mouse[0].type = INPUT_MOUSE;
    mouse[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, mouse, sizeof(INPUT));
    g_prev_ms_state.left_button_pressed = true;
}

void left_mouse_button_released()
{
    INPUT mouse[1] = { 0 };
    mouse[0].type = INPUT_MOUSE;
    mouse[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, mouse, sizeof(INPUT));
    g_prev_ms_state.left_button_pressed = false;
}

void right_mouse_button_pressed()
{
    INPUT mouse[1] = { 0 };
    mouse[0].type = INPUT_MOUSE;
    mouse[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, mouse, sizeof(INPUT));
    g_prev_ms_state.right_button_pressed = true;
}

void right_mouse_button_released()
{
    INPUT mouse[1] = { 0 };
    mouse[0].type = INPUT_MOUSE;
    mouse[0].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, mouse, sizeof(INPUT));
    g_prev_ms_state.right_button_pressed = false;
}

void up_arrow_pressed()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_UP;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.up_arrow_pressed = true;
}

void up_arrow_released()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_UP;
    keyboard[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.up_arrow_pressed = false;
}

void down_arrow_pressed()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_DOWN;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.down_arrow_pressed = true;
}

void down_arrow_released()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_DOWN;
    keyboard[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.down_arrow_pressed = false;
}

void left_arrow_pressed()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_LEFT;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.left_arrow_pressed = true;
}

void left_arrow_released()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_LEFT;
    keyboard[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.left_arrow_pressed = false;
}

void right_arrow_pressed()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_RIGHT;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.right_arrow_pressed = true;
}

void right_arrow_released()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_RIGHT;
    keyboard[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.right_arrow_pressed = false;
}

void space_pressed()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_SPACE;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.space_pressed = true;
}

void space_released()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_SPACE;
    keyboard[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.space_pressed = false;
}

void enter_pressed()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_RETURN;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.enter_pressed = true;
}

void enter_released()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_RETURN;
    keyboard[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.enter_pressed = false;
}

void win_pressed()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_LWIN;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.win_pressed = true;
}

void win_released()
{
    INPUT keyboard[1] = { 0 };
    keyboard[0].type = INPUT_KEYBOARD;
    keyboard[0].ki.wVk = VK_LWIN;
    keyboard[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, keyboard, sizeof(INPUT));
    g_prev_kb_state.win_pressed = false;
}

void minimize_pressed()
{
    g_prev_vir_state.minimize_pressed = true;
}

void minimize_released()
{
    POINT p;
    GetCursorPos(&p);
    auto hWnd = WindowFromPoint(p);
    PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    g_prev_vir_state.minimize_pressed = false;
}

void maximize_pressed()
{
    g_prev_vir_state.maximize_pressed = true;
}

void maximize_released()
{
    POINT p;
    GetCursorPos(&p);
    auto hWnd = WindowFromPoint(p);
    PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    g_prev_vir_state.maximize_pressed = false;
}

void select_all_pressed()
{
    g_prev_vir_state.select_all_pressed = true;
}

void select_all_released()
{
    INPUT inputs[4] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'A';

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'A';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    g_prev_vir_state.select_all_pressed = false;
}

void copy_pressed()
{
    g_prev_vir_state.copy_pressed = true;
}

void copy_released()
{
    INPUT inputs[4] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'C';

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'C';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    g_prev_vir_state.copy_pressed = false;
}

void paste_pressed()
{
    g_prev_vir_state.paste_pressed = true;
}

void paste_released()
{
    INPUT inputs[4] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'V';

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    g_prev_vir_state.paste_pressed = false;
}

void check_keyboard_input()
{
    while (true) {
        if (GetKeyState(VK_F12) < 0 && (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0)) {
            while (ShowCursor(TRUE) <= 0);
            g_is_enabled = true;
            check_joysticks();
            cout << "GamepadMouse has been enabled." << endl;
            MessageBeep(MB_ICONASTERISK);
        } else if (GetKeyState(VK_F11) < 0 && (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0)) {
            g_is_enabled = false;
            MessageBeep(MB_ICONERROR);
            cout << "GamepadMouse has been disabled." << endl;
        } else if (GetKeyState(VK_F10) < 0 && (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0)) {
            ShowWindow(GetConsoleWindow(), SW_SHOW);
        } else if (GetKeyState(VK_F9) < 0 && (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0)) {
            g_should_quit = true;
            return;
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (!glfwInit()) {
        cout << "Failed to initialize GLFW!" << endl;
        return -1;
    }

    glfwSetErrorCallback(error_callback);
    glfwSetJoystickCallback(joystick_callback);

    check_joysticks();

    memset(&g_prev_ms_state, 0, sizeof(mouse_state));
    memset(&g_prev_kb_state, 0, sizeof(keyboard_state));
    memset(&g_prev_vir_state, 0, sizeof(virtual_state));

    auto left_bumper = REVERSE_MOUSE_BUTTONS ? GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER : GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
    auto right_bumper = REVERSE_MOUSE_BUTTONS ? GLFW_GAMEPAD_BUTTON_LEFT_BUMPER : GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;

    thread keyboard_input_thread(check_keyboard_input);
    keyboard_input_thread.detach();

    unsigned int dpad_repeat_count = 0;

    while(true) {
        if (g_should_quit) break;

        if (g_is_enabled) {
            glfwPollEvents();

            if (g_js_connected) {
                GLFWgamepadstate state;

                if (!glfwGetGamepadState(g_js_id, &state)) continue;

                if (ALLOW_ENABLE_VIA_GAMEPAD) {
                    if (state.buttons[GLFW_GAMEPAD_BUTTON_X] && state.buttons[GLFW_GAMEPAD_BUTTON_Y] &&
                        state.buttons[GLFW_GAMEPAD_BUTTON_A] && state.buttons[GLFW_GAMEPAD_BUTTON_B]) {
                        g_is_enabled = false;
                        MessageBeep(MB_ICONERROR);
                        cout << "GamepadMouse has been disabled." << endl; 
                        this_thread::sleep_for(chrono::seconds(3));
                        continue;
                    }
                }

                POINT cur_pos;
                if (GetCursorPos(&cur_pos))
                {
                    auto x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] + state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
                    auto y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] + state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
                    if (abs(x) < 0.2) x = 0;
                    if (abs(y) < 0.2) y = 0;
                    auto x_velocity = x * MOUSE_SPEED;
                    auto y_velocity = y * MOUSE_SPEED;
                    SetCursorPos(cur_pos.x + x_velocity, cur_pos.y + y_velocity);
                }

                if (!g_prev_ms_state.left_button_pressed && state.buttons[left_bumper])
                    left_mouse_button_pressed();
                else if (g_prev_ms_state.left_button_pressed && !state.buttons[left_bumper])
                    left_mouse_button_released();

                if (!g_prev_ms_state.right_button_pressed && state.buttons[right_bumper])
                    right_mouse_button_pressed();
                else if (g_prev_ms_state.right_button_pressed && !state.buttons[right_bumper])
                    right_mouse_button_released();

                if (!g_prev_kb_state.up_arrow_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) {
                    dpad_repeat_count = 0;
                    up_arrow_pressed();
                } else if (g_prev_kb_state.up_arrow_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) {
                    dpad_repeat_count++;
                    if (dpad_repeat_count > 50 && dpad_repeat_count % 10 == 0)
                        up_arrow_pressed();
                } else if (g_prev_kb_state.up_arrow_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) {
                    dpad_repeat_count = 0;
                    up_arrow_released();
                }

                if (!g_prev_kb_state.down_arrow_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
                    dpad_repeat_count = 0;
                    down_arrow_pressed();
                } else if (g_prev_kb_state.down_arrow_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
                    dpad_repeat_count++;
                    if (dpad_repeat_count > 50 && dpad_repeat_count % 10 == 0)
                        down_arrow_pressed();
                } else if (g_prev_kb_state.down_arrow_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
                    dpad_repeat_count = 0;
                    down_arrow_released();
                }

                if (!g_prev_kb_state.left_arrow_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) {
                    dpad_repeat_count = 0;
                    left_arrow_pressed();
                } else if (g_prev_kb_state.left_arrow_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) {
                    dpad_repeat_count++;
                    if (dpad_repeat_count > 50 && dpad_repeat_count % 10 == 0)
                        left_arrow_pressed();
                } else if (g_prev_kb_state.left_arrow_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) {
                    dpad_repeat_count = 0;
                    left_arrow_released();
                }

                if (!g_prev_kb_state.right_arrow_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) {
                    dpad_repeat_count = 0;
                    right_arrow_pressed();
                } else if (g_prev_kb_state.right_arrow_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) {
                    dpad_repeat_count++;
                    if (dpad_repeat_count > 50 && dpad_repeat_count % 10 == 0)
                        right_arrow_pressed();
                } else if (g_prev_kb_state.right_arrow_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) {
                    dpad_repeat_count = 0;
                    right_arrow_released();
                }

                if (!g_prev_kb_state.space_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_A])
                    space_pressed();
                else if (g_prev_kb_state.space_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_A])
                    space_released();

                if (!g_prev_kb_state.enter_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_B])
                    enter_pressed();
                else if (g_prev_kb_state.enter_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_B])
                    enter_released();

                if (!g_prev_vir_state.minimize_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_X])
                    minimize_pressed();
                else if (g_prev_vir_state.minimize_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_X])
                    minimize_released();

                if (!g_prev_vir_state.maximize_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_Y])
                    maximize_pressed();
                else if (g_prev_vir_state.maximize_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_Y])
                    maximize_released();

                if (!g_prev_kb_state.win_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_START])
                    win_pressed();
                else if (g_prev_kb_state.win_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_START])
                    win_released();

                if (!g_prev_vir_state.select_all_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_BACK])
                    select_all_pressed();
                else if (g_prev_vir_state.select_all_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_BACK])
                    select_all_released();

                if (!g_prev_vir_state.copy_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB])
                    copy_pressed();
                else if (g_prev_vir_state.copy_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB])
                    copy_released();

                if (!g_prev_vir_state.paste_pressed && state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB])
                    paste_pressed();
                else if (g_prev_vir_state.paste_pressed && !state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB])
                    paste_released();

                this_thread::sleep_for(chrono::milliseconds(5));
                continue;
                
            }
        } else {
            if (ALLOW_ENABLE_VIA_GAMEPAD && g_js_connected) {
                GLFWgamepadstate state;
                if (!glfwGetGamepadState(g_js_id, &state)) continue;

                if (state.buttons[GLFW_GAMEPAD_BUTTON_X] && state.buttons[GLFW_GAMEPAD_BUTTON_Y] &&
                    state.buttons[GLFW_GAMEPAD_BUTTON_A] && state.buttons[GLFW_GAMEPAD_BUTTON_B]) {
                    while (ShowCursor(TRUE) <= 0);
                    g_is_enabled = true;
                    cout << "GamepadMouse has been enabled." << endl;
                    MessageBeep(MB_ICONASTERISK);
                }
            }
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
    
    glfwTerminate();
}
