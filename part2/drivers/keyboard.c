#include "io.h"
#include "frame_buffer.h"

#define KEYBOARD_DATA_PORT 0x60

// Track whether the Ctrl key is currently being held down
// This lets us detect key combinations like Ctrl+L for screen clearing
static u8int ctrl_pressed = 0;

// Track if we received an extended scan code prefix (0xE0)
static u8int extended_code = 0;

// Special key codes for arrow keys and tab (returned as high values to distinguish from ASCII)
#define KEY_UP_ARROW 0x80
#define KEY_DOWN_ARROW 0x81
#define KEY_LEFT_ARROW 0x82
#define KEY_RIGHT_ARROW 0x83
#define KEY_TAB 0x09  // Tab is actually ASCII 9, but we'll handle it specially

/** read_scan_code:
 *  Reads a scan code from the keyboard
 *
 *  @return The scan code (NOT an ASCII character!)
 */

u8int keyboard_read_scan_code(void)
{
    return inb(KEYBOARD_DATA_PORT);
}

// Check if user is currently holding down the Ctrl key
u8int keyboard_is_ctrl_pressed(void)
{
    return ctrl_pressed;
}

u8int keyboard_scan_code_to_ascii(u8int scan_code)
{
    // Extended scan code prefix - used for special keys and mouse events
    // When we see 0xE0, the next scan code is part of an extended sequence
    if (scan_code == 0xE0)
    {
        extended_code = 1;
        return 0;
    }

    // If this is part of an extended sequence, check for arrow keys
    if (extended_code)
    {
        extended_code = 0;
        
        // Arrow key scan codes (after 0xE0 prefix)
        switch (scan_code)
        {
            case 0x48: return KEY_UP_ARROW;    // Up arrow
            case 0x50: return KEY_DOWN_ARROW;  // Down arrow
            case 0x4B: return KEY_LEFT_ARROW;  // Left arrow
            case 0x4D: return KEY_RIGHT_ARROW; // Right arrow
            default: return 0; // Other extended keys (mouse, etc.) - ignore
        }
    }

    // When the left Ctrl key is pressed down (scan code 0x1D),
    // remember it so we can detect Ctrl+letter combinations
    if (scan_code == 0x1D)
    {
        ctrl_pressed = 1;
        return 0; // Don't print anything, just track the state
    }

    // When Ctrl is released (scan code 0x9D, which is 0x1D with bit 7 set),
    // clear our flag so regular keys work normally again
    if (scan_code == 0x9D)
    {
        ctrl_pressed = 0;
        return 0;
    }

    // Ignore all key release events (they have bit 7 set, making them > 0x80)
    // We only care when keys are pressed, not when they're let go
    if (scan_code & 0x80)
    {
        return 0;
    }

    // If Ctrl is held and user presses 'L' (scan code 0x26),
    // return a special code (0x0C) that tells the system to clear the screen
    // This mimics the behavior of Ctrl+L in Linux terminals
    if (ctrl_pressed && scan_code == 0x26)
    {
        return 0x0C;
    }

    // Scan code to ASCII mapping for standard US QWERTY keyboard
    switch (scan_code)
    {
    // Numbers row
    case 0x02:
        return '1';
    case 0x03:
        return '2';
    case 0x04:
        return '3';
    case 0x05:
        return '4';
    case 0x06:
        return '5';
    case 0x07:
        return '6';
    case 0x08:
        return '7';
    case 0x09:
        return '8';
    case 0x0A:
        return '9';
    case 0x0B:
        return '0';
    case 0x0C:
        return '-';
    case 0x0D:
        return '=';
    case 0x0E:
        return '\b'; // Backspace
    case 0x0F:
        return KEY_TAB; // Tab key

    // Top letter row
    case 0x10:
        return 'q';
    case 0x11:
        return 'w';
    case 0x12:
        return 'e';
    case 0x13:
        return 'r';
    case 0x14:
        return 't';
    case 0x15:
        return 'y';
    case 0x16:
        return 'u';
    case 0x17:
        return 'i';
    case 0x18:
        return 'o';
    case 0x19:
        return 'p';
    case 0x1A:
        return '[';
    case 0x1B:
        return ']';
    case 0x1C:
        return '\n'; // Enter

    // Middle letter row
    case 0x1E:
        return 'a';
    case 0x1F:
        return 's';
    case 0x20:
        return 'd';
    case 0x21:
        return 'f';
    case 0x22:
        return 'g';
    case 0x23:
        return 'h';
    case 0x24:
        return 'j';
    case 0x25:
        return 'k';
    case 0x26:
        return 'l';
    case 0x27:
        return ';';
    case 0x28:
        return '\'';
    case 0x29:
        return '`';

    // Bottom letter row
    case 0x2B:
        return '\\';
    case 0x2C:
        return 'z';
    case 0x2D:
        return 'x';
    case 0x2E:
        return 'c';
    case 0x2F:
        return 'v';
    case 0x30:
        return 'b';
    case 0x31:
        return 'n';
    case 0x32:
        return 'm';
    case 0x33:
        return ',';
    case 0x34:
        return '.';
    case 0x35:
        return '/';
    case 0x39:
        return ' '; // Space bar

    // Numpad
    case 0x37:
        return '*'; // Numpad *
    case 0x47:
        return '7';
    case 0x48:
        return '8';
    case 0x49:
        return '9';
    case 0x4A:
        return '-';
    case 0x4B:
        return '4';
    case 0x4C:
        return '5';
    case 0x4D:
        return '6';
    case 0x4E:
        return '+';
    case 0x4F:
        return '1';
    case 0x50:
        return '2';
    case 0x51:
        return '3';
    case 0x52:
        return '0';
    case 0x53:
        return '.';

    default:
        return 0; // Unknown scan code
    }
}
