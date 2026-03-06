const int touchpin0 = 27;
const int touchpin1 = 33;
const int touchpin2 = 32;
const int touchpin = 14;
const int ledpin = 12;
const int led0 = 25;
const int led1 = 26;
const int led2 = 13;
const int threshold = 1040;
const int pin2 = LED_BUILTIN;

// 'volatile' tells the compiler that this variable can change 
// unexpectedly (inside the interrupt) so don't "optimize" it.
volatile int count = 0;
volatile int n = 1;
volatile bool touchDetected = false;
volatile bool touchDetected0 = false;
volatile bool touchDetected1 = false;
volatile bool touchDetected2 = false;

// Variables for the non-blocking blinker
int blinkCount = 0;
unsigned long lastAction = 0;
const long blinkSpeed = 150;
const long cycleGap = 750;
bool ledState = false;

//password values
const int passwd[3] = {1,2,3};
int i = 0;
const int code_length = 3;


// The Interrupt Service Routine (ISR)
// This function runs INSTANTLY when the touch threshold is met.
void IRAM_ATTR handle_0_Touch() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Software Debouncing: ignore interrupts faster than 250ms
  if (interruptTime - lastInterruptTime > 250) {
    count ^= 1;
    if (count > 7) count = 0;
    touchDetected0 = !touchDetected0; 
  }
  lastInterruptTime = interruptTime;
}

void IRAM_ATTR handle_1_Touch() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Software Debouncing: ignore interrupts faster than 250ms
  if (interruptTime - lastInterruptTime > 250) {
    count ^= 2;
    if (count > 7) count = 0;
    touchDetected1 = !touchDetected1; 
  }
  lastInterruptTime = interruptTime;
}

void IRAM_ATTR handle_2_Touch() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Software Debouncing: ignore interrupts faster than 250ms
  if (interruptTime - lastInterruptTime > 250) {
    count ^= 4;
    if (count > 7) count = 0;
    touchDetected2 = !touchDetected2; 
  }
  lastInterruptTime = interruptTime;
}

void IRAM_ATTR handleTouch() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Software Debouncing
  if (interruptTime - lastInterruptTime > 500) {
    // 1. The Atomic Copy (Take the Snapshot)
    n = count;
    
    // 2. Alert the Consumer (Main Loop)
    touchDetected = true;
    
    // 3. Clear the Production Buffer and UI Flags
    count = 0;
    touchDetected0 = false;
    touchDetected1 = false;
    touchDetected2 = false;
  }
  lastInterruptTime = interruptTime;
}

void setup() {
  Serial.begin(115200);
  pinMode(ledpin, OUTPUT);
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(pin2, OUTPUT);

  // Attach the interrupt to the touch pin
  // Parameters: Pin, Function to run, Threshold
  touchAttachInterrupt(touchpin0, handle_0_Touch, threshold);
  touchAttachInterrupt(touchpin1, handle_1_Touch, threshold);
  touchAttachInterrupt(touchpin2, handle_2_Touch, threshold);
  touchAttachInterrupt(touchpin, handleTouch, threshold);
  
  Serial.println("Interrupt System Initialized.");
}

void loop() {
  unsigned long now = millis();

  // 1. Instantly Drive UI based on ISR Toggle Flags
  digitalWrite(led0, touchDetected0 ? HIGH : LOW);
  digitalWrite(led1, touchDetected1 ? HIGH : LOW);
  digitalWrite(led2, touchDetected2 ? HIGH : LOW);

  // 2. Consume the Snapshot
  if (touchDetected) {
    Serial.print("Submit Triggered! Evaluating Snapshot: ");
    Serial.println(n);

    // Evaluate against the snapshot 'n', not the volatile 'count'
    if (n == passwd[i]) {
      Serial.println("Correct digit: progress++");
      i++;
    } else {
      Serial.println("Incorrect digit: Sequence reset.");
      i = 0;
    }

    // Check for complete system unlock
    if (i == code_length) {
      Serial.println("System Unlocked!");
      digitalWrite(pin2, HIGH);
      i = 0; // Reset progress for future entries
    } else {
      digitalWrite(pin2, LOW);
    }

    // Acknowledge receipt and reset the trigger flag
    touchDetected = false; 
    blinkCount = 0; // Optional: restart the sequencer
  }

  // 3. THE SEQUENCER (Unchanged)
  if (blinkCount < n) {
    if (now - lastAction >= blinkSpeed) {
      lastAction = now;
      ledState = !ledState;
      digitalWrite(ledpin, ledState);
      if (!ledState) blinkCount++;
    }
  } else {
    digitalWrite(ledpin, LOW);
    if (now - lastAction >= cycleGap) {
      blinkCount = 0;
      lastAction = now;
    }
  }
}
