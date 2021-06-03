#include "RTClib.h"  // RTC library

// initialize RTC library
RTC_DS1307 rtc;
DateTime   now;

void setupRtc()
{
  Serial.println("Starting RTC setup");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("RTC is running!");
  }
}

//String createDateTimeString(DateTime now){
//  return now.year() + '-' + now.month() + '-' + now.day +  " " + now.hour + ":" + now.minute() + ":" + now.second();
//}

String prevDay;
boolean alarmRunned;
boolean alarmIsRunning;

void loopRtc()
{
      now = rtc.now();
    if(String(now.day()) != prevDay) {
      prevDay = now.day();
      alarmRunned = false;
      alarmIsRunning = false;
    }
    if(alarmRunned == true){
      return;
    }
    if((atLeastTwoDigits(String(now.hour())) + ":" + atLeastTwoDigits(String(now.minute()))) > getWakeUpTime()) {
      if(getWakeUpTime() == "") {
        return;
      }
      alarmRunned = true;
      alarmIsRunning = true;
    }
}

boolean alarmActivated(){
  return alarmIsRunning;
}
