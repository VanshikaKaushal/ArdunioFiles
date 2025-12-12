#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the I²C LCD at address 0x27 with 16 columns and 2 rows.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Button pin definitions for the ESP32.
const int startButtonPin = 18; // Start/resume button
const int stopButtonPin  = 19; // Stop/pause/reset button

const int ledPin = 25;

// Timer states.
enum TimerState { IDLE, RUNNING, PAUSED };
TimerState timerState = IDLE;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(115200);

  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(stopButtonPin, INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // LED starts OFF
}

void loop() {
  switch(timerState) {

  case IDLE:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press Start");
    lcd.setCursor(0, 1);
    lcd.print(" ");

    if (digitalRead(startButtonPin) == LOW) {
      delay(50);
      while (digitalRead(startButtonPin) == LOW) {
        delay(10);
      }

      startTime = millis();
      elapsedTime = 0;
      timerState = RUNNING;

      digitalWrite(ledPin, HIGH); 
      lcd.clear();
    }
    break;

  case RUNNING:
    elapsedTime = millis() - startTime;
    displayTime(elapsedTime);

    if (digitalRead(stopButtonPin) == LOW) {
      delay(50);
      while (digitalRead(stopButtonPin) == LOW) {
        delay(10);
      }

      elapsedTime = millis() - startTime;
      timerState = PAUSED;

      digitalWrite(ledPin, LOW); 
    }
    break;

  case PAUSED:
    displayTime(elapsedTime);

    if (digitalRead(startButtonPin) == LOW) {
      delay(50);
      while (digitalRead(startButtonPin) == LOW) {
        delay(10);
      }

      startTime = millis() - elapsedTime;
      timerState = RUNNING;

      digitalWrite(ledPin, HIGH); 
      lcd.clear();
    }
    else if (digitalRead(stopButtonPin) == LOW) {
      delay(50);
      while (digitalRead(stopButtonPin) == LOW) {
        delay(10);
      }

      timerState = IDLE;
      digitalWrite(ledPin, LOW); 
      lcd.clear();
    }
    break;
  }
}

// Helper function to format and display the elapsed time on row 0.
void displayTime(unsigned long timeInMillis) {
  unsigned int minutes = timeInMillis / 60000;
  unsigned int seconds = (timeInMillis % 60000) / 1000;
  unsigned int milliseconds = timeInMillis % 1000;

  char timeStr[10];
  sprintf(timeStr, "%02u:%02u:%03u", minutes, seconds, milliseconds);

  lcd.setCursor(0, 0);
  lcd.print(timeStr);

  Serial.println(timeStr);
}
