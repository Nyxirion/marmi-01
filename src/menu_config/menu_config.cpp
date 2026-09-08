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

/***************************************
 *          BUTTON CONFIG              *
 ***************************************/
Button upButton(UP_PIN);
Button downButton(DOWN_PIN);
Button enterButton(ENTER_PIN);
Button backButton(BACK_PIN);

ButtonAdapter upButtonA(&menu, &upButton, UP, 500, 200);  // hold to repeat
ButtonAdapter downButtonA(&menu, &downButton, DOWN, 500, 200);
ButtonAdapter enterButtonA(&menu, &enterButton, ENTER);
ButtonAdapter backButtonA(&menu, &backButton, BACK);



void setMenu(void){
    renderer.begin();
    menu.setScreen(mainScreen);
};

void initButtons(void){
    upButton.begin();
    downButton.begin();
    enterButton.begin();
    backButton.begin();
}

void buttonObserver(void){
    upButtonA.observe();
    downButtonA.observe();
    enterButtonA.observe();
    backButtonA.observe();
}