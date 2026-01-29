#include "terminal.h"
#include "drivers/input_buffer.h"  // For getc() and readline()
#include "drivers/frame_buffer.h"  // For fb_put_char() and fb_clear()
#include "strings.h"                // For strcmp() string comparison

// ===== EXTENSION 3: COLOR SUPPORT =====
// VGA color codes for text (foreground)
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// Color scheme for terminal elements
#define TERM_COLOR_PROMPT COLOR_LIGHT_CYAN     // Prompt "hakimos>" in cyan
#define TERM_COLOR_NORMAL COLOR_LIGHT_GREY     // Normal text in light grey
#define TERM_COLOR_ERROR COLOR_LIGHT_RED       // Error messages in red
#define TERM_COLOR_SUCCESS COLOR_LIGHT_GREEN   // Success messages in green
#define TERM_COLOR_INFO COLOR_YELLOW           // Info/highlights in yellow
#define TERM_COLOR_BG COLOR_BLACK              // Background always black
#define TERM_COLOR_YELLOW COLOR_YELLOW         // Alias for yellow
#define TERM_COLOR_LIGHT_CYAN COLOR_LIGHT_CYAN // Alias for light cyan

// ===== TASK 3: TERMINAL IMPLEMENTATION =====
// This implements a command-line interface similar to a basic Unix shell
// Users can type commands and the terminal parses and executes them

// Maximum length of a command line (including null terminator)
#define MAX_COMMAND_LENGTH 128

// Maximum number of commands we can have (not currently used, but here for future expansion)
#define MAX_COMMANDS 10

// ===== EXTENSION 1: COMMAND HISTORY =====
// Store previously executed commands for recall with up/down arrows
#define HISTORY_SIZE 10  // Number of commands to remember

// Arrow key codes (must match keyboard.c)
#define KEY_UP_ARROW 0x80
#define KEY_DOWN_ARROW 0x81
#define KEY_TAB 0x09  // Tab key for command completion

// History buffer - stores last HISTORY_SIZE commands
static char history[HISTORY_SIZE][MAX_COMMAND_LENGTH];
static int history_count = 0;      // Number of commands in history
static int history_index = 0;      // Current position when browsing history

// Type definition for command handler functions
// Each command is a function that takes a string argument (or NULL if no args)
typedef void (*command_func_t)(char *args);

// Structure that defines a command
// Each command has a name (what user types), description (for help), 
// and a function pointer to execute when command is called
struct command {
    const char *name;           // Command name (e.g., "help", "echo")
    const char *description;    // Human-readable description for help text
    command_func_t function;    // Function to call when command is executed
};

// ===== EXTENSION 4: SIMULATED FILE SYSTEM =====
// Simple in-memory file system for demonstration
struct virtual_file {
    const char *name;
    const char *type;
    const char *content;
};

// Virtual file system - simulated files stored in memory
static struct virtual_file vfs[] = {
    {"readme.txt", "document", "Welcome to HakimOS!\nThis is a simulated file system.\nType 'help' for available commands.\n"},
    {"boot.cfg", "config", "kernel=kernel.bin\nmemory=128MB\nboot_timeout=5\n"},
    {"system.log", "log", "[BOOT] HakimOS starting...\n[INFO] Framebuffer initialized\n[INFO] Interrupts enabled\n[INFO] Terminal ready\n"},
    {"kernel.bin", "binary", "[Binary data - cannot display]\n"},
    {"version.txt", "document", "HakimOS Version 3.5 - Elite Edition\nBuild Date: December 2025\n"},
    {0, 0, 0}  // Sentinel
};

// Forward declarations for command handler functions
// (implementations are below)
static void cmd_help(char *args);
static void cmd_echo(char *args);
static void cmd_clear(char *args);
static void cmd_version(char *args);
static void cmd_shutdown(char *args);
static void cmd_ls(char *args);
static void cmd_cat(char *args);
static void cmd_pwd(char *args);

