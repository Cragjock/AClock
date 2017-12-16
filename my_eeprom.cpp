
#include "my_eeprom.h"




const uint16_t nv_sizes[EE_END] = 
{
    15, // EE_SUNRISE      0  
    15, // EE_SUNSET       1
    15, // EE_MOONRISE     2
    15, // EE_MOONSET      3
    15, // EE_MOONPERCENT  4
    4,  // EE_MOONCURPHASE 5
    15, // EE_MOONLAST     6
    15, // EE_MOONNEW      7
    15, // EE_MOONFULL     8
    15, // EE_MOONFIRST    9
    1,  // EE_DST          10
    4,  // EE_LAT          11
    4,  // EE_LNG          12
    1,  // EE_UTC_OFFSET   13
    15, // EE_LASTUPDATE   14
};



//========================================================
bool my_EE_write(EE_Name e, String &data, uint8_t len)
{
  uint16_t wr_address = EE_BASE;
  //Serial.print(" the EE write string is: ");
  //Serial.println(data);
  
  for(int i=0; i<e; i++)
  {
    wr_address = wr_address + nv_sizes[i];
    //Serial.print("wr_address: ");
    //Serial.println(wr_address);
  }

  //Serial.print("wr_address: ");
  //Serial.println(wr_address);
  
  EEPROM.begin(EE_SIZE);
  {
    for(int i=0; i< len; i++)
      EEPROM.write(  (wr_address+i) , data[i]);
   
  }
  EEPROM.end();
  return true; 
  
}


//================================================================
bool my_EE_read(EE_Name e, char buf[], uint8_t len)
{
  uint16_t rd_address = EE_BASE;
  //Serial.print(" the EE read string ");
  
  for(int i=0; i<e; i++)
  {
    rd_address = rd_address + nv_sizes[i];
    //Serial.print("rd_address: ");
    //Serial.println(rd_address);
  }

  //Serial.print("rd_address: ");
  //Serial.println(rd_address);
  
  EEPROM.begin(EE_SIZE);
  {
    for(int i=0; i< len; i++)
      buf[i] = EEPROM.read((rd_address+i));
   
  }
  EEPROM.end();
  return true; 
  
}


