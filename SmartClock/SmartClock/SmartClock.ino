void setup()
{
  Serial.println("Starting...");
  Serial.begin(115200);
  setupRtc();
  setupMpu9060();
}

void loop()
{
  loopRtc();
  loopMpu9060();
  delay(100); 
}