// TASK 3 REQUIREMENT: Command table using suggested structure
// This is our "command registry" - all available commands are listed here
// The terminal loops through this array to find and execute commands
// To add a new command: (1) write the handler function, (2) add entry here
static struct command commands[] = {
    {"help", "Display available commands", cmd_help},        // Shows all commands
    {"echo", "Display text to screen", cmd_echo},           // Prints user's text
    {"clear", "Clear the screen", cmd_clear},               // Clears screen
    {"version", "Show OS version", cmd_version},            // Shows OS info
    {"ls", "List files in virtual file system", cmd_ls},    // EXTENSION 4: List files
    {"cat", "Display file contents", cmd_cat},              // EXTENSION 4: Show file
    {"pwd", "Print working directory", cmd_pwd},            // EXTENSION 4: Show current dir
    {"shutdown", "Prepare system for shutdown", cmd_shutdown}, // Halts system
    {0, 0, 0} // Sentinel value (all NULLs) marks end of table
};

/**
 * Helper function: Print a string to the framebuffer
 * Walks through string character by character until null terminator
 */
static void print(const char *str)
{
    // Loop until we hit the null terminator (\0) at end of string
    while (*str)
    {
        fb_put_char(*str);  // Print current character
        str++;              // Move to next character
    }
}

/**
 * Helper function: Print a string followed by newline
 * Useful for output formatting - most messages end with newline
 */
static void println(const char *str)
{
    print(str);           // Print the string
    fb_put_char('\n');    // Add newline at the end
}

/**
 * EXTENSION 3: Print string with specific color
 */
static void print_colored(const char *str, u8int color)
{
    fb_set_color(color, TERM_COLOR_BG);
    print(str);
    fb_set_color(TERM_COLOR_NORMAL, TERM_COLOR_BG);  // Reset to normal
}

/**
 * EXTENSION 3: Print string with color and newline
 */
static void println_colored(const char *str, u8int color)
{
    fb_set_color(color, TERM_COLOR_BG);
    println(str);
    fb_set_color(TERM_COLOR_NORMAL, TERM_COLOR_BG);  // Reset to normal
}

/**
 * EXTENSION 1: Add command to history buffer
 * Stores command in circular buffer, oldest commands are overwritten
 */
static void history_add(const char *cmd)
{
    // Don't add empty commands to history
    if (!cmd || !*cmd)
        return;
    
    // Copy command to history buffer at current index
    int idx = history_count % HISTORY_SIZE;
    int i;
    for (i = 0; i < MAX_COMMAND_LENGTH - 1 && cmd[i]; i++)
    {
        history[idx][i] = cmd[i];
    }
    history[idx][i] = '\0';
    
    // Increment count (wraps around after HISTORY_SIZE)
    history_count++;
    
    // Reset browse index to end (most recent)
    history_index = history_count;
}

/**
 * EXTENSION 2: Tab completion for commands
 * Attempts to complete the partial command in the buffer
 * If multiple matches, displays all possibilities
 * Returns: 1 if completion happened, 0 otherwise
 */
static int tab_complete(char *buf, s32int *pos, s32int size)
{
    // Find matching commands
    int match_count = 0;
    int last_match = -1;
    char *partial = buf;
    int partial_len = *pos;
    
    // Don't complete if buffer is empty
    if (partial_len == 0)
        return 0;
    
    // Find all commands that start with the partial input
    for (int i = 0; commands[i].name != 0; i++)
    {
        int match = 1;
        for (int j = 0; j < partial_len; j++)
        {
            if (commands[i].name[j] != partial[j])
            {
                match = 0;
                break;
            }
        }
        if (match)
        {
            match_count++;
            last_match = i;
        }
    }
    
    // No matches
    if (match_count == 0)
    {
        return 0;
    }
    
    // Exactly one match - complete it
    if (match_count == 1)
    {
        const char *cmd_name = commands[last_match].name;
        int cmd_len = strlen(cmd_name);
        
        // Clear current partial command from screen
        for (int i = 0; i < *pos; i++)
        {
            fb_put_char('\b');
        }
        
        // Write full command to buffer and screen
        for (int i = 0; i < cmd_len && i < size - 1; i++)
        {
            buf[i] = cmd_name[i];
            fb_put_char(cmd_name[i]);
        }
        *pos = cmd_len;
        
        // Add a space after completion for convenience
        if (*pos < size - 1)
        {
            buf[*pos] = ' ';
            fb_put_char(' ');
            (*pos)++;
        }
        
        return 1;
    }
    
    // Multiple matches - show all possibilities
    fb_put_char('\n');
    for (int i = 0; commands[i].name != 0; i++)
    {
        int match = 1;
        for (int j = 0; j < partial_len; j++)
        {
            if (commands[i].name[j] != partial[j])
            {
                match = 0;
                break;
            }
        }
        if (match)
        {
            print("  ");
            println_colored(commands[i].name, TERM_COLOR_SUCCESS);
        }
    }
    
    // Redisplay prompt and partial command
    print_colored("hakimos> ", TERM_COLOR_PROMPT);
    for (int i = 0; i < *pos; i++)
    {
        fb_put_char(buf[i]);
    }
    
    return 1;
}

