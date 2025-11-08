#pragma once

#include <stdint.h>
#include <stddef.h>
#include "string.hpp"

enum class VgaColor : uint8_t
{
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGrey = 7,
    DarkGrey = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    LightBrown = 14,
    White = 15,
};

// The VGA text buffer is simply a special area of memory that the graphics hardware reads to show characters on your screen in text mode.
class VgaTextBuffer
{
private:
    // The VGA text-mode screen has a resolution of 80 columns (characters per row)
    // and 25 rows (lines on screen), for a total of 2000 visible character positions.
    // Each character cell consists of two bytes in memory:
    //   - 1 byte for the ASCII character (e.g., 'A')
    //   - 1 byte for the color attributes (foreground + background colors)
    //
    // The video buffer is memory-mapped at physical address 0xB8000,
    // which is a special location reserved for VGA text mode.
    //
    // We use this memory to directly control what is displayed on the screen.

    size_t row = 0;
    size_t column = 0;

    static constexpr size_t WIDTH = 80;                                 // Characters per row
    static constexpr size_t HEIGHT = 25;                                // Number of rows
    static constexpr size_t TOTAL_CHARACTERS = WIDTH * HEIGHT;          // Total screen cells (2000)
    static constexpr uintptr_t VGA_MEMORY_ADDRESS = 0xB8000; // VGA text buffer address

    // Pointer to the start of the VGA memory.
    // We treat it as an array of 16-bit values (1 char + 1 color byte per character cell).
    uint16_t *buffer = reinterpret_cast<uint16_t *>(VGA_MEMORY_ADDRESS);

    uint8_t color;

public:
    // Constructor sets default color and clears the screen by filling it with spaces.
    VgaTextBuffer();

    void put(char c);
    void setColors(VgaColor bg, VgaColor fg);
    void scroll_up();

private:
    size_t next_position();

    // Construct vga color: |7: blink | 6–4: bg | 3–0: fg |
    uint8_t toVgaCharColor(VgaColor background, VgaColor foreground);

    // Construct vga character: |15–8: color | 7–0: ASCII |
    uint16_t toVgaChar(char c, uint8_t color);
};

// This is the high-level interface used by the kernel to print text to the screen.
class VgaTerminal
{
private:
    VgaTextBuffer buffer;

public:
    void write(const char *str);
    void setColors(VgaColor bg, VgaColor fg);
};