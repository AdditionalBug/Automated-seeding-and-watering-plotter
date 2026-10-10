#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#define FIREBASE_HOST "https://esp8266webserver-31a5e-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "UNn3KnbUGTwJesmgrDjXPXyXFwzCzpwoCi7dXPqP"
#define WIFI_SSID "Redmi Note 11"
#define WIFI_PASS "11110000"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


void setup() {

  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);

  Serial.begin(9600);
  Serial1.begin(4800, SERIAL_8N1, 20, 21);
  analogSetAttenuation(ADC_11db);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(10, HIGH);
    delay(500);
    digitalWrite(10, LOW);
    delay(500);
  }

  Serial.println();
  Serial.print("Connected: ");
  Serial.println(WiFi.localIP());

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Serial.println("Firebase connected ");
    digitalWrite(10, HIGH);
  } else {
    Serial.println("Firebase NOT connected ");
    digitalWrite(10, LOW);
  }
  

}


  bool seeded = false; 
void loop() {

  
  int valuearr[5] = {0,0,0,0,0};
  int pinarr[5] = {0, 1, 2, 3, 4};
  int sensorpin[6] = {11,12,13,14,15};
  int value = 0;

  for(int i =0; i<4; i++){
    value = analogRead(pinarr[i]);
    value = constrain(value, 2100, 4095);
    valuearr[i] = map(value, 2100,4095,100,0); 
  }
  value = analogRead(pinarr[4]);
  value = constrain(value, 0, 4095);
  valuearr[4] = map(value, 0, 4095, 0, 100);
 
  if(seeded == true ){
   if (valuearr[0] < 30 && valuearr[1] < 30) {
      Serial1.println(15);
      Serial.println("Sent 15 (Sensors 1 & 2 low)");
    }
    if (valuearr[2] < 30 && valuearr[3] < 30) {
      Serial1.println(16);
      Serial.println("Sent 16 (Sensors 3 & 4 low)");
    }
   else{
    for(int i = 0; i<4; i++){
      if( valuearr[i] < 30){
        Serial1.println(sensorpin[i]);
      // Serial.print(" | ");
      }
    }
   }
   
  }
  

  //sensor reading
  // for (int i = 0; i < 4; i++) {
  //   String moisturePath = "/sensors/moisture_" + String(i + 1);
  //   Firebase.RTDB.setInt(&fbdo, moisturePath, valuearr[i]);

  // };
  for(int i = 0; i < 4; i++){
    String moisturePath = "/sensors/moisture_" + String(i + 1);
    Firebase.RTDB.setInt(&fbdo, moisturePath, valuearr[i]);
  }


  String waterLevelPath = "/sensors/water_level";
  Firebase.RTDB.setInt(&fbdo, waterLevelPath, valuearr[4]);

  if (Firebase.RTDB.getString(&fbdo, "/btn/action")) {
  String action = fbdo.stringData();

    //btn action
    if (action == "home") {
      Serial1.println(7);
      Serial.println("home");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    } 
    else if (action == "full") {
      Serial1.println(9);
      seeded = true;
      Serial.println("full");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    }
    else if (action == "1") {
      Serial1.println(1);
      Serial.println("position 1");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    }
    else if (action == "2") {
      Serial1.println(2);
      Serial.println("position 2");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    }
    else if (action == "3") {
      Serial1.println(3);
      Serial.println("position 3");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    }
    else if (action == "4") {
      Serial1.println(4);
      Serial.println("position 4");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    }
    else if (action == "5") {
      Serial1.println(5);
      Serial.println("position 5");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    }
    else if (action == "6") {
      Serial1.println(6);
      Serial.println("position 6");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    }
    else if (action == "water") {
      Serial1.println(8);
      Serial.println("water");
      Firebase.RTDB.setString(&fbdo, "/btn/action", ""); // Reset to prevent re-triggering
    }

  };

}
