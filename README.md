
# Mini-OLED Workout display
A small wall-mounted display with input to set and display excersize repetitions per day.

![Kiku](assets/breadboard.jpg)

## Setup
This project depends on the arduino library `Adafruit SSD1306` and it`s dependencies.

From Arduino IDE library can be added by navigating to the following menu item: Sketch -> Include Library -> Manage Libaries. From here search for `Adafruit SSD1306`, this should prompt to also download `Adafruit GFX Library` and `Adafruit BusIO`; select All or install them manually.

## Components
OLED screen - wakes and dims on a timeout.
Potentiometer - setting value to display based on put input.

Either button or time based commit to off-chip db over http.

## Graphics
Ideas for what to display on the screen.

 - Large print of count.
 - This weeks avg.
 - Animations on save ?
 - View history
   - Scroll back needs to fetch & maybe cache results.
   - Loading animation.
