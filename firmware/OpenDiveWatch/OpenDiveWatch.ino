
/******************************************************************************
 * Freedive Watch
 * https://github.com/nicshackle/FreediveWatch
 * Distributed as-is; no warranty is given.
******************************************************************************/

#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Time.h>
#include <TimeLib.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

MS5803 sensor(0x76);

//Create variables to store results
float temperature_c;
double pressure_abs, altitude_delta, pressure_baseline;

void setup() {
  Serial.begin(9600);

  Wire.begin();
  TWBR = 152;

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  //Retrieve calibration constants for conversion math.
  sensor.reset();
  sensor.begin();

  pressure_baseline = sensor.getPressure(ADC_4096);
}

int maxDepth = 0;
long maxDiveLength = 0;
int diveSecond;
int diveMinute;

#include <Dive.h>
Dive dive;

boolean serialSim = false;
int simDepth = 0;

void loop() {

  // To measure to higher degrees of precision use the following sensor settings:
  // ADC_256
  // ADC_512
  // ADC_1024
  // ADC_2048
  // ADC_4096

  // Read temperature from the sensor in deg C. This operation takes about
  temperature_c = sensor.getTemperature(CELSIUS, ADC_512);

  // Read pressure from the sensor in mbar.
  pressure_abs = sensor.getPressure(ADC_4096);

  // Taking our baseline pressure at the beginning we can find an approximate
  // change in altitude based on the differences in pressure.
  altitude_delta = altitude(pressure_abs , pressure_baseline);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(F("Totals T")); display.print(hour(millis()/1000)); display.print(F(":")); display.print(minute(millis()/1000)); display.print(F(" Dives:")); display.println(dive.number);
  display.print(F("Maxes D:")); display.print(maxDepth); display.print(F("m T")); display.print(minute(maxDiveLength/1000));display.print(F(":"));display.print(second(maxDiveLength/1000));
  if(serialSim) display.print(F(" SIM"));

  int depth = serialSim ? simDepth : map(pressure_abs, pressure_baseline, pressure_baseline * 4, 0, 40);
  depth = abs(depth);
  if (depth>maxDepth) maxDepth = depth;
  display.setCursor(70 + 0, 16); display.setTextSize(1); display.print("dpth pres");
  display.setCursor(70 + 0, 25); display.setTextSize(2);  display.print(depth);
  display.setCursor(70 + 25, 23); display.setTextSize(1); display.print("m");
  display.setCursor(70 + 25, 33); display.setTextSize(1); display.print(int(pressure_abs));


  //Temp
  display.setCursor(70 + 0, 25 + 16); display.setTextSize(1); display.print("temp");
  display.setCursor(70 + 0, 25 + 25); display.setTextSize(2);  display.print(int(temperature_c));
  display.setCursor(70 + 25, 25 + 23); display.setTextSize(1); display.print("c");
  display.setCursor(70 + 25, 25 + 31); display.setTextSize(1); display.print(int((temperature_c - int(temperature_c)) * 10));


  //Dive time
  if (!dive.active() && depth > 1 && dive.timeSince()>3000) {
    dive.start();
    display.invertDisplay(true);
  } else if (dive.active() && depth < 1 && dive.length>3000){
    dive.stop();
    display.invertDisplay(false);
  }
  if(dive.active() && maxDiveLength < dive.length) maxDiveLength = dive.length;
  display.setCursor(0, 16); display.setTextSize(1); display.print("dive time");
  display.setCursor(0, 25); display.setTextSize(2);  display.print(minute(dive.length/1000));
  display.setCursor(24, 25); display.setTextSize(1);  display.print(":");
  display.setCursor(31, 25); display.setTextSize(2);  display.print(second(dive.length/1000));


  //Rest time
  display.setCursor(0, 25 + 16); display.setTextSize(1); display.print("rest time");
  display.setCursor(0, 25 + 25); display.setTextSize(2);  display.print(minute(dive.timeSince()/1000));
  display.setCursor(24, 25 + 25); display.setTextSize(1);  display.print(":");
  display.setCursor(31, 25 + 25); display.setTextSize(2);  display.print(second(dive.timeSince()/1000));

  while (Serial.available() > 0) {
    serialSim = true;
    simDepth = Serial.parseInt();
    Serial.println(simDepth);
  }

  display.display();
  dive.update();

}

double sealevel(double P, double A)
// Given a pressure P (mbar) taken at a specific altitude (meters),
// return the equivalent pressure (mbar) at sea level.
// This produces pressure readings that can be used for weather measurements.
{
  return (P / pow(1 - (A / 44330.0), 5.255));
}


double altitude(double P, double P0)
// Given a pressure measurement P (mbar) and the pressure at a baseline P0 (mbar),
// return altitude (meters) above baseline.
{
  return (44330.0 * (1 - pow(P / P0, 1 / 5.255)));
}
