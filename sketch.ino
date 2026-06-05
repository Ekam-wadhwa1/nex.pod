
#include <Wire.h>
#include <U8g2lib.h>
#include <RTClib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
RTC_DS1307 rtc;

// Pins
#define BTN1 2
#define BTN2 3
#define BUZZER 4

enum Mode {MENU, PET, CLOCKMODE, STOPWATCHMODE, GAMEMODE};
Mode mode = MENU;

const char* menuItems[] = {"Pet","Clock","Stopwatch","Game"};
int menuIndex = 0;

// Buttons
bool lastB1 = HIGH;
bool lastB2 = HIGH;

// Pet
int mood = 80;
unsigned long lastMoodDrop = 0;
unsigned long lastAnim = 0;
unsigned long lastInteract = 0;
int eyeX = 0;
int eyeY = 0;
bool blinkState = false;

// Stopwatch
bool swRunning = false;
unsigned long swStart = 0;
unsigned long swElapsed = 0;

// Game
bool gameWaiting = false;
bool gameReady = false;
unsigned long gameStart = 0;
unsigned long waitStart = 0;
unsigned long waitTime = 0;

bool pressed(int pin, bool &lastState){
  bool cur = digitalRead(pin);
  bool p = (lastState == HIGH && cur == LOW);
  lastState = cur;
  return p;
}

void beep(int f, int d=40){
  tone(BUZZER,f,d);
}

void drawEyes(int xOff,int yOff,bool blinked){
  if(blinked){
    u8g2.drawRBox(8,26,50,8,4);
    u8g2.drawRBox(70,26,50,8,4);
    return;
  }

  u8g2.drawRBox(8+xOff,12+yOff,50,35,9);
  u8g2.drawRBox(70+xOff,12+yOff,50,35,9);
}

void drawHappy(){
  u8g2.drawRBox(8,12,50,35,11);
  u8g2.drawRBox(70,12,50,35,11);
  u8g2.setDrawColor(0);
  u8g2.drawDisc(33,62,38,U8G2_DRAW_ALL);
  u8g2.drawDisc(95,62,38,U8G2_DRAW_ALL);
  u8g2.setDrawColor(1);
}

void drawSad(){
  u8g2.drawRBox(8,18,50,29,9);
  u8g2.drawRBox(70,18,50,29,9);
  u8g2.setDrawColor(0);
  u8g2.drawTriangle(3,14,64,14,3,36);
  u8g2.drawTriangle(68,14,124,36,124,14);
  u8g2.setDrawColor(1);
}

void setup() {
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  randomSeed(analogRead(A0));

  u8g2.begin();
  rtc.begin();
}

void loop() {
  switch(mode){
    case MENU: menuScreen(); break;
    case PET: petScreen(); break;
    case CLOCKMODE: clockScreen(); break;
    case STOPWATCHMODE: stopwatchScreen(); break;
    case GAMEMODE: gameScreen(); break;
  }
}

void menuScreen(){
  u8g2.clearBuffer();
  for(int i=0;i<4;i++){
    u8g2.setCursor(5,15+i*12);
    if(i==menuIndex) u8g2.print(">");
    else u8g2.print(" ");
    u8g2.print(menuItems[i]);
  }

  if(pressed(BTN1,lastB1)){
    menuIndex=(menuIndex+1)%4;
    beep(1400);
  }

  if(pressed(BTN2,lastB2)){
    mode=(Mode)(menuIndex+1);
    beep(1800);
  }

  u8g2.sendBuffer();
}

void petScreen(){
  if(millis()-lastMoodDrop>10000){
    mood=max(0,mood-1);
    lastMoodDrop=millis();
  }

  if(pressed(BTN1,lastB1)){
    mood=min(100,mood+5);
    lastInteract=millis();
    beep(2000);
  }

  if(pressed(BTN2,lastB2)){
    mode=MENU;
    return;
  }

  if(millis()-lastAnim>2000){
    eyeX=random(-6,7);
    eyeY=random(-2,3);
    blinkState=random(0,5)==0;
    lastAnim=millis();
  }

  u8g2.clearBuffer();

  if(mood>70) drawHappy();
  else if(mood<25) drawSad();
  else drawEyes(eyeX,eyeY,blinkState);

  u8g2.setCursor(0,62);
  u8g2.print("Mood:");
  u8g2.print(mood);

  u8g2.sendBuffer();
}

void clockScreen(){
  if(pressed(BTN2,lastB2)){
    mode=MENU;
    return;
  }

  DateTime now = rtc.now();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso20_tf);

  char buf[12];
  sprintf(buf,"%02d:%02d",now.hour(),now.minute());
  u8g2.drawStr(5,30,buf);

  u8g2.setFont(u8g2_font_6x10_tf);
  sprintf(buf,"%02d/%02d/%04d",now.day(),now.month(),now.year());
  u8g2.drawStr(20,55,buf);

  u8g2.sendBuffer();
}

void stopwatchScreen(){
  if(pressed(BTN1,lastB1)){
    swRunning=!swRunning;

    if(swRunning)
      swStart=millis()-swElapsed;
    else
      swElapsed=millis()-swStart;
  }

  if(pressed(BTN2,lastB2)){
    swRunning=false;
    swElapsed=0;
    mode=MENU;
    return;
  }

  unsigned long t=swRunning?(millis()-swStart):swElapsed;

  unsigned long sec=t/1000;
  unsigned long min=sec/60;
  sec%=60;

  char buf[20];
  sprintf(buf,"%02lu:%02lu",min,sec);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso20_tf);
  u8g2.drawStr(5,35,buf);
  u8g2.sendBuffer();
}

void gameScreen(){

  if(!gameWaiting && !gameReady){
    waitTime=random(2000,5000);
    waitStart=millis();
    gameWaiting=true;
  }

  if(gameWaiting){
    u8g2.clearBuffer();
    u8g2.drawStr(20,30,"Wait...");
    u8g2.sendBuffer();

    if(millis()-waitStart>=waitTime){
      gameWaiting=false;
      gameReady=true;
      gameStart=millis();
    }
  }

  if(gameReady){
    u8g2.clearBuffer();
    u8g2.drawStr(20,30,"PRESS!");
    u8g2.sendBuffer();

    if(pressed(BTN1,lastB1)){
      unsigned long reaction=millis()-gameStart;

      u8g2.clearBuffer();
      char buf[20];
      sprintf(buf,"%lums",reaction);
      u8g2.drawStr(20,30,buf);
      u8g2.sendBuffer();

      beep(2200,100);
      delay(1500);

      gameReady=false;
      mode=MENU;
    }
  }

  if(pressed(BTN2,lastB2)){
    gameWaiting=false;
    gameReady=false;
    mode=MENU;
  }
}


//ekam <3
