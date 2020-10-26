#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);
byte cursorChar[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

const byte backSpaceBtn = 2, delBtn = 3;

char page[40];
int pagePos = 0;
unsigned long blinkTimer = 0;
int buttonValue;
char btnValueStr[3];
char buff[7];
int curX = 0, curY = 0, curXOld = 0, curYOld = 0;



void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, cursorChar);

  Serial.begin(9600);
  
  blinkTimer = 0;

  for (int i = 0; i < 40; i++) {
    page[i] = ' ';
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(backSpaceBtn, INPUT_PULLUP);
  pinMode(delBtn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(backSpaceBtn), backSpaceFunc, FALLING);
  attachInterrupt(digitalPinToInterrupt(delBtn), delBtnFunc, FALLING);
}


void backSpaceFunc(){
  bool del = false;
  
  if (curX == 0 && curY == 1) {
    curX = 19;
    curY--;
    pagePos--;
    del = true;
  } else if (curX > 0) {
    curX--;
    pagePos--;
    del = true;
  }
  if (del) {
    for (int i = curY * 20 + curX + 1; i < 40; i++){
      page[i-1] = page[i];
      page[i] = ' ';
    }
  }
}

void delBtnFunc(){
  bool del = true;
  if (del) {
    for (int i = curY * 20 + curX + 1; i < 39; i++){
      page[i] = page[i+1];
      page[i+1] = ' ';
    }
  }
}

void arrowKeys() {
  buttonValue = analogRead(A0);
  if (buttonValue < 20){ // RIGHT
    if (curX == 19 && curY == 0) {
      curX = 0;
      curY = 1;
      pagePos = 20;
    } else if (curX < 20) {
      curX++;
      pagePos++;
    }
    curX++;
    pagePos++;
    while(buttonValue == analogRead(A0));
  } else if (buttonValue > 510 && buttonValue < 550){  //UP
    if (curY == 1){
      curY--;
      pagePos -=20;
    }
    while(buttonValue == analogRead(A0));
  } else if (buttonValue > 680 && buttonValue < 710){  //DOWN
    if (curY == 0){
      curY++;
      pagePos +=20;
    }
    while(buttonValue == analogRead(A0));
  } else if (buttonValue > 890 && buttonValue < 930){  //LEFT
    if (curX == 0 && curY == 1) {
      curX = 19;
      curY = 0;
      pagePos = 19;
    } else if (curX > 0) {
      curX--;
      pagePos--;
    }
    while(buttonValue == analogRead(A0));
  }

  if (curX < 0) curX = 0;
  if (curX > 19) curX = 19;
  if (curY < 0) curY = 0;
  if (curY > 3) curY = 3;
}

void writeText() {
  if ((curX != curXOld) || (curY != curYOld)){
    lcd.setCursor(curXOld, curYOld);
    lcd.print(page[curXOld * 20 + curYOld]);
    curXOld = curX;
    curYOld = curY;
  }
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 20; j++){
      lcd.setCursor(j, i);
      if (i == curY && j == curX) {
        if (blinkTimer < 4) {
          lcd.write(0);
        } else if (blinkTimer < 6) {
          lcd.print(page[i * 20 + j]);
        } else {
          blinkTimer = 0;
        }
      } else {
          lcd.print(page[i * 20 + j]); 
      }
    }
  }
}

void loop()
{
  blinkTimer++;
  writeText();
  arrowKeys();
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (pagePos < 40) {
      page[pagePos] = inChar;
      if (curX == 19 && curY == 0) {
        curX = 0;
        curY = 1;
        pagePos = 20;
      } else if (curX < 20) {
        curX++;
        pagePos++;
      }
    } else {
      for (int i = 0; i < 40; i++) {
        page[i] = ' ';
      }
      pagePos = 0;
      curX = 0;
      curY = 0;
    }
  }
}

//void setup()
//{
//  pinMode(A0, INPUT_PULLUP);
//  Serial.begin(9600);
//  
//}
//
//void loop()
//{
//  Serial.println(analogRead(0));
//  delay(100);
//}
