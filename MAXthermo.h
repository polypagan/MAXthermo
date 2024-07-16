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
 * see license.txt in this folder
 * 
 * This code determines which interface chip is being addressed on the fly,
 * and it does so at each temperature read. That's probably not really necessary, 
 * but the actual cost is quite low. (Same as reading MAX31655.)
 * 
 * Additional methods are provided to:
 *  read temperature in Fahrenheit degrees,
 *  read internal chip temperature (only meaningful if MAX31655)
 *  read cause of errors (temp reported to be NAN)
 *  set a mask for error conditions (defined in header file)
 * 
 */
#ifndef MAXTHERMO_H
#define MAXTHERMO_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// Error mask bits (add or OR in setErrorMask() call)
const uint8_t SCV = 4,  // TC shorted to Vdd
              SCG = 2,  // TC shorted to Vss
                        // TC open circuit (missing)
              OC = 1,   // as per datasheet
              OTC = 1;  // more mnemonic pseudonym

class MAXthermo {
 public:
    MAXthermo(int8_t clk, int8_t cs, int8_t miso);
    MAXthermo(int8_t cs);
    void begin(void);
    double readCelsius(void);
    double readFahrenheit(void) { return readCelsius() * 9.0/5.0 + 32; }
    double readInternal(void);
    int8_t getError(void);
    void setErrorMask(uint8_t mask);
 private:
    boolean _begun;
    int8_t _clk, _miso, _cs;
    uint8_t _error;
    uint8_t _mask;
    uint16_t _internal;
    
    uint32_t spiread32(void);
};

#endif
