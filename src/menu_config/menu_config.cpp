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

int hum;
int set_hum;

bool systemTest;
bool modo;

// Global structure of the pid
pid_controller_t pid_temp;


void pidToggle(bool isOn){
    pidState = isOn;
};

void callbackTemp(const float value){
    if(modo == 0){
        pid_temp.setpoint = value;
        Serial.println(value);
    }
};

void callbackSkinTemp(const float tempPiel){
    if(modo == 1){
        pid_temp.setpoint = tempPiel;
        Serial.println(tempPiel);
    }
}

void callbackHum(const int value){
    set_hum = value;
};

void callbackSystemTest(bool isOn){
    systemTest = isOn;
}

std::vector<const char*> options = {"Cabina", "Piel"};
void callbackModo(const uint8_t option) {
    modo = option;
    Serial.println(option);
    Serial.println(options[option]);
}



// clang-format off
MENU_SCREEN(mainScreen, mainItems,
    ITEM_TOGGLE("Equipo", pidToggle),
    ITEM_WIDGET(
        "Modo", callbackModo, WIDGET_LIST(options, 0, "%s", 0, true)),
    ITEM_VALUE("Temp.Cabin", temperature, "%.2f\xDF""C"),
    ITEM_VALUE("Hum. Cabin", hum, "%d %%"),
    ITEM_RANGE("Sel.Temp", 35.0f, 0.5f, 30.0f, 37.5f, callbackTemp, "%.2f\xDF""C", 2),
    ITEM_RANGE("Sel.Hum", 60, 5, 40, 80, callbackHum, "%d%%", 2),
    ITEM_RANGE("Sel.Temp.P", 36.0f, 0.1f, 34.0f, 37.5f, callbackSkinTemp, "%.2f\xDF""C", 2),
    ITEM_TOGGLE("System Test", callbackSystemTest),
    
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
