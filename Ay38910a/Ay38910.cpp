/*
 
  Ay38910a.cpp source code for the library Ay38910a
  
*/

#include "Arduino.h"
#include "Ay38910a.h"


int tp[] = {//MIDI note number
  15289, 14431, 13621, 12856, 12135, 11454, 10811, 10204,//0-o7
  9631, 9091, 8581, 8099, 7645, 7215, 6810, 6428,//8-15
  6067, 5727, 5405, 5102, 4816, 4545, 4290, 4050,//16-23
  3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551,//24-31
  2408, 2273, 2145, 2025, 1911, 1804, 1703, 1607,//32-39
  1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,//40-47
  956, 902, 851, 804, 758, 716, 676, 638,//48-55
  602, 568, 536, 506, 478, 451, 426, 402,//56-63
  379, 358, 338, 319, 301, 284, 268, 253,//64-71
  239, 225, 213, 201, 190, 179, 169, 159,//72-79
  150, 142, 134, 127, 119, 113, 106, 100,//80-87
  95, 89, 84, 80, 75, 71, 67, 63,//88-95
  60, 56, 53, 50, 47, 45, 42, 40,//96-103
  38, 36, 34, 32, 30, 28, 27, 25,//104-111
  24, 22, 21, 20, 19, 18, 17, 16,//112-119
  15, 14, 13, 13, 12, 11, 11, 10,//120-127
  0//off
};


Ay38910a::Ay38910a( int dataPin, int latchPin, int clockPin, int pinBC1, int pinBCDIR, int freqOutputPin, int prescale, float period, float freq, int ocr2aval ){

    _dataPin = dataPin;
    _latchPin = latchPin;
    _clockPin = clockPin;
    _pinBC1 = pinBC1;
    _pinBCDIR = pinBCDIR;
    _freqOutputPin = freqOutputPin;
    _prescale = prescale;
    _period = period;
    _freq = freq;
    _ocr2aval = ocr2aval;
    
    pinMode(_latchPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);  
    pinMode(_clockPin, OUTPUT);
    pinMode(_pinBC1, OUTPUT);
    pinMode(_pinBCDIR, OUTPUT);        
    pinMode(_freqOutputPin, OUTPUT);

}

void Ay38910a::set_mix( boolean chA_tone,boolean chB_tone,boolean chC_tone,boolean chA_noise,boolean chB_noise,boolean chC_noise )
{
   write_data(7, B11000000 | 
                   (chC_noise == true ? 0 : B00100000)|
                    (chB_noise == true? 0 : B00010000) | 
                    (chA_noise == true ? 0 : B00001000) | 
                    (chC_tone == true ? 0 : B00000100) |
                    (chB_tone == true ? 0 : B00000010) | 
                    (chA_tone == true ? 0 : B00000001) 
   );
}

void Ay38910a::set_chA_amplitude(int amplitude, boolean useEnvelope )
{
   write_data(8, (amplitude & 0xf) | (useEnvelope != true ? 0 : B00010000 ) );
}

void Ay38910a::set_chB_amplitude(int amplitude, boolean useEnvelope )
{
  write_data(9, (amplitude & 0xf) | (useEnvelope != true ? 0 : B00010000 ) );
}

void Ay38910a::set_chC_amplitude(int amplitude, boolean useEnvelope )
{
  write_data(10, (amplitude & 0xf) | (useEnvelope != true ? 0: B00010000 ) );
}

void Ay38910a::set_envelope( boolean hold, boolean alternate, boolean attack, boolean cont, unsigned long frequency )
{
    write_data(13, (hold == true ? 0 : 1)|
                    (alternate == true? 0 : 2) | 
                    (attack == true ? 0 : 4) | 
                    (cont == true ? 0 : 8) 
                );
                
    write_data(11,frequency & 0xff );
    write_data(12,(frequency >> 8)& 0xff );
    
}

void Ay38910a::note_chA(int i)
{
  write_data(0x00, tp[i]&0xff);
  write_data(0x01, (tp[i] >> 8)&0x0f);    
}

void Ay38910a::note_chB(int i)
{
  write_data(0x02, tp[i]&0xff);
  write_data(0x03, (tp[i] >> 8)&0x0f);
}

void Ay38910a::note_chC(int i)
{
  write_data(0x04, tp[i]&0xff);
  write_data(0x05, (tp[i] >> 8)&0x0f);
}

void Ay38910a::noise(int i)
{
  write_data(0x06, i&0x1f);
}

void Ay38910a::mode_latch(){
    digitalWrite(_pinBC1, HIGH);
    digitalWrite(_pinBCDIR, HIGH);
}

void Ay38910a::mode_write(){
    digitalWrite(_pinBC1, LOW);
    digitalWrite(_pinBCDIR, HIGH);
}

void Ay38910a::mode_inactive(){
    digitalWrite(_pinBC1, LOW);
    digitalWrite(_pinBCDIR, LOW);    
}

void Ay38910a::write_data(unsigned char address, unsigned char data)
{  
  
  mode_inactive();  
  //write address

  digitalWrite(_latchPin, LOW);
  // shift out the bits:
  shiftOut(_dataPin, _clockPin, MSBFIRST, address);  

  //take the latch pin high so the LEDs will light up:
  digitalWrite(_latchPin, HIGH);

  mode_latch();  
  mode_inactive();

  //write data
  mode_write();  

  digitalWrite(_latchPin, LOW);
  // shift out the bits:
  shiftOut(_dataPin, _clockPin, MSBFIRST, data);  

  //take the latch pin high so the LEDs will light up:
  digitalWrite(_latchPin, HIGH);

  mode_inactive();
  
}

void Ay38910a::init2MhzClock()
{
    // Set Timer 2 CTC mode with no prescaling.  OC2A toggles on compare match
    //
    // WGM22:0 = 010: CTC Mode, toggle OC 
    // WGM2 bits 1 and 0 are in TCCR2A,
    // WGM2 bit 2 is in TCCR2B
    // COM2A0 sets OC2A (arduino pin 11 on Uno or Duemilanove) to toggle on compare match
    //
    TCCR2A = ((1 << WGM21) | (1 << COM2A0));

    // Set Timer 2  No prescaling  (i.e. prescale division = 1)
    //
    // CS22:0 = 001: Use CPU clock with no prescaling
    // CS2 bits 2:0 are all in TCCR2B
    TCCR2B = (1 << CS20);

    // Make sure Compare-match register A interrupt for timer2 is disabled
    TIMSK2 = 0;
    // This value determines the output frequency
    OCR2A = _ocr2aval;
    
}
