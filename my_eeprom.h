
#ifndef MY_EEPROM_H
#define MY_EEPROM_H


#include <ESP8266WiFi.h>
#include <EEPROM.h>


#define EE_SIZE   512 // total EEPROM size
#define EE_BASE   10  // base address


typedef enum 
{
    EE_SUNRISE,         // 0
    EE_SUNSET,          // 1
    EE_MOONRISE,        // 2
    EE_MOONSET,         // 3
    EE_MOONPERCENT,     // 4
    EE_MOONCURPHASE,    // 5
    EE_MOONLAST,        // 6
    EE_MOONNEW,         // 7
    EE_MOONFULL,        // 8
    EE_MOONFIRST,       // 9
    EE_DST,             // 10
    EE_LAT,             // 11
    EE_LNG,             // 12
    EE_UTC_OFFSET,      // 13
    EE_LASTUPDATE,      // 14
    EE_END              // 15
} EE_Name;

extern const uint16_t nv_sizes[EE_END];

bool my_EE_write(EE_Name e, String &data, uint8_t len);
bool my_EE_read(EE_Name e, char buf[], uint8_t len);


#endif
