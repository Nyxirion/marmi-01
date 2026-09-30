#pragma once

#include <LiquidCrystal_I2C.h>
#include <LcdMenu.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>
#include <MenuScreen.h>
#include <ItemToggle.h>
#include <ItemRange.h>
#include <ItemValue.h>
#include <Button.h>
#include <input/ButtonAdapter.h>
#include <pid.h>



#define UP_PIN      4
#define DOWN_PIN    36
#define ENTER_PIN   34
#define BACK_PIN    3

#define LCD_COLS 20
#define LCD_ROWS 4

extern LiquidCrystal_I2C lcd;
extern pid_controller_t pid_temp;

extern bool pidState;
extern float temperature;
extern int hum;
extern int set_hum;

void setMenu(void);
void updateDisplay(void);

void initButtons(void);
void buttonObserver(void);