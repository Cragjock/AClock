 /******************************************************************
  BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 base was mapdrawplay 5
 ******************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <stdint.h>

#include "RTClib.h"

#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"


#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "Fonts/FreeMonoBoldOblique9pt8b.h"
#include "Fonts/FreeSerifItalic24pt8b.h"    // includes extended ASCII characters 
#include "Fonts/FreeSerifItalic18pt8b.h"
#include "Fonts/FreeSerif9pt8b.h"   // this size ok to use, still readable 

#include "sdc.h"
#include "my_wifi.h"
#include "my_eeprom.h"
#include "my_Time.h"

#define RA8875_INT 3
#define RA8875_CS 2   // tied to blue LED too? 
#define RA8875_RESET 16

#define SEALEVELPRESSURE_HPA (1013.25)

  Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
  Adafruit_BME280 bme;  // I2C
  //RTC_DS1307 rtc;
  RTC_DS3231 rtc;
  DateTime now1; 
  uint8_t RTCresponse; 


  char timebuf[12];
  const uint8_t time_len = 15; 

  String formatRTC_Time();
  void splash_screen(Adafruit_RA8875 &mytft);

  enum what_phase {FQ, F, LC,LQ, NC, NH, OH} moondisplay; 




/**************************************************
 * SETUP START
 */
void setup() 
{
  
  const uint16_t* phase_array[]={firstQ, full, lastcres, lastQ, newcresent, newhalf, oldhalf};
  
  bool status;
  Serial.begin(115200);


  splash_screen(tft);

  String feedback = start_wifi();
  // Serial.print(F("Wifi feedback "));
  // Serial.println(feedback);
  
  NTP_setup();

  
  // ======================  
  update_wifi();   // need time date first


  Serial.println(F("read EEPROM? "));

  //String theTime = formatDate();
  String theDate;

  //my_EE_write(EE_LASTUPDATE, theTime, theDate.length());

  if(NTPResponse != false)
  {
      theDate = formatDate();
      my_EE_write(EE_LASTUPDATE, theDate, theDate.length());
  }

  else
   {
      my_EE_read(EE_LASTUPDATE, timebuf, time_len);
      theDate = timebuf;
    }

  Serial.print(F("date as a string "));
  Serial.println(theDate);
 

  
  Serial.println(F("BME start"));
  status = bme.begin();  
  if (!status) 
  {
        Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
        while (1);
  }
  Serial.println(F("RA8875 start"));
  
  #ifdef ESP8266    // THIS IS CORRECT 
  Serial.println(F("ESP8266 yes"));
  #else
  Serial.println(F("ESP8266 = no"));
  #endif

  if (!tft.begin(RA8875_800x480)) 
  {
    Serial.println(F("RA8875 Not Found!"));
    while (1);
  }

  rtc.begin(); 
  
  //RTCresponse = rtc.isrunning(); 
  RTCresponse = rtc.lostPower(); 

  Serial.print(F("RTC is running: "));
  Serial.println(RTCresponse);

  now1 = rtc.now();
  Serial.print(F("RTC now: "));
  Serial.println(now1.month());
  Serial.println(now1.day());
  Serial.println(now1.year());
  Serial.println(now1.hour());
  Serial.println(now1.minute());
  Serial.println(now1.second());


  Serial.print("Setup Time status: ");
  Serial.println(timeStatus());

/*********************************
  THe followin is to sync RTC and NTP if no connection to the NTP server on start-up
*********************************/
  
  if (timeStatus() == timeNotSet)     // typedef enum {timeNotSet, timeNeedsSync, timeSet}  timeStatus_t ;
  {
    setTime(now1.hour(), now1.minute(), now1.second(),now1.day(), now1.month(), now1.year());

  }
  else
  {
    rtc.adjust(DateTime(now()));      // sets RTC to NTP time 
  }

  

/**********************/
 if(NTPResponse != false)
  {
    if(abs(second()-now1.second())>2)
    {
      rtc.setSecond(second());
      Serial.println(F("Adjusting RTC time "));
    }
  }
/*********************/ 


  // ================================= 
  // update_wifi(); // why does this cause the splash screen to blank when it is here tft reference??????

  uint8_t gpiox_state = tft.readReg(RA8875_GPIOX);
  Serial.print(F("GPIO status: "));
  Serial.println(gpiox_state);

  uint8_t display_state = tft.readReg(RA8875_PWRR);
  Serial.print(F("display_state status: "));
  Serial.println(display_state);
  

// ==================== orig location ============
/****************************/
  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255); 
  tft.fillScreen(RA8875_PURPLE);
