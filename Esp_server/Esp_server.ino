#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

ESP8266WebServer server ( 80 );
AutoConnect Portal ( server );

String payload;

void rootPage() {
  
    char content[] = "Esp funzionante";
    server.send(200, "text/plain", content);
}

void setup () {
    
    
    Serial.begin ( 9600 ); 


 

    server.on("/", HTTP_POST, handleBody); //Associate the handler function to the path

    if ( Portal.begin() ) {
    
        Serial.println("Connesso alla rete! IP: " + WiFi.localIP().toString());
    
    } // if connessione

    pinMode ( 2, OUTPUT );

    
}//setup



void handleBody() {
    
    
    //CONTENT TYPE Application/x-www-form-urlencoded
     
    if ( ! server.hasArg("type") ) { 
    
        server.send( 200, "text/plain", "Variabile type non presente nella richiesta POST!" ); // Codice 200 OK
        
        return;    
 
    }//if per verificare presenza dela variabile type nella richiesta POST
      
      
    payload = server.arg( "type" );



    String message = "Valore ricevuto con successo! type = " ;
    message += payload;
    message += "\n";
         

    server.send ( 200, "text/plain", message );
    //Serial.println ( "type = " + payload );


    //Passa la variabile via seriale all'arduino
    Serial.print ( payload ); 

    //Led acceso per dare un riscontro visivo dell'avvenuto invio
    digitalWrite ( 2, HIGH );
    delay ( 100 );
    digitalWrite ( 2, LOW );

}//handleBody



void loop() {
 
    Portal.handleClient(); //Handling of incoming requests
 
}//loop
