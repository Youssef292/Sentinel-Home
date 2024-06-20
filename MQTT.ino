#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "DHT.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define DHTPIN  D8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const int Main_Room = D4;
const int Personal = D0;
const int Garage = D6;
const int Outside_var = D7;
const int sensorPin = D3;
const int mq2Pin = A0;
const int buzzer =  D2;
const int Rain_var = 10;
int LDR_flg = 1;
int Pir_flg = 1;
int PirVal = 0;
int MQ_2Val = 0;

unsigned long previousMillis = 0;   // Variable to store the last time DHT data was sent
const long interval = 5000;   
float humidity = 0;
float temperature = 0;
int Rain_data = false;
SoftwareSerial mySerial(D1,D2); // rx , tx

//#define ServoTwoPin 9
//#define ServoThreePin 8
//Servo WindowServo;
//Servo GarageServo;
/*#define wifi_ssid "Abdelhakem"
#define wifi_password "You_2353836@MM$$_aMeR"
#define mqtt_server "a3aea2a70f7b43d1809561231ab50b37.s1.eu.hivemq.cloud"
#define mqtt_port 8883
#define mqtt_username "ESP32"
#define mqtt_password "123456aA"*/

// Wi-Fi credentials
const char* ssid = "Abdelhakem";
const char* password = "You_2353836@MM$$_aMeR";

// MQTT broker credentials
const char* mqtt_server = "a3aea2a70f7b43d1809561231ab50b37.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "ESP32";
const char* mqtt_password = "123456aA";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define Gas_topic  "Gas_level"
#define humidity_topic "humidity"
#define temperature_celsius_topic "temperature_celsius"
#define Main_Room_Light_topic "Main_Room"
#define Personal_Room_topic "Personal_Room"
#define Garage_topic "Garage"
#define Outside_topic "Outside"
//#define window_control_topic "window_control"
//#define Garag_control_topic "Garage_control"
#define Rain_Status "Rain_ST"


static const char *root_ca PROGMEM = R"EOF(
  -----BEGIN CERTIFICATE-----
  MIIFazCCA10gAwIBAgIRAIIQZ7DSQONZRGPgu20CiwAwDQYJKOZIhvcNAQELBQAW
  TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFN1Y3VyaXR5IFJ1c2Vh
  cmNoIEdyb3VWMRUWEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
  WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
  ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAXFTATBgNVBAMTDE1TUkcgUm9vdCBY
  MTCCAIIWDQYJKOZIhvcNAQEBBQADggIPADCCAgoCggIBAK30JHP0FDfzm54rVygc
  h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
  0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
  A5/TR5d8mUgjU+g4rk8Kb4MuUlXjIBOttovDiNewNwIRt18jA8+o+u3dpjq+sW
  T8KOEUt+zwvo/7V3LvSye0rgTBI1DHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
  B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
  B5iPNgiV5+131g02dZ77DnKxHZu8A/1JBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
  KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
  OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
  jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
  qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
  rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGJQJBAMA4GA1UdDWEB/WQEAWIBBJAPBgNV
  HRMBAF8EBTADAQH/MBOGA1UdDgQWBBR5tFnme7b15AFzgAiIyBpY9umbbjANBgkq
  hkiG9w0BAQSFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V91ZL
  ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
  3BebYhtF8GaVenxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
  NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
  ORAZI4JMPJ+Gs1WYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
  TkXWStAmzOVvvghapZXiFaH30O3JLF+1+/+sKAIuvtd7u+Nxe5AW0wdeR1N8NwdC
  jNPE1pzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
  oyi3843njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
  4RgqsahDYVVTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
  mRGunUHBcnWEvgJBQ19nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
  emyPxgcYxn/eR44/KJ4EBs+1VDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
  -----END CERTIFICATE-----
)EOF";

void setup_wifi() {
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
}

/*** Connect to MQTT Broker ************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-"; // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(Main_Room_Light_topic);
      client.subscribe(Personal_Room_topic);
      client.subscribe(Garage_topic);
      client.subscribe(Outside_topic);
      //client.subscribe(window_control_topic);
      //client.subscribe(Garag_control_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds"); // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void Callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message arrived [" + String(topic) + "]");
  
  if (strcmp(topic, Main_Room_Light_topic) == 0) {
    Main_RoomControl(payload, length);
  }/*else if (strcmp(topic, window_control_topic) == 0) {
    Main_WindowControl(WindowServo, payload, length);
  }*/else if (strcmp(topic, Personal_Room_topic) == 0) {
    Personal_Control(payload, length);
  }else if (strcmp(topic, Garage_topic) == 0) {
    Garage_Control(payload, length);
  }
  else if (strcmp(topic, Outside_topic) == 0) {
    OutSide_Control(payload, length);
  }/*else if (strcmp(topic, Garage_topic) == 0) {
    Main_GarageControl(GarageServo, payload, length);
  }*/
}

void Main_RoomControl(byte* payload, unsigned int length) {
  String incomingMessage = "";
  for (int i = 0; i < length; i++) incomingMessage += (char)payload[i];

  if (incomingMessage.equals("off")) {
    digitalWrite(Main_Room, HIGH); // Turn on the LED
   // WindowServo.write(180);
    Serial.println("LED turned OFF");
  } else if (incomingMessage.equals("on")) {
    digitalWrite(Main_Room, LOW); // Turn off the LED
    //WindowServo.write(90);
    Serial.println("LED turned ON");
  }
}

