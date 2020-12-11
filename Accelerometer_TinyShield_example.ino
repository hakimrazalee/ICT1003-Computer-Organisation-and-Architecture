/*************************************************************************
   BMA250 Tutorial:
   This example program will show the basic method of printing out the
   accelerometer values from the BMA250 to the Serial Monitor, and the
   Serial Plotter

   Hardware by: TinyCircuits
   Code by: Laverena Wienclaw for TinyCircuits

   Initiated: Mon. 11/1/2018
   Updated: Tue. 11/2/2018
 ************************************************************************/

#include <Wire.h>         // For I2C communication with sensor
#include "BMA250.h"       // For interfacing with the accel. sensor
#include <TinyScreen.h>   // For Button input
#include <SPI.h>
#include <STBLE.h>        //ST Bluetooth
#include <string.h>


// Accelerometer sensor variables for the sensor and its values
BMA250 accel_sensor;
int x, y, z, fx, fy, fz;
double temp;
TinyScreen display = TinyScreen(TinyScreenDefault);

#if defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#else
#define SerialMonitorInterface Serial
#endif

//Debug output adds extra flash and memory requirements!
#ifndef BLE_DEBUG
#define BLE_DEBUG true
#endif

//#if defined (ARDUINO_ARCH_AVR)
//#define SerialMonitorInterface Serial
//#elif defined(ARDUINO_ARCH_SAMD)
//#define SerialMonitorInterface SerialUSB
//#endif


uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_connection_state = false;
#define PIPE_UART_OVER_BTLE_UART_TX_TX 0
int flag = 0;
void setup() {
  SerialMonitorInterface.begin(115200);
  Wire.begin();
  display.begin();
  //setBrightness(brightness);//sets main current level, valid levels are 0-15
  display.setBrightness(10);
  while (!SerialMonitorInterface); //This line will block until a serial monitor is opened with TinyScreen+!
  BLEsetup();

  SerialMonitorInterface.print("Initializing BMA...");
  // Set up the BMA250 acccelerometer sensor
  accel_sensor.begin(BMA250_range_2g, BMA250_update_time_64ms);

  // uncomment below for tinyscreen stuff
  //display.setCursor(0, 0);

  initialvalues();
}

void initialvalues() {
  accel_sensor.read();
  fx = accel_sensor.X;
  fy = accel_sensor.Y;
  fz = accel_sensor.Z;
}
int pass = 0;
int buttonLoop() {
  display.setCursor(0, 0);
  //getButtons() function can be used to test if any button is pressed, or used like:
  //getButtons(TSButtonUpperLeft) to test a particular button, or even like:
  //getButtons(TSButtonUpperLeft|TSButtonUpperRight) to test multiple buttons
  //results are flipped as you would expect when setFlip(true)
  if (display.getButtons(TSButtonUpperLeft)) {
    display.println("TAN!");
  } else {
    display.println("          ");
  }
  display.setCursor(0, 54);
  if (display.getButtons(TSButtonLowerLeft)) {
    display.println("FOREST!");
  } else {
    display.println("          ");
  }
  display.setCursor(95 - display.getPrintWidth("STEVEN LIM!"), 0);
  if (display.getButtons(TSButtonUpperRight) && display.getButtons(TSButtonLowerRight)) {
    display.println("STEVEN LIM!");
    flag = 1;
    return 0;

  } else {
    display.println("          ");
  }
  display.setCursor(95 - display.getPrintWidth("STEVEN!"), 0);
  if (display.getButtons(TSButtonUpperRight)) {
    display.println("STEVEN!");
    
  } else {
    display.println("          ");
  }
  display.setCursor(95 - display.getPrintWidth("LIM!"), 54);
  if (display.getButtons(TSButtonLowerRight)) {
    display.println("LIM!");
  } else {
    display.println("          ");
  }
}
void initial() {
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_White, TS_8b_Black);
  display.clearScreen();
  display.setCursor(48 - (display.getPrintWidth("Connect Device") / 2), 32 - (display.getFontHeight() / 2));
  display.print("Connect Device");
}

