#include <ESP8266WiFi.h>
 
const char* ssid = "Virigilio";
const char* password = "cafetaria55";
int ledPin = 2; // Digital Pin 4
int relayPin = 13; // Digital Pin 7
int value = LOW;
bool isOn = false;
int count = 0;

WiFiServer server(80);

bool connectWifi()
{
  digitalWrite(ledPin, LOW);
  count++;
  if(count == 1)
  {
    Serial.println("");
    Serial.println("Connection not established.");
    Serial.println("Retrying ");
    WiFi.disconnect(true);
    WiFi.begin(ssid, password);
    return false;
  }
  else if(count>100)
  {
    count = 0;
    return false;
  }
  else
    Serial.print(".");
    
  if(WiFi.status() != WL_CONNECTED)
    return false;
  else
  {
    count = 0;
    digitalWrite(ledPin, HIGH);
    server.begin();
    Serial.println("");
    Serial.println("Reconnected to WiFi Network");
    Serial.print(WiFi.localIP()); 
    return true;
  }
}
  
void setup() 
{
  Serial.begin(115200);
  delay(10);
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  digitalWrite(relayPin, LOW);
  
  Serial.println("First Connection attempt");
  while(!isOn)
  { 
    isOn = connectWifi();
    delay(500);
  }
}
 
void loop() {
  //Check Wifi is Running
  if(WiFi.status() != WL_CONNECTED)
    isOn=false;

  //Wifi Lost, retry connection
  if(!isOn)
  {
    isOn = connectWifi();
    delay(500);
  }
  else
  {
    WiFiClient client = server.available();
    if (!client)
    {return;} 
    
    while(!client.available())
      delay(1);
    
    String request = client.readStringUntil('\r');
    client.flush();
    
    if (request.indexOf("/ROOMLIGHT") != -1)  {
      value = (value==HIGH) ? LOW : HIGH;
      digitalWrite(relayPin, value);
    }
    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.print("Relay is now: ");
    if(value == HIGH) 
      client.print("On");
    else 
      client.print("Off");
    client.println("<br><br>");
    client.println("<a href=\"/ROOMLIGHT\"\"><button>Change </button></a>");
    client.println("</html>");
    delay(1); 
  }
}
