#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configuración de la pantalla OLED
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
#define OLED_RESET -1
Adafruit_SSD1306 pantalla(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);

// Configuración de pines para los botones
#define J1_ARRIBA 12
#define J1_ABAJO 13
#define J2_ARRIBA 14
#define J2_ABAJO 27

// Variables del juego
int led2 = 2;
int posicion_paleta1 = 24;                          // Posición inicial de la paleta del jugador 1
int posicion_paleta2 = 24;                          // Posición inicial de la paleta del jugador 2
int pelota_x = 64, pelota_y = 32;                   // Posición inicial de la pelota
int direccion_pelota_x = 1, direccion_pelota_y = 1; // Dirección de movimiento de la pelota
const int altura_paleta = 16;
const int ancho_paleta = 3;
const int tamaño_pelota = 2;
int puntuacion1 = 0, puntuacion2 = 0;
int velocidad_juego = 50;

boolean isInicializado = false;
boolean isPausado = false;

// variables CONEXION
const char *ssid = "master";
const char *password = "123456789";
WebServer server_esp(80);

String device = "";
String answer = "";

// Configuración inicial
void setup()
{
    pinMode(led2, OUTPUT);
    Serial.begin(115200);
    Serial.println("ESP32 Server");
    WiFi.softAP(ssid, password);
    IPAddress ip = WiFi.softAPIP();

    Serial.print("El nombre del servidor ESP32 es: ");
    Serial.println(ssid);
    Serial.print("La IP asignada es: ");
    Serial.println(ip);

    // Configuración de rutas en el servidor
    server_esp.on("/", handleConnectionRoot);
    server_esp.on("/jugador1", handleDevice1);
    // server_esp.on("/jugador2", handleDevice2);
    // server_esp.on("/espectador1", handleDevice3);
    /* Manejo de rutas no encontradas */
    server_esp.onNotFound(handleNotFound);

    if (!pantalla.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("failed to start SSD1306 OLED"));
        while (1)
            ;
    }
    pantalla.clearDisplay();
    pantalla.display();

    // Inicia el servidor
    server_esp.begin();
    Serial.println("Servidor iniciado");
}

// Dibujar el juego en la pantalla
void dibujarJuego()
{
    pantalla.clearDisplay();

    // Dibujar las paletas
    pantalla.fillRect(0, posicion_paleta1, ancho_paleta, altura_paleta, SSD1306_WHITE);                             // Paleta jugador 1
    pantalla.fillRect(ANCHO_PANTALLA - ancho_paleta, posicion_paleta2, ancho_paleta, altura_paleta, SSD1306_WHITE); // Paleta jugador 2

    // Dibujar la pelota
    pantalla.fillRect(pelota_x, pelota_y, tamaño_pelota, tamaño_pelota, SSD1306_WHITE);

    // Dibujar la puntuación
    pantalla.setTextSize(1);
    pantalla.setTextColor(SSD1306_WHITE);
    pantalla.setCursor(30, 0);
    pantalla.print("p1: ");
    pantalla.print(puntuacion1);
    pantalla.setCursor(80, 0);
    pantalla.print("p2: ");
    pantalla.print(puntuacion2);

    pantalla.display();
}

// Actualizar la posición de las paletas

void actualizarPaletas()
{
    // Jugador 1
    if (answer == "D" && posicion_paleta1 > 0)
    {
        posicion_paleta1 -= 4;
        answer = "z";
    }

    if (answer == "B" && posicion_paleta1 < ALTO_PANTALLA - altura_paleta)
    {
        posicion_paleta1 += 4;
        answer = "z";
    }
    // Jugador 2
    if (answer == "0" && posicion_paleta2 > 0)
    {
        posicion_paleta2 -= 4;
        answer = "z";
    }
    if (answer == "5" && posicion_paleta2 < ALTO_PANTALLA - altura_paleta)
    {
        posicion_paleta2 += 4;
        answer = "z";
    }
    if (answer == "9")
        pausarJuego();
    if (answer != "9")
        despausaJuego();
}

// Actualizar la posición de la pelota
void actualizarPelota()
{
    pelota_x += direccion_pelota_x;
    pelota_y += direccion_pelota_y;

    // Rebote en los bordes superior e inferior
    if (pelota_y <= 0 || pelota_y >= ALTO_PANTALLA - tamaño_pelota)
    {
        direccion_pelota_y = -direccion_pelota_y;
    }

    // Rebote en las paletas
    if (pelota_x <= ancho_paleta && pelota_y >= posicion_paleta1 && pelota_y <= posicion_paleta1 + altura_paleta)
    {
        direccion_pelota_x = -direccion_pelota_x;
    }
    if (pelota_x >= ANCHO_PANTALLA - ancho_paleta - tamaño_pelota && pelota_y >= posicion_paleta2 && pelota_y <= posicion_paleta2 + altura_paleta)
    {
        direccion_pelota_x = -direccion_pelota_x;
    }

    // Salida de la pelota: puntuación
    if (pelota_x <= 0)
    {
        puntuacion2++;
        velocidad_juego -= 2;
        reiniciarPelota();
    }
    else if (pelota_x >= ANCHO_PANTALLA - tamaño_pelota)
    {
        velocidad_juego -= 2;
        puntuacion1++;
        reiniciarPelota();
    }
}

// Reiniciar la posición de la pelota
void reiniciarPelota()
{
    pelota_x = ANCHO_PANTALLA / 2;
    pelota_y = ALTO_PANTALLA / 2;
    direccion_pelota_x = direccion_pelota_x > 0 ? 1 : -1;
    direccion_pelota_y = direccion_pelota_y > 0 ? 1 : -1;
}
void encenderLed()
{
    digitalWrite(led2, HIGH);
    delay(10);
    digitalWrite(led2, LOW);
    delay(10);
}
void ejecutarJuego()
{
    actualizarPaletas();
    actualizarPelota();
    dibujarJuego();
    delay(velocidad_juego);
}
void mostrarMensaje(const char *mensaje)
{
    pantalla.clearDisplay();
    pantalla.setTextSize(1);
    pantalla.setTextColor(SSD1306_WHITE);
    pantalla.setCursor(10, ALTO_PANTALLA / 2 - 10);
    pantalla.print(mensaje);
    pantalla.display();
}
void despausaJuego()
{
    isPausado = false;
}
void pausarJuego()
{
    isPausado = true;
    mostrarMensaje("En pausa..");
    delay(5000);
}
void handleConnectionRoot()
{
    server_esp.send(200, "text/html", "Bienvenido al servidor ESP32");
}
void handleNotFound()
{
    server_esp.send(404, "text/html", "Error 404: Página no encontrada");
}
void handleDevice1()
{
    device = "ESP32-01";
    Serial.println(device);

    if (server_esp.hasArg("tecla"))
    {
        answer = server_esp.arg("tecla");
        Serial.print("Tecla recibida: ");
        Serial.println(answer);
        server_esp.send(200, "text/plain", "Tecla recibida correctamente.");
        encenderLed();
    }
    else
    {
        server_esp.send(400, "text/plain", "Error: no se envio una tecla.");
    }
}
// Bucle principal
void loop()
{

    server_esp.handleClient();
    if (!isInicializado)
    {
        mostrarMensaje("Bienvenido al juego");
        isInicializado = true;
        delay(3000);
    }
    if (!isPausado)
    {
        ejecutarJuego();
    }
    else
    {
        mostrarMensaje("En pausa");
    }
    // encenderLed();
}
