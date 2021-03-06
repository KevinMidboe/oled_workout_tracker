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
    #define OLED_RESET   -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#elif defined(ESP8266)
  #define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
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


boolean readEncoderAndUpdateCounter() {
  currentStateCLK = digitalRead(CLK);

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(DT) != currentStateCLK) {
      counter --;
      currentDir ="CCW";
    } else {
      // Encoder is rotating CW so increment
      counter ++;
      currentDir ="CW";
    }

    lastStateCLK = currentStateCLK;
    return true;
  }
  lastStateCLK = currentStateCLK;

  return false;
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
  display.clearDisplay();
  drawHeader("Daily push-ups:");
  drawText(String(count));
  display.display();
}


void checkButtonState() {
  int btnState = digitalRead(SW);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
      mode = !mode;

      if (mode == false) {
        drawPushups(counter);
      } else {
        displayMenu();
      }
    }

    // Remember last button press event
    lastButtonPress = millis();
  }
}

String menuLookup[4] = { "daily", "weekly", "goal" };
void displayMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);

  for (int i = 0; i < 4; i++) {
    if (counter == i) {
      display.print("- ");
    } else {
      display.print("  ");  
    }
    display.println(menuLookup[i]);
  }
  display.display();
}

void displayCounterValue() {
  if (readEncoderAndUpdateCounter()) {
    drawPushups(counter);
  }
  delay(1);
}

int menuPos = 0;
void displayMenuSelect() {
  if (readEncoderAndUpdateCounter()) {
    displayMenu();
  }
  delay(1);
}

void loop() {
  // displayFromSerialInput();

  if (mode == false) {
    displayCounterValue();
  } else {
    displayMenuSelect();
  }
  checkButtonState();
}
