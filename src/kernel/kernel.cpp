#include "vga_terminal.hpp"

// This is the first function that GRUB calls after loading the kernel.
extern "C" void kernel_main()
{
    VgaTerminal terminal;
    terminal.setColors(VgaColor::Black, VgaColor::Green);
    terminal.write(
        "Line 1: Welcome to the VGA text mode test program.\n"
        "Line 2: This text is used to check scrolling behavior.\n"
        "Line 3: Each line is counted so we know the total.\n"
        "Line 4: The VGA screen typically shows 25 lines, but\n"
        "Line 5: for our test we will consider only 20 visible lines.\n"
        "Line 6: When the text exceeds the visible limit, scrolling should occur.\n"
        "Line 7: Scrolling means older lines move up to make space.\n"
        "Line 8: This helps in console applications to see the latest output.\n"
        "Line 9: Each new line added past the limit triggers a scroll.\n"
        "Line 10: Notice how the first line will disappear first.\n"
        "Line 11: We are writing test lines to observe the behavior.\n"
        "Line 12: The content of each line is not important.\n"
        "Line 13: What matters is that enough lines exist to scroll.\n"
        "Line 14: As you continue adding lines, the screen should update.\n"
        "Line 15: VGA memory starts at 0xB8000 in text mode.\n"
        "Line 16: Each character cell is 2 bytes: ASCII + color.\n"
        "Line 17: Writing beyond the visible area requires shifting memory.\n"
        "Line 18: This simulates a real console or terminal behavior.\n"
        "Line 19: The user should be able to see the newest lines at the bottom.\n"
        "Line 20: Older lines are pushed out as new ones arrive.\n"
        "Line 21:\n"
        "Line 22:\n"
        "Line 23:\n"
        "Line 24:\n"
        "Line 25:\n"
        "Line 26: Line 26 confirms that scrolling works beyond 25 lines.");
}