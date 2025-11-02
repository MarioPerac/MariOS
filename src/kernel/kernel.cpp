#include <stddef.h>
#include <stdint.h>

// Ensure a proper cross-compiler is used, e.g., i686-elf-g++
// This code targets bare-metal (no OS), so host compilers like gcc on Linux are not allowed.
// A correct cross-compiler will define architecture macros like __i686__,
// but NOT OS-specific macros like __linux__
#if defined(__linux__)
#error "You are not using a cross-compiler."
#endif

#if !defined(__x86_64__)
#error "This code requires 64-bit x86_64 architecture."
#endif

// NOTE: Start by reading kernel_main, then work your way up the file.

// --- 5. calculate string length ---
size_t strlen(const char *string)
{
    size_t length = 0;
    while (string[length])
        length++;

    return length;
}

// --- 4. VGA color enumeration ---
// These match the standard 16 VGA text colors (4 bits)
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

// --- 3. VgaTextBuffer ---
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

    static constexpr size_t WIDTH = 80;                        // Characters per row
    static constexpr size_t HEIGHT = 25;                       // Number of rows
    static constexpr size_t TOTAL_CHARACTERS = WIDTH * HEIGHT; // Total screen cells (2000)
    static constexpr uintptr_t VGA_MEMORY_ADDRESS = 0xFFFF8000000B8000;   // VGA text buffer address

    // Pointer to the start of the VGA memory.
    // We treat it as an array of 16-bit values (1 char + 1 color byte per character cell).
    uint16_t *buffer = reinterpret_cast<uint16_t *>(VGA_MEMORY_ADDRESS);

    uint8_t color;

public:
    // Constructor sets default color and clears the screen by filling it with spaces.
    VgaTextBuffer() : color(toVgaCharColor(VgaColor::Black, VgaColor::LightGrey))
    {
        for (size_t i = 0; i < TOTAL_CHARACTERS; i++)
            put(' ');
    }

    void put(char c)
    {
        if (c == '\n')
        {
            row++;
            column = 0;
            return;
        }
        
        buffer[next_position()] = toVgaChar(c, color);
    }

    void setColors(VgaColor bg, VgaColor fg)
    {
        color = toVgaCharColor(bg, fg);
    }

private:
    size_t next_position()
    {
        size_t position = (row * WIDTH) + column;

        if (++column == WIDTH)
        {
            column = 0;
            if (++row == HEIGHT)
                row = 0;
        }

        return position;
    }

    // Construct vga color: |7: blink | 6–4: bg | 3–0: fg |
    uint8_t toVgaCharColor(VgaColor background, VgaColor foreground)
    {
        return (static_cast<uint8_t>(background) << 4) | static_cast<uint8_t>(foreground);
    }

    // Construct vga character: |15–8: color | 7–0: ASCII |
    uint16_t toVgaChar(char c, uint8_t color)
    {
        return (static_cast<uint16_t>(color) << 8) | static_cast<uint16_t>(c);
    }
};

// --- 2. VgaTerminal ---
// This is the high-level interface used by the kernel to print text to the screen.
class VgaTerminal
{
private:
    VgaTextBuffer buffer;

public:
    void write(const char *str)
    {
        size_t length = strlen(str);
        for (size_t i = 0; i < length; i++)
            buffer.put(str[i]);
    }

    void setColors(VgaColor bg, VgaColor fg)
    {
        buffer.setColors(bg, fg);
    }
};

// --- 1. Kernel entry point ---
// This is the first function that GRUB calls after loading the kernel.
extern "C" void kernel_main()
{
    VgaTerminal terminal;
    terminal.setColors(VgaColor::Black, VgaColor::Green);
    terminal.write("Hello, kernel world!\nThe second line works.\nIt works again.");
}