/*********************************/

  gpiox_state = tft.readReg(RA8875_GPIOX);
  Serial.print(F("GPIO status: "));
  Serial.println(gpiox_state);
  display_state = tft.readReg(RA8875_PWRR);
  Serial.print(F("display_state status: "));
  Serial.println(display_state);

// === load icon =====
  LoadCustomChar(bmSatright, location_2);   // sat bmp text mode items only
  LoadCustomChar(bmSatLeft, location_1);   // sat bmp text mode items only
  LoadCustomChar(ant_L, location_3);
  LoadCustomChar(ant_R, location_4);
  LoadCustomChar(X_L,location_5);
  LoadCustomChar(X_R,location_6);
  LoadCustomChar(bars_L,location_7);
  LoadCustomChar(bars_R,location_8);

// ============ BASIC SETUP Complete =================


  const char string1[15] = "start where!  ";
  const char sdceng[] ={0x53,0x74,0x65,0x76,0x65,0x6e,0xB0,0x0};
  //const char sdceng1[] ={0x53,0x74,0x65,0x76,0x65,0x6e,0xB0,0x0};
  const char sdceng1[] ={'S','t','e','v','e','n',0xB0,0x0};     // Steven with degree symbol 
  uint16_t row=0; 
  uint16_t column=0;
  const char ylhome[] = "Yorba Linda, CA";
  const char latlong[] = "33.909N 117.782W"; 
  const char copyright[] = {0xA9, 0x0};

 

  tft.textMode();   // =========TEXT MODE ON ====


// ========== Set up and display icon  
  tft.textSetCursor(400,400);
  tft.textEnlarge(2);   // set to 3x size 
  tft.textColor(RA8875_WHITE, RA8875_RED);
  displaycustomChar(location_1,1);    // dsiplay sat icon 

  tft.textSetCursor(1,25);
  tft.textColor(RA8875_WHITE, RA8875_PURPLE);
  tft.textEnlarge(0);
  displaycustomChar(location_7,1);  

  tft.textColor(RA8875_WHITE, RA8875_RED);
  tft.textEnlarge(2);
// ========== Sat out done

  tft.textEnlarge(0);   // set back to 1x size 
  tft.graphicsMode();   // ==== GRAPHICS MODE ON     


    
  int j=0; 
  uint16_t HB=0;
  uint16_t LB=0;
  int ALL=0;
  uint16_t border = 10; 
  const uint16_t* cc=0;


  Box mapbox={150,127,500,225, RA8875_RED}; // was 321 x 163 for world 565, 213 for charmap fpt LCD2, 320 x144
  tft.drawRect(mapbox.x, mapbox.y, mapbox.w, mapbox.h, mapbox.c);

  Box LCDbox={400,200,321,145, RA8875_RED};  // LCD2, 321 145
  tft.drawBorder(mapbox.x ,mapbox.y, mapbox.w, mapbox.h, border);   // i added drawborder into Adafruit lib 


