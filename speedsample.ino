#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>


// WiFi parameters
const char* ssid = "MI8pro";
const char* password = "12345600";

//Web Server
ESP8266WebServer server(8080);

String rootpage = String("")+
"<!DOCTYPE html>\n" +
"<script src='https://cdn.bootcdn.net/ajax/libs/jquery/3.5.1/jquery.js'></script>\n" +
"<h1>Speed Sample</h1>\n" +
"<br/>\n" +
"<body>\n" +
"<script type='text/javascript'>\n" +
"    $(function () {\n" +
"        setInterval(reflush, 1000)\n" +
"    })\n" +
"    function reflush() {\n" +
"        $.getJSON('./get', function(json) {\n" +
"            $('#tmp')[0].innerHTML = json.t1\n" +
"            $('#t1').html($('#tmp').html())\n" +
"\n" +
"            $('#tmp')[0].innerHTML = json.t2\n" +
"            $('#t2').html($('#tmp').html())\n" +
"\n" +
"            $('#tmp')[0].innerHTML = json.delta\n" +
"            $('#delta').html($('#tmp').html())\n" +
"\n" +
"            $('#tmp')[0].innerHTML = json.distance\n" +
"            $('#distance').html($('#tmp').html())\n" +
"\n" +
"            $('#tmp')[0].innerHTML = json.speed\n" +
"            $('#speed').html($('#tmp').html())\n" +
"\n" +
"            $('#tmp')[0].innerHTML = json.serial\n" +
"            $('#serial').html($('#tmp').html())\n" +
"        })\n" +
"    }\n" +
"</script>\n" +
"    <label id='tmp' hidden></label>\n" +
"    <label>Start Time: </label>\n" +
"    <b>\n" +
"        <i>\n" +
"            <label id='t1'>0</label>\n" +
"        </i>\n" +
"    </b>\n" +
"    us<br/>\n" +
"    <label>End Time: </label>\n" +
"    <b>\n" +
"        <i>\n" +
"            <label id='t2'>0.00</label>\n" +
"        </i>\n" +
"    </b>\n" +
"    us<br/>\n" +
"    <label>Delta Time: </label>\n" +
"    <b>\n" +
"        <i>\n" +
"            <label id='power'>0.00</label>\n" +
"        </i>\n" +
"    </b>\n" +
"    us<br/>\n" +
"    <label>Distance: </label>\n" +
"    <b>\n" +
"        <i>\n" +
"            <label id='distance'>0.00</label>\n" +
"        </i>\n" +
"    </b>\n" +
"    cm<br/>\n" +
"    <label>Speed: </label>\n" +
"    <b>\n" +
"        <i>\n" +
"            <label id='speed'>0.00</label>\n" +
"        </i>\n" +
"    </b>\n" +
"    m/s<br/>\n" +
"    <label>serial-number: </label>\n" +
"    <b>\n" +
"        <i>\n" +
"            <label id='serial'>0.00</label>\n" +
"        </i>\n" +
"    </b>\n" +
"</body>\n";

// 采样时间变量
unsigned long t1, t2, delta;
// distance单位cm
unsigned long distance;
double sspeed;


// 引脚
// D1是第一个传感器的引脚
int startPin = D1;
int endPin = D2;
void setup()
{
  //设置端口号
  Serial.begin(115200);
  Serial.println();

  Serial.println("Config WIFI, first connect to WIFI, second release a WIFI spot");

  //设置WiFi连接
  Serial.printf("start connecting to WIFI:%s\n", ssid);
  WiFi.begin(ssid, password);
  //失败重试次数
  int count = 30;
  while (count >= 0) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("\nWiFi %s is connected\n", ssid);
      Serial.print("device IP is ");
      Serial.println(WiFi.localIP());
      break;
    }
    delay(500);
    count --;
    Serial.print(".");
  }
  Serial.println("end connecting to WiFi");

  // 设置服务器
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/get", HTTP_GET, handleGet);
  server.begin();

}
void handleRoot() {
  server.send(200, "text/html", rootpage);  
}
void handleGet() {
  StaticJsonDocument<200> json;
  json["t1"] = t1;
  json["t2"] = t2;
  json["delta"] = delta;
  json["distance"] = distance;
  json["speed"] = sspeed;
  char serial[100];
  sprintf(serial, "0x%x", millis());
  json["serial"] = serial;
  char msg[measureJson(json) + 1];
  serializeJson(json, msg, measureJson(json) + 1);
  server.send(200, "text/plain", msg);
  Serial.printf("handleGet: %s\n", msg);
}
void loop() {
  
  //server处理客户端的请求
  server.handleClient();
}
