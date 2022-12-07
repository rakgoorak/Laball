#include <ESP8266WiFi.h>

// เปลี่ยนตรงนี้เป็นของเครือข่ายตัวเอง
const char* ssid = "RAK";
const char* password = "12345678";

// ตั้งค่าพอร์ตเป็นพอร์ต 80
WiFiServer server(80);

// ประกาศตัวแปรสำหรับเก็บหน้า HTTP
String header;

// กำหนดสถานะ LED ที่แสดงบนหน้าเว็บ
String output0State = "OFF";

// กำหนด Pin สำหรับ LED
const int output0 = D0;

void setup() {

  Serial.begin(115200);

  // กำหนด Pin และตั้งค่าสถานะ LOW (ไฟดับ)
  pinMode(output0, OUTPUT);
  digitalWrite(output0, LOW);

  // เชื่อมต่อกับเครือข่าย WIFI
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // ถ้าเชื่อมต่อสำเร็จให้แสดง IP Address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // เริ่มการทำงานของ Server
  server.begin();
}


void loop() {

  // รอ Client มาเชื่อมต่อ
  WiFiClient client = server.available();

  // ถ้ามี Client ใหม่มาเชื่อมต่อ
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";

    // เช็คสถานะว่า Cient ยังเชื่อมต่ออยู่หรือไม่
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c); //
        header += c;

        if (c == '\n') {

          // ถ้าไม่มีข้อมูลเข้ามาแสดงว่า Client ตัดการเชื่อมต่อไปแล้ว
          if (currentLine.length() == 0) {

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // ชุดคำสั่งในการเปิด-ปิด LED
            if (header.indexOf("GET /0/on") >= 0) {
              Serial.println("GPIO 0 on");
              output0State = "ON";
              digitalWrite(output0, HIGH);
            } else if (header.indexOf("GET /0/off") >= 0) {
              Serial.println("GPIO 0 off");
              output0State = "OFF";
              digitalWrite(output0, LOW);
            }

            // ส่วนโค้ดแสดงหน้าเว็บที่ส่งไปให้ Client แสดง
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // CSS style ของปุ่มกด
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".red{background-color: red; width: 150px; height: 150px; border-radius: 100%; margin: 0 auto;}");
            client.println(".black{background-color: black; width: 150px; height: 150px; border-radius: 100%; margin: 0 auto;}");
            client.println("</style></head>");



            // ส่วนหัวของหน้า
            client.println("<body><h1>Digital Signal Communication</h1>");
            if (output0State == "OFF"){
            client.println("<div class=\"black\"></div>");
            }else{
              client.println("<div class=\"red\"></div>");
            }
            client.println("<p>LED - State " + output0State + "</p>");

            if (output0State == "OFF") {
              client.println("<a href=\"/0/on\"><button class=\"button\">LED ON</button></a>");
              client.println("<a href=\"/0/off\"><button class=\"button\">LED OFF</button></a>");
            }
            if (output0State == "ON") {
              client.println("<a href=\"/0/on\"><button class=\"button\">LED ON</button></a>");
              client.println("<a href=\"/0/off\"><button class=\"button\">LED OFF</button></a>");
            }
            client.println("</body></html>");

            // ส่วนตอนปิดการทำงาน
            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // เคลียร์ส่วน Header
    header = "";

    // ตัดการเชื่อมต่อ
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
