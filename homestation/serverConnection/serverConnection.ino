#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi-instellingen
const char* ssid = "De Froststreet 74";
const char* password = "Paultje4Life";

// Server-URL
const String serverUrl = "http://192.168.68.58:5000/data";

// Maak een WiFiClient object aan
WiFiClient client;

void setup() {
    Serial.begin(115200);

    // Verbind met WiFi
    Serial.print("Verbinden met WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nVerbonden met WiFi!");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Gebruik de nieuwe API: begin(WiFiClient, url)
        http.begin(client, serverUrl);

        // Voorbeelddata in JSON-formaat
        String jsonData = "{\"location\": \"Amsterdam\", \"temperature\": 21.5, \"humidity\": 45}";

        // Stel headers in
        http.addHeader("Content-Type", "application/json");

        // Verstuur het POST-verzoek
        int httpResponseCode = http.POST(jsonData);

        // Controleer de HTTP-respons
        if (httpResponseCode > 0) {
            Serial.println("Data verzonden. HTTP response code: " + String(httpResponseCode));
        } else {
            Serial.println("Error met HTTP POST: " + String(http.errorToString(httpResponseCode).c_str()));
        }

        http.end(); // Sluit de HTTP-verbinding
    } else {
        Serial.println("Geen verbinding met WiFi");
    }

    delay(10000); // Wacht 10 seconden
}
