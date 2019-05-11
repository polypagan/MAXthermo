/* MAXthermo thermocouple Library for MAXIM MAX6675 and MAX31655 chips
 * 
 * D. del Vecchio 20180205 version 0.0.2
 * 
 * Revision history:
 * 0.0.0 20180205 First working version
 * 0.0.1 20180206 Used _begun logic to make backwards compatible with MAX6675 library
 * 0.0.2 20180206 Removed switch/case logic, simplified
 * 
 * Based on the work of Limor Fried of Adafruit Industries
 * (MAX6675 and Adafruit_MAX31855 libraries)
 * see license.txt in this folder.
 * 
 * This code determines which interface chip is being addressed on the fly,
 * and it does so at each temperature read. That's probably not really necessary, 
 * but the actual cost is quite low.
 * 
 * Additional methods are provided to:
 *  read temperature in Fahrenheit degrees,
 *  read internal chip temperature (only meaningful if MAX31655)
 *  read cause of errors (temp reported to be NAN)
 *  set a mask for error conditions (defined in header file)
 * 
 */

#include "MAXthermo.h"
#include <SPI.h>


// constuctor for software SPI interface mode
MAXthermo::MAXthermo(int8_t clk, int8_t cs, int8_t miso) {

    _clk = clk;
    _cs = cs;
    _miso = miso;
    
    _mask = 7;

    _begun = false;
}

// constructor for hardware SPI interface mode
MAXthermo::MAXthermo(int8_t cs) {
    _cs = cs;
    _clk = _miso = -1;

    _mask = 7;

    _begun = false;
}

// begin() may be called repeatedly to re-assert pinMode and levels
// should be called before any of the following routines.
void MAXthermo::begin(void) {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  
 if (_clk == -1) {
    // we are using hardware SPI
    // so start it.
    SPI.begin();
  } else {
    pinMode(_clk, OUTPUT); 
    pinMode(_miso, INPUT);
  }
  _begun = true;
}

// this is the basic routine. Returns double Celsius temperature or NAN on error
double MAXthermo::readCelsius(void) {  // this is where most of the work gets done
int32_t v;
double centigrade;
int8_t _ct;

    v = spiread32();    // always read 32 bits

    _error = 0;     // wipe old errors

    if ((v >> 16) == (v & 0xffff)) {    // if 2 16-bit halves match, must be 16-bit device (MAX6675)
        v >>= 16;    // just look at 16 bits (both halves equal)
        
        _internal = 0;  // MAX6675 uses fixed 0 degree reference

        if (v & 4)_error = OC;   // save error (only OC detected)

            if ((_mask & OC) && _error) {  // open thermocouple
                centigrade = NAN;
            } else {    // no un-masked error

 //           Serial.print('(');
 //           Serial.print(v >> 5, DEC);      // integer part
 //           Serial.println(')');
            
            v >>= 3;                        // toss error, etc bits
            centigrade = (double)v * 0.25;  // LSB = 0.25 degrees
        }
        
    } else {    // 32-bit device (MAX31655)
        
            _error = v & 0x7;   // record errors from this read

            if (v & _mask) {    // return NAN if error specified
                centigrade = NAN;
            } else {    // no un-masked errors
                
//              copy bits 4..15 
                _internal = (v >> 4) & 0xfff;         // record internal temp (scaled when returned)
            
//                Serial.println((v >> 20), DEC);   // integer part

                v >>= 18;           // done with low 18 bits
                if (v & 0x2000) {   // negative
                    v |= 0xffffc000;  // sign extend
                }
      
                centigrade = (double)v * 0.25;  // LSB = 0.25 degrees

            }     
    }
    
    return centigrade;
  
} // end readCelsius()

// returns internal temperature from most recent reading
double MAXthermo::readInternal(void) {
    
    if (_internal & 0x800) {    // internal temperature negative
    // Convert to negative value by extending sign and casting to signed type.
        _internal |= 0xf800;    // and extend
    }
    return (double)_internal * 0.0625;
}

// returns most recent error state
int8_t MAXthermo::getError(void) {
        return _error;
}

// use to set which error states are of interest (and produce NAN result)
void MAXthermo::setErrorMask(uint8_t mask) {
    _mask = mask & 7;
}

// private routine to read 32 bits from hardware/software SPI
// this trivial yet crucial routine was lifted almost intact from Lady Ada.
uint32_t MAXthermo::spiread32(void) { 
  int i;
  uint32_t d = 0;

  if(!_begun)        // sop to back compatibility & lazy, forgetful programmers
      begin();

  digitalWrite(_cs, LOW);   // stop any conversion, get ready for transfer.
  delay(1);

  if(_clk == -1) {
    // hardware SPI
    // MAX6675 spec'd at 4.3 MHz, MSBFIRST, idle low, falling edge clock
    // MAX31655 can do 5 MHz, rest same.
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

    
    // do four end-to-end byte transfers, packing into d
    d = SPI.transfer(0);
    d <<= 8;
    d |= SPI.transfer(0);
    d <<= 8;
    d |= SPI.transfer(0);
    d <<= 8;
    d |= SPI.transfer(0);

    SPI.endTransaction();
  } else {
    // software SPI

    digitalWrite(_clk, LOW);    // seems redundant (?)
    delay(1);

    for (i=31; i>=0; i--) {
      digitalWrite(_clk, LOW);
      delay(1);
      d <<= 1;
      if (digitalRead(_miso)) {
	d |= 1;
      }
      
      digitalWrite(_clk, HIGH);
      delay(1);
    }
  }

  digitalWrite(_cs, HIGH);
  return d;
}
