#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

String wakeUpTime;
bool wakeUpTimeSetted;

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
                      "<input type='time' id='wakeUpTime' name='wakeUpTime'>"
                      "<input type='hidden' id='currentTime' name='currentTime'>"
                      "<input type='submit'>"
                      "</form>"
                      "<script type='text/javascript'>"
                      "var d = new Date();"
                      "var hours = d.getHours();"
                      "if (hours < 10) {"
                      "hours = '0' + hours;"
                      "}"
                      "var mins = d.getMinutes();"
                      "if (mins < 10) {"
                      "mins = '0' + mins;"
                      "}"
                      "var seconds = d.getSeconds();"
                      "if (seconds < 10) {"
                      "seconds = '0' + seconds;"
                      "}"
                      "document.getElementById('currentTime').value = hours + ':' + mins + ':' + seconds;"
                      "</script>";
/* HTML Pages */

/* HTTP Request Handlers */
void handle_wake_up_time() {
  Serial.println("[POST] /wake_up_time");
  
  if(webServer.hasArg("wakeUpTime")){
      Serial.print("Updating wake up time: ");
      wakeUpTime = webServer.arg("wakeUpTime");
      wakeUpTimeSetted = true;
      Serial.println(wakeUpTime);
  }

  Serial.println("\n");
  webServer.send(200, "text/html", "ok");
}

void handle_not_found() {
    Serial.println("Accessing captive portal");
    webServer.send(200, "text/html", captivePortalHTML);
}
/* HTTP Request Handlers */

void setupSoftAP()
{
  Serial.println();

  Serial.println("Setting soft-AP ... ");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.setOutputPower(20.5);
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

void loopSoftAP()
{
  dnsServer.processNextRequest();
  webServer.handleClient();
//  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
}

boolean wakeUpTimeIsSetted(){
  return wakeUpTimeSetted;
}

String getWakeUpTime() {
  return wakeUpTime;
}
