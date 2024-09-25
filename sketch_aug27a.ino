#include <WiFiNINA.h>  //here i have Included library for WiFi connection 

// WiFi network credentials 
char ssid[] = "oplus_co_apdmkd"; // wifi name
char pass[] = "tohm3279"; //password

WiFiClient client;  //  WiFi client object created for making HTTP request

BH1750 lightMeter; // here i have created an instance of the BH1750 light sensor library

const int ledCheck = 2;  // led pin to connect led

// String containing data to send to the IFTTT webhook 
String queryString = "?value1=57&value2=25";

// Web server address and path for the IFTTT webhook trigger
char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME = "https://maker.ifttt.com/trigger/3.1P/with/key/n585t7sKIA-lZdXyFEGxe6gzpGsdXKs7-wyZMYWHyOI";

void setup() {
  Serial.begin(9600);          // Initialize serial communication for debugging

  pinMode(ledCheck, OUTPUT);   // Setting the LED pin as output

  Wire.begin();               // Initialize I2C communication
  lightMeter.begin();          // Initialize the BH1750 light sensor

  Serial.println(F("BH1750 Test begin"));

  connectWiFi();               // Attempt to connect to WiFi network
}

void loop() {
  float lux = lightMeter.readLightLevel(); // Read light level from sensor
  Serial.print("Lux: ");
  Serial.println(lux);

  // Trigger IFTTT webhook based on light level thresholds 
  if (lux >= 360) {
    triggerWebhook("Sunlight");  // Send "Sunlight" event if lux is high
  } else if (lux < 80) {
    triggerWebhook("Sundown");   // Send "Sundown" event if lux is low
  }

  delay(60000);                 // Wait for 1 minute before next reading
}

void connectWiFi() {
  Serial.println("Attempting to connect to WiFi...");

  // Keep trying to connect wifi until successful
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(5000);                 // Wait 5 seconds before retrying
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
}

void triggerWebhook(String eventName) {
  if (WiFi.status() != WL_CONNECTED) {  // Check WiFi connection before sending
    connectWiFi();                       // Reconnect if necessary
  }

  // Connect to IFTTT webhook server and send HTTP request
  if (client.connect(HOST_NAME, 80)) {
    client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();
    delay(500);
    client.stop();
    Serial.println("Webhook triggered successfully for: " + eventName);
  } else {
    Serial.println("Failed to connect to webhook server");
  }
}
