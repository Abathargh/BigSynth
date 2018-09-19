#include <Ay38910a.h>
#include <SoftwareSerial.h>


////Pin connected to Data in (DS) of 74HC595
int dataPin = 5;
//Pin connected to latch pin (ST_CP) of 74HC595
int latchPin = 6;
//Pin connected to clock pin (SH_CP) of 74HC595
int clockPin = 7;

int pinBC1 =  13;
int pinBCDIR =  12;

// 2mz clock pin
int freqOutputPin = 11;   // OC2A output pin for ATmega328 boards

int prescale  = 1;
int ocr2aval  = 3; 
float period    = 2.0 * prescale * (ocr2aval + 1) / (F_CPU / 1.0e6);
float freq      = 1.0e6 / period;

Ay38910a chip ( dataPin, latchPin, clockPin, pinBC1, pinBCDIR, freqOutputPin, prescale, period, freq, ocr2aval );

//Gestione input
int pinEffetto = A0;
int tipoEffetto = -1;
int ottava = 0;
boolean effettoOn = false;
boolean uno = false, due = false, tre = true, quattro = false;
int effetto = 0;
int volume = 8;

int numeroCanali = 3;
int dimensioneTastiera = 12;
int pinTastiera[] = { 0, 1, 2, 3, 4, A1, A2, A3, A4, A5, 8, 9 };
int midiNotes[] = { 60 , 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71 };
int canali[] = { -1, -1, -1 };
int noteToChannel[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

int counterNote = 0;

String payload;
SoftwareSerial ESP_Serial ( A0, 10 ); //Tx,Rx


void setup () {
  
    for ( int i = 0; i < dimensioneTastiera; i++ ) {
    
        pinMode( pinTastiera[ i ], INPUT_PULLUP);
        
    }//for
    
    
    chip.init2MhzClock();
    chip.set_mix( false, false, false, false, false, false );
    chip.set_envelope( false, false, true, false, 0 );
    pinMode(A0, INPUT);

    Serial.begin( 115200 );
    ESP_Serial.begin ( 9600 );
    
    
    
}//setup

void playNote( int tasto, int amplitude ) {

    int canaleDaAccendere = -1;
    for ( int i = 0; canaleDaAccendere == -1 &&  i < numeroCanali; i++ )  {
    
        if ( canali[ i ] == -1 ) {

            switch ( i ) {

                case 0:

                    chip.set_mix( true, canali[ 1 ] == -1 ? false : true , canali[ 2 ] == -1 ? false : true , false, false, false );
                    chip.set_chA_amplitude( amplitude, true );
                    chip.note_chA( midiNotes[ tasto ] + ( ottava * 12) );
                    canaleDaAccendere = 0;
                    break;

                case 1:
                        
                    chip.set_mix( canali[ 0 ] == -1 ? false : true , true, canali[ 2 ] == -1 ? false : true , false, false, false );
                    chip.set_chB_amplitude( amplitude, true );
                    chip.note_chB( midiNotes[ tasto ]  + ( ottava * 12) );
                    canaleDaAccendere = 1;
                    break;

                case 2:

                    chip.set_mix( canali[ 0 ] == -1 ? false : true , canali[ 1 ] == -1 ? false : true , true, false, false, false );
                    chip.set_chC_amplitude( amplitude, true );
                    chip.note_chC( midiNotes[ tasto ]  + ( ottava * 12) );
                    canaleDaAccendere = 2;
                    break;


                default:
                
                    return;


            }//switch


            //Serial.print("Nota suonata ");
            //Serial.print(tasto, DEC);
            //Serial.print("\n");
            canali[ canaleDaAccendere ] = tasto;
            noteToChannel[ tasto ] = canaleDaAccendere;
            counterNote++;


        }//if


    }//for


}//playNote



void closeChannel( int tasto ) {

    int canale = noteToChannel[ tasto ];

    if ( canale != -1 ) {
        switch ( canale ) {
        
            case 0:
                    
                chip.set_chA_amplitude( 0, false );
                chip.note_chA( 0 );
                chip.set_mix( false, canali[ 1 ] == -1 ? false : true, canali[ 2 ] == -1 ? false : true, false, false, false );

                break;

            case 1:

                chip.set_chB_amplitude( 0, false );
                chip.note_chB( 0 );
                chip.set_mix( canali[ 0 ] == -1 ? false : true, false, canali[ 2 ] == -1 ? false : true, false, false, false );
                break;

            case 2:

                chip.set_chC_amplitude( 0, false );
                chip.note_chC( 0 );
                chip.set_mix( canali[ 0 ] == -1 ? false : true, canali[ 1 ] == -1 ? false : true, false, false, false, false );
                break;

            default:

                return;



        }//switch


        //Serial.print("Canale chiuso ");
        //Serial.print(canale, DEC);
        //Serial.print("\n");

        canali[ canale ] = -1;
        noteToChannel [ tasto ] = -1;

    }//if


}//closeChannel

void applicaEffetto ( int tipo ) {

    switch ( tipo ) {
  
        case 1 :
  
            ottava--;
            break;
  
        case 2 :
  
            ottava++;
            break;
            
        case 3 :
  
            uno = true;
            due = false;
            tre = true;
            quattro = false;
            effetto = 300;
            volume = 2;
            chip.set_envelope( uno, due, tre, quattro, effetto );
    
            break;
        
        case 4 : 
        
            uno = true;
            due = false;
            tre = false;
            quattro = false;
            effetto = 1000;
            volume = 2;
            chip.set_envelope( uno, due, tre, quattro, effetto );
    
            break;
        
        case 5 :

            chip.set_envelope ( false, false, true, false, 0 );
            volume = 8;
            break;
            
            
        default:
            
            break;



    }//switch

           
}//effetto


void loop() {
    
    if ( ESP_Serial.available() > 0 ) {
        
        payload = ESP_Serial.readString().substring(0,1);
        Serial.println( payload );
        
        applicaEffetto( payload.toInt() );
    
    }//if serial


    counterNote = 0;
    

    for ( int i = 0; counterNote < numeroCanali && i < dimensioneTastiera; i++ ) {

        if ( digitalRead( pinTastiera[ i ] ) == 0 ) {

            if ( noteToChannel[ i ] == -1 ) {

                playNote( i, volume );
                //Serial.print(i);

            }
      


        } else {


            if ( noteToChannel[ i ]  != -1 ) {

                closeChannel( i );

            }

        }//else

    delay(4);

    }//for

}//loop
