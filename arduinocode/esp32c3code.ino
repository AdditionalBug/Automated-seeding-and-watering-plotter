#include <Servo.h>


Servo servo;

const int X_STEP_PIN = 2;
const int X_DIR_PIN = 5;
const int Y_STEP_PIN = 3;
const int Y_DIR_PIN = 6;
const int Z_STEP_PIN = 4;
const int Z_DIR_PIN = 7;
const int ENABLE_PIN = 8;

const int X_LIMIT_PIN = 9;

const int servoPin = 10; //y axis

const int waterPin = 11; //z axis
const int CONTINUE_BTN_PIN = A2;

// Homing speed delay (microseconds between steps - lower is faster)
const int STEP_DELAY = 100;  // -> 200 or 400
const int down = 5000;
const int up = 0;

long positions[7][2] = {
  { 5000, 3000 },    // Position 1: Column 1, Row 1
  { 5000, 12000 },   // Position 2: Column 2, Row 1
  { 10000, 3000 },   // Position 3: Column 2, Row 2
  { 10000, 12000 },  // Position 4: Column 1, Row 2
  { 20000, 3000 },   // Position 5: Column 1, Row 3
  { 20000, 12000 },  // Position 6: Column 2, Row 3
  { 0, 0 }           // Position 7: Back to Home (0, 0)
};

long currentX = 0;
long currentY = 0;
long currentZ = 0;

/*----------------------------------------------------------------------------------*/


void setup() {
  Serial.begin(9600);

  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);

  pinMode(waterPin, OUTPUT);

  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);
  pinMode(CONTINUE_BTN_PIN, INPUT_PULLUP);

  pinMode(X_LIMIT_PIN, INPUT_PULLUP);

  servo.attach(servoPin);
  servo.write(0);

  // Set initial homing directions (Change HIGH/LOW if your motors move away from switches)
  digitalWrite(X_DIR_PIN, LOW);
  digitalWrite(Y_DIR_PIN, HIGH);
  // z directions
  digitalWrite(Z_DIR_PIN, LOW);
}





void loop() {
  static bool seeded = false;  // MUST be static so it remembers across loop cycles!
  int index = -1;              // Default to -1 so it doesn't trigger by accident

  if (Serial.available() > 0) {
    index = Serial.parseInt();
    Serial.println(index);


    // Clear out remaining characters (like \n or \r) from the serial buffer
    while (Serial.available() > 0) {
      Serial.read();
    }
  }

  if (index == 9) {
    for (int x = 1; x <= 7; x++) {
      ToPosition(positions[x - 1][0], positions[x - 1][1]);
      if(x != 7){
        toUD(down);
        seedDrop();
        water();
        toUD(up);
      }
    }
    seeded = true;  // Marks that seeding has occurred
  } else if (index >= 1 && index <= 7) {
    ToPosition(positions[index - 1][0], positions[index - 1][1]);
  } else if (index == 8) {
    water();
  } else if (index >= 11 && index <= 14) {
    if (seeded == true) {
      zonelogic(index);
      toUD(down);
      water();
      toUD(up);
    }
  }
}





/*----------------------------------------------------------------------------------*/


unsigned long limitPause() {
  // Check if limit pin is LOW
  if (digitalRead(X_LIMIT_PIN) == LOW) {
    delay(15); // Short verification window to filter out electrical noise
    
    // Confirm it is still LOW (real trigger, not a noise spike)
    if (digitalRead(X_LIMIT_PIN) == LOW) {
      digitalWrite(X_STEP_PIN, LOW);
      digitalWrite(Y_STEP_PIN, LOW);
      digitalWrite(Z_STEP_PIN, LOW);
      digitalWrite(waterPin, LOW); // Turn off water instantly

      unsigned long pauseStart = millis();

      while (true) {
        if (digitalRead(CONTINUE_BTN_PIN) == LOW) {
          delay(500);  // Debounce delay
          break;       
        }
      }
      return millis() - pauseStart; // Give back the exact pause duration
    }
  }
  return 0;
}



/*----------------------------------------------------------------------------------*/



// Move
void move(long steps, bool xDir, bool yDir) {
  // Set directions
  digitalWrite(X_DIR_PIN, xDir);
  digitalWrite(Y_DIR_PIN, yDir);


  // Pulse both motors for the specified number of steps
  for (long i = 0; i < steps; i++) {
    limitPause();
    digitalWrite(X_STEP_PIN, HIGH);
    digitalWrite(Y_STEP_PIN, HIGH);

    delayMicroseconds(STEP_DELAY);  // Control speed

    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);

    delayMicroseconds(STEP_DELAY);
  }
}

