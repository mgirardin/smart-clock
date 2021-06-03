void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");
  setupRtc();
  setupMpu9060();
  setupBeeper();
  setupSoftAP();
}

void loop()
{
  loopRtc();
  loopMpu9060();
  loopSoftAP();
  smartBeep();
  delay(150); 
}
