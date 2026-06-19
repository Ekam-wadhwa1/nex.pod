#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ssd1306.h>
#include <RTClib.h>

//defining dimensions
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_ssd1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RTC_DS3231 rtc;

//pins
const int BUTTON_MODE = D1
const int BUTTON_ACTION = D2
const int BUZZER_PIN = D3 

enum Mode {CLOCK, TAMAGOTCHI, STOPWATCH };

Mode currentmode = CLOCK;

//debounce
bool lastModeBtnState = HIGH;
bool lastActionBtnState = HIGH;

// Tamagotchi
int HUNGER = 50;
int HAPPINESS = 100;
unsigned long lastpetupdate = 0;

// stopwatch
unsigned long stopwatchstart = 0;
unsigned long stopwatchelapsed =0;
bool stopwatchrunning =false;

bool stopwatchrunning =false;

void setup()
{
    Serial.begin (115200);
    pinmode (BUTTON_MODE, INPUT_PULLUP);
    pinmode (BUTTON_ACTION, INPUT_PULLUP);
    pinmode(BUZZER_PIN, OUTPUT);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("OLED MEMORY allocation failed"));
        for(;;);
    }

    display.clearDisplay();
    display.setTextColor(WHITE);

    if (!rtc.begin()) 
    {
    Serial.println("Couldn't find RTC");
    while (1);
    }

    if (!rtc.isrunning())
    {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

}

void loop ()
{

    bool modeBtnState = digitalRead(BUTTON_MODE);
    bool actionBtnState = digitalRead(BUTTON_ACTION);

  // --- Handle Mode Button (Left) ---
  if (modeBtnState == LOW && lastModeBtnState == HIGH) {
    tone(BUZZER_PIN, 1500, 50); // Beep on press
    // Cycle modes: CLOCK -> TAMAGOTCHI -> STOPWATCH -> CLOCK
    if (currentMode == CLOCK) currentMode = TAMAGOTCHI;
    else if (currentMode == TAMAGOTCHI) currentMode = STOPWATCH;
    else currentMode = CLOCK;
    delay(200); // Simple debounce
  }
  lastModeBtnState = modeBtnState;

  // --- Handle Action Button (Right) & Rendering ---
  display.clearDisplay();
  
  switch (currentMode) {
    case CLOCK:
      handleClockMode(actionBtnState);
      break;
    case TAMAGOTCHI:
      handleTamagotchiMode(actionBtnState);
      break;
    case STOPWATCH:
      handleStopwatchMode(actionBtnState);
      break;
  }

  display.display();
  delay(50); // Small loop delay
}

// ==================== MODE HANDLERS ====================

void handleClockMode(bool actionBtnState) {
  DateTime now = rtc.now();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("--- DIGITAL CLOCK ---");

  // Format Time
  display.setTextSize(2);
  display.setCursor(15, 25);
  if(now.hour() < 10) display.print('0');
  display.print(now.hour(), DEC);
  display.print(':');
  if(now.minute() < 10) display.print('0');
  display.print(now.minute(), DEC);
  display.print(':');
  if(now.second() < 10) display.print('0');
  display.print(now.second(), DEC);

  // Action Button emits a little sound in Clock mode just for fun
  if (actionBtnState == LOW && lastActionBtnState == HIGH) {
    tone(BUZZER_PIN, 2000, 100);
    delay(150);
  }
  lastActionBtnState = actionBtnState;
}

void handleTamagotchiMode(bool actionBtnState) {
  // Decay stats every 5 seconds
  if (millis() - lastPetUpdate > 5000) {
    if (hunger > 0) hunger -= 2;
    if (happiness > 0) happiness -= 1;
    lastPetUpdate = millis();
  }

  // Interaction with Action Button (Feed/Play)
  if (actionBtnState == LOW && lastActionBtnState == HIGH) {
    tone(BUZZER_PIN, 2500, 80);
    hunger = min(100, hunger + 15);
    happiness = min(100, happiness + 10);
    delay(150);
  }
  lastActionBtnState = actionBtnState;

  // Draw UI
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("TAMAGOTCHI PET");

  // Display stats
  display.setCursor(0, 15);
  display.print("Hunger: "); display.print(hunger); display.print("%");
  display.setCursor(0, 25);
  display.print("Happy : "); display.print(happiness); display.print("%");

  // Simple adaptive expression based on status
  display.setTextSize(2);
  display.setCursor(45, 45);
  if (hunger < 20 || happiness < 20) {
    display.print("( >_< )"); // Sad/Starving
  } else if (hunger > 80 && happiness > 80) {
    display.print("( ^_^)b"); // Very happy
  } else {
    display.print("( -_- )"); // Normal
  }
}

void handleStopwatchMode(bool actionBtnState) {
  // Toggle StopWatch on/off with Action button
  if (actionBtnState == LOW && lastActionBtnState == HIGH) {
    tone(BUZZER_PIN, 1800, 50);
    if (!stopwatchRunning) {
      stopwatchStart = millis() - stopwatchElapsed;
      stopwatchRunning = true;
    } else {
      stopwatchRunning = false;
    }
    delay(150);
  }
  lastActionBtnState = actionBtnState;

  if (stopwatchRunning) {
    stopwatchElapsed = millis() - stopwatchStart;
  }

  // Long press Action button to reset when stopped
  if (actionBtnState == LOW && !stopwatchRunning && stopwatchElapsed > 0) {
    delay(800); // hold for nearly a second
    if (digitalRead(BUTTON_ACTION) == LOW) {
      stopwatchElapsed = 0;
      tone(BUZZER_PIN, 1000, 200);
    }
  }

  // UI Display
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("STOPWATCH");
  
  display.setCursor(0, 55);
  display.print(stopwatchRunning ? "[R] Stop" : "[R] Start (Hold=Reset)");

  // Format Elapsed Time (MM:SS:CC)
  unsigned long totalSeconds = stopwatchElapsed / 1000;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  unsigned long centiseconds = (stopwatchElapsed % 1000) / 10;

  display.setTextSize(2);
  display.setCursor(15, 25);
  if(minutes < 10) display.print('0');
  display.print(minutes);
  display.print(':');
  if(seconds < 10) display.print('0');
  display.print(seconds);
  display.print('.');
  if(centiseconds < 10) display.print('0');
  display.print(centiseconds);
}
