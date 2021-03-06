
#define BLYNK_PRINT Serial

#include <SimpleTimer.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

String apiKey = "O3457Q5IASBD985Q";  //Change this key to your "Write API key"

int gas_sensor = A0; //Sensor pin 
float m = -0.3376; //Slope 
float b = 0.7165; //Y-Intercept 
float R0 = 2.82; //Sensor Resistance in fresh air from previous code

//int CO_sensor = A1; //Sensor pin 
//float m1 = -0.6527; //Slope 
//float b1 = 1.30; //Y-Intercept 
//float R01 = 7.22; //Sensor Resistance

char ssid[] = "Yowan";
char pass[] = "123456789";

void setup() {
  
 
  Serial.begin(9600);      // PC to Arduino Serial Monitor
  Serial1.begin(115200);   // Arduino to ESP01 Communication
  pinMode(gas_sensor, INPUT);
  
  //pinMode(A2,INPUT); //For DHT Sensor
 } 

void loop() { 
  // put your main code here, to run repeatedly:
  float sensor_volt; //Define variable for sensor voltage 
  float RS_gas; //Define variable for sensor resistance  
  float ratio; //Define variable for ratio
  float sensorValue = analogRead(gas_sensor); //Read analog values of sensor  
  sensor_volt = sensorValue*(5.0/1023.0); //Convert analog values to voltage 
    RS_gas = ((5.0*10.0)/sensor_volt)-10.0; //Get value of RS in a gas
  ratio = RS_gas/R0;  // Get ratio RS_gas/RS_air
  double ppm_log = (log10(ratio)-b)/m; //Get ppm value in linear scale according to the the ratio value  
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale 
  Serial.print("Our desired PPM = ");
  Serial.println(ppm);
  




 // int chk = DHT.read11(DHT11_PIN);
 // Serial.print("Temperature = ");
 // float temp = DHT.temperature;
 // Serial.println(temp);
 // float hum = DHT.humidity;
 // Serial.print("Humidity = ");
 // Serial.println(hum);



  Serial1.println("AT+CIPMUX=0\r\n");      // To Set MUX = 0
  delay(2000);                             // Wait for 2 sec

  //TCP connection 
  String cmd = "AT+CIPSTART=\"TCP\",\"";   // TCP connection with https://thingspeak.com server
  cmd += "184.106.153.149";                // IP addr of api.thingspeak.com
  cmd += "\",80\r\n\r\n";                  // Port No. = 80

  Serial1.println(cmd);                    // Display above Command on PC
  Serial.println(cmd);                     // Send above command to Rx1, Tx1

  delay(1000);                         

  if(Serial1.find("ERROR"))                // If returns error in TCP connection
  { 
    Serial.println("AT+CIPSTART error");   // Display error msg to PC
    //return; 
  }

  // prepare GET string 
  String getStr = "GET /update?api_key=";   
  getStr += apiKey;
  getStr +="&field1=";
  getStr += ppm;
  getStr +="&field2=";
//  getStr += ppm1; 
 // getStr +="&field3=";
 // getStr += temp;
  //getStr +="&field4=";
  //getStr += hum;
  //getStr += "\r\n\r\n"; 

  Serial.println(getStr);                 // Display GET String on PC

  cmd = "AT+CIPSEND=";                    // send data length 
  cmd += String(getStr.length());
  cmd+="\r\n";

  Serial.println(cmd);                   // Display Data length on PC
  Serial1.println(cmd);                  // Send Data length command to Tx1, Rx1
  if(Serial1.find(">"))                    // If prompt opens //verify connection with cloud
  {
    Serial.println("Pushed whole data TO CLOUD");  // Display confirmation msg to PC
    Serial1.print(getStr);                 // Send GET String to Rx1, Tx1
  }
  else
  { 
    Serial1.println("AT+CIPCLOSE\r\n");    // Send Close Connection command to Rx1, Tx1
    Serial.println("AT+CIPCLOSE");         // Display Connection closed command on PC
  } 
  // thingspeak free version needs 15-20 sec delay between every push
  delay(15000);                            // wait for 16sec
 }
