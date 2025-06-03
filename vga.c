#include "vga.h"

// Global variables to track current cursor position
uint16_t column = 0;  // Current horizontal position (0 to width-1)
uint16_t line = 0;    // Current vertical position (0 to height-1)

// Memory-mapped pointer to VGA text buffer at physical address 0xB8000
// This is where the video card reads character/color data from
uint16_t *const vga = (uint16_t *const)0xB8000;

// Default color: light blue text on black background
// Upper 8 bits = background color, lower 8 bits = foreground color
const uint16_t defaultColor = (COLOR8_LIGHT_BLUE << 8) | (COLOR8_BLACK << 12);

// Current color being used for new characters
uint16_t currentColor = defaultColor;

// Function to clear screen and reset cursor to top-left
void Reset()
{
    line = 0;           // Reset cursor to top row
    column = 0;         // Reset cursor to leftmost column
    currentColor = defaultColor;  // Reset color to default

    // Loop through every position on screen
    for (uint16_t y = 0; y < height; y++)
    {
        for (uint16_t x = 0; x < width; x++)
        {
            // Fill each position with space character and default color
            // Each VGA cell is 16 bits: 8 bits char + 8 bits color
            vga[y * width + x] = ' ' | defaultColor;
        }
    }
}

// Function to move cursor to next line
void newLine()
{
    // Check if we're not on the last line
    if (line < height - 1)
    {
        line++;         // Move to next line
        column = 0;     // Reset to beginning of line
    }
    else
    {
        // If on last line, scroll screen up instead
        scrollUp();
        column = 0;     // Reset to beginning of line
    }
}

// Function to scroll entire screen up by one line
void scrollUp()
{
    // Copy each line to the line above it
    // Start from line 1 (second row) to avoid accessing negative memory addresses
    for (uint16_t y = 1; y < height; y++)
    {
        for (uint16_t x = 0; x < width; x++)
        {
            // Copy current line to previous line
            // Line 1 goes to line 0, line 2 goes to line 1, etc.
            vga[(y - 1) * width + x] = vga[y * width + x];
        }
    }

    // Clear the bottom line with spaces
    for (uint16_t x = 0; x < width; x++)
    {
        vga[(height - 1) * width + x] = ' ' | currentColor;
    }
}

// Main function to print a string to screen
void print(const char *s)
{
    // Loop through each character in the string
    while (*s)
    {
        // Handle special characters
        switch (*s)
        {
        case '\n':      // Newline character
            newLine();  // Move to next line
            break;
            
        case '\r':      // Carriage return
            column = 0; // Move cursor to beginning of current line
            break;
            
        case '\t':      // Tab character
            // If at end of line, wrap to next line
            if (column == width)
            {
                newLine();
            }
            
            // Calculate spaces needed to reach next tab stop (4-character intervals)
            uint16_t tabLen = 4 - (column % 4);
            
            // Fill with spaces until next tab stop
            while (tabLen != 0)
            {
                // Write space character with current color at cursor position
                vga[line * width + (column++)] = ' ' | currentColor;
                tabLen--;   // Decrease remaining spaces to write
            }
            break;

        default:        // Regular printable character
            // If at end of line, wrap to next line
            if (column == width) {
                newLine();
            }
            
            // Write character with current color at cursor position
            // OR operation combines character (lower 8 bits) with color (upper 8 bits)
            vga[line * width + (column++)] = *s | currentColor;
            break;
        }
        s++;    // Move to next character in string
    }
}