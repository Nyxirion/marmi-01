#pragma once

#include <LiquidCrystal_I2C.h>
#include <LcdMenu.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>
#include <MenuScreen.h>
#include <ItemToggle.h>
#include <ItemRange.h>
#include <Button.h>
#include <input/ButtonAdapter.h>

#define LCD_COLS 20
#define LCD_ROWS 4

extern LiquidCrystal_I2C lcd;

void setMenu(void);