#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>


// WiFi parameters
const char* ssid = "HHHHHHHHH";
const char* password = "hanshoupeng";
const char* deviceName = "esp8266";

//Web Server
ESP8266WebServer server(80);

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
"    function reset(){\n" + 
"        $.post(./reset);\n" + 
"    }\n" + 
"    function reflush() {\n" +
"        $.getJSON('./get', function(json) {\n" +
"            $('#tmp')[0].innerHTML = json.name\n" +
"            $('#id').html($('#tmp').html())\n" +
"\n" +
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
"    <label>Device ID: </label>\n" +
"    <b>\n" +
"        <i>\n" +
"            <label id='id'>" + deviceName + "</label>\n" +
"        </i>\n" +
"    </b>\n" +
"    <br/>\n" +
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
"            <label id='delta'>0.00</label>\n" +
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
"    <br/>\n" +
"    <button class='btn btn-app btn-block' type='submit'\n" +
"	    style='width: 70%' id='del' onclick='reset()'>RESET</button>\n" + 
"</body>\n";

// ??????????????????
volatile unsigned long t1, t2, delta;
// distance??????cm
volatile unsigned long distance = 10;
volatile double sspeed;
volatile int state = 0;


// ??????
// D1??????????????????????????????
int startPin = D1;
int endPin = D2;
void setup()
{
  //???????????????
  Serial.begin(115200);
  Serial.println();

  
  //  ??????????????????
  attachInterrupt(startPin, startInt, FALLING);
  attachInterrupt(endPin, endInt, FALLING);

  Serial.println("Config WIFI, first connect to WIFI, second release a WIFI spot");

  //??????WiFi??????
  Serial.printf("start connecting to WIFI:%s\n", ssid);

  WiFi.hostname(deviceName);
  WiFi.begin(ssid, password);
//  WiFi.hostname(deviceName);
  //??????????????????
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
//  Serial.println(WiFi.());

  if (MDNS.begin(deviceName)) {
    Serial.println("MDNS started");
  }

  // ???????????????
  Serial.println("start config server");
  server.on("/",  handleRoot);
  server.on("/get", HTTP_GET, handleGet);
  server.on("/reset", handleReset)
  server.begin();
  Serial.println("end config server");

  // ??????mDNS
  Serial.println("starting config mNDS");

  Serial.println("mDNS responder started");
//  MDNS.addService("http", "tcp", 80); // Announce esp tcp service on port 8080
  Serial.println("Ending mDNS config");

}
void handleRoot() {
  server.send(200, "text/html", rootpage);  
}
void handleGet() {
  StaticJsonDocument<400> json;
  json["name"] = deviceName;
  json["t1"] = t1;
  json["t2"] = t2;
  json["delta"] = delta;
  json["distance"] = distance;
  json["speed"] = String(sspeed);
  char serial[100];
  sprintf(serial, "0x%x", millis());
  json["serial"] = serial;
  char msg[measureJson(json) + 1];
  serializeJson(json, msg, measureJson(json) + 1);
  server.send(200, "text/plain", msg);
//  Serial.printf("handleGet: %s\n", msg);
}
void handleReset() {
  Serial.printf("reset state to 0, cur:[%d]\n", state);
  state = 0;
}
void loop() {

  delta = t2 - t1;
  sspeed = distance * 10000.0 / delta;
  
  //server????????????????????????
  server.handleClient();
  delay(200);
}
// ????????????????????????
ICACHE_RAM_ATTR void startInt() {
  if (state == 0) {
    t1 = micros();
    state = 1;
  }
//  Serial.printf("Start get a falling signal at [%d]\n", t1);
}
ICACHE_RAM_ATTR void endInt() {
  if (state == 1) {
    t2 = micros();
    state = 2;
  }
//  Serial.printf("end get a falling signal at [%d]\n", t2);
}
