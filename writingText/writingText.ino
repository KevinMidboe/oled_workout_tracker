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
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

// input pin definition
int buttonModePin = 2;
int potPin = 2;


void setup() {
  Serial.begin(9600);

  // Setup button interrupt for changing modes
  pinMode(buttonModePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), changeMode, FALLING);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
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

void drawHeader(String textToDisplay) {
  display.setTextSize(1);
  display.setCursor(2,0);
  display.println(textToDisplay);
}

void drawText(String textToDisplay) {
  display.setTextSize(3);
  display.setCursor(2, 10);
  display.println(textToDisplay);
}

void drawPushups(int count) {
  Serial.print("refreshing display w/ value: ");
  Serial.println(count);
      
  display.clearDisplay();
  drawHeader("Daily push-ups:");
  drawText(String(count));
  display.display();
}

int positionBuffer = 10;
int getPotValue() {
  return analogRead(potPin) / positionBuffer;
}


bool mode = false;

int pos = 0;
int currentPos;
bool active;

unsigned long lastEvent = 0;
unsigned long currentTime;
const long timeout = 1000;
void displayNumberWithPotValue() {
  currentPos = getPotValue();
  if (currentPos < pos - 2 || currentPos > pos + 2) {
    active = true;
  }

  while (active && mode == false) {
    currentTime = millis();
    if (currentTime - lastEvent > timeout) {
      active = false;
    }
    
    currentPos = getPotValue();
    if (currentPos != pos) {
      lastEvent = currentTime;
      pos = currentPos;
      drawPushups(pos);
    }
    delay(10);
  }
}

String menuLookup[4] = { "daily", "weekly", "goal" };

void displayMenuSelect() {
  currentPos = analogRead(potPin) / 257; // split in four (1028/4)

  if (pos == currentPos) {
    return;  
  }

  pos = currentPos;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);

  for (int i = 0; i < 4; i++) {
    if (pos == i) {
      display.print("- ");
    } else {
      display.print("  ");  
    }
    display.println(menuLookup[i]);
  }
  display.display();
}

bool beingPressed = false;
void changeMode() {
  if (beingPressed == true) {
    return;
  }
  Serial.println("CHANGING MODE");

  beingPressed = true;
  mode = !mode;
  delay(800);
  beingPressed = false;
}

void loop() {
  // displayFromSerialInput();

  if (mode == false) {
    displayNumberWithPotValue();
  } else {
    displayMenuSelect();
  }
  delay(10);
}