/*----------------------------------------------------------------------------------*/


void zmove(long steps, bool zDir) {
  digitalWrite(Z_DIR_PIN, zDir);

  for (long i = 0; i < steps; i++) {
    limitPause();
    digitalWrite(Z_STEP_PIN, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(Z_STEP_PIN, LOW);
    delayMicroseconds(STEP_DELAY);
  }
}

void toUD(long targetZ) {
  long deltaZ = targetZ - currentZ;
  if (deltaZ != 0) {
    bool dir = (deltaZ > 0) ? LOW : HIGH;  // Simplified direction logic
    zmove(abs(deltaZ), dir);
    currentZ = targetZ;
  }
}


/*----------------------------------------------------------------------------------*/



//move to coordinates
void ToPosition(long targetX, long targetY) {
  // --- Move X Axis (Sideways) ---
  long deltaX = targetX - currentX;
  if (deltaX != 0) {
    bool dir1 = (deltaX > 0) ? HIGH : LOW;
    bool dir2 = (deltaX > 0) ? LOW : HIGH;  // Opposite directions for X
    move(abs(deltaX), dir1, dir2);
    currentX = targetX;
  }

  // --- Move Y Axis (Forward/Backward) ---
  long deltaY = targetY - currentY;
  if (deltaY != 0) {
    bool dir1 = (deltaY > 0) ? HIGH : LOW;
    bool dir2 = (deltaY > 0) ? HIGH : LOW;  // Same directions for Y
    move(abs(deltaY), dir1, dir2);
    currentY = targetY;
  }
}



/*----------------------------------------------------------------------------------*/

void zonelogic(int index) {
  if (index == 11) {
    ToPosition(positions[0][0], positions[0][1]);
    ToPosition(positions[2][0], positions[2][1]);
  };
  if (index == 12) {
    ToPosition(positions[1][0], positions[1][1]);
    ToPosition(positions[3][0], positions[3][1]);
  };
  if (index == 13) {
    ToPosition(positions[2][0], positions[2][1]);
    ToPosition(positions[4][0], positions[4][1]);
  };
  if (index == 14) {
    ToPosition(positions[0][0], positions[0][1]);
    ToPosition(positions[2][0], positions[2][1]);
  }
  if (index == 11 || index == 13) {
    ToPosition(positions[0][0], positions[0][1]);
    ToPosition(positions[2][0], positions[2][1]);
    ToPosition(positions[4][0], positions[4][1]);
  };
  if (index == 12 || index == 14) {
    ToPosition(positions[1][0], positions[1][1]);
    ToPosition(positions[3][0], positions[3][1]);
    ToPosition(positions[5][0], positions[5][1]);
  }
}

/*----------------------------------------------------------------------------------*/

void water() {
  limitPause();
  analogWrite(waterPin, 110);
  
  unsigned long startTime = millis();
  unsigned long totalWaterTime = 10000; // 7 seconds total watering time

  while (true) {
    unsigned long elapsed = millis() - startTime;
    if (elapsed >= totalWaterTime) {
      break; // Finished watering duration
    }

    // Check if limit hit during watering
    unsigned long pauseDuration = limitPause();
    if (pauseDuration > 0) {
      // Shift startTime forward by the amount of time we were paused 
      // so the timer freezes while paused and resumes right where it left off
      startTime += pauseDuration; 
      analogWrite(waterPin, 110);// Turn water back on after resume
    }
  }
  // analogWrite(waterPin, 110);
  // delay(10000);
  analogWrite(waterPin, 0);
}


/*----------------------------------------------------------------------------------*/


void seedDrop() {
  limitPause();
  servo.write(120);
  
  unsigned long startTime = millis();
  unsigned long dropTime = 500; // 100ms hold time

  while (true) {
    unsigned long elapsed = millis() - startTime;
    if (elapsed >= dropTime) {
      break; 
    }

    // Check if limit switch is hit while dropping seed
    unsigned long pauseDuration = limitPause();
    if (pauseDuration > 0) {
      startTime += pauseDuration; // Freeze timer during pause
      servo.write(120);           // Re-apply servo position after resume
    }
  }
  
  servo.write(0);

}