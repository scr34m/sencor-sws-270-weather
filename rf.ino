// Leonardo PIN 3
// Uno / Nano PIN 2
#define DATAPIN 2
#define INTERRUPT 0

#define RCSWITCH_MAX_CHANGES 256

#define T 480
const unsigned int nSeparationLimit = 8 * T;
unsigned int timings[RCSWITCH_MAX_CHANGES];

volatile unsigned int ringIndex = 0;
int nReceiveTolerance = 60;

bool THRESHOLD(int value, unsigned long delay, unsigned long tolerance) {
  if (value < delay - tolerance) {
    return false;
  }
  if (value > delay + tolerance) {
    return false;
  }
  return true;
}

bool receiveProtocol5(unsigned int changeCount){
  unsigned long code = 0;
  unsigned long delayTolerance = T * nReceiveTolerance * 0.01; 
  unsigned int firstSignal;
  unsigned int secondSignal;
      
  int i = 1; // skip 1 timing the separator

  while(i + 2 < changeCount) {
      firstSignal = timings[i++];
      secondSignal = timings[i++];
      if (THRESHOLD(firstSignal, T, delayTolerance) && THRESHOLD(secondSignal, T*2, delayTolerance)) {
        Serial.print("0");
      } else if (THRESHOLD(firstSignal, T, delayTolerance) && THRESHOLD(secondSignal, T*4, delayTolerance)) {
        Serial.print("1");
      } else {
        Serial.print("?");
      }
    }

  Serial.println();    
}

static inline unsigned int diff(int A, int B) {
  return abs(A - B);
}

void handleInterrupt() {
  static unsigned int changeCount = 0;
  static unsigned long lastTime = 0;
  static unsigned int repeatCount = 0;

  const long time = micros();
  const unsigned int duration = time - lastTime;

  if (duration > nSeparationLimit) {
    // A long stretch without signal level change occurred. This could
    // be the gap between two transmission.
    if (diff(duration, timings[0]) < 200) {
      // This long signal is close in length to the long signal which
      // started the previously recorded timings; this suggests that
      // it may indeed by a a gap between two transmissions (we assume
      // here that a sender will send the signal multiple times,
      // with roughly the same gap between them).
      repeatCount++;
      if (repeatCount == 2) {
        receiveProtocol5(changeCount);
        repeatCount = 0;
      }
    }
    changeCount = 0;
  }
 
  // detect overflow
  if (changeCount >= RCSWITCH_MAX_CHANGES) {
    changeCount = 0;
    repeatCount = 0;
  }

  timings[changeCount++] = duration;
  lastTime = time;  
}

void setup() {
  Serial.begin(9600);
  attachInterrupt(INTERRUPT, handleInterrupt, CHANGE);
}

void loop() {
}
