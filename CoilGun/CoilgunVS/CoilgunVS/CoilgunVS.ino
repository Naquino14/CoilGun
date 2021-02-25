#pragma region variables

const int diodePin = A0;
const int safetyButton = 4;
const int safetyIndicator = 5;
const int safetySwitch = 6;
const int fireSignal = 7;
const int resetPin = 8;
int flashDelay = 100;

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
float onTime = 1000; // coil on time

enum CurrentMode {
    idle,
    safe,
    hot
};

CurrentMode currentMode = idle;


bool safetyToggle = false;
bool flashFO = true;
bool resetFO = false;

bool fired = false;

#pragma endregion

void setup() {
    Serial.begin(9600);
    pinMode(safetyButton, INPUT_PULLUP);
    pinMode(safetyIndicator, OUTPUT);
    pinMode(safetySwitch, OUTPUT);
    pinMode(fireSignal, INPUT);
    currentMode = safe;

}

void loop() {
    // voltage readouts

    CheckVoltage();
    SerialPrintVoltage();

    // gun stuff
    HandleSafety();
    if (CheckFire() == false) {
        digitalWrite(safetySwitch, LOW);
        resetFO = true;
        delay(200);
    }

    if (resetFO && digitalRead(safetyButton) == LOW) {
        pinMode(resetPin, OUTPUT);

    }
}

void HandleSafety() {
    if (!safetyToggle && digitalRead(safetyButton) == LOW) {
        currentMode = hot;
        safetyToggle = true;
        digitalWrite(safetySwitch, HIGH); //pnp

        Flash(true);
    }
    if (safetyToggle && digitalRead(safetyButton) == LOW) {
        currentMode = safe;
        safetyToggle = false;
        digitalWrite(safetySwitch, LOW);

        Flash(false);
    }
}

void CheckVoltage() {
    bp1Val = analogRead(bp1);
    bp1Val = map(bp1Val, 0, 1023, 655, 860);

    bp2Val = analogRead(bp2);
    bp2Val = map(bp2Val, 0, 1023, 655, 860);

    bp3Val = analogRead(bp3);
    bp3Val = map(bp3Val, 0, 1023, 655, 860);

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

bool CheckFire() {
    if (digitalRead(fireSignal) == HIGH) {
        delay(onTime);
        return true;
    }
    else {
        return false;
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