/**
 * EXTENSION 1 & 2: Enhanced readline with history and tab completion
 * Supports up/down arrows to navigate command history
 * Supports Tab key for command completion
 */
static s32int readline_with_history(char *buf, s32int size)
{
    s32int pos = 0;  // Current position in buffer
    u8int c;
    
    while (pos < size - 1)
    {
        c = getc();
        
        // Handle Tab - command completion
        if (c == KEY_TAB)
        {
            tab_complete(buf, &pos, size);
            continue;
        }
        
        // Handle up arrow - go back in history (older commands)
        if (c == KEY_UP_ARROW)
        {
            if (history_index > 0 && history_count > 0)
            {
                history_index--;
                
                // Clear current line on screen
                for (int i = 0; i < pos; i++)
                {
                    fb_put_char('\b');
                }
                
                // Copy history entry to buffer
                int hist_idx = history_index % HISTORY_SIZE;
                pos = 0;
                while (history[hist_idx][pos] && pos < size - 1)
                {
                    buf[pos] = history[hist_idx][pos];
                    fb_put_char(buf[pos]);
                    pos++;
                }
            }
            continue;
        }
        
        // Handle down arrow - go forward in history (newer commands)
        if (c == KEY_DOWN_ARROW)
        {
            if (history_index < history_count)
            {
                history_index++;
                
                // Clear current line on screen
                for (int i = 0; i < pos; i++)
                {
                    fb_put_char('\b');
                }
                
                // If at end of history, show empty line
                if (history_index >= history_count)
                {
                    pos = 0;
                    buf[0] = '\0';
                }
                else
                {
                    // Copy history entry to buffer
                    int hist_idx = history_index % HISTORY_SIZE;
                    pos = 0;
                    while (history[hist_idx][pos] && pos < size - 1)
                    {
                        buf[pos] = history[hist_idx][pos];
                        fb_put_char(buf[pos]);
                        pos++;
                    }
                }
            }
            continue;
        }
        
        // Handle Enter - line complete
        if (c == '\n')
        {
            break;
        }
        
        // Handle backspace
        if (c == '\b')
        {
            if (pos > 0)
            {
                pos--;
            }
            continue;
        }
        
        // Normal character - add to buffer
        buf[pos++] = c;
    }
    
    buf[pos] = '\0';
    return pos;
}

/**
 * TASK 3 REQUIREMENT: help command
 * Display all available commands with their descriptions
 * This helps users discover what the terminal can do
 */
static void cmd_help(char *args)
{
    (void)args; // Suppress "unused parameter" warning (help doesn't use args)
    
    println_colored("Available commands:", TERM_COLOR_INFO);
    
    // Loop through command table until we hit the sentinel (NULL entry)
    for (int i = 0; commands[i].name != 0; i++)
    {
        print("  ");                      // Indent for readability
        print_colored(commands[i].name, TERM_COLOR_SUCCESS);  // Command name in green
        print(" - ");                     // Separator
        println(commands[i].description); // Print what it does
    }
}

/**
 * TASK 3 REQUIREMENT: echo command
 * Display the provided text (arguments) to screen
 * Example: "echo hello world" prints "hello world"
 */
static void cmd_echo(char *args)
{
    // Check if we have arguments (args is NULL if no arguments provided)
    if (args && *args)  // args exists AND first char isn't null terminator
    {
        println(args);  // Print whatever the user typed after "echo"
    }
    else
    {
        // No arguments - just print a blank line (like Unix echo)
        fb_put_char('\n');
    }
}

/**
 * TASK 3 REQUIREMENT: clear command
 * Clear the screen (like Unix 'clear' command)
 */
static void cmd_clear(char *args)
{
    (void)args; // Suppress warning - clear doesn't need arguments
    fb_clear(); // Call framebuffer function to clear screen
}

/**
 * TASK 3 REQUIREMENT: version command
 * Display OS version and author information
 */
