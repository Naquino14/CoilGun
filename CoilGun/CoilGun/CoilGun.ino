const int diodePin = A0;
const int safetyButton = 4;
const int safetyIndicator = 5;
const int triggerSafety = 6;
const int checkFire = 7;
const int resetPin = 8;
int flashDelay = 100;

const int hiBatInd = 9;
const int mdBatInd = 10;
const int loBatInd = 11;

// read lipo battery voltage
const int bp1 = A0;
const int bp2 = A1;
const int bp3 = A2;

int bp1Val;
int bp2Val;
int bp3Val;

// read external battery voltage (for now, i dont wanna use a bec)
const int exbp = A3;
int exbpVal;


/// finding ratios for lipo:
/// volt  :  analog
///  5    :   1024
/// divide by 1.19047619
/// 4.2max: ~860.16
///
///  5    :   1024
/// divide by 1.5625
/// 3.2min:  655.36
// map for 655.36, 860.16

float sensorValue;
float onTime = 200; // coil on time

enum CurrentMode {
    idle,
    safe,
    hot
};

enum VoltageLevel {
    hi,
    md,
    lo
};

CurrentMode currentMode = idle;
VoltageLevel voltageLevel = hi;

bool safetyToggle = false;
bool flashFO = true;
bool resetFO = false;
bool thresholdFO = true;

bool fired = false;


void setup() {
    Serial.begin(9600);
    pinMode(safetyButton, INPUT_PULLUP);
    pinMode(safetyIndicator, OUTPUT);
    pinMode(triggerSafety, OUTPUT);
    pinMode(checkFire, INPUT);
    pinMode(hiBatInd, OUTPUT);
    pinMode(mdBatInd, OUTPUT);
    pinMode(loBatInd, OUTPUT);
    currentMode = safe;

    CheckVoltage();
    CheckThreshold();
}

void loop() {
    // voltage readouts

    CheckVoltage();
    //SerialPrintVoltage();

    CheckThreshold();

    // gun stuff
    HandleSafety();

    //check fire here
    if (CheckFire() && currentMode == hot){
      delay(onTime);
      digitalWrite(triggerSafety, LOW);
      Flash(false);
      resetFO = true;
      
    }
    if (resetFO && digitalRead(safetyButton) == LOW) {
        pinMode(resetPin, OUTPUT); // not very fancy way of force resetting

    }
}

void HandleSafety() {
    if (!safetyToggle && digitalRead(safetyButton) == LOW) {
        currentMode = hot;
        safetyToggle = true;
        digitalWrite(triggerSafety, HIGH); //pnp
        Serial.println("disabling safety...");
        Serial.println("////////");
        Flash(true);
    }
    if (safetyToggle && digitalRead(safetyButton) == LOW) {
        currentMode = safe;
        safetyToggle = false;
        digitalWrite(triggerSafety, LOW);
        Serial.println("enabling safety");
        Serial.println("\\\\\\\\");
        Flash(false);
    }
}

bool CheckFire(){
  if (digitalRead(checkFire)){
    return true;
  }
  else{
    return false;
  }
}

void CheckVoltage() {
    bp1Val = analogRead(bp1);
    bp1Val = map(bp1Val, 655, 860, 0, 1024);

    bp2Val = analogRead(bp2);
    bp2Val = map(bp2Val, 655, 860, 0, 1024);

    bp3Val = analogRead(bp3);
    bp3Val = map(bp3Val, 655, 860, 0, 1024);

    exbpVal = analogRead(exbp);
}

void SerialPrintVoltage() {
    Serial.println("//////");

    Serial.print("S1: ");
    Serial.println(bp1Val);

    Serial.print("S2: ");
    Serial.println(bp2Val);

    Serial.print("S3: ");
    Serial.println(bp3Val);

    Serial.print("XS: ");
    Serial.println(exbpVal);
}

void CheckThreshold() {
    int _val = min(bp1Val, min(bp2Val, bp3Val));
    if (_val > 685) {
        voltageLevel = hi;
        DisplayVoltage(voltageLevel);
    }
    else if (_val > 342) {
        voltageLevel = md;
        DisplayVoltage(voltageLevel);
    }
    else {
        voltageLevel = lo;
        DisplayVoltage(voltageLevel);
    }
}

void DisplayVoltage(VoltageLevel voltageLevel) {
    if (voltageLevel == hi) {
        digitalWrite(hiBatInd, HIGH);
        digitalWrite(mdBatInd, LOW);
        digitalWrite(loBatInd, LOW);
    }
    if (voltageLevel == md) {
        digitalWrite(hiBatInd, LOW);
        digitalWrite(mdBatInd, HIGH);
        digitalWrite(loBatInd, LOW);
    }
    if (voltageLevel == lo) {
        digitalWrite(hiBatInd, LOW);
        digitalWrite(mdBatInd, LOW);
        digitalWrite(loBatInd, HIGH);
    }
}

void Flash(bool hot) {
    if (hot) {
        digitalWrite(safetyIndicator, HIGH);
        delay(flashDelay);
        digitalWrite(safetyIndicator, LOW);
        delay(flashDelay);
        digitalWrite(safetyIndicator, HIGH);
        delay(flashDelay);
        digitalWrite(safetyIndicator, LOW);
        delay(flashDelay);
        digitalWrite(safetyIndicator, HIGH);
        delay(flashDelay);
        digitalWrite(safetyIndicator, LOW);
        delay(flashDelay);
        digitalWrite(safetyIndicator, HIGH);
    }
    if (!hot) {
        digitalWrite(safetyIndicator, LOW);
        delay(flashDelay);
        digitalWrite(safetyIndicator, HIGH);
        delay(flashDelay);
        digitalWrite(safetyIndicator, LOW);
    }
}
