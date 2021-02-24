const int diodePin = A0;
const int safetyButton = 4;
const int safetyIndicator = 5;
const int safetySwitch = 6;
const int fireSignal = 7;
const int resetPin = 8;
int flashDelay = 100;

float sensorValue;
float onTime = 1000; // coil on time

enum CurrentMode{
  idle,
  safe,
  hot
};

CurrentMode currentMode = idle;


bool safetyToggle = false;
bool flashFO = true;
bool resetFO = false;

bool fired = false;


void setup() {
  Serial.begin(9600);
  pinMode(safetyButton, INPUT_PULLUP);
  pinMode(safetyIndicator, OUTPUT);
  pinMode(safetySwitch, OUTPUT);
  pinMode(fireSignal, INPUT);
  currentMode = safe;
  
}

void loop() {
  HandleSafety();
  if (CheckFire() == false){
    digitalWrite(safetySwitch, LOW);
    resetFO = true;
    delay(200);
  }

  if (resetFO && digitalRead(safetyButton) == LOW){
    pinMode(resetPin, OUTPUT);
    
  }
}

void HandleSafety() {
  if (!safetyToggle && digitalRead(safetyButton) == LOW){
    currentMode = hot;
    safetyToggle = true;
    digitalWrite(safetySwitch, HIGH); //pnp
    
    Flash(true);
  }
  if (safetyToggle && digitalRead(safetyButton) == LOW){
    currentMode = safe;
    safetyToggle = false;
    digitalWrite(safetySwitch, LOW);
    
    Flash(false);
  }
}

bool CheckFire() {
  if (digitalRead(fireSignal) == HIGH){
    delay(onTime);
    return true;
  } else {
    return false;
  }
}

void Flash(bool hot) {
  if (hot){
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
  if (!hot){
    digitalWrite(safetyIndicator, LOW);
    delay(flashDelay);
    digitalWrite(safetyIndicator, HIGH);
    delay(flashDelay);
    digitalWrite(safetyIndicator, LOW);
  }
}
