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
  if(alarmActivated() && !isWalking()){
//    Serial.println("BEEPANDO");
    smartBeep();
  } else {
////    Serial.println("Alarm activated: ");
////    Serial.println(alarmActivated());
////    Serial.println("Is walking: ");
////    Serial.println(isWalking());
  }
  if(!wakeUpTimeIsSetted()) loopSoftAP();
}
