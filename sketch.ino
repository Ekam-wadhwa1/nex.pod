#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

RTC_DS1307 rtc;

// Pins
#define BTN1 1
#define BTN2 2
#define BUZZER D2
#define SDA_PIN D4
#define SCL_PIN D5

// Modes
int mode = 0;
int menuIndex = 0;
const char* menuItems[] = {"Pet", "Clock", "Stopwatch", "Game"};

// Buttons
bool lastBtn1 = HIGH;
bool lastBtn2 = HIGH;

// Pet
int mood = 80;
bool eyesClosed = false;
int eyeOffset = 0;
unsigned long lastBlink = 0;
unsigned long lastEyeMove = 0;
unsigned long lastMoodDrop = 0;
unsigned long lastInteract = 0;

// Game
bool gameActive = false;
unsigned long gameStart = 0;
int targetTime = 0;

// Stopwatch
unsigned long swStart = 0;
unsigned long swElapsed = 0;
bool swRunning = false;

//setup
void setup() {
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  rtc.begin();
}

//buttob
bool btnPressed(int pin, bool &lastState) {
  bool current = digitalRead(pin);
  if (lastState == HIGH && current == LOW) {
    lastState = current;
    delay(5);
    return true;
  }
  lastState = current;
  return false;
}

//looop
void loop() {
  switch (mode) {
    case 0: menu(); break;
    case 1: petMode(); break;
    case 2: clockMode(); break;
    case 3: stopwatchMode(); break;
    case 4: gameMode(); break;
  }
}

//menu
void menu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  for (int i = 0; i < 4; i++) {
    if (i == menuIndex) display.print("> ");
    else display.print("  ");
    display.println(menuItems[i]);
  }

  if (btnPressed(BTN1, lastBtn1)) {
    menuIndex = (menuIndex + 1) % 4;
    beep(1200);
  }

  if (btnPressed(BTN2, lastBtn2)) {
    mode = menuIndex + 1;
    beep(800);
  }

  display.display();
}

// pet mode
void petMode() {
  display.clearDisplay();

  // mood
  if (millis() - lastMoodDrop > 5000) {
    mood--;
    lastMoodDrop = millis();
  }

  if (millis() - lastInteract > 15000) {
    mood -= 2;
  }

  if (btnPressed(BTN1, lastBtn1)) {
    mood += 5;
    lastInteract = millis();
    beep(1500);
  }

  if (btnPressed(BTN2, lastBtn2)) {
    mode = 0;
    return;
  }

  mood = constrain(mood, 0, 100);

  // Blink
  if (millis() - lastBlink > random(2000, 5000)) {
    eyesClosed = true;
    lastBlink = millis();
  }
  if (eyesClosed && millis() - lastBlink > 120) {
    eyesClosed = false;
  }

  // Eyes
  if (millis() - lastEyeMove > 1000) {
    eyeOffset = random(-2, 3);
    lastEyeMove = millis();
  }

  drawFace();

  display.setCursor(35, 54);
  display.print("Mood:");
  display.print(mood);

  display.display();
}

//faces
void drawFace() {
  int y = 20;
  int lx = 35 + eyeOffset;
  int rx = 75 + eyeOffset;

  if (eyesClosed) {
    display.drawLine(lx, y, lx + 10, y, WHITE);
    display.drawLine(rx, y, rx + 10, y, WHITE);
  } else {
    if (mood > 70) {
      display.fillCircle(lx, y, 4, WHITE);
      display.fillCircle(rx, y, 4, WHITE);
    } else if (mood > 30) {
      display.drawCircle(lx, y, 4, WHITE);
      display.drawCircle(rx, y, 4, WHITE);
    } else {
      display.drawLine(lx, y + 2, lx + 8, y - 2, WHITE);
      display.drawLine(rx, y + 2, rx + 8, y - 2, WHITE);
    }
  }

  // Mouth
  int bounce = sin(millis() / 200.0) * 2;

  if (mood > 70)
    display.drawLine(50, 40 + bounce, 78, 40 + bounce, WHITE);
  else if (mood > 30)
    display.drawPixel(64, 40, WHITE);
  else
    display.drawLine(50, 42, 78, 42, WHITE);
}

//clock
void clockMode() {
  display.clearDisplay();

  DateTime now = rtc.now();

  display.setTextSize(2);
  display.setCursor(10, 10);
  display.print(now.hour());
  display.print(":");
  if (now.minute() < 10) display.print("0");
  display.print(now.minute());

  if (btnPressed(BTN2, lastBtn2)) mode = 0;

  display.display();
}

// stopwatch
void stopwatchMode() {
  display.clearDisplay();

  if (btnPressed(BTN1, lastBtn1)) {
    swRunning = !swRunning;
    if (swRunning) swStart = millis() - swElapsed;
    else swElapsed = millis() - swStart;
    beep(1000);
  }

  if (btnPressed(BTN2, lastBtn2)) {
    swRunning = false;
    swElapsed = 0;
    mode = 0;
    beep(600);
  }

  unsigned long t = swRunning ? millis() - swStart : swElapsed;

  int sec = t / 1000;
  int min = sec / 60;
  sec %= 60;

  display.setTextSize(2);
  display.setCursor(20, 20);
  display.print(min);
  display.print(":");
  if (sec < 10) display.print("0");
  display.print(sec);

  display.display();
}

//<3
// games
void gameMode() {
  display.clearDisplay();

  if (!gameActive) {
    display.setCursor(10, 20);
    display.print("Wait...");

    targetTime = random(2000, 5000);
    delay(targetTime);

    gameActive = true;
    gameStart = millis();
  }

  display.setCursor(20, 30);
  display.print("PRESS!");

  if (btnPressed(BTN1, lastBtn1)) {
    int reaction = millis() - gameStart;

    display.clearDisplay();
    display.setCursor(10, 20);
    display.print("Time:");
    display.print(reaction);
    display.print("ms");

    beep(1500);
    delay(2000);

    gameActive = false;
    mode = 0;
  }

  if (btnPressed(BTN2, lastBtn2)) {
    gameActive = false;
    mode = 0;
  }

  display.display();
}

//sound
void beep(int freq) {
  tone(BUZZER, freq, 50);
}

//ekam <3
