#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Make sure to install this library

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const String merchantId = "YOUR_MERCHANT_ID";
const String sharedSecret = "YOUR_SHARED_SECRET_KEY";
const String apiEndpoint = "https://api.cybersource.com/pts/v2/payments"; // Adjust endpoint as needed

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Example: Generate a payment token
    String token = generatePaymentToken(1000.00); // Amount in currency units
    Serial.println("Generated Token: " + token);
}

void loop() {
    // Your main code here
}

// Function to generate a payment token from CyberSource
String generatePaymentToken(float amount) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String payload = createPayload(amount);

        http.begin(apiEndpoint);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Accept", "application/json");
        http.addHeader("Merchant-Id", merchantId);
        
        // You may need to add authentication headers, e.g., Basic Auth or OAuth, depending on your CyberSource configuration
        
        int httpResponseCode = http.POST(payload);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Response code: " + String(httpResponseCode));

            // Parse the JSON response
            return parseResponse(response);
        } else {
            Serial.println("Error on sending: " + String(httpResponseCode));
            return "";
        }
        http.end();
    }
    return "";
}

// Function to create JSON payload for the payment request
String createPayload(float amount) {
    StaticJsonDocument<200> doc;
    doc["amount"] = amount;
    doc["currency"] = "USD"; // Change as needed
    doc["merchantId"] = merchantId;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

// Function to parse the response from CyberSource
String parseResponse(String response) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
        String token = doc["id"]; // Adjust based on the actual response structure
        return token;
    } else {
        Serial.println("Failed to parse JSON: " + String(error.c_str()));
        return "";
    }
}