void Personal_Control(byte* payload, unsigned int length) {
  String incomingMessage = "";
  for (int i = 0; i < length; i++) incomingMessage += (char)payload[i];

  if (incomingMessage.equals("off")) {
    digitalWrite(Personal, HIGH); // Turn on the LED
    Pir_flg = 1;
   // WindowServo.write(180);
    Serial.println("LED turned OFF");
  } else if (incomingMessage.equals("on")) {
    digitalWrite(Personal, LOW); // Turn off the LED
    Pir_flg = 0;
    //WindowServo.write(90);
    Serial.println("LED turned ON");
  }
}

void Garage_Control(byte* payload, unsigned int length) {
  String incomingMessage = "";
  for (int i = 0; i < length; i++) incomingMessage += (char)payload[i];

  if (incomingMessage.equals("off")) {
    digitalWrite(Garage, HIGH); // Turn on the LED
   // WindowServo.write(180);
    Serial.println("LED turned OFF");
  } else if (incomingMessage.equals("on")) {
    digitalWrite(Garage, LOW); // Turn off the LED
    //WindowServo.write(90);
    Serial.println("LED turned ON");
  }
}

void OutSide_Control(byte* payload, unsigned int length) {
  
  String incomingMessage = "";
  for (int i = 0; i < length; i++) incomingMessage += (char)payload[i];

  if (incomingMessage.equals("off")) {
    digitalWrite(Outside_var, HIGH); // Turn on the LED
    LDR_flg = 1;
   // WindowServo.write(180);
    Serial.println("LED turned OFF");
  } else if (incomingMessage.equals("on")) {
    digitalWrite(Outside_var, LOW); // Turn off the LED
    LDR_flg = 0;
    //WindowServo.write(90);
    Serial.println("LED turned ON");
  }
}


/*void Main_WindowControl(Servo& servo, byte* payload, unsigned int length) {
  String incomingMessage = "";
  
  for (int i = 0; i < length; i++) incomingMessage += (char)payload[i];

  if (incomingMessage.equals("open")) {
    servo.write(160); // Open the servo (adjust the angle as needed)
    Serial.println("Servo opened");
  } else if (incomingMessage.equals("close")) {
    servo.write(45); // Close the servo (adjust the angle as needed)
    Serial.println("Servo closed");
  }
}


void Main_GarageControl(Servo& servo, byte* payload, unsigned int length) {
  String incomingMessage = "";
  
  for (int i = 0; i < length; i++) incomingMessage += (char)payload[i];

  if (incomingMessage.equals("open")) {
    servo.write(90); // Open the servo (adjust the angle as needed)
    Serial.println("Servo opened");
  } else if (incomingMessage.equals("close")) {
    servo.write(180); // Close the servo (adjust the angle as needed)
    Serial.println("Servo closed");
  }
}*/




void setup() {
  dht.begin(); // Set up DHT11 sensor
  pinMode(Main_Room, OUTPUT);              // Set up LED
  pinMode(Personal, OUTPUT);
  pinMode(Garage, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(Outside_var, OUTPUT);
  pinMode(sensorPin , INPUT);
  pinMode(mq2Pin , INPUT);
  pinMode(Rain_var,INPUT);
  //WindowServo.attach(ServoTwoPin);
  Serial.begin(115200);
  mySerial.begin(9600);
  while (!Serial)
    delay(1);
  setup_wifi();

#ifdef ESP8266
  espClient.setInsecure();
#else
  espClient.setCACert(root_ca); // Enable this line and the "certificate" code for a secure connection
#endif


 


  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(Callback);  // Set the callback for topics
 
  
}


void loop() {
  unsigned long currentMillis = millis();
  int sensorValue = digitalRead(sensorPin);
  String msg = mySerial.readStringUntil('\r');
  PirVal = msg.toInt();
  int mq2Value = analogRead(mq2Pin);

  // Check if the client is connected
  if (!client.connected()) {
    reconnect(); // Reconnect if not connected
  }
  
  // Handle MQTT messages
  client.loop();
  
  // Read LDR sensor value
  if (LDR_flg == 1) {
    // Control Outside_var based on sensor value only if flg is 1
    if (sensorValue == 1) {
      digitalWrite(Outside_var, LOW); // Turn off Outside_var
    } else {
      digitalWrite(Outside_var, HIGH); // Turn on Outside_var
    }
  }

  // Read PIR value 
  if (Pir_flg == 1) {
    // Control Personal room based on PIR value only if flag is 1
    if (PirVal == 1) {
      digitalWrite(Personal, LOW); // Turn on Personal room
    } else if (PirVal == 0) {
      digitalWrite(Personal, HIGH); // Turn off Personal room
    }
  }
  
  if(mq2Value >= 300){
    digitalWrite(buzzer, HIGH);
  }else{
    digitalWrite(buzzer, LOW);
  }
  // Read DHT11 temperature and humidity readings
  if (currentMillis - previousMillis >= interval) {
    // Save the last time DHT data was sent
    previousMillis = currentMillis;
    //Read MQ-2 Data
    // Read and send DHT data
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    Rain_data = digitalRead(Rain_var);
    
    // Check if the readings are valid
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Publish DHT11 readings on the specified topics
    client.publish(humidity_topic, String(humidity).c_str(), true);
    client.publish(temperature_celsius_topic, String(temperature).c_str(), true);
    client.publish(Gas_topic, String(mq2Value).c_str(), true);
    client.publish(Rain_Status, String(Rain_data).c_str(), true);
    
   
  }
}


