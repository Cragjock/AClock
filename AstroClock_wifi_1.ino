/******************************************************************
  BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 base was mapdrawplay 5
 ******************************************************************/

#include <SPI.h>
#include <EEPROM.h>
#include <stdint.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "Fonts/FreeMonoBoldOblique9pt8b.h"
#include "Fonts/FreeSerifItalic24pt8b.h"    // includes extended ASCII characters 
#include "Fonts/FreeSerifItalic18pt8b.h"
#include "Fonts/FreeSerif9pt8b.h"   // this size ok to use, still readable 
#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"
#include "sdc.h"
#include "my_wifi.h"
#include "my_eeprom.h"



#define RA8875_INT 3
#define RA8875_CS 2   // tied to blue LED too? 
#define RA8875_RESET 16

#define SEALEVELPRESSURE_HPA (1013.25)


Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
Adafruit_BME280 bme;  // I2C


/**************************************************
 * SETUP START
 */
void setup() 
{
  bool status;
  Serial.begin(115200);

  String feedback = my_wifi();
  Serial.print("Wifi feedback ");
  Serial.println(feedback);

  Serial.print("read EEPROM? ");


  
  
  //EEPROM.begin(512);
  // char ee1 = EEPROM.read(0x02);
  //      char test1[13];

  //    my_EE_read(EE_SUNSET, test1, 12);
  //    test1[9] = '\0';
  
/**********************************
  for(int i=0; i<12; i++)
  {
    test1[i] = EEPROM.read(0x10+i);
    Serial.println("EEPROM.read");
    Serial.println(test1[i]);
  }
  test1[9] = '\0';
   EEPROM.end();
******************************/
  
  //Serial.print("read test1? ");
  //Serial.println(test1);
  //Serial.println();
 
 // EEPROM.end();

  // Serial.println(ESP.getFlashChipSize());


  
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

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255); 
  tft.fillScreen(RA8875_PURPLE);



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
  const char sdceng1[] ={'S','t','e','v','e','n',0xB0,0x0}; 
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

  /********************
  int16_t px;
  int16_t py;
  uint16_t pw;
  uint16_t ph;
  tft.getTextBounds("-[Welcome!]-", 266, 5, &px, &py, &pw, &ph);    // this works ok 
  Serial.println(px);
  Serial.println(py);
  Serial.println(pw);
  Serial.println(ph);
  ***************************/


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
  

  tft.graphicsMode(); // ==== GRAPHICS MODE ON ====

  // === redraws data in the LCD image test 

  tft.setFont(&FreeSerifItalic24pt8b);    
  tft.setCursor(200,200); 
  for(int i=0; i<strlen(sdceng1); i++)
    tft.write(sdceng1[i]); 


  tft.setFont(&FreeSerif9pt8b); 
    tft.setCursor(400,200); 
  for(int i=0; i<strlen(sdceng1); i++)
    tft.write(sdceng1[i]);   

  tft.drawRect(5,165, 130, 75, RA8875_YELLOW);
  tft.setCursor(5,160); 




  char test1[15];
  my_EE_read(EE_SUNRISE, test1, 12);
  test1[9] = '\0';

  char printBuffer[25];

  tft.print("-[Sun Data]- \n");
  sprintf(printBuffer, "   Rise: %s \n",test1);   // from EEPROM 
  //tft.print("   Rise: 6:54 AM \n");
  tft.print(printBuffer);

  my_EE_read(EE_SUNSET, test1, 12);
  test1[9] = '\0';
  sprintf(printBuffer, "   Set: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer);
  //tft.print("   Set: 5:12 PM");

  tft.print("   High: 6:54 AM \n");




  tft.drawRect(5,265, 130, 150, RA8875_YELLOW);
  tft.setCursor(5,260); 
  tft.print("-[Moon Data]- \n");

  
  my_EE_read(EE_MOONRISE, test1, 12);
  test1[9] = '\0';
  sprintf(printBuffer, "   Rise: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer);
  //tft.print("   Rise: 6:54 AM \n");


  my_EE_read(EE_MOONSET, test1, 12);
  test1[9] = '\0';
  sprintf(printBuffer, "   Set: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer);
  //tft.print("   High: 6:54 AM \n");  



  my_EE_read(EE_MOONLAST, test1, 12);
  test1[11] = '\0';
  sprintf(printBuffer, "   LQ: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer);
  //tft.print("   LQ: 5:12 PM \n");


  my_EE_read(EE_MOONNEW, test1, 12);
  test1[11] = '\0';
  sprintf(printBuffer, "   New: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer);  
  //tft.print("   New: 6:54 AM \n");

  my_EE_read(EE_MOONFIRST, test1, 12);
  test1[11] = '\0';
  sprintf(printBuffer, "   1Q: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer); 
  //tft.print("   1Q: 6:54 AM \n");


  my_EE_read(EE_MOONFULL, test1, 12);
  test1[11] = '\0';
  sprintf(printBuffer, "   Full: %s \n",test1);   // from EEPROM 
  tft.print(printBuffer); 
  //tft.print("   Full: 5:12 PM");







  
  tft.setFont(&FreeMonoBoldOblique9pt8b); 
  tft.drawRect(0,479-20, 799, 20, RA8875_YELLOW);
  tft.setCursor(316,479-4); 

  char buf[27];
  int n = sprintf(buf, "AstroClock%c KC6FEW, 2017",0xA9); 
  Serial.println(n);
  //tft.print("AstroClock KC6FEW, 2017");
  tft.print(buf);
  






    

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

void loop() 
{
  uint16_t row=0; 
  uint16_t column=0;
  tft.textEnlarge(1);  
  // printValues();
  tft.textMode();
  tft.textColor(RA8875_YELLOW, RA8875_PURPLE);
  tft.textSetCursor(10+1, 40+1);
  
  delay(1000);
  char t_buffer[20];


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
  tft.textWrite(dtostrf(bm_humid, 2,2, t_buffer) );
  tft.textWrite("%");

  float bm_alt = bme.readAltitude(SEALEVELPRESSURE_HPA); 
  bm_alt = MtoFt(bm_alt);
  tft.textSetCursor(500, 75);
  tft.textWrite("Alt: ");
  tft.textWrite(dtostrf(bm_alt, 2,2, t_buffer) );
  tft.textWrite("ft");





  


 tft.graphicsMode(); // ==== GRAPHICS MODE ON ====

 
  //tft.drawRect(200, 200, 272, 33, RA8875_YELLOW); // for row 1 box
  // redraw     tft.drawRect(200, 250, 272, 33, RA8875_YELLOW); // for row 2 box
  // Box mapbox={150,127,500,225, RA8875_RED} for big LCD 
  //    tft.drawRect(475, 250, 90, 30, RA8875_WHITE);
  
  Box source = {471, 250, 95, 31, RA8875_WHITE};
  //tft.drawRect(source.x, source.y, source.w, source.h, source.c);

  // for flicker avoidance ============

/*************************************/
  BTE_block_move  ( source.x, //SourceX
                    source.y, //SourceY 
                    source.w, //Width 125
                    source.h, //Height
                    210, //DestX
                    250, //DestY
                    0x02);
/************************************/ 
  Box mapbox={150,127,500,225, RA8875_RED};
 // for(row=125; row<168; row++) 
 //   {
 //     yield(); 
 //     for(column=50; column<175; column++) 
 //     {
 //         tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&lcd3[column + row*500]));    // GOOD
 //     
 //     }
 //   }
  
  Box fontposition ={210, 250, 99, 24, RA8875_GREEN};
  // tft.drawRect(fontposition.x, fontposition.y, fontposition.w, fontposition.h, fontposition.c);   // font posiioning rect 
  tft.setFont(&FreeSerifItalic18pt8b);    
  tft.setCursor(fontposition.x,fontposition.y + fontposition.h); // font position is lower left !! need pad ?
  tft.print(bm_temp); // this works too 
  tft.print(degreefont);
  


//  tft.getTextBounds(t_buffer, 10, 40, &px, &py, &pw, &ph);    // this works ok 
//  Serial.println(px);
//  Serial.println(py);
//  Serial.println(pw);
//  Serial.println(ph);

  tft.textMode();
 
}
// ============== END END END ===============

void printValues() 
{
    delay(100); // let sensor boot up 
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}


