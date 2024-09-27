#include <HTTPClient.h>
#include <ArduinoJson.h>  // Ensure this library is installed

const String merchantId = "YOUR_MERCHANT_ID";
const String sharedSecret = "YOUR_SHARED_SECRET_KEY";
const String apiEndpoint = "https://api.cybersource.com/pts/v2/payments"; // Adjust endpoint as needed

void setup() {
    Serial.begin(115200); // Initialize Serial Monitor

    // Example: Generate a payment token
    String token = generatePaymentToken("4111111111111111", "12", "2025", "123"); // Example card details
    Serial.println("Generated Token: " + token);  // Print the token to the Serial Monitor
}

void loop() {
    // No loop logic needed
}

// Function to generate a payment token from CyberSource using customer credit card details
String generatePaymentToken(String cardNumber, String expirationMonth, String expirationYear, String cvv) {
    HTTPClient http;
    String payload = createPayload(cardNumber, expirationMonth, expirationYear, cvv);

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
}

// Function to create JSON payload for the payment request
String createPayload(String cardNumber, String expirationMonth, String expirationYear, String cvv) {
    StaticJsonDocument<256> doc;
    doc["merchantId"] = merchantId;
    doc["paymentInstrument"]["card"]["number"] = cardNumber;
    doc["paymentInstrument"]["card"]["expirationMonth"] = expirationMonth;
    doc["paymentInstrument"]["card"]["expirationYear"] = expirationYear;
    doc["paymentInstrument"]["card"]["securityCode"] = cvv;  // CVV is the security code

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

// Function to parse the response from CyberSource and extract the token
String parseResponse(String response) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
        String token = doc["id"];  // Adjust based on CyberSource's actual response structure
        return token;
    } else {
        Serial.println("Failed to parse JSON: " + String(error.c_str()));
        return "";
    }
}
