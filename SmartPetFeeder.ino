#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <ESP32Servo.h>

const char* ssid = "qwerty";
const char* password = "10689707";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Servo myservo;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", getPageContent());
  });
  server.on("/rotate", HTTP_GET, [](AsyncWebServerRequest *request) {
    rotateServo();
    request->send(200, "text/plain", "Servo rotated");
  });
  server.begin();

  myservo.attach(13); // Attach servo to pin 13
}

void loop() {
  ws.cleanupClients(); 
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  // Handle WebSocket events
}

String getPageContent() {
  String content = "<!DOCTYPE html><html>";
  content += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<link rel=\"icon\" href=\"data:,\">";
  content += "<style>html { font-family: Arial, sans-serif; text-align: center;}";
  content += "input[type='number'] { padding: 10px; margin: 10px; width: 60px;}";
  content += ".button { border-radius: 10px; padding: 10px 20px; margin: 10px; cursor: pointer; font-size: 18px;}";
  content += "#startButton { background-color: #6fe1bc; border: 2px solid #6fe1bc; color: white;}";
  content += "#stopButton { background-color: #febb00; border: 2px solid #febb00; color: white;}";
  content += "#resetButton { background-color: #ff0037; border: 2px solid #ff0037; color: white;}";
  content += "#quickFeedButton { background-color: #2596be; border: 2px solid #2596be; color: white;}";
  content += ".button:hover { opacity: 0.8;}";
  content += "</style></head>";
  content += "<body>";
  content += "<h1>Control Smart Pet Feeder</h1>";
  content += "<button id=\"quickFeedButton\" class=\"button\" onclick=\"quickFeed()\">Quick Feed</button>";
  content += "<br><br>";
  content += "<label for=\"hours\">Hours:</label>";
  content += "<input type=\"number\" id=\"hours\" name=\"hours\" min=\"0\" max=\"23\">";
  content += "<label for=\"minutes\">Minutes:</label>";
  content += "<input type=\"number\" id=\"minutes\" name=\"minutes\" min=\"0\" max=\"59\">";
  content += "<label for=\"seconds\">Seconds:</label>";
  content += "<input type=\"number\" id=\"seconds\" name=\"seconds\" min=\"0\" max=\"59\">";
  content += "<br><br>";
  content += "<button id=\"startButton\" class=\"button\" onclick=\"startTimer()\">Start Timer</button>";
  content += "<button id=\"stopButton\" class=\"button\" onclick=\"stopTimer()\">Stop Timer</button>";
  content += "<button id=\"resetButton\" class=\"button\" onclick=\"resetTimer()\">Reset Timer</button>";
  content += "<div id=\"timerDisplay\"></div>";
  content += "<script>";
  content += "var timer;";
  content += "var remainingSeconds = 0;"; // Variable to store remaining seconds
  
  // Start Timer Function
  content += "function startTimer() {";
  content += "var hours = document.getElementById('hours').value;";
  content += "var minutes = document.getElementById('minutes').value;";
  content += "var seconds = document.getElementById('seconds').value;";
  content += "remainingSeconds = (hours * 3600) + (minutes * 60) + parseInt(seconds);"; // Initialize remainingSeconds
  content += "if (remainingSeconds > 0) {";
  content += "document.getElementById('startButton').disabled = true;";
  content += "document.getElementById('hours').disabled = true;";
  content += "document.getElementById('minutes').disabled = true;";
  content += "document.getElementById('seconds').disabled = true;";
  content += "timer = setInterval(function() {";
  content += "remainingSeconds--;";
  content += "var hoursLeft = Math.floor(remainingSeconds / 3600);";
  content += "var minutesLeft = Math.floor((remainingSeconds % 3600) / 60);";
  content += "var secondsLeft = remainingSeconds % 60;";
  content += "document.getElementById('timerDisplay').innerHTML = hoursLeft + 'h ' + minutesLeft + 'm ' + secondsLeft + 's';";
  content += "if (remainingSeconds <= 0) {";
  content += "clearInterval(timer);";
  content += "rotateServo();";
  content += "document.getElementById('hours').disabled = false;";
  content += "document.getElementById('minutes').disabled = false;";
  content += "document.getElementById('seconds').disabled = false;";
  content += "document.getElementById('startButton').disabled = false;";
  content += "resetTimer();";
  content += "}";
  content += "}, 1000);";
  content += "} ";
  content += "}";

  // Stop Timer Function
  content += "function stopTimer() {";
  content += "clearInterval(timer);";
  content += "document.getElementById('startButton').disabled = false;";
  content += "document.getElementById('hours').disabled = false;";
  content += "document.getElementById('minutes').disabled = false;";
  content += "document.getElementById('seconds').disabled = false;";
  content += "}";

  // Reset Timer Function
  content += "function resetTimer() {";
  content += "clearInterval(timer);";
  content += "document.getElementById('startButton').disabled = false;";
  content += "document.getElementById('hours').value = '';";
  content += "document.getElementById('minutes').value = '';";
  content += "document.getElementById('seconds').value = '';";
  content += "document.getElementById('hours').disabled = false;";
  content += "document.getElementById('minutes').disabled = false;";
  content += "document.getElementById('seconds').disabled = false;";
  content += "document.getElementById('timerDisplay').innerHTML = '';";
  content += "}";
  
  // Quick Feed Function
  content += "function quickFeed() {";
  content += "document.getElementById('quickFeedButton').disabled = true;"; // Disable the button
  content += "rotateServo();"; // Call the rotate servo function
  content += "setTimeout(function() {"; // Set a timeout to enable the button after 10 seconds
  content += "document.getElementById('quickFeedButton').disabled = false;";
  content += "}, 10000);"; // 10 seconds
  content += "}";
  
  content += "function rotateServo() {";
  content += "var xhr = new XMLHttpRequest();";
  content += "xhr.open('GET', '/rotate', true);";
  content += "xhr.send();";
  content += "}";
  content += "</script>";
  content += "<div id=\"timerDisplay\"></div>";
  content += "</body></html>";
  return content;
}


void rotateServo() {
  myservo.write(0); // Rotate servo to 180 degrees
  delay(100); // Wait for 1 second
  myservo.write(90); // Rotate servo back to 0 degrees
}
