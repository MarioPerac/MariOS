#include "vga_terminal.hpp"

VgaTextBuffer::VgaTextBuffer() : color(toVgaCharColor(VgaColor::Black, VgaColor::LightGrey))
{
    for (size_t i = 0; i < TOTAL_CHARACTERS; i++)
        put(' ');
}

void VgaTextBuffer::put(char c)
{
    if (c == '\n')
    {
        if (row == HEIGHT - 1)
            scroll_up();
        else
            row++;
        column = 0;
        return;
    }

    buffer[next_position()] = toVgaChar(c, color);
}

void VgaTextBuffer::setColors(VgaColor bg, VgaColor fg)
{
    color = toVgaCharColor(bg, fg);
}

void VgaTextBuffer::scroll_up()
{
    for (size_t i = 0; i < (HEIGHT - 1) * WIDTH; i++)
        buffer[i] = buffer[i + WIDTH];

    for (size_t i = (HEIGHT - 1) * WIDTH; i < TOTAL_CHARACTERS; i++)
        buffer[i] = toVgaChar(' ', color);
}

size_t VgaTextBuffer::next_position()
{
    size_t position = (row * WIDTH) + column;

    if (++column == WIDTH)
    {
        column = 0;
        if (++row == HEIGHT)
        {
            row--;
        }
    }

    return position;
}

uint8_t VgaTextBuffer::toVgaCharColor(VgaColor background, VgaColor foreground)
{
    return (static_cast<uint8_t>(background) << 4) | static_cast<uint8_t>(foreground);
}

uint16_t VgaTextBuffer::toVgaChar(char c, uint8_t color)
{
    return (static_cast<uint16_t>(color) << 8) | static_cast<uint16_t>(c);
}

void VgaTerminal::write(const char *str)
{
    size_t length = strlen(str);
    for (size_t i = 0; i < length; i++)
        buffer.put(str[i]);
}

void VgaTerminal::setColors(VgaColor bg, VgaColor fg)
{
    buffer.setColors(bg, fg);
}