void afterConnect() {
  display.clearScreen();
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_White, TS_8b_Black);

  display.setCursor(48 - (display.getPrintWidth("SURVEILENCE MODE") / 2), 32 - (display.getFontHeight() / 2));
  display.print("SURVEILENCE MODE");
}

void endAlarm() {
  display.clearScreen();
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_Green, TS_8b_Black);
  display.setCursor(48 - (display.getPrintWidth("Alarm Disarmed!") / 2), 32 - (display.getFontHeight() / 2));
  display.print("Alarm Disarmed!");
  delay(3000);
  display.clearScreen();
}
void intruderAlert() {

  display.clearScreen();
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_Red, TS_8b_Black);
  display.setCursor(48 - (display.getPrintWidth("INTRUDER ALERT") / 2), 32 - (display.getFontHeight() / 2));
  display.print("INTRUDER ALERT");
  delay(3000);
  display.clearScreen();
  display.setCursor(48 - (display.getPrintWidth("INPUT PASS") / 2), 32 - (display.getFontHeight() / 2));
  display.print("INPUT PASS");
  unsigned long startTime = millis();
  while (millis() - startTime < 3000)buttonLoop();
  display.clearScreen();

}

void loop() {

  if (ble_connection_state == true) {
    afterConnect();
    accel_sensor.read();//This function gets new data from the acccelerometer

    // Get the acceleration values from the sensor and store them into global variables
    // (Makes reading the rest of the program easier)
    x = accel_sensor.X;
    y = accel_sensor.Y;
    z = accel_sensor.Z;
    temp = ((accel_sensor.rawTemp * 0.5) + 24.0);

    // If the BMA250 is not found, nor connected correctly, these values will be produced
    // by the sensor
    if (x == -1 && y == -1 && z == -1) {
      // Print error message to Serial Monitor
      SerialMonitorInterface.print("ERROR! NO BMA250 DETECTED!");
    }

    else { // if we have correct sensor readings:
      if ((fx - 100 <= x && x <= fx + 100) && (fy - 100 <= y && y <= fy + 100) && (fz - 100 <= z && z <= fz + 100))
      {
        SerialMonitorInterface.print("X = ");
        SerialMonitorInterface.print(x);

        SerialMonitorInterface.print("  Y = ");
        SerialMonitorInterface.print(y);

        SerialMonitorInterface.print("  Z = ");
        SerialMonitorInterface.println(z);
      }

      else
      {
        SerialMonitorInterface.println("IT HAS BEEN TEMPERED WITH");


        while (1) {
          flag = 0;
          intruderAlert();

          aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
          if (ble_rx_buffer_len) {//Check if data is available
            SerialMonitorInterface.print(ble_rx_buffer_len);
            SerialMonitorInterface.print(" : ");
            SerialMonitorInterface.println((char*)ble_rx_buffer);
            ble_rx_buffer_len = 0;//clear afer reading
          }
          if (SerialMonitorInterface.available()) {//Check if serial input is available to send
            delay(10);//should catch input
            uint8_t sendBuffer[21];
            uint8_t sendLength = 0;
            while (SerialMonitorInterface.available() && sendLength < 19) {
              sendBuffer[sendLength] = SerialMonitorInterface.read();
              sendLength++;
            }
            if (SerialMonitorInterface.available()) {
              SerialMonitorInterface.print(F("Input truncated, dropped: "));
              if (SerialMonitorInterface.available()) {
                SerialMonitorInterface.write(SerialMonitorInterface.read());
              }
            }
            sendBuffer[sendLength] = '\0'; //Terminate string
            sendLength++;
            if (!lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)sendBuffer, sendLength))
            {
              SerialMonitorInterface.println(F("TX dropped!"));
            }
          }

          if (strcmp((char*)ble_rx_buffer, "1") == 0 || flag == 1)
          {
            endAlarm();
            flag == 0;
            initialvalues();
            break;
          }

        }
        ble_rx_buffer_len = 0;//clear afer reading
        ble_rx_buffer[0] = 0;//this to "clear" rx buffer
      }
    }
  }
  else
  {
    initial();
  }
  // The BMA250 can only poll new sensor values every 64ms, so this delay
  // will ensure that we can continue to read values
  delay(250);
  // ***Without the delay, there would not be any sensor output***

  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
}
