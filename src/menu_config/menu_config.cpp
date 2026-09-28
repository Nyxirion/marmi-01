#include "menu_config.h"
/***************************************
 *           SCREEN SETUP              *
 ***************************************/

LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, LCD_COLS, LCD_ROWS);
LcdMenu menu(renderer);

// Variables to display and modify the values in the menu, this is shared between all files 
bool pidState;
float temperature;

void pidToggle(bool isOn){
    pidState = isOn;
};

void callbackTemp(const float value){
    Serial.println(value);
};

// clang-format off
MENU_SCREEN(mainScreen, mainItems,
    ITEM_TOGGLE("PID Running", pidToggle),
    ITEM_RANGE("Temp", -1.0f, 0.01f, -1.0f, 1.0f, callbackTemp, "%.2f°C", 2),
    ITEM_VALUE("Temp. Cabina", temperature, "%f°C"),
    
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

void updateDisplay(void){
    menu.poll();
}
