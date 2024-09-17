#include <PicoDVI.h>  
#include "touch.h"
#include <FastLED.h>

#define LED_PIN     28
#define NUM_LEDS    17
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

uint8_t startIndex = 0;

DVIGFX16 display(DVI_RES_320x240p60, picodvi_dvi_cfg);

const int maxPoints = 300; // Maximum number of points to display (width of the screen minus 10 pixels for the scale)
int tempValues[maxPoints] = {0}; // Array to store temperature values, initialized to 0
int cpuValues[maxPoints] = {0}; // Array to store CPU usage values, initialized to 0
int currentIndex = 0; // Current index in the array

void setup() { 
  //delay( 3000 ); // power-up safety delay

  Serial1.begin(115200);

  //FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  //FastLED.setBrightness(  BRIGHTNESS );

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;

  display.begin();
  display.setRotation(0);

  display.fillScreen(0x0000);
  display.print("hello, elecrow");

  delay(100);
}

void loop() {
  if (Serial1.available()){
    String data = Serial1.readString();
    int firstSeparator = data.indexOf(','); // Find the first separator
    int secondSeparator = data.indexOf(',', firstSeparator + 1); // Find the second separator
    int thirdSeparator = data.indexOf(',', secondSeparator + 1); // Find the third separator

    if (firstSeparator != -1 && secondSeparator != -1 && thirdSeparator != -1) {
      String tempString = data.substring(0, firstSeparator);
      String cpuString = data.substring(firstSeparator + 1, secondSeparator);
      String uploadString = data.substring(secondSeparator + 1, thirdSeparator);
      String downloadString = data.substring(thirdSeparator + 1);

      int temperature = tempString.toInt(); // Convert temperature string to integer
      int cpuUsage = cpuString.toInt(); // Convert CPU usage string to integer
      int uploadSpeed = uploadString.toInt(); // Convert upload speed string to integer
      int downloadSpeed = downloadString.toInt(); // Convert download speed string to integer

      // Store the values in the arrays
      tempValues[currentIndex] = temperature;
      cpuValues[currentIndex] = cpuUsage;
      currentIndex = (currentIndex + 1) % maxPoints; // Move to the next index, wrap around if necessary

      // Clear the screen
      display.fillScreen(0x0000);

      // Draw the temperature graph
      for (int j = 1; j < maxPoints; j++) {
        int x1 = j - 1;
        int y1 = map(tempValues[(currentIndex + j - 1) % maxPoints], 0, 100, display.height() - 20, display.height() / 2);
        int x2 = j;
        int y2 = map(tempValues[(currentIndex + j) % maxPoints], 0, 100, display.height() - 20, display.height() / 2);
        display.drawLine(x1, y1, x2, y2, 0x07E0); // Draw line (green color)
      }

      // Draw the CPU usage graph
      for (int j = 1; j < maxPoints; j++) {
        int x1 = j - 1;
        int y1 = map(cpuValues[(currentIndex + j - 1) % maxPoints], 0, 100, display.height() / 2 - 20, 0);
        int x2 = j;
        int y2 = map(cpuValues[(currentIndex + j) % maxPoints], 0, 100, display.height() / 2 - 20, 0);
        display.drawLine(x1, y1, x2, y2, 0xF800); // Draw line (red color)
      }

      // Draw the scales
      for (int i = 0; i <= 100; i += 20) {
        int yTemp = map(i, 0, 100, display.height() / 2 - 20, 0);
        display.setCursor(display.width() - 15, yTemp - 5);
        if(i != 0 && i != 100){
          display.print(i);
          display.setCursor(display.width() - 15, yTemp + display.height() / 2 - 5);
          display.print(i);
        }
      }

      // Draw white lines below and to the right of the graphs
      display.drawLine(0, display.height() / 2 - 20, display.width(), display.height() / 2 - 20, 0xFFFF); // Line below CPU graph
      display.drawLine(0, display.height() - 20, display.width(), display.height() - 20, 0xFFFF); // Line below temperature graph
      display.drawLine(display.width() - 20, 0, display.width() - 20, display.height(), 0xFFFF); // Line to the right of both graphs

      // Display the latest CPU usage value
      display.setCursor(5, 5);
      display.print("CPU: ");
      display.print(cpuUsage);
      display.print(" %");

      // Display the latest temperature value
      display.setCursor(5, display.height() / 2 + 5);
      display.print("Temp: ");
      display.print(temperature);
      display.print(" C");

      // Display the upload and download speeds
      display.setCursor(5, display.height() - 12);
      display.print("Upload: ");
      display.print(uploadSpeed);
      display.print(" kB/s");

      display.setCursor(145, display.height() - 12);
      display.print("Download: ");
      display.print(downloadSpeed);
      display.print(" kB/s");
    }
  }
  else{
    delay(1); // LED Delay
  }
  // LED Loop
  uint8_t colorIndex = startIndex++;
  for( int i = 0; i < NUM_LEDS; ++i) {
    //leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
    colorIndex += 3;
  }
  //FastLED.show();
}