// ===== draws the LCD Display image =====
  // OUT FOR THE MOMENT, ADD IN LATER  draw_map(lcd3,225,500, mapbox); // the BIG LCD graphic 
  
  Box testbox={10,20,321,145, RA8875_RED}; 
 // draw_map(LCD2,144,320, testbox); // the smaller LCD graphic
 
  Box testbox1={500,200,151,101, RA8875_RED}; 
  draw_map(char150,100,150,testbox1);

    //tft.drawRect(200, 200, 272, 33, RA8875_YELLOW); // for row 1 box
    //tft.drawRect(210, 200, 272, 33, RA8875_YELLOW); // for row 1 box    // better alignment 
    //  tft.drawRect(200, 250, 272, 33, RA8875_YELLOW); // for row 2 box


  



  
  // ========== top header area =============
  tft.textSetCursor(10, 10);

  Box header_L = {0,5,266,20};
  //tft.drawRect(0,5,266,20, RA8875_WHITE);
  Box header_C = {266,5,268,20};
  //tft.drawRect(266,5,268,20, RA8875_WHITE);
  Box header_R = {534,5,266,20};
  //tft.drawRect(534,5,266,20, RA8875_WHITE);
  
  tft.textMode();   // ==== TEXT MODE ON ====
  char string[17] = " Hey de KC6FEW! ";
  tft.textSetCursor(header_L.x, header_L.y); 
  tft.textTransparent(RA8875_WHITE);
  tft.textWrite(string);
  tft.textColor(RA8875_WHITE, RA8875_RED);    // only changes text mode, not graphics mode
  tft.textWrite(string);

  tft.graphicsMode(); // ==== GRAPHICS MODE ON ====
  tft.setTextColor(RA8875_YELLOW);
  tft.setFont(&FreeSerifItalic24pt8b);  
  tft.setCursor(header_C.x,header_C.y+40); 
  tft.print("-[Welcome!]-"); // this works too 
  tft.setTextColor(RA8875_WHITE);


  tft.textMode();   // ==== TEXT MODE ON ====
  
  tft.textSetCursor(header_R.x, header_R.y);
  //tft.textTransparent(RA8875_CYAN);
  //tft.textWrite(string);
  //tft.textTransparent(RA8875_GREEN);
  //tft.textWrite(string);
  
  tft.textColor(RA8875_YELLOW, RA8875_CYAN);
  tft.textWrite(string);
  tft.textColor(RA8875_BLACK, RA8875_MAGENTA);
  //tft.print(string); 
  tft.textWrite(string);
  tft.textSetCursor(5, 55);
  
  for(int i=0; i<3; i++)
  {
    tft.textWrite(ylhome);
    tft.textWrite(latlong);
  }
  
// ========== header area END =============

  

/* Change the cursor location and color ... */  
  tft.textEnlarge(1);
  tft.textSetCursor(235, 200);
  tft.textTransparent(RA8875_YELLOW);
  //    tft.textColor(RA8875_YELLOW, RA8875_BLACK);
  char h_buffer[20];
  float bm_humid = bme.readHumidity(); 
  //tft.textWrite(dtostrf(bm_humid, 3,3, h_buffer) );
  
 // tft.textWrite(string);
  
  tft.textSetCursor(10, 40);
  tft.textEnlarge(2);

  float bm_temp = bme.readTemperature();
  char t_buffer[20];
  
 // tft.textWrite(dtostrf(bm_temp, 3,3, t_buffer) );


  // tft.drawRect(10,40,(8*3*16), 16*3+3, RA8875_YELLOW);

  printValues();
  

  

// === redraws data in the LCD image test 

  tft.graphicsMode(); // ==== GRAPHICS MODE ON ====
  tft.setFont(&FreeSerifItalic24pt8b);    
  tft.setCursor(200,200); 
  for(int i=0; i<strlen(sdceng1); i++)       // Steven with degree symbol 
    tft.write(sdceng1[i]); 


  tft.setFont(&FreeSerif9pt8b);              // Steven with degree symbol 
    tft.setCursor(400,200); 
  for(int i=0; i<strlen(sdceng1); i++)
    tft.write(sdceng1[i]);   

  tft.drawRect(5,165, 130, 75, RA8875_YELLOW);
  tft.setCursor(5,160); 


