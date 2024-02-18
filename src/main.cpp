#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "OneButton.h"

#define TFT_WIDTH 170
#define TFT_HEIGHT 320

#define UP_BUTTON 0
#define DOWN_BUTTON 14

#define LONG_PRESS_INTERVAL 500

OneButton buttonUp(UP_BUTTON, true);
OneButton buttonDown(DOWN_BUTTON, true);

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite binary_sprite = TFT_eSprite(&tft);
TFT_eSprite decimal_sprite = TFT_eSprite(&tft);

bool binary_array[8] = {1, 1, 1, 1, 1, 1, 1, 1}; // 二进制数组，最高有效位（MSB）在前，最低有效位（LSB）在后
short binary_idx = 0;                            // 当前选中的位

// 更新二进制显示
void updateBinarySprite()
{
  binary_sprite.fillSprite(TFT_BLACK);
  binary_sprite.setTextSize(6);
  binary_sprite.setTextColor(TFT_WHITE);
  for (int i = 0; i < 8; i++)
  {
    binary_sprite.drawString(String(binary_array[i]), 34 * i, 0);
  }

  binary_sprite.fillRect(3 + 34 * (7 - binary_idx), 45, 25, 5, TFT_GREEN);

  binary_sprite.pushSprite(10, 30);
}

// 更新十进制显示
void updateDecimalSprite()
{
  decimal_sprite.fillSprite(TFT_LIGHTGREY);
  decimal_sprite.setTextSize(6);
  decimal_sprite.setTextDatum(MC_DATUM);
  decimal_sprite.setTextColor(TFT_BLACK);
  int decimalNum = 0;
  for (int i = 0; i < 8; i++)
  {
    decimalNum += binary_array[i] * pow(2, 7 - i);
  }
  decimal_sprite.drawNumber(decimalNum, decimal_sprite.width() / 2, decimal_sprite.height() / 2);

  decimal_sprite.pushSprite(10, 100);
}

// 位转换
void binaryBitSwap()
{
  binary_array[(7 - binary_idx)] = !binary_array[(7 - binary_idx)];
  updateBinarySprite();
  updateDecimalSprite();
}

// 画转换原理提示
void drwaMultiplies()
{
  tft.setTextColor(TFT_WHITE);
  for (int i = 7; i >= 0; i--)
  {
    tft.setTextSize(2);
    tft.drawString("2", 34 * (7 - i), 0);
    tft.setTextSize(1);
    tft.drawString(String(i), 11 + 34 * (7 - i), 0);
    tft.setTextSize(1);
    tft.drawString("x", 11 + 34 * (7 - i), 20);
    if (i != 0)
    {
      tft.setTextSize(2);
      tft.drawString("+", 18 + 34 * (7 - i), 0);
    }
  }
}

/***** 按钮事件相关方法 *****/
void upBtnClick()
{
  binary_idx++;
  if (binary_idx > 7)
  {
    binary_idx = 0;
  }
  updateBinarySprite();
}

void upBtnDuringLongPress(void *oneButton)
{
  upBtnClick();
}

void upBtnDoubleClick()
{
  binaryBitSwap();
}

void downBtnClick()
{
  binary_idx--;
  if (binary_idx < 0)
  {
    binary_idx = 7;
  }
  updateBinarySprite();
}

void downBtnDoubleClick()
{
  binaryBitSwap();
}

void downBtnDuringLongPress(void *oneButton)
{
  downBtnClick();
}
/***** 按钮事件相关方法（end) *****/

void setup()
{
  Serial.begin(115200);

  // 设置按钮事件
  buttonUp.attachClick(upBtnClick);
  buttonUp.attachDoubleClick(upBtnDoubleClick);
  buttonUp.attachDuringLongPress(upBtnDuringLongPress, &buttonUp);
  buttonUp.setLongPressIntervalMs(LONG_PRESS_INTERVAL);

  buttonDown.attachClick(downBtnClick);
  buttonDown.attachDoubleClick(downBtnDoubleClick);
  buttonDown.attachDuringLongPress(downBtnDuringLongPress, &buttonDown);
  buttonDown.setLongPressIntervalMs(LONG_PRESS_INTERVAL);

  // 初始化屏幕
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  binary_sprite.createSprite(300, 50);
  decimal_sprite.createSprite(300, 70);

  drwaMultiplies();
  updateBinarySprite();
  updateDecimalSprite();
}

void loop()
{
  buttonUp.tick();
  buttonDown.tick();
  delay(10);
}
