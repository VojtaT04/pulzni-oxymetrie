/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <CircularBuffer.h>
#include <MAX30100.h>
#include <MAX30100_BeatDetector.h>
#include <MAX30100_Filters.h>
#include <MAX30100_PulseOximeter.h>
#include <MAX30100_Registers.h>
#include <MAX30100_SpO2Calculator.h>
#include <Wire.h>
#include <U8x8lib.h>
#define REPORTING_PERIOD_MS   1000
#define RED_PERIOD_MS   100
#define BUFFER_SIZE   135168 //bytes
#define Battery_CHARGING   1900
#define Battery_FULL   1613
#define Battery_EMPTY   1241
PulseOximeter pox;
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ 5, /* data=*/ 4);  
uint32_t tsLastReport = 0;
int incomingByte = 0;
int i = 0;
int heartRate = 0;
int battery = 0;
int batteryPercentage = 0;
unsigned char spO2 = 0;
unsigned char measurementsBuffer[BUFFER_SIZE];
char heartRateString[] = "   ";
char spO2String[] = "   ";
char batteryString[] = "    ";
char sprintfBuffer[50];

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  Serial.println("Tep!");
}

void setup()
{
  //This has to be done before the oxymeter, otherwise it won't be able to comunicate.
  u8x8.begin();
  u8x8.setPowerSave(0);
  pinMode(17, OUTPUT);
  Serial.begin(115200);
  Serial.print("Initializing pulse oximeter..");
  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for(;;);
  } else {
    Serial.println("SUCCESS");
  }
  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
  analogReadResolution(12);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 3, "           RESET");
}

void loop()
{
  // Make sure to call update as fast as possible
  pox.update();
  // Asynchronously dump heart rate and oxidation levels to the serial and print them to the display
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    heartRate = (int)pox.getHeartRate();
    spO2 = (unsigned char)pox.getSpO2();
    Serial.print("Heart rate:");
    Serial.print(heartRate);
    Serial.print("bpm / SpO2:");
    Serial.print(spO2);
    Serial.println("%");
    if (i * 2 + 1 < BUFFER_SIZE){
      measurementsBuffer[i * 2] = (unsigned char)heartRate;
      measurementsBuffer[i * 2 + 1] = spO2;
    }
    Serial.println(measurementsBuffer[i * 2]);
    Serial.println(i);
    sprintf(sprintfBuffer, "%i", heartRate);
    heartRateString[0] = sprintfBuffer[0];
    heartRateString[1] = sprintfBuffer[1];
    heartRateString[2] = sprintfBuffer[2];
    u8x8.setFont(u8x8_font_px437wyse700a_2x2_r);
    u8x8.drawString(0, 1, "   ");
    if (0 < heartRate){
      u8x8.drawString(0, 1, heartRateString);
    }
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(6, 2, "BPM");
    sprintf(sprintfBuffer, "%hhu", spO2);
    spO2String[0] = sprintfBuffer[0];
    spO2String[1] = sprintfBuffer[1];
    spO2String[2] = sprintfBuffer[2];
    u8x8.setFont(u8x8_font_px437wyse700a_2x2_r);
    u8x8.drawString(9, 1, "   ");
    if (0 < spO2){
      if (spO2 < 101){
        u8x8.drawString(9, 1, spO2String);
      }
    }
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(15, 2, "%");

    battery = analogRead(3);
    if (battery > Battery_CHARGING){
      u8x8.drawString(7, 0, "NABIJIENI");
    }
    else if (battery > Battery_FULL){
      u8x8.drawString(7, 0, "     100%");
    }
    else if (battery > Battery_EMPTY){
      batteryPercentage = (100 * (battery - Battery_EMPTY) / (Battery_FULL - Battery_EMPTY));
      sprintf(sprintfBuffer, "%i", batteryPercentage);
      batteryString[0] = sprintfBuffer[0];
      batteryString[1] = sprintfBuffer[1];
      batteryString[2] = sprintfBuffer[2];
      u8x8.setFont(u8x8_font_chroma48medium8_r);
      u8x8.drawString(7, 0, "        %");
      u8x8.drawString(12, 0, batteryString);
    }
    else {
      u8x8.drawString(7, 0, "     0  %");
    }
    i++;
    if (millis() - tsLastReport > 2*REPORTING_PERIOD_MS) {tsLastReport = tsLastReport + REPORTING_PERIOD_MS;}
    else{tsLastReport = millis();}
  }
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if ((int)incomingByte == (int)10){Serial.println("Ignoring line ending (newline).");}
    if ((int)incomingByte == (int)13){Serial.println("Ignoring line ending (carrige return).");}
    if ((int)incomingByte == (int)68){
      Serial.print("About to print ");
      Serial.print(i + 1);
      Serial.println(" pairs of csv (heartRate,spO2)");
      Serial.println("Aditional info:");
      Serial.print("i: ");
      Serial.println(i);
      Serial.print("REPORTING_PERIOD_MS: ");
      Serial.println(REPORTING_PERIOD_MS);
      Serial.println("*csv data stream start*");
      for (int j = 0; j <= i; j++) {
        Serial.print(measurementsBuffer[j * 2]);
        Serial.print(",");
        Serial.println(measurementsBuffer[j * 2 + 1]);
      }
      Serial.println("*csv data stream end*");
    }
  }
}
