# MAXthermo

This library (designed for Arduino) implements a class that fetches thermocouple data from MAX6675 or MAX31855 thermocouple interface chips.

The former part is now obsolete, but still available in small quantities. It is slightly easier to use (designed for 5v, works just fine at 3v3), only reports positive temperatures, gives 0.25 degree (C) values.

The later part is newer, currently available, more expensive, works on 3v3 only, is pin-compatable with the older part (even though many, many webpages claim otherwise), provides negative values, allows reading internal chip temp, comes in variants for different types of thermocouples.

The point of this library is that it doesn't care which you use. It discovers which part you're using each time you request a reading (because that's easy; takes no more than just reading MAX31855 would anyway).

I've based this on Adafruit MAX6675 and MAX31855 libraries. Thanks, Lady Ada!

I've added methods to get errors (when read returns NaN) and to set an error mask, allowing certain types of errors to be ignored.