static void cmd_version(char *args)
{
    (void)args; // Suppress warning - version doesn't need arguments
    
    // Display customized version information with colors
    println_colored("HakimOS Version 3.5 - Elite Edition", TERM_COLOR_INFO);
    println_colored("Developed by Chouaib Hakim", TERM_COLOR_SUCCESS);
    println("Built for UFCFWK-15-2");
}

/**
 * TASK 3 REQUIREMENT: shutdown command
 * Prepare system for shutdown and halt the CPU
 * In a real OS, this would close files, sync disks, etc.
 */
static void cmd_shutdown(char *args)
{
    (void)args; // Suppress warning - shutdown doesn't need arguments
    
    // Display shutdown message to user in red (warning color)
    println_colored("System is shutting down...", TERM_COLOR_ERROR);
    println_colored("It is now safe to turn off your computer.", TERM_COLOR_SUCCESS);
    
    // Halt the system permanently
    while (1)
    {
        // cli = disable interrupts, hlt = halt CPU
        // CPU will stay halted until hardware reset
        __asm__ volatile("cli; hlt");
    }
}

/**
 * EXTENSION 4: ls command - List files in virtual file system
 * Shows all files with their types, similar to Unix ls
 */
static void cmd_ls(char *args)
{
    (void)args; // ls doesn't use arguments in this simple implementation
    
    println_colored("Files in /:", TERM_COLOR_INFO);
    
    // Loop through virtual file system and display each file
    for (int i = 0; vfs[i].name != 0; i++)
    {
        print("  ");
        print_colored(vfs[i].name, TERM_COLOR_SUCCESS);
        
        // Add some spacing
        int padding = 20 - strlen(vfs[i].name);
        for (int j = 0; j < padding && j < 20; j++)
        {
            fb_put_char(' ');
        }
        
        print("[");
        print(vfs[i].type);
        println("]");
    }
}

/**
 * EXTENSION 4: cat command - Display contents of a file
 * Takes filename as argument and shows its contents
 */
static void cmd_cat(char *args)
{
    // Check if filename was provided
    if (!args || !*args)
    {
        println_colored("Usage: cat <filename>", TERM_COLOR_ERROR);
        println("Example: cat readme.txt");
        return;
    }
    
    // Search for the file in virtual file system
    for (int i = 0; vfs[i].name != 0; i++)
    {
        if (strcmp(args, vfs[i].name) == 0)
        {
            // File found - display its contents
            print(vfs[i].content);
            return;
        }
    }
    
    // File not found
    print_colored("cat: ", TERM_COLOR_ERROR);
    print(args);
    println(": No such file");
    println_colored("Use 'ls' to see available files", TERM_COLOR_INFO);
}

/**
 * EXTENSION 4: pwd command - Print working directory
 * Shows current directory (always "/" in this simple implementation)
 */
static void cmd_pwd(char *args)
{
    (void)args; // pwd doesn't use arguments
    println("/");
}

/**
 * TASK 3 REQUIREMENT: Parse input into command and arguments
 * This implements the command parsing steps from the requirements:
 * 1. Identify the command (first word)
 * 2. Separate arguments (remaining text)
 * 
 * IMPORTANT: This function modifies the input string by adding null terminators
 * 
 * Parameters:
 *   input - the input string (WILL BE MODIFIED - null terminators inserted)
 *   args - output parameter, will point to arguments (or NULL if none)
 * 
 * Returns: pointer to the command name (first word of input)
 */
static char *parse_command(char *input, char **args)
{
    char *cmd = input;  // Start at beginning of input
    
    // Skip any leading whitespace (spaces/tabs before command)
    // Handles cases like "  help" where user typed spaces first
    while (*cmd == ' ' || *cmd == '\t')
    {
        cmd++;  // Move forward past whitespace
    }
    
    // Clean up the input: remove any trailing newline character
    // (readline doesn't include it, but just to be safe)
    char *newline = cmd;
    while (*newline && *newline != '\n')
    {
        newline++;  // Find the newline
    }
    if (*newline == '\n')
    {
        *newline = '\0';  // Replace newline with null terminator
    }
    
    // Find the end of the command word (first space or tab)
    // Everything before this is the command, everything after is arguments
    char *p = cmd;
    while (*p && *p != ' ' && *p != '\t')
    {
        p++;  // Move forward until we hit whitespace or end of string
    }
    
    // If we found a separator (space/tab), there might be arguments
    if (*p)
    {
        *p = '\0'; // Null-terminate the command (splits string in two)
        p++;       // Move past the null terminator to start of arguments
        
        // Skip any whitespace between command and arguments
        // Handles "echo    hello" (multiple spaces)
        while (*p == ' ' || *p == '\t')
        {
            p++;
        }
        
        // Set args pointer to start of arguments
        // If p points to null terminator (no args after spaces), set args to NULL
        *args = (*p) ? p : 0;
    }
    else
    {
        // No separator found - command has no arguments
        *args = 0;
    }
    
    // Return pointer to command name (now null-terminated)
    return cmd;
}

