#include <ESP8266WiFi.h> //ESP8266 Library
#include <ESP8266WebServer.h> //ESP8266 Library


ESP8266WebServer server(80);
 
const char* ssid = "TIM-30109331";
const char* password =  "WM0vE65nCZQGR908dKSFJ6QL";

String payload;

void setup () {
    
    
    Serial.begin ( 9600 ); //initialise serial monitor to send data to Arduino
    WiFi.begin ( ssid, password ); //connect to the network specified above
    
    while (WiFi.status() != WL_CONNECTED) { //Wait till Wi-Fi is connected
        
        delay(1000);
        Serial.println ( "Conessione in corso" );  
    
    }

    Serial.println(WiFi.localIP());


    server.on("/", HTTP_POST, handleBody); //Associate the handler function to the path
    server.begin(); //Start the server
    Serial.println("Server listening");


}//setup



void handleBody() {
    
    
    //CONTENT TYPE RICORDATELO PER PIACERE GIAN NON SCORDARTELO GRAZIE FIRMATO ABATHARGH
    //Application/x-www-form-urlencoded
     
    if ( ! server.hasArg("type") ) { 
    
        server.send( 200, "text/plain", "Variabile type non presente nella richiesta POST!" ); // Codice 200 OK
        
        return;    
 
    }//if per verificare presenza dela variabile type nella richiesta POST
      
      
    payload = server.arg( "type" );      


    String message = "Valore ricevuto con successo! type = " ;
    message += payload;
    message += "\n";
    message += "\n";

         

    server.send ( 200, "text/plain", message );
    //Serial.println ( "type = " + payload );


    //Passa la variabile via seriale all'arduino
    Serial.print ( payload ); //send the location details to Arduino


}//handleBody



void loop() {
 
    server.handleClient(); //Handling of incoming requests
 
}//loop
