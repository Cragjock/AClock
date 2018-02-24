/*
 * Time_NTP.pde
 * Example showing time sync to NTP time source
 *
 * This sketch uses the ESP8266WiFi library
 */
 
#include "my_Time.h"
bool NTPResponse = false; 


// NTP Servers: http://tf.nist.gov/tf-cgi/servers.cgi 
// IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov 129.6.15.28
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov 129.6.15.29
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov
IPAddress timeServer(129, 6, 15, 30); // time-c.timefreq.bldrdoc.gov 129.6.15.30


// const int timeZone = 1;     // Central European Time
// const int timeZone = -5;  // Eastern Standard Time (USA)
// const int timeZone = -4;  // Eastern Daylight Time (USA)
const int timeZone = -8;  // Pacific Standard Time (USA)
// const int timeZone = -7;  // Pacific Daylight Time (USA)


WiFiUDP Udp;


unsigned int localPort = 8888;  // local port to listen for UDP packets

void NTP_setup() 
{
  
 // while (WiFi.status() != WL_CONNECTED) 
 // {
 //   delay(500);
 //   Serial.print(".");
 // }

  if(WiFi.status() != WL_CONNECTED)
  {

    Serial.println (F("No WiFi"));
    return;
  }

  
  Serial.println(F("Starting UDP"));
  if(!Udp.begin(localPort))
  {
      Serial.println (F("UDP startup failed"));
      return;
  }

  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  
  Serial.println(F("waiting for sync"));
  setSyncProvider(getNtpTime);
  setSyncInterval(mySyncInterval);
}

time_t prevDisplay = 0; // when the digital clock was displayed

void NTP_loop()
{  
  // Serial.print("NTP loop Time status: ");
  // Serial.println(timeStatus());
  
  if (timeStatus() != timeNotSet)     // typedef enum {timeNotSet, timeNeedsSync, timeSet}  timeStatus_t ;
  {
    if (now() != prevDisplay)       //update the display only if time has changed
    { 
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }

/**************************
 *  setTime(now1.hour(), now1.minute(), now1.second(),now1.day(), now1.month(), now1.year());
 typedef enum {timeNotSet, timeNeedsSync, timeSet}  timeStatus_t ;
 
  if(timeStatus()!= timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
*************************/
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year()); 
  Serial.println(); 
}



void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  NTPResponse =false;
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) 
  {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) 
    {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];

      NTPResponse = true; 
      
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  NTPResponse = false; 
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}



String formatDate()
{
   //String time_string = String(month() + "." + day() + "." + year()); 
   String spacer = "/";
   String date_string = String(month() + spacer + day() + spacer + year());
   return date_string;
}

String formatTime()
{
  String time_string;
  String m_minute;
  String m_second;
  String spacer = ":";
  String lead_zero = "0";

  int sec_digit = second();
  int min_digit = minute();
  if(min_digit < 10)
  {
    m_minute = String(lead_zero + minute());
  }
  else
    m_minute = String(minute());

  if(sec_digit < 10)
  {
    m_second = String(lead_zero + second());
  }
  else
    m_second = String(second());

  String date_string = String(hour() + spacer + m_minute + spacer + m_second);

  //Serial.print("from format time ");
  //Serial.println(date_string);
  //Serial.println(now());    // returns seconds since 1970
  
  return date_string; 
}




double JD_Now(const tmElements_t* pTM)
{
// use Meeus page 61
    double Y,M,D,H,S;
    double jd;
    double A,B; 
    Y=pTM->Year+1970;   // see timelib.h 
    M=pTM->Month;
    if(M<=2)
    {
        Y=Y-1;
        M=M=+2;
    }
    D=pTM->Day;

    double dsec = (double)(pTM->Second)/60.0;
    //Serial.println(dsec);
    double dmin = (double)(pTM->Minute);
    //Serial.println(dmin);
    H=( (double)pTM->Hour+ (dmin + dsec )/60.0)/24.0;
    //Serial.println(H);
    D=D+H;
    //Serial.print("D: ");
    //Serial.println(D);
    A=(int)(Y/100);
    B=2-A+(int)(A/4);
    int C=365.25*(Y+4716);
    int E=30.6001*14;
    //tempjd=(C+E)+B-1524.5;
    jd=(int)(365.25*(Y+4716))+(int)(30.6001*(M+1))+D+B-1524.5;
    //tempjd=((365.25*(yr+4716))+(30.6001*14))+B-1524.5;

//======================

  double ETime=0.0;
  int yr=1999;
  if(pTM->Year>99)
    yr=2016;            // why hard coded ?????????
  ETime=JD_Jan0(yr);
  ETime=ETime+(pTM->Day+1+(pTM->Hour+(pTM->Minute+pTM->Second/60.0)/60.0)/24.0);

/************************
  Serial.print("time data in JD ");
  Serial.println(jd);
  Serial.println(pTM->Month);
  Serial.println(pTM->Day);
  Serial.println(pTM->Year);
  Serial.println(pTM->Hour);
  Serial.println(pTM->Minute);
  Serial.println(pTM->Second);
  Serial.print("time calulations in JD ");
  Serial.println(Y);
  Serial.println(M);
  Serial.println(A);
  Serial.println(B);
  Serial.println(H);
  Serial.println(D);
  Serial.println(JD_Jan0(yr));
*******************************/




  
  //return ETime;
  return jd; 

}

double JD_Jan0(int jyear)
{
  int yr=365.25*(jyear-1);
  int A=((jyear-1)/100);
  double temp=yr-A+(A/4)+1721424.5;
  double temp1=0.0;

  // second method using general JD equation but setting to Jan 0.0
  // page 61 Meeus and Celestrack page 2, Coord System, part 2
  // Month is Jan, so year is year-1 and month is M+12=13
  // D = 0 since day is day 0
  yr=jyear-1;
  A=(yr/100);
  int B=2-A+(A/4);
  int C=365.25*(yr+4716);
  int E=30.6001*14;
  temp1=(C+E)+B-1524.5;
  //temp1=((365.25*(yr+4716))+(30.6001*14))+B-1524.5;

  return temp1;
}

