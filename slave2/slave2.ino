[1:56 p.m., 9/12/2024] +57 313 4865955: #include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Configuración de red Wi-Fi
const char* ssid = "master";
const char* password = "123456789";

// Inicialización del servidor web en el puerto 80
WebServer server_esp(80);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables globales
String device = "";
String answer = "";
int arriba1 = 0;
int abajo1 = 0;
int arriba2 = 0;
int abajo2 = 0;
int pausa1 = 0;
int start1 = 0;
int pausa2 = 0;
int start2 = 0;
int velo = 0;
bool pausa = false;
bool start = false;
int paddle1_y = 24;                                                  // Posición inicial del jugador 1
int paddle2_y = 24;                                                  // Posición inicial del jugador 2
int ball_x = 64, ball_y = 32;                                        // Posición inicial de la pelota
int ball_dx = random(0, 1) * 2 - 1, ball_dy = random(0, 1) * 2 - 1;  // Dirección de la pelota
const int paddle_height = 16;
const int paddle_width = 3;
const int ball_size = 2;
int score1 = 0, score2 = 0;


// Configuración inicial del ESP32
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Server");

  // Configuración del punto de acceso Wi-Fi
  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();

  Serial.print("El nombre del servidor ESP32 es: ");
  Serial.println(ssid);
  Serial.print("La IP asignada es: ");
  Serial.println(ip);

  // Configuración de rutas en el servidor
  server_esp.on("/", handleConnectionRoot);
  server_esp.on("/jugador1", handleDevice1);
  server_esp.on("/jugador2", handleDevice2);
  server_esp.on("/espectador1", handleDevice3);
  server_esp.on("/espectador2", handleDevice4);

  // Manejo de rutas no encontradas
  server_esp.onNotFound(handleNotFound);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1)
      ;
  }
  display.clearDisplay();
  display.display();

  // Inicia el servidor
  server_esp.begin();
  Serial.println("Servidor iniciado");
}

// Bucle principal
void loop() {
  server_esp.handleClient();
  Serial.println(start);
  Serial.println(pausa);
  if (start) {
    if (!pausa) {

      updatePaddles();
      updateBall();
      drawGame();

      if (score1 > 5 || score2 > 5) {

        if (score1 < score2) {

          display.clearDisplay();
          display.setCursor(58, 32);
          display.print("Gana player 2");

        } else {

          display.clearDisplay();
          display.setCursor(58, 32);
          display.print("Gana player 1");
        }

        score1 = 0;
        score2 = 0;
        start1 = 0;
        start2 = 0;
      }
      if (score1 > score2) {
        velo = score1;
      } else {
        velo = score2;
      }
      delay(8 - velo);  // Control de velocidad del juego

    } else {

      display.clearDisplay();
      display.setCursor(58, 32);
      display.print("EN PAUSA");
    }
  } else {
    display.clearDisplay();
    display.setCursor(30, 32);
    display.print("Pulsa play para jugar");
  }
}



// Manejo de la raíz del servidor
void handleConnectionRoot() {
  server_esp.send(200, "text/html", "Bienvenido al servidor ESP32");
}

// Función para manejar solicitudes de /device1
void handleDevice1() {
  String response = "Datos recibidos:    ";
  if (server_esp.hasArg("plain")) {
    String verif = server_esp.arg("plain");
    int index1 = verif.indexOf(',');
    int index2 = verif.indexOf(',', index1 + 1);
    int index3 = verif.indexOf(',', index2 + 1);
    if (index1 != -1 && index2 != -1 && index3 != -1) {
      String var1 = verif.substring(0, index1);
      String var2 = verif.substring(index1 + 1, index2);
      String var3 = verif.substring(index2 + 1, index3);
      String var4 = verif.substring(index3 + 1);
      arriba1 = var1.toInt();
      abajo1 = var2.toInt();
      pausa1 = var3.toInt();
      start1 = var4.toInt();

      if (start1 == 1) {
        start = !start;
      }
      if (pausa1 == 1) {
        pausa = !pausa;
      }
      response += var1 + " - " + var2 + " - " + var3 + " - " + var4;
    } else {
      server_esp.send(400, "text/plain", "Formato invalido");
    }
  } else {
    response = " Nada";
  }
  server_esp.send(200, "text/plain", response);
}

// Función para manejar solicitudes de /device2
void handleDevice2() {
  String response = "Datos recibidos:    ";
  if (server_esp.hasArg("plain")) {
    String verif = server_esp.arg("plain");
    int index1 = verif.indexOf(',');
    int index2 = verif.indexOf(',', index1 + 1);
    int index3 = verif.indexOf(',', index2 + 1);
    if (index1 != -1 && index2 != -1 && index3 != -1) {
      String var1 = verif.substring(0, index1);
      String var2 = verif.substring(index1 + 1, index2);
      String var3 = verif.substring(index2 + 1, index3);
      String var4 = verif.substring(index3 + 1);
      abajo2 = var1.toInt();
      arriba2 = var2.toInt();
      pausa2 = var3.toInt();
      start2 = var4.toInt();

      if (start2 == 1) {
        start = !start;
      }
      if (pausa2 == 1) {
        pausa = !pausa;
      }
      response += var1 + " - " + var2 + " - " + var3 + " - " + var4;
    } else {
      server_esp.send(400, "text/plain", "Formato invalido");
    }
  } else {
    response = " Nada";
  }
  server_esp.send(200, "text/plain", response);
}

