#include <LedControl.h>

#include <Wire.h>
#include <Servo.h>
#include "Game.h"


#define INVALID_DIRECTION -1

const char keyUp = 'U';
const char keyDown = 'D';
const char keyRight = 'R';
const char keyLeft = 'L';
const char keyNone = 'N';

const int keypadAdderss = 10;

const int boardSizeX = 24;
const int boardSizeY = 8;

Game game(boardSizeX, boardSizeY);

LedControl ledDisplay = LedControl(26, 22, 24, 3);  // (DIN, CLK, CS, Количество дисплеев)

uint64_t lastGameUpdate;
uint64_t lastDisplayUpdate;
uint64_t lastFoodBlink;
const uint64_t gameUpdateDelayMs = 500;
const uint64_t displayUpdateDelayMs = 100;
const uint64_t foodBlinkDelayMs = 200;
bool showingFood = true;

char currentKey1 = keyNone;
char currentKey2 = keyNone;


const byte rowAmount = 4;
const byte colAmount = 4;

char keyMatrix[rowAmount][colAmount] = {
  {keyNone, keyUp,    keyNone,  keyNone},
  {keyRight, keyDown,  keyLeft, keyNone},
  {keyNone, keyNone,  keyNone,  keyNone},
  {keyNone, keyNone,  keyNone,  keyNone}
};

static bool keyDownMatrix[rowAmount][colAmount];

byte rowPins1[rowAmount] = { 45, 44, 43, 42 };
byte colPins1[colAmount] = { 46, 47, 48, 49 };

byte rowPins2[rowAmount] = { 17, 16, 15, 14 };
byte colPins2[colAmount] = { 18, 19, 20, 21 };

const int address = 10;

bool gameStarted;

Servo servo1;
Servo servo2;

void setup()
{
  for (int i=0; i<ledDisplay.getDeviceCount(); i++)
  {
    ledDisplay.shutdown(i, false);
    ledDisplay.setIntensity(i, 10);
    ledDisplay.clearDisplay(i);
  }
  Point point;
  point.x = 1;
  point.y = 2;
  drawPoint(point);
  Serial.begin(115200);
  Serial.println(game.isRunning());


  for (int i = 0; i < rowAmount; i++) {
    pinMode(rowPins1[i], OUTPUT);
    digitalWrite(rowPins1[i], HIGH);
  }

  for (int i = 0; i < colAmount; i++) {
    pinMode(colPins1[i], INPUT);
    digitalWrite(colPins1[i], HIGH);
  }

  for (int i = 0; i < rowAmount; i++) {
    pinMode(rowPins2[i], OUTPUT);
    digitalWrite(rowPins2[i], HIGH);
  }

  for (int i = 0; i < colAmount; i++) {
    pinMode(colPins2[i], INPUT);
    digitalWrite(colPins2[i], HIGH);
  }
  gameStarted=true;
  servo1.attach(10);
  servo2.attach(9);
}

void loop()
{
  currentKey1 = getKey(1);
  currentKey2 = getKey(2);
  if (game.isRunning()) {
    updateInput();
    updateGame();
    updateDisplay();
  }
  if (!game.isRunning() && gameStarted){
    gameStarted=false;
    if (game.isTie())
    {
      servo1.write(90);
      servo2.write(90);
      delay(1000);
      servo1.write(0);
      servo2.write(0);
    }
    else
    {
      if (game.getWinner()==1)
      {
        servo1.write(90);
          delay(1000);
        servo1.write(0);
      }
      else
      {
        servo2.write(90);
          delay(1000);
        servo2.write(0);
      }
      Serial.println(game.getWinner());
    }
  }
  delay(gameUpdateDelayMs);
}

Direction keyToDirection(char key)
{
  switch(key) {
    case keyUp: return UP;
    case keyDown: return DOWN;
    case keyRight: return RIGHT;
    case keyLeft: return LEFT;
  }
  return INVALID_DIRECTION;
}

char getKey(int player)
{
  char result = keyNone;
  for (int i = 0; i < rowAmount; i++) {
    for (int j = 0; j < colAmount; j++) {
      if (isKeyDown(i, j, player)) {
        return keyMatrix[i][j];
      }
    } 
  }
  return keyNone;
}


bool isKeyDown(int i, int j, int player)
{
  if (player==1)
  {
    bool result = false;
  digitalWrite(rowPins1[i], LOW);
  if (digitalRead(colPins1[j]) == LOW) {
    result = true;
  }
  digitalWrite(rowPins1[i], HIGH);
  return result;
  }
  else
  {
    bool result = false;
  digitalWrite(rowPins2[i], LOW);
  if (digitalRead(colPins2[j]) == LOW) {
    result = true;
  }
  digitalWrite(rowPins2[i], HIGH);
  return result;
  }
}


void drawPoint(Point point)
{
  int displayIndex=(int)point.x/8;
  ledDisplay.setLed(displayIndex, boardSizeY - 1 - point.y, point.x-8*displayIndex, true);// передат ьиндекс панели
}

void drawSnake(Snake &snake)
{
  for (int i = 0; i < snake.getSize(); i++) {
    drawPoint(snake.getPosition(i));
  }
}

void drawFood(Point &food)
{
  if (showingFood) {
    drawPoint(food);
  }
  if (millis() - lastFoodBlink > foodBlinkDelayMs) {
    showingFood = !showingFood;
    lastFoodBlink = millis();
  }
}

void updateInput() {
  char key1 = getKey(1);
  char key2 = getKey(2);
  if (key1 != keyNone) {
    currentKey1 = key1;
  }
  if (key2 != keyNone) {
    currentKey2 = key2;
  }
}

void updateGame()
{
  if (millis() - lastGameUpdate > gameUpdateDelayMs) {
    if (currentKey1 != keyNone) {
      game.setSnakeDirection(keyToDirection(currentKey1), 1);
    }
    if (currentKey2 != keyNone) {
      game.setSnakeDirection(keyToDirection(currentKey2), 2);
    }
    game.update();
    currentKey1 = keyNone;
    currentKey2 = keyNone;
    lastGameUpdate = millis();
  }
}

void updateDisplay()
{
  if (millis() - lastDisplayUpdate > displayUpdateDelayMs) {
    Snake snake1 = game.getSnake1();
    Snake snake2 = game.getSnake2();
    Point food = game.getFood();
    for (int i=0; i<ledDisplay.getDeviceCount();i++)
    {
      ledDisplay.clearDisplay(i);
    }
    drawSnake(snake1);
    drawSnake(snake2);
    drawFood(food);
    lastDisplayUpdate = millis();
  }
}