/**
 * TASK 3 REQUIREMENT: Look up command in command table and execute it
 * This implements: "Look up command in your command table" and 
 * "Pass arguments to appropriate handler"
 * 
 * Parameters:
 *   cmd - command name (what user typed)
 *   args - arguments (may be NULL if no arguments)
 * 
 * Returns: 1 if command found and executed, 0 if not found
 */
static int execute_command(char *cmd, char *args)
{
    // Handle empty command (user just pressed Enter)
    // This is graceful handling of empty input (TASK 3 requirement)
    if (!cmd || !*cmd)
    {
        return 1;  // Return success (not an error, just nothing to do)
    }
    
    // Search through command table to find matching command
    // Loop until we hit the sentinel value (NULL name)
    for (int i = 0; commands[i].name != 0; i++)
    {
        // Compare user's input with command name (case-sensitive)
        if (strcmp(cmd, commands[i].name) == 0)
        {
            // Found it! Call the command's handler function
            // Pass the arguments (or NULL if no args)
            commands[i].function(args);
            return 1;  // Success - command was executed
        }
    }
    
    // Command not found in table
    return 0;  // Signals caller to display "unknown command" message
}

/**
 * Initialize the terminal system
 * Called once at boot to set up the terminal and display welcome message
 */
void terminal_init(void)
{
    fb_clear();  // Start with clean screen
    
    // Set normal text color
    fb_set_color(TERM_COLOR_NORMAL, TERM_COLOR_BG);
    
    // Display welcome banner with colors
    println_colored("========================================", TERM_COLOR_LIGHT_CYAN);
    println_colored("      Welcome to HakimOS Terminal", TERM_COLOR_YELLOW);
    println_colored("========================================", TERM_COLOR_LIGHT_CYAN);
    println_colored("Type 'help' for available commands", TERM_COLOR_SUCCESS);
    fb_put_char('\n');  // Extra blank line for spacing
}

/**
 * TASK 3 REQUIREMENT: Main terminal loop
 * This implements all the command processing requirements:
 * - Display a prompt
 * - Accept user input until enter is pressed
 * - Parse input into command and arguments
 * - Execute appropriate function based on command
 * - Handle unknown commands gracefully
 * 
 * EXTENSION 1: Added command history with up/down arrow navigation
 * 
 * This function never returns - it's the main loop of our OS
 */
void terminal_run(void)
{
    char input[MAX_COMMAND_LENGTH];  // Buffer for user's input line
    char *cmd;                       // Will point to command name
    char *args;                      // Will point to arguments (or NULL)
    
    // Infinite loop - terminal runs forever until shutdown command
    while (1)
    {
        // TASK 3 + EXTENSION 3: Display a prompt with color
        print_colored("hakimos> ", TERM_COLOR_PROMPT);
        
        // Reset history browsing to end (ready for new command)
        history_index = history_count;
        
        // TASK 3 + EXTENSION 1: Accept user input with history support
        // readline_with_history() blocks until Enter, handles backspace and arrows
        readline_with_history(input, MAX_COMMAND_LENGTH);
        
        // TASK 3: Parse input into command and arguments
        // This modifies input buffer (adds null terminators)
        cmd = parse_command(input, &args);
        
        // EXTENSION 1: Add command to history before executing
        // (Skip empty commands - handled in history_add)
        history_add(input);
        
        // TASK 3: Execute appropriate function based on command
        // Returns 0 if command not found, 1 if executed
        if (!execute_command(cmd, args))
        {
            // TASK 3: Handle unknown commands gracefully with colored error
            // Friendly error message instead of crashing
            print_colored("Unknown command: ", TERM_COLOR_ERROR);
            println(cmd);
            println_colored("Type 'help' for available commands", TERM_COLOR_INFO);
        }
        
        // Loop continues - prompt appears again for next command
    }
}
