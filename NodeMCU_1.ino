#include <ESP8266WiFi.h>
int ledPin =16;

const char* ssid = "Lifez";
const char* password = "0845918889";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  int value;
  if (req.indexOf("/gpio/1") != -1)
    for(value = 0 ; value <= 1023; value+=10) // fade in (from min to max)
     {
       analogWrite(ledPin, value);           // sets the value (range from 0 to 255)
       delay(30);                            // waits for 30 milli seconds to see the dimming effect
     }

  else if (req.indexOf("/gpio/0") != -1)
    for(value = 1023; value >=0; value-=10)   // fade out (from max to min)
    {
      analogWrite(ledPin, value);
      delay(30);
    }
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  // Set GPIO16 according to the request
  analogWrite(ledPin, val);
  client.flush();

  // Prepare the response
  String web = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  web += "<!DOCTYPE HTML>\r\n";
  web += "<html>\r\n";
  web += "<body>\r\n";
  web += "<h1>LED Status</h1>\r\n";
  web += "<p>\r\n";
  if(val==1){
    web += "<button><a href='http://192.168.2.111/gpio/1'>LED Off</a></button>\r\n";
  }else{
    web += "<button><a href='http://192.168.2.111/gpio/0'>LED On</a></button>\r\n";
  }
  web += "</p>\r\n";
  web += "</body>\r\n";
  web += "</html>\r\n";
  // Send the response to the client
  client.print(web);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
