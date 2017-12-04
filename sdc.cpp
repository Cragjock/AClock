
#include "sdc.h"


extern Adafruit_RA8875 tft;
  uint16_t row=0; 
  uint16_t column=0;


bool setvisiblecursor(enum RA8875tcursor c, bool blink)
{
  uint8_t cW = 0;
  uint8_t cH = 0;
  uint8_t temp_val = tft.readReg(RA8875_MWCR0);
  c == NOCURSOR ? temp_val &= ~(1 << 6) : temp_val |= (1 << 6); // turn cursor and blink off
  
  if(!blink)
  {
      temp_val &= ~(0x20);
      tft.writeReg(RA8875_MWCR0, temp_val);
  }
  
  else
  {
      temp_val|= 0x20;
      tft.writeReg(RA8875_MWCR0, temp_val); 
      switch (c) 
      {
        case IBEAM:
            cW = 0x01;
            cH = 0x1F;
            break;
        case UNDER:
            cW = 0x07;
            cH = 0x00;    // default is 0 in spec pg 29
            break;
        case BLOCK:
            cW = 0x07;
            cH = 0x1F;
            break;
        case NOCURSOR:
        default:
            break;
    }
  }
  tft.writeReg(RA8875_CURHS, cW);
  tft.writeReg(RA8875_CURVS, cH);
 return blink; 
}

/***************
 * 
 *  Blink rate (sec)  = BTCR reg value * (1/frame rate) fr = 60hz, pg 89
 *  for .5 seconds, rate = 0x1E (30)
 *  for 1 sec, rate = 0x3c (60)
 * 
 */

void setCursorBlinkRate(uint8_t rate)
{
  if(rate > 60)
    rate = 30;

  tft.writeReg(RA8875_BTCR,rate); 
  
}

// ============================================
void LoadCustomChar(const uint8_t symbol[],enum RA8875_custom_font address)
{
    bool mode_changed = false; 

    tft.writeCommand(RA8875_MWCR1);
    uint8_t tempMWCR1 = tft.readData();

    /* Set graphics mode per spec page 113*/
    uint8_t what_mode = tft.readReg(RA8875_MWCR0);
    
    if( (what_mode & (0x80)) == 0x80) // therefore in text mode 
    {
      tft.graphicsMode(); 
      Serial.println("set graph mode"); 
      mode_changed = true;
    }
    
    // set CGRAM custom char location address
    tft.writeReg(RA8875_CGSR, address);    // reg 23h

    //set reg 21 bit 7 =0
    tft.writeReg(RA8875_FNCR0, 0x00);       // set reg 21h bit 7 = 0, one layer is selected 
    tft.writeReg(RA8875_MWCR1, (2<<CGRAM)); // set reg 41h bit 2/3 to CGRAM 

    tft.writeCommand(RA8875_MRWC);  // set the write register 
    for (int i=0;i<16;i++)
    {
      tft.writeData(pgm_read_byte(&symbol[i]));   
    }
  
    //restore register
    tft.writeReg(RA8875_MWCR1,tempMWCR1);
    if(mode_changed)
      tft.textMode(); 
}

//=========== Display custom char ==========
//=== more defines additional custom char to display. 
void displaycustomChar(enum RA8875_custom_font address, uint8_t more)
{
    bool mode_changed = false;
    uint8_t tempFNCR0 = tft.readReg(RA8875_FNCR0); 
    uint8_t what_mode = tft.readReg(RA8875_MWCR0);
    if( (what_mode & (0x80)) != 0x80) // therefore in graphcis mode 
    {
      tft.textMode(); 
      Serial.println("set graph mode"); 
      mode_changed = true;
    }

    uint8_t setCGRAM = tempFNCR0 | selectCGRAM; // need to set bit 7 and 5 in reg 21h
    tft.writeReg(RA8875_FNCR0,setCGRAM);

    tft.writeCommand(RA8875_MRWC); // prepare to write
    if(more > 0)
    {
      for(int i=0; i<=more; i++)
      {
        delay(1);
        tft.writeData(address+i);
        Serial.print("custom font: "); 
        Serial.println(i); 
      }
    }
    else
      tft.writeData(address);
   
  if(mode_changed)
      tft.graphicsMode(); 
    tft.writeReg(RA8875_FNCR0, tempFNCR0);
}







  

// ===== drawsthe LCD Display image =====
void draw_map(const uint16_t scmap[],uint16_t r, uint16_t c, Box mapbox)
{
    int row, column =0;
    int j =0; 
    
    for(row=0; row<r; row++)    // was 163 for world 565, 213 for charmap fpt LCD2, 320 x144
    {
      yield(); 
      for(column=0; column<c; column++) 
      {
          //tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&charmap[j])); // 320x215
          //tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word(&world565[j]));  //320x163
          tft.drawPixel((column + mapbox.x), (row+mapbox.y), pgm_read_word( &(scmap[j]) ));    // GOOD
          // tft.drawPixel((column + mapbox.x), (row+mapbox.y), scmap[j]);    // GOOD
          //Serial.println(scmap[j]);
          //Serial.print("j ");
          //Serial.println(j);
          
          
          j=j+1;
      }
    }  
}






const uint8_t g1_cursor[] PROGMEM = {
0xFF,  0xFA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xEA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAA,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0xAB,
0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAF, 0xFF
};


// my ISS looking icon for test mode 
  const uint8_t bmSatLeft[]PROGMEM ={0x80, 0x80, 0x90, 0x91, 0x91, 0x91, 0x93, 0xFF, 0x93, 0x91, 0x91, 0x91, 0x90, 0x80, 0x80, 0x00 };
  const uint8_t bmSatright[]PROGMEM = {0x02, 0x02, 0x12, 0x12, 0x12, 0x12, 0x92, 0xFE, 0x92, 0x12, 0x12, 0x12, 0x12, 0x02, 0x02, 0x00 };





