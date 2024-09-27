#include <HTTPClient.h>
#include <ArduinoJson.h>     // Ensure this library is installed

const String merchantId = "YOUR_MERCHANT_ID";
const String sharedSecret = "YOUR_SHARED_SECRET_KEY";
const String apiEndpoint = "https://api.cybersource.com/pts/v2/payments"; // Adjust endpoint as needed

void setup() {
    Serial.begin(115200);

    // Example: Generate a payment token
    String token = generatePaymentToken(1000.00); // Amount in currency units
    Serial.println("Generated Token: " + token);
}

void loop() {
    // No need to loop; code runs once in `setup()`
}

// Function to generate a payment token from CyberSource
String generatePaymentToken(float amount) {
    HTTPClient http;
    String payload = createPayload(amount);

    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");
    http.addHeader("Merchant-Id", merchantId);
    
    // Example: Adding Basic Auth (adjust as per CyberSource docs)
    // http.addHeader("Authorization", "Basic " + base64::encode(sharedSecret));

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

    return "";
}

// Function to create JSON payload for the payment request
String createPayload(float amount) {
    StaticJsonDocument<256> doc; // Increased buffer size
    doc["amountDetails"]["totalAmount"] = amount;
    doc["amountDetails"]["currency"] = "USD"; // Adjust as needed
    doc["merchantId"] = merchantId;
    doc["paymentInstrument"]["card"]["number"] = "4111111111111111"; // Sample test card number
    doc["paymentInstrument"]["card"]["expirationMonth"] = "12";
    doc["paymentInstrument"]["card"]["expirationYear"] = "2025";

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

// Function to parse the response from CyberSource
String parseResponse(String response) {
    StaticJsonDocument<512> doc; // Increased buffer size to handle larger responses
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
        String token = doc["id"]; // Adjust based on the actual response structure
        return token;
    } else {
        Serial.println("Failed to parse JSON: " + String(error.c_str()));
        return "";
    }
}
