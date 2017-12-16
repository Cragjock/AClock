
#ifndef MY_TIME_H
#define MY_TIME_H


#include <TimeLib.h> 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define mySyncInterval (5*60)



void NTP_loop(); 
void NTP_setup();
void digitalClockDisplay();
void printDigits(int digits); 
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);
String formatDate();
String formatTime();

double JD_Now(const tmElements_t* pTM);
double JD_Jan0(int year);
extern bool NTPResponse; 




#endif