//=== Display the Sun and Moon data ================

  char test1[20];
  my_EE_read(EE_SUNRISE, test1, 12);
  test1[9] = '\0';

  char printBuffer[25];

  tft.print("-[Sun Data]- \n");
  sprintf(printBuffer, "  Rise: %s \n",test1);   // from EEPROM 
  //tft.print("   Rise: 6:54 AM \n");
  tft.print(printBuffer);

  my_EE_read(EE_SUNSET, test1, 12);
  test1[9] = '\0';
  sprintf(printBuffer, "  Set: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer);
  //tft.print("   Set: 5:12 PM");

  tft.print("  High: 6:54 AM \n");


//================== MOON =====

 
  Box moonbox={20,280,100,100, RA8875_RED}; 
  // draw_map(lastcres,100,100,moonbox);
  draw_map(phase_array[moondisplay],100,100,moonbox);
  
  tft.drawRect(5,265, 130, 150, RA8875_YELLOW);
  tft.setCursor(5,260); 
  tft.print("-[Moon Data]- \n");


  //my_EE_read(EE_MOONRISE, test1, 12);
  //test1[9] = '\0';
  //sprintf(printBuffer, "  Rise: %s \n",test1);   // from EEPROM 
  //tft.print(printBuffer);
  //tft.print("   Rise: 6:54 AM \n");

  //my_EE_read(EE_MOONSET, test1, 12);
  //test1[9] = '\0';
  //sprintf(printBuffer, "  Set: %s \n",test1);   // from EEPROM 
  //tft.print(printBuffer);
  //tft.print("   High: 6:54 AM \n");  

  my_EE_read(EE_MOONLAST, test1, 15);
  test1[11] = '\0';
  sprintf(printBuffer, "  LQ: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer);
  //tft.print("   LQ: 5:12 PM \n");

  my_EE_read(EE_MOONNEW, test1, 15);
  test1[11] = '\0';
  sprintf(printBuffer, "  New: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer);  
  //tft.print("   New: 6:54 AM \n");

  my_EE_read(EE_MOONFIRST, test1, 15);
  test1[11] = '\0';
  sprintf(printBuffer, "  1Q: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer); 
  //tft.print("   1Q: 6:54 AM \n");


  my_EE_read(EE_MOONFULL, test1, 15);
  test1[11] = '\0';
  sprintf(printBuffer, "  Full: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer); 
  //tft.print("   Full: 5:12 PM");

  my_EE_read(EE_MOONCURPHASE, test1, 15);
  test1[15] = '\0';
  sprintf(printBuffer, "  Phase: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer); 

 

  
//=== END Display the Sun and Moon data ================


// =============== Footer Area =============================
  
  tft.setFont(&FreeMonoBoldOblique9pt8b); 
  tft.drawRect(0,479-20, 799, 20, RA8875_YELLOW);
  tft.setCursor(200,479-4); // was 316

  char buf[27];
  int n = sprintf(buf, "AstroClock%c KC6FEW 2018 ",0xA9); // 0xA9 is the circled c symbol 
  Serial.println(n);
  //tft.print("AstroClock KC6FEW, 2017");
  tft.print(buf);
  tft.print(theDate);
  if(NTPResponse != false)
    tft.fillCircle(625, 468, 8, RA8875_GREEN);
   else
    tft.fillCircle(625, 468, 8, RA8875_RED);
    
// =============== Footer Area =============================

  tft.setFont(&FreeSerifItalic24pt8b); 
  
  mapbox={150,127,500,225, RA8875_RED};
  for(row=50; row<100; row++) 
    {
      yield(); 
      for(column=50; column<100; column++) 
      {
          tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&lcd3[column + row*500]));    // GOOD
      }
    }


    
  tft.setFont(&FreeSerifItalic24pt8b);    
  tft.setCursor(200,200); 
  for(int i=0; i<strlen(sdceng1); i++)
    tft.write(sdceng1[i]); 


    
  tft.textMode();   // ==== TEXT MODE ON ====


  //uint8_t wtf = rtc.SetClockType(HR_12, 8);
  //Serial.print("SetClockType time: "); 
  //Serial.println(wtf);


}
// ================== END =============

  float bm_temp=0;
  float bm_prior=0;
  const char degreefont[] ={0xB0,0x0};
  //const char newline[] = {0x0d, 0x0a, 0x00}; // newline is not in the internal font map 
  uint8_t newline = '\n';     // still not working 


  int16_t px;
  int16_t py;
  uint16_t pw;
  uint16_t ph;

//====================================================================================
void loop() 
{
  now1 = rtc.now(); 
  String RTC_time = formatRTC_Time();
  Serial.print(F("RTC time: ")); 
  Serial.println(RTC_time);
  
  /********************

  uint8_t cl_type = rtc.GetClockType();
  Serial.print("GetClockType time: "); 
  Serial.println(cl_type);
  
  Serial.print("NTP response is: ");
  Serial.println(NTPResponse);
  if(NTPResponse == false)
  {
      setTime(now1.hour(), now1.minute(), now1.second(),now1.day(), now1.month(), now1.year());
      Serial.println("Selecting RTC as master clock, NTP error" );
  }
  else
    Serial.println("Selecting NTP as master clock" );
  ****************************/
  
  NTP_loop();

  
  uint16_t row=0; 
  uint16_t column=0;
  tft.textEnlarge(1);  
  // printValues();
  tft.textMode();
  tft.textColor(RA8875_YELLOW, RA8875_PURPLE);
  tft.textSetCursor(10+1, 40+1);
  
  delay(1000);
  char t_buffer[20];
  char h_buffer[20];
  char a_buffer[20];


  Box TEMP_box={10,40,500,225, RA8875_RED};
  Box HUMID_box={10,127,500,225, RA8875_RED};


  tft.textSetCursor(5, 75);
  bm_temp = bme.readTemperature();
  bm_temp = CtoF(bm_temp); 
  
  tft.textWrite("Temp(");
  tft.textWrite(degreefont);
  tft.textWrite("f): ");
  tft.textWrite(dtostrf(bm_temp, 2,2, t_buffer) );

  float bm_humid = bme.readHumidity(); 
  tft.textSetCursor(290, 75);
  tft.textWrite("Hum: ");
  tft.textWrite(dtostrf(bm_humid, 2,2, h_buffer) );
  tft.textWrite("%");

  float bm_alt = bme.readAltitude(SEALEVELPRESSURE_HPA); 
  bm_alt = MtoFt(bm_alt);
  tft.textSetCursor(500, 75);
  tft.textWrite("Alt: ");
  tft.textWrite(dtostrf(bm_alt, 2,2, a_buffer) );
  tft.textWrite("ft");






/*******************
 * #include <iomanip> // setprecision
#include <sstream> // stringstream

double pi = 3.14159265359;
stringstream stream;
stream << fixed << setprecision(2) << pi;
string s = stream.str();
 */


  tmElements_t thetime;
  time_t mytest = now();
  breakTime(mytest, thetime);
  double jday= JD_Now(&thetime);
  //Serial.print(" JD == ");
  //Serial.println(jday);


 tft.graphicsMode(); // ==== GRAPHICS MODE ON ====

 
  //tft.drawRect(200, 200, 272, 33, RA8875_YELLOW); // for row 1 box
  // redraw     tft.drawRect(200, 250, 272, 33, RA8875_YELLOW); // for row 2 box
  // Box mapbox={150,127,500,225, RA8875_RED} for big LCD 
  //    tft.drawRect(475, 250, 90, 30, RA8875_WHITE);
  
  Box source = {471, 250, 95, 31, RA8875_WHITE};    // width was 95
  //tft.drawRect(source.x, source.y, source.w, source.h, source.c);

  // for flicker avoidance ??? ============

/*************************************
  BTE_block_move  ( source.x, //SourceX
                    source.y, //SourceY 
                    source.w, //Width 125
                    source.h, //Height
                    210, //DestX
                    250, //DestY
                    0x02);
************************************/ 
  Box mapbox={150,127,500,225, RA8875_RED};
//  for(row=125; row<168; row++) 
//    {
//      yield(); 
//      for(column=50; column<175; column++) 
//      {
//          tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&lcd3[column + row*500]));    // GOOD
      
//      }
//    }


 // tft.setTextSize(2); // makes graphic fonts bigger 
 
  tft.textColor(RA8875_YELLOW, RA8875_DARK_GREY);
  Box fontposition ={210, 250, 99, 24, RA8875_GREEN};
  //tft.drawRect(fontposition.x, fontposition.y, fontposition.w, fontposition.h, RA8875_WHITE);   // font posiioning rect 
  //      tft.fillRect(fontposition.x, fontposition.y, fontposition.w, fontposition.h, RA8875_DARK_GREY); // blank out the flicker vs BTE move? about the same 
  
  
  
  tft.setFont(&FreeSerifItalic18pt8b);    
  tft.setCursor(fontposition.x,fontposition.y + fontposition.h); // font position is lower left !! need pad ?

  //tft.setTextSize(2);
  // tft.textColor(RA8875_YELLOW, RA8875_DARK_GREY);   // no change on graphic font color but for text fonts! 
  
 // tft.drawRect(fontposition.x, fontposition.y, fontposition.w, fontposition.h, RA8875_WHITE); //rect for font position check 
  
  tft.setTextColor(RA8875_WHITE, RA8875_DARK_GREY);    // causes a fg, bg color change for graphic fonts for sdcwrite 
  
  // ================ Uses the modified drawChar routine for flicker reduction, still some stray pixels ===============


  for(int i=0; i<5; i++)
    tft.SDCwrite(t_buffer[i]);
  tft.SDCwrite(degreefont[0]);
  tft.SDCwrite(0x20);   // add a space

 // tft.drawLine(fontposition.x, 0, fontposition.x, 479, RA8875_WHITE);   // debug line for font position 

  tft.setCursor(fontposition.x,fontposition.y + (2*fontposition.h)+10);
 

  for(int i=0; i<5; i++)
    tft.SDCwrite(h_buffer[i]);
  tft.SDCwrite('%');
 // tft.println(h_buffer);    // need to write my version of this, print and println 

  

  //tft.print("   "); 
  //tft.print('Z');     // font color follows the setTextColor for SDCdraw but if used, need to do seTextColor for any graphic font write
 // tft.setTextSize(1); 
  
  
  //tft.print(bm_temp); // this works too 
  //tft.print(degreefont);
  // tft.print(" JD = ");
  // tft.print(jday);


  tft.textMode();
  tft.textColor(RA8875_YELLOW, RA8875_DARK_GREY);    // need this as the fillrect color becomes the text color !!!
  String theTime = formatTime();
  tft.textSetCursor(210, 325);
  tft.textWrite(theTime.c_str());
  tft.textWrite(" JD=");
  tft.textWrite(String(jday).c_str());

  tft.textSetCursor(210, 360);
  tft.textWrite(RTC_time.c_str());



  

//  tft.getTextBounds(t_buffer, 10, 40, &px, &py, &pw, &ph);    // this works ok 
//  Serial.println(px);
//  Serial.println(py);
//  Serial.println(pw);
//  Serial.println(ph);




 


 
}
// ============== END END END ===============

void printValues() 
{
    delay(100); // let sensor boot up 
    Serial.print(F("Temperature = "));
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print(F("Approx. Altitude = "));
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print(F("Humidity = "));
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}

// ===============================================================
String formatRTC_Time()
{
  String time_string;
  String m_minute;
  String m_second;
  String m_hour;
  String spacer = ":";
  String lead_zero = "0";

  uint8_t sec_digit = now1.second();
  uint8_t min_digit = now1.minute();
  uint8_t hrs_digit = now1.hour();

  if(hrs_digit < 10)
  {
    m_hour = String(lead_zero + now1.hour());
  }
  else
    m_hour = String(now1.hour());

  
  if(min_digit < 10)
  {
    m_minute = String(lead_zero + now1.minute());
  }
  else
    m_minute = String(now1.minute());

  if(sec_digit < 10)
  {
    m_second = String(lead_zero + now1.second());
  }
  else
    m_second = String(now1.second());

  String date_string = String(m_hour + spacer + m_minute + spacer + m_second);

  //Serial.print("from format time ");
  //Serial.println(date_string);
  //Serial.println(now());    // returns seconds since 1970
  
  return date_string; 
}

// =============================================================
void splash_screen(Adafruit_RA8875 &mytft)
{

 if (!mytft.begin(RA8875_800x480)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  mytft.displayOn(true);
  mytft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  mytft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  mytft.PWM1out(255);
  mytft.fillScreen(RA8875_PURPLE);

  /* Switch to text mode */  
  //tft.textMode();
  mytft.graphicsMode();
  mytft.setFont(&FreeSerifItalic18pt8b); 

  mytft.textSetCursor(10, 10);


  // check font set
  //tft.writeCommand(0x21);
  //Serial.print("font set selected: ");
  //tft.writeData(0x01); 
  //Serial.println(tft.readData());



/* trun on cursor blink for 0.5 seconds  */
  mytft.writeCommand(RA8875_MWCR0);
  mytft.writeData(0xF0);
  mytft.writeCommand(0x44);
  mytft.writeData(0x1E);
  
  /* Render some text! */
  char string[15] = "Hello, World! ";
  mytft.textTransparent(RA8875_WHITE);
  mytft.textWrite(string);
  mytft.textColor(RA8875_WHITE, RA8875_RED);
  mytft.textWrite(string);
  mytft.textTransparent(RA8875_CYAN);
  mytft.textWrite(string);
  mytft.textTransparent(RA8875_GREEN);
  mytft.textWrite(string);
  mytft.textColor(RA8875_YELLOW, RA8875_CYAN);
  mytft.textWrite(string);
  mytft.textColor(RA8875_BLACK, RA8875_MAGENTA);
  //tft.print(string); 
  mytft.textWrite(string);

  /* Change the cursor location and color ... */  

  mytft.textSetCursor(100, 100);
  //tft.textTransparent(RA8875_RED);
  mytft.textColor(RA8875_WHITE, RA8875_RED);
  /* If necessary, enlarge the font */
  mytft.textEnlarge(1);
  /* ... and render some more text! */
  //tft.setFont();
  mytft.textWrite(string);
  mytft.textSetCursor(100, 150);
  mytft.textEnlarge(2);
  mytft.textWrite(string);
  
}



