#include "menu_config.h"
/***************************************
 *           SCREEN SETUP              *
 ***************************************/

LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, LCD_COLS, LCD_ROWS);
LcdMenu menu(renderer);

// clang-format off
MENU_SCREEN(mainScreen, mainItems,
    ITEM_RANGE<int>("Volume", 50, 5, 0, 100, [](const int value) {
        Serial.println(value);
    }, "%d%%")
);
// clang-format on






void setMenu(void){
    renderer.begin();
    menu.setScreen(mainScreen);
};
