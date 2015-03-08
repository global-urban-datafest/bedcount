
/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using a WiFi shield.
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 Circuit:
 * WiFi shield attached
 
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */


#include <SPI.h>
#include <WiFi.h>

char ssid[] = "Hackathon"; //  your network SSID (name) 
char pass[] = "secretPassword";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "ec2-79-125-15-15.eu-west-1.compute.amazonaws.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
int loopCount = 0;

void setup() {
  pinMode(13, OUTPUT);  // for the onboard led
  pinMode(8, OUTPUT); // for the green led
  pinMode(9, OUTPUT); // for the red led
  pinMode(2,INPUT);  // for the force sensor
  pinMode(3,INPUT);  // for the PIR sensor
  
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 

  String fv = WiFi.firmwareVersion();
  if( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid);
  
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
  printWifiStatus();
  
}

void loop() {
    delay(1000);   
  
  loopCount++;
  int fsrValue = analogRead(A2);
  int temperatureValue = analogRead(A0);
  int temperatureCelcius = getTemperature(temperatureValue);
  int occupancy =0;

  
  if(fsrValue > 400) {
    digitalWrite(9, HIGH); 
    occupancy = 1;
  } else {
    digitalWrite(9, LOW);
    occupancy =0;
  }
   
   
  sendInfoToServer(fsrValue, temperatureCelcius, occupancy);
  Serial.println("Setting LED to flash.");
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(8, HIGH);  // turn the LED on
  delay(200);
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(8, LOW);
}

void sendInfoToServer(int fsr, int pirSensor, int occupancy) {
  
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    String tmpStr = String("GET /bed") 
      + "?fsr=" + String(fsr)
      + "&temp=" + String(pirSensor)
      + "&occupancy=" + String(occupancy)
      + String(" HTTP/1.1");
    client.println(tmpStr);
    client.println("Host: ec2-79-125-15-15.eu-west-1.compute.amazonaws.com");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("NOT connected to server");
  }
  
  // if there are incoming bytes available 
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

// if the server's disconnected, stop the client:
  if (client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

  }
}

void disconnectClient() {
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    //while(true);
  }
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

int getTemperature(int tempReading) {
  Serial.print("Temp reading = ");
Serial.print(tempReading); // the raw analog reading
// converting that reading to voltage, which is based off the reference voltage
float voltage = tempReading * 5.0;
voltage /= 1024.0;
// print out the voltage
Serial.print(" - ");
Serial.print(voltage); Serial.println(" volts");
// now print out the temperature
int temperatureC = (voltage - 0.5) * 100 ; //converting from 10 mv per degree wit 500 mV offset
//to degrees ((volatge - 500mV) times 100)
Serial.print(temperatureC); Serial.println(" degrees C"); 
return temperatureC;
}



