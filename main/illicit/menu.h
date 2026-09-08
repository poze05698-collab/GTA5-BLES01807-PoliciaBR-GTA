#ifndef POLICIA_BR_MENU_H
#define POLICIA_BR_MENU_H

#include "includes.h"
#include "types.h"
#include "colors.h"

namespace Menu_f
{
    extern bool isOpen;
    extern int currentOption;
    extern int currentMenu;
    extern int subOptionCount[32];

    void createMenuStruct();
    void drawMenu();
    void updateInput();
    void closeMenu();
}

#endif
