/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#if defined(__AVR__)
    #define OLED_RESET   4 // Reset pin # (or -1 if sharing Arduino reset pin)
#elif defined(ESP8266)
  #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#endif
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#if defined(__AVR__)
  // Arduino for any board with AVR architecture.
  #define CLK 2
  #define DT 3
  #define SW 4
#elif defined(ESP8266)
  // ESP8266 Wemos D1
  #define CLK 13  // D7
  #define DT 12   // D6
  #define SW 14   // D5
#endif

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;

bool mode = false;
int menuPos = 0;

String menu[4][2] = { 
  { "push-ups", "0" },
  { "pullups", "0" },
  { "situps", "0" }
};

int selectedMenu = 0;


void setup() {
  Serial.begin(9600);

  // Set encoder pins as inputs
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);

  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  updateDisplay();
}

String input;
void displayFromSerialInput() {
  while (Serial.available() > 0) {
    input = Serial.readString();
    Serial.print("Displaying input text: ");
    Serial.println(input);

    display.clearDisplay();
    drawHeader("Push-ups:");
    drawText(input);
    display.display();
  }
}

int getSelectedMenuValue() {
  return menu[selectedMenu][1].toInt();
}

void setSelectedMenuValue(int value) {
  menu[selectedMenu][1] = String(value);
}

void incrementSelectedMenuValue() {
  if (mode == true) {
    int val = getSelectedMenuValue() + 1;
    setSelectedMenuValue(val);
  }
  else {
    menuPos = menuPos + 1;
  }
}

void decrementSelectedMenuValue() {
  if (mode == true) {
    int val = getSelectedMenuValue() - 1;
    if (val < 0) { return; }
    setSelectedMenuValue(val);
  }
  else {
    if (menuPos < 0) { return; }
    menuPos = menuPos - 1;
  }
}

bool readEncoderForChange() {
  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    if (digitalRead(DT) != currentStateCLK) {
      decrementSelectedMenuValue();
    } else {
      incrementSelectedMenuValue();
    }

    lastStateCLK = currentStateCLK;
    return true;
  }
  lastStateCLK = currentStateCLK;
  return false;
}


void drawText(String textToDisplay) {
  display.setTextSize(1);
  display.setCursor(2, 10);
  display.println(textToDisplay);
}

void drawHeader(String textToDisplay) {
  display.setTextSize(1);
  display.setCursor(2,0);
  display.println(textToDisplay);
}

void drawCounter(String textToDisplay) {
  display.setTextSize(3);
  display.setCursor(2, 10);
  display.println(textToDisplay);
}

void displayCounter() {
  display.clearDisplay();
  drawHeader("Daily " + menu[selectedMenu][0] + ":");
  drawCounter(String(getSelectedMenuValue()));
  display.display();
}

int btnState;
int lastBtnState = HIGH;
unsigned long lastBtnDebounceTime = 0;
bool isButtonPressed() {
  int btnReading = digitalRead(SW);

  if (btnReading != lastBtnState)
    lastBtnDebounceTime = millis();

  if ((millis() - lastBtnDebounceTime) > 50) {
    if (btnReading != btnState) {
      btnState = btnReading;

      if (btnState == LOW) {
        Serial.println("Button pressed!");
        lastBtnState = btnReading;
        return true;
      }
    }
  }
  lastBtnState = btnReading;
  return false;
}

void displayMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  for (int i = 0; i < 4; i++) {
    if (menuPos == i) {
      display.print("- ");
    } else {
      display.print("  ");
    }
    display.println(menu[i][0]);
  }
  display.display();
}


void updateDisplay() {
  if (mode == false) {
    displayMenu();
  } else {
    displayCounter();
  }
  delay(1);
}

void loop() {
  if (isButtonPressed()) {
    Serial.println("changing modes");
    if (mode == false) {
      selectedMenu = menuPos;
    }
    mode = !mode;
    updateDisplay();
  }

  if (readEncoderForChange()) {
    updateDisplay();
  }
}
