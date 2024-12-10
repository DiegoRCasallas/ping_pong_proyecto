#include <Keypad.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Configuración Wi-Fi
const char* ssid = "master";        // Nombre del punto de acceso del maestro
const char* password = "123456789";      // Contraseña del punto de acceso
const char* serverIP = "192.168.4.1/jugador1"; // IP del servidor maestro

// Configuración del teclado matricial 4x4
const byte ROWS = 4; // Número de filas
const byte COLS = 4; // Número de columnas
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Pines conectados al teclado 4x4
byte rowPins[ROWS] = {13, 12, 14, 27};  
byte colPins[COLS] = {26, 25, 33, 32}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
    Serial.begin(115200);

    // Configuración Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConexión establecida");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // Leer tecla presionada
    char key = keypad.getKey();
    if (key) { 
        Serial.print("Tecla presionada: ");
        Serial.println(key);
        enviarTecla(key);
    }
    
}

// Función para enviar la tecla presionada al maestro
void enviarTecla(char key) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "http://"+String(serverIP) +"?tecla="+String(key);
        http.begin(url);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("Respuesta del servidor: ");
            Serial.println(http.getString());
        } else {
            Serial.print("Error al enviar la tecla: ");
            Serial.println(http.errorToString(httpResponseCode));
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado, no se pudo enviar la tecla.");
    }
}

