
#include "sdc.h"


extern Adafruit_RA8875 tft;
static uint16_t row=0; 
static uint16_t column=0;



// =========== BTE block move ===============
void BTE_block_move(  int16_t SourceX, 
                int16_t SourceY, 
                int16_t Width, 
                int16_t Height, 
                int16_t DestX, 
                int16_t DestY, 
                uint8_t ROP)
{
  tft.graphicsMode(); 
  BTE_source(SourceX,SourceY);
  BTE_size(Width,Height);
  BTE_destination(DestX,DestY);
  BTE_ropcode(0xc2);
  BTE_enable(true); 
  int j=0; 

  
  BTE_check_busy();
  // Serial.println("hello .....");
}

//===================================================
//===== never got this working correctly ==========
void BTE_write_move(  int16_t SourceX, 
                int16_t SourceY, 
                int16_t Width, 
                int16_t Height, 
                int16_t DestX, 
                int16_t DestY, 
                uint8_t ROP)
{
  tft.graphicsMode(); 
  BTE_source(SourceX,SourceY);
  BTE_size(Width,Height);
  BTE_destination(DestX,DestY);
  BTE_ropcode(0x0c);
  BTE_enable(true);
   
  int j=0; 
  tft.writeCommand(RA8875_MRWC);
    for(row=0; row<144; row++)    // was 163 for world 565, 213 for charmap fpt LCD2, 320 x144
    {
      BTE_memory_busy();
      BTE_check_busy();
      //yield(); 
      Serial.println(j);
      for(column=0; column<320; column++) 
      {
          tft.writeData(pgm_read_word(&LCD2[j])); 
          Serial.println(pgm_read_word(&LCD2[j]));
          j=j+1;
          delay(2);
      }
    }
}

//============== BTE set size ===============
void BTE_size(int16_t w, int16_t h)
{
    tft.writeReg(RA8875_BEWR0, w & 0xFF);  
    tft.writeReg(RA8875_BEWR1, w >> 8);     
    tft.writeReg(RA8875_BEHR0, h & 0xFF);  
    tft.writeReg(RA8875_BEHR1, h >> 8);      
} 

//============== BTE set source ===============
void BTE_source(int16_t SX,int16_t SY)
{
  tft.writeReg(RA8875_HSBE0,SX & 0xFF);
  tft.writeReg(RA8875_HSBE1,SX >> 8);
  tft.writeReg(RA8875_VSBE0,SY & 0xFF);
  tft.writeReg(RA8875_VSBE1,0x00);      // force layer1 for now
  tft.writeReg(RA8875_VSBE1,SY >> 8);
} 

//============= BTE Set destination ===========
void BTE_destination(int16_t DX,int16_t DY)
{
  tft.writeReg(RA8875_HDBE0, DX & 0xFF);
  tft.writeReg(RA8875_HDBE1, DX >> 8);
  tft.writeReg(RA8875_VDBE0, DY & 0xFF);
  tft.writeReg(RA8875_VDBE1, 0x00);     // force layer1 for now
  tft.writeReg(RA8875_VDBE1, DY >> 8);
}

//=============== BTE set ROP ===================
void BTE_ropcode(unsigned char setx)
{
    tft.writeReg(RA8875_BECR1,setx); //BECR1 0xc2 for basic block move  
}

//=================== BTE enable ==========
void BTE_enable(bool on) 
{  
  tft.writeCommand(RA8875_BECR0);
  uint8_t temp = tft.readData();
  temp = 0x80;
  tft.writeData(temp);
}

//=================== BTE disable ==========
void BTE_disable(bool on) 
{  
  tft.writeCommand(RA8875_BECR0);
  uint8_t temp = tft.readData();
  temp = 0x00;
  tft.writeData(temp);
}

// ===============BTE check busy ===================
void BTE_check_busy()
{  
  uint16_t statusreg =0xab;
  //yield();
  do
  {
    //yield();
    statusreg = tft.readStatus();
    //Serial.print("hello .....  ");
    //Serial.println(statusreg);
  }
  while(  (statusreg & (0x40) ) == 0x40);
  //Serial.println("good bye check busy .....");
}

// ===============BTE check memory ===================
void BTE_memory_busy()
{  
  uint16_t statusreg =0xab;
  //yield();
  do
  {
    //yield();
    statusreg = tft.readStatus();
    //Serial.print("hello memory .....  ");
    //Serial.println(statusreg);
  }
  while(  (statusreg & (0x90) ) == 0x80);
  //Serial.println("good bye memory busy .....");
}

