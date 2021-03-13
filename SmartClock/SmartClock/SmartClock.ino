#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

String wakeUpTime;

/* Debug Config */
const static int debugPortBaud = 115200;
/* Debug Config */

/* Server Config Variables */
const static byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer webServer(80);
String softApSsid = "SmartClock";
String softApPassword = "12341234";
IPAddress apIP(192, 168, 4, 1);
/* Server Config Variables */

/* HTML Pages */
const static String captivePortalHTML = ""
                      "<!DOCTYPE html><html lang='en'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>SmartClock</title></head><body>"
                      "<h2>Smart Clock</h2>"
                      "<p></p>"
                      "<p></p>"
                      "<form action='/wake_up_time' method='post'>"
                      "<label for='time'>Horário:</label><br>"
                      "<input type='time' id='time' name='time'>"
                      "<input type='submit'>"
                      "</form>";
/* HTML Pages */

/* HTTP Request Handlers */
void handle_wake_up_time() {
  Serial.println("Posting wake up time");
  Serial.print("Number of args: ");
  Serial.println(webServer.args());
  if(webServer.args() >= 1){
    Serial.print("Setting wake up time to arg: ");
    Serial.println(webServer.arg(0));
    wakeUpTime = webServer.arg(0);
  } else {
    Serial.println("Not changing wake up time because request didnt have any argument");
  }
  
  if(webServer.hasHeader("Date")){
    Serial.print("Date header: ");
    Serial.println(webServer.header("Date"));
  }
  Serial.print("Updated wake up time: ");
  Serial.println(wakeUpTime);
  webServer.send(200, "text/html", "ok");
}

void handle_not_found() {
    Serial.println("Accessing captive portal");
    webServer.send(200, "text/html", captivePortalHTML);
}
/* HTTP Request Handlers */

void setup()
{
  Serial.begin(debugPortBaud);
  Serial.println();

  Serial.println("Setting soft-AP ... ");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  boolean softApGenerationResult = WiFi.softAP(softApSsid, softApPassword);
  if(softApGenerationResult) Serial.println("Soft-AP ready");
  else Serial.println("Soft-AP failed!");
  
  dnsServer.start(DNS_PORT, "*", apIP);

  /* Endpoint registers */
  webServer.on("/wake_up_time", HTTP_POST, handle_wake_up_time);
  webServer.onNotFound(handle_not_found);
  /* Endpoint registers */

  webServer.begin();
}

void loop()
{
  dnsServer.processNextRequest();
  webServer.handleClient();
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
}
