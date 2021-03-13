#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer webServer(80);
IPAddress apIP(192, 168, 4, 1);
String responseHTML = ""
                      "<!DOCTYPE html><html lang='en'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>CaptivePortal</title></head><body>"
                      "<h1>Hello World!</h1><p>This is a captive portal example."
                      " All requests will be redirected here.</p></body></html>";

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.println("Setting soft-AP ... ");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  boolean softApResult = WiFi.softAP("SmartClock", "12341234");
  if(softApResult) Serial.println("Ready");
  else Serial.println("Failed!");

  dnsServer.start(DNS_PORT, "*", apIP);
  
  webServer.onNotFound([]() {
    Serial.println("Accessing captive portal");
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
}

void loop()
{
  dnsServer.processNextRequest();
  webServer.handleClient();
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
}
