/*

  Ay38910a.h library containing functions that make easier to interact
  with the Ay38910 sound chip

*/

#ifndef Ay38910a_h
#define Ay38910a_h

#include "Arduino.h"

class Ay38910a{

    public:
    
        Ay38910a ( int dataPin, int latchPin, int clockPin, int pinBC1, int pinBCDIR, int freqOutputPin, int prescale, float period, float freq, int ocr2aval );
        void set_mix ( boolean chA_tone, boolean chB_tone, boolean chC_tone, boolean chA_noise, boolean chB_noise, boolean chC_noise );
        void set_envelope ( boolean hold, boolean alternate, boolean attack, boolean cont, unsigned long frequency );
        void set_chA_amplitude ( int amplitude, boolean useEnvelope );
        void set_chB_amplitude ( int amplitude, boolean useEnvelope );        
        void set_chC_amplitude ( int amplitude, boolean useEnvelope );
        void note_chA ( int i );
        void note_chB ( int i );
        void note_chC ( int i );
        void noise ( int i );
        void init2MhzClock();
            
    private:
        
        int _dataPin;
        int _latchPin;
        int _clockPin;
        int _pinBC1;
        int _pinBCDIR;
        int _freqOutputPin;
        int _prescale;
        float _period;
        float _freq;
        int _ocr2aval;
        
        void mode_latch();
        void mode_write();
        void mode_inactive();
        void write_data(unsigned char address, unsigned char data);
        
        
  
};

#endif
