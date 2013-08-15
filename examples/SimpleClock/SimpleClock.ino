/*
This a simple serial port clock application using the RTC Nanoshield.

Copyright (c) 2013 Circuitar
This software is released under the MIT license. See the attached LICENSE file for details.
*/

#include <Wire.h>
#include <stdio.h>
#include "Nanoshield_RTC.h"
#include "Deuligne.h"

Nanoshield_RTC rtc;
Deuligne lcd;

#define BUFFER_SIZE 50
char buf[BUFFER_SIZE];
char date[11];
char time[9];

void setup()
{
  Serial.begin(115200);
  Serial.println("-------------------------");
  Serial.println(" Nanoshield Serial Clock");
  Serial.println("-------------------------");
  Serial.println("");

  // Initialize LCD
  lcd.init();
  lcd.clear();
  lcd.print("Initializing...");
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Failed starting RTC");
    lcd.clear();
    lcd.print("Cannot start RTC");
    while(true);
  };

  // Print current time and ask for the new time
  Serial.print("Current time: ");
  rtc.read();
  sprintf(date, "%04d-%02d-%02d", rtc.getYear(), rtc.getMonth(), rtc.getDay());
  sprintf(time, "%02d:%02d:%02d", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  Serial.print(date);
  Serial.print(" ");
  Serial.println(time);
  Serial.println("Press Enter to adjust the time");
  Serial.setTimeout(3000);
  if (Serial.find("\n")) {
    Serial.setTimeout(99999);
    Serial.println("Type the new time in the format above and press Enter");
    Serial.println("Press only Enter if you want to keep the same time");
    buf[0] = '\0';
    if (Serial.readBytesUntil('\n', buf, BUFFER_SIZE) > 0) {
      setTime(rtc, buf);
    }
  }

  lcd.clear();
}

void loop()
{
  // Read time from RTC
  rtc.read();

  // Print time in the serial port
  sprintf(date, "%04d-%02d-%02d", rtc.getYear(), rtc.getMonth(), rtc.getDay());
  sprintf(time, "%02d:%02d:%02d", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  Serial.print(date);
  Serial.print(" ");
  Serial.println(time);
  
  // Print time in the LCD
  lcd.setCursor(0, 0);
  lcd.print(date);
  lcd.setCursor(0, 1);
  lcd.print(time);

  // Wait for next second
  delay(1000);
}

void setTime(Nanoshield_RTC& rtc, char* time) {
  char *p = time;
  char *tok;
  int year, mon, day, hour, min, sec;
  
  tok = strtok_r(p, "-", &p);
  year = atoi(tok);
  tok = strtok_r(p, "-", &p);
  mon = atoi(tok);
  tok = strtok_r(p, " ", &p);
  day = atoi(tok);
  tok = strtok_r(p, ":", &p);
  hour = atoi(tok);
  tok = strtok_r(p, ":", &p);
  min = atoi(tok);
  tok = strtok_r(p, "\n", &p);
  sec = atoi(tok);

  sprintf(buf, "New time: %04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
  Serial.println(buf);
  
  rtc.writeYear(year);
  rtc.writeMonth(mon);
  rtc.writeDay(day);
  rtc.writeHours(hour);
  rtc.writeMinutes(min);
  rtc.writeSeconds(sec);
}

