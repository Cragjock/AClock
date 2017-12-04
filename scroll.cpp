
#include "sdc.h"


extern Adafruit_RA8875 tft;
int16_t _scrollXL,_scrollXR,_scrollYT,_scrollYB;


/*!     
        Sets the scroll mode. This is controlled by bits 6 and 7 of  
        REG[52h] Layer Transparency Register0 (LTPR0)
		Author: The Experimentalist
*/
/**************************************************************************/
void setScrollMode(enum RA8875scrollMode mode)
{
    uint8_t temp = tft.readReg(RA8875_LTPR0);
    temp &= 0x3F;            // Clear bits 6 and 7 to zero
    switch(mode){           // bit 7,6 of LTPR0
        case SIMULTANEOUS:  // 00b : Layer 1/2 scroll simultaneously.
            // Do nothing
        break;
        case LAYER1ONLY:        // 01b : Only Layer 1 scroll.
            temp |= 0x40;
        break;
        case LAYER2ONLY:        // 10b : Only Layer 2 scroll.
            temp |= 0x80;
        break;
        case BUFFERED:      	// 11b: Buffer scroll (using Layer 2 as scroll buffer)
            temp |= 0xC0;
        break;
        default:
            return;         	//do nothing
    }
    //TODO: Should this be conditional on multi layer?
    //if (_useMultiLayers) _writeRegister(RA8875_LTPR0,temp);
    //writeReg(RA8875_LTPR0,temp);
    tft.writeData(temp);
}

/**************************************************************************/
/*!		
		Define a window for perform scroll
		Parameters:
		XL: x window start left
		XR: x window end right
		YT: y window start top
		YB: y window end bottom

*/
/**************************************************************************/
void setScrollWindow(int16_t XL,int16_t XR ,int16_t YT ,int16_t YB)
{
	_scrollXL = XL; _scrollXR = XR; _scrollYT = YT; _scrollYB = YB;
    tft.writeReg(RA8875_HSSW0,(_scrollXL & 0xFF));
    tft.writeReg(RA8875_HSSW0+1,(_scrollXL >> 8));
  
    tft.writeReg(RA8875_HESW0,(_scrollXR & 0xFF));
    tft.writeReg(RA8875_HESW0+1,(_scrollXR >> 8));   
    
    tft.writeReg(RA8875_VSSW0,(_scrollYT & 0xFF));
    tft.writeReg(RA8875_VSSW0+1,(_scrollYT >> 8));   
 
    tft.writeReg(RA8875_VESW0,(_scrollYB & 0xFF));
    tft.writeReg(RA8875_VESW0+1,(_scrollYB >> 8));
	delay(1);
}

/**************************************************************************/
/*!
		Perform the scroll

*/
/**************************************************************************/
void scroll(int16_t x,int16_t y)
{ 

	if (_scrollXL == 0 && _scrollXR == 0 && _scrollYT == 0 && _scrollYB == 0){
		//do nothing, scroll window inactive
	} 
	else 
	{
		tft.writeReg(RA8875_HOFS0,(x & 0xFF)); 
		tft.writeReg(RA8875_HOFS1,(x >> 8));
 
		tft.writeReg(RA8875_VOFS0,(y & 0xFF));
		tft.writeReg(RA8875_VOFS1,(y >> 8));
	}
}	 


/******************************************
 * 
 * 
// ===== scroll ================= 
  setScrollWindow(0, tft.width() - 1, 0, 80);  //Specifies scrolling activity area
  uint16_t i;
  for (i = 0; i < 80; i++) 
  {
    scroll(0, i);
    delay(10);
  }
  
  delay(500);
  for (i = 79; i > 0; i--) 
  {
    scroll(0, i);
    delay(10);
  }
  delay(500);
  for (i = 0; i < tft.width(); i++) 
  {
    scroll(i, 0);
    delay(5);
  }
  delay(500);
  for (i = tft.width() - 1; i > 0; i--) 
  {
    scroll(i, 0);
    delay(5);
  }
  delay(500);
 *********************************************/