// Función para manejar solicitudes de /device3
void handleDevice3() {
  String html = "<!DOCTYPE html>\
  <html>\
  <head>\
    <title>Estado del Juego</title>\
    <style>\
      body { font-family: Arial, sans-serif; text-align: center; }\
      .game { display: inline-block; background-color: black; color: white; padding: 10px; font-size: 12px; }\
      .score { margin-bottom: 10px; }\
      .paddle { background-color: white; width: 5px; height: 16px; display: inline-block; margin: 0 5px; }\
      .ball { background-color: white; width: 2px; height: 2px; display: inline-block; }\
    </style>\
  </head>\
  <body>\
    <h1>Juego Pong</h1>\
    <div class='game'>\
      <div class='score'>P1: " + String(score1) + " | P2: " + String(score2) + "</div>\
      <div>";

  // Representa las paletas y la pelota en el área de juego.
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    if (y >= paddle1_y && y < paddle1_y + paddle_height) {
      html += "<div class='paddle'></div>";
    } else {
      html += "<div style='width: 5px; display: inline-block;'></div>";
    }

    if (y == ball_y && ball_x == 64) {
      html += "<div class='ball'></div>";
    } else {
      html += "<div style='width: 128px; display: inline-block;'></div>";
    }

    if (y >= paddle2_y && y < paddle2_y + paddle_height) {
      html += "<div class='paddle'></div>";
    } else {
      html += "<div style='width: 5px; display: inline-block;'></div>";
    }
    html += "<br/>";
  }

  html += "</div>\
    </div>\
  </body>\
  </html>";
  server_esp.send(200, "text/html", html);
}

// Función para manejar solicitudes de /device4
void handleDevice4() {
  String html = "<!DOCTYPE html>\
  <html>\
  <head>\
    <title>Estado del Juego</title>\
    <style>\
      body { font-family: Arial, sans-serif; text-align: center; }\
      .game { display: inline-block; background-color: black; color: white; padding: 10px; font-size: 12px; }\
      .score { margin-bottom: 10px; }\
      .paddle { background-color: white; width: 5px; height: 16px; display: inline-block; margin: 0 5px; }\
      .ball { background-color: white; width: 2px; height: 2px; display: inline-block; }\
    </style>\
  </head>\
  <body>\
    <h1>Juego Pong</h1>\
    <div class='game'>\
      <div class='score'>P1: " + String(score1) + " | P2: " + String(score2) + "</div>\
      <div>";

  // Representa las paletas y la pelota en el área de juego.
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    if (y >= paddle1_y && y < paddle1_y + paddle_height) {
      html += "<div class='paddle'></div>";
    } else {
      html += "<div style='width: 5px; display: inline-block;'></div>";
    }

    if (y == ball_y && ball_x == 64) {
      html += "<div class='ball'></div>";
    } else {
      html += "<div style='width: 128px; display: inline-block;'></div>";
    }

    if (y >= paddle2_y && y < paddle2_y + paddle_height) {
      html += "<div class='paddle'></div>";
    } else {
      html += "<div style='width: 5px; display: inline-block;'></div>";
    }
    html += "<br/>";
  }

  html += "</div>\
    </div>\
  </body>\
  </html>";
  server_esp.send(200, "text/html", html);
}

// Manejo de rutas no encontradas
void handleNotFound() {
  server_esp.send(404, "text/html", "Error 404: Página no encontrada");
}

void drawGame() {
  display.clearDisplay();

  // Dibujar las paletas
  display.fillRect(0, paddle1_y, paddle_width, paddle_height, SSD1306_WHITE);                            // Paleta jugador 1
  display.fillRect(SCREEN_WIDTH - paddle_width, paddle2_y, paddle_width, paddle_height, SSD1306_WHITE);  // Paleta jugador 2

  // Dibujar la pelota
  display.fillRect(ball_x, ball_y, ball_size, ball_size, SSD1306_WHITE);

  // Dibujar puntuación
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 0);
  display.print("P1: ");
  display.print(score1);
  display.setCursor(80, 0);
  display.print("P2: ");
  display.print(score2);

  display.display();
}
void updatePaddles() {
  // Jugador 1
  if (arriba1 == 1 && paddle1_y > 0) paddle1_y -= 2;
  if (abajo1 == 1 && paddle1_y < SCREEN_HEIGHT - paddle_height) paddle1_y += 2;

  // Jugador 2
  if (arriba2 == 1 && paddle2_y > 0) paddle2_y -= 2;
  if (abajo2 == 1 && paddle2_y < SCREEN_HEIGHT - paddle_height) paddle2_y += 2;


  arriba1 = 0;
  abajo1 = 0;
  arriba2 = 0;
  abajo2 = 0;
}

// Actualizar la posición de la pelota
void updateBall() {
  ball_x += ball_dx;
  ball_y += ball_dy;

  // Rebote en los bordes superior e inferior
  if (ball_y <= 0 || ball_y >= SCREEN_HEIGHT - ball_size) {
    ball_dy = -ball_dy;
  }

  // Rebote en las paletas
  if (ball_x <= paddle_width && ball_y >= paddle1_y && ball_y <= paddle1_y + paddle_height) {
    ball_dx = -ball_dx;
  }
  if (ball_x >= SCREEN_WIDTH - paddle_width - ball_size && ball_y >= paddle2_y && ball_y <= paddle2_y + paddle_height) {
    ball_dx = -ball_dx;
  }

  // Salida de la pelota: puntaje
  if (ball_x <= 0) {
    score2++;
    resetBall();
  } else if (ball_x >= SCREEN_WIDTH - ball_size) {
    score1++;
    resetBall();
  }
}

// Resetear la posición de la pelota
void resetBall() {
  ball_x = SCREEN_WIDTH / 2;
  ball_y = SCREEN_HEIGHT / 2;
  ball_dx = random(0, 1) * 2 - 1;
  ball_dy = random(0, 1) * 2 - 1;
}
[3:49 p.m., 9/12/2024] +57 313 4865955: http: