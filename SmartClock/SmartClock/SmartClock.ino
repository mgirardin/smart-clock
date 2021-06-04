const int buttonPin = 15;

void setup()
{
//  Serial.begin(115200);
//  Serial.println("Starting...");
  setupRtc();
  setupMpu9060();
  setupBeeper();
  setupSoftAP();
  pinMode(buttonPin, INPUT);
}

int buttonState = 0;

void loop()
{
  loopRtc();
  loopMpu9060();
  if(alarmActivated() && !isWalking()){
//    Serial.println("BEEPANDO");
    smartBeep();
  } else {
//    Serial.println("Alarm activated: ");
//    Serial.println(alarmActivated());
//    Serial.println("Is walking: ");
//    Serial.println(isWalking());
  }
  if(alarmActivated(){
    buttonState = digitalRead(buttonPin);
  }
  if(buttonState == HIGH) {
    turnOffAlarm();
//    Serial.println("DESLIG");
//    Serial.println("BUTTON STATE: TRUE");
  } else {
//    Serial.println("BUTTON STATE: FALSE");
  }
  if(!wakeUpTimeIsSetted()) loopSoftAP();
  else {
    turnOffSoftAP();
  }
}
