#include <WiFiNINA.h>   // WiFiNINA library for WiFi functionality
#include <Wire.h>      // Wire library for I2C communication
#include <BH1750.h>    //  BH1750 library for light sensor functionality

// WiFi credentials
char ssid [] = "oplus_co_apdmkd"; // SSID of the WiFi network
char pass [] = "tohm3279";        // Password for the WiFi network

WiFiClient client;                 // Created a WiFiClient object for making HTTP requests
BH1750 lightMeter;                 // Created an instance of the BH1750 light sensor library

String queryString = "?value1=57&value2=25"; // Query parameters for the IFTTT webhook

// IFTTT configuration
char HOST_NAME[] = "maker.ifttt.com"; // Host name for IFTTT
String PATH_NAME = "https://maker.ifttt.com/trigger/3.1P/with/key/n585t7sKIA-lZdXyFEGxe6gzpGsdXKs7-wyZMYWHyOI"; // IFTTT webhook URL with key

void setup() {
    WiFi.begin(ssid, pass);           // Begin WiFi connection with credentials
    Serial.begin(9600);               // Start serial communication at 9600 baud rate

    pinMode(ledCheck, OUTPUT);        // Set the LED pin as an output

    Wire.begin();                     // Initialize I2C communication
    lightMeter.begin();               // Initialize the BH1750 light sensor
    Serial.println(F("BH1750 Test begin")); // Print a message indicating the test has started

    // Attempt to connect to WiFi
    connectWiFi();                    
}

void loop() {
    float lux = lightMeter.readLightLevel(); // Read the light level in lux
    Serial.print("Lux: ");                   // Print "Lux: " to the serial monitor
    Serial.println(lux);                      // Print the light level to the serial monitor

    // Check the light level and trigger the corresponding webhook
    if (lux >= 360) {
        triggerWebhook("Sunlight");          // Trigger webhook for sunlight condition
    } else if (lux < 80) {
        triggerWebhook("Sundown");           // Trigger webhook for sundown condition
    }
    
    delay(60000); // Wait for 1 minute before reading light level again
}

void connectWiFi() {
    Serial.println("Attempting to connect to WiFi..."); // Print message indicating WiFi connection attempt
    while (WiFi.status() != WL_CONNECTED) {              // Loop until connected to WiFi
        WiFi.begin(ssid, pass);                          // Reattempt to connect
        delay(5000); // Wait 5 seconds before retrying
        Serial.print("."); // Print a dot to indicate progress
    }
    Serial.println("\nConnected to WiFi"); // Print message indicating successful connection
}

void triggerWebhook(String eventName) {
    // Check if the device is connected to WiFi
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi(); // Reconnect to WiFi if not connected
    }

    // Attempt to connect to the webhook server
    if (client.connect("maker.ifttt.com", 80)) {
        // Send the HTTP GET request
        client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
        client.println("Host: " + String(HOST_NAME)); // Specify the host
        client.println("Connection: close"); // Close the connection after the request
        client.println(); // End of headers
        delay(500); // Wait for a moment to ensure request is sent
        client.stop(); // Close the connection
        Serial.println("Webhook triggered successfully"); // Print success message
    } else {
        Serial.println("Failed to connect to webhook server"); // Print failure message
    }
}
