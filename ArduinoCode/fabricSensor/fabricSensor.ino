///////////////////////////
// zPatch Sensor Code /////
///////////////////////////

// paul.strohmeier@gmail.com
// zPatch.github.io

// This Sketch is for testing zPatch, presented at TEI 2018

// The key things to know are the function 
// dualAnalogRead(pinA, pinB, samples) and capacitiveRead(pinA, pinB, samples)
// Both functions take two analog pins as parameters (where your sensors is connected)
// The third parameter is the number of times you want to sample. 
// capacitiveRead needs more samples, dualAnalogRead is quite stable.

// Based on the adcTouch library --> https://github.com/martin2250/ADCTouch

//Values for filtering and storing capacitive readings
int baseline;
int capValues;
int prevCapValues;
int rawCapacitance;
float k = 0.2; // this adjusts the low-pass filter: 
               // 0 == no signal
               // 0.001 == very aggressive (slow but steady) 
               // 0.999 == not aggressive at all (fast but noisy)
               // 1 == no filter

//value for storing resistive readings
int resValues;

void setup()
{
  Serial.begin(9600); // open the arduino serial port
  delay(50); //wait a bit before establishing baseline
  resValues = dualAnalogRead(A0, A1, 3); //this line should not be needed, but might stabalize readings
  baseline = capacitiveRead(A0, A1, 10); //set the baseline for capacitive readings
  Serial.println(baseline);
  delay(50);
}


void loop()
{
  rawCapacitance = capacitiveRead(A0, A1, 10); //sample capacitance
  rawCapacitance = (rawCapacitance - baseline); //baseline capacitance value 
  capValues = prevCapValues + (k * (rawCapacitance - prevCapValues)); //filter capacitive value
  prevCapValues = capValues; //for filtering

  resValues = dualAnalogRead(A0, A1, 3); //sample resistance

//print to serial port
  Serial.print(capValues); 
  Serial.print(", ");
  Serial.print(resValues); 
  Serial.println(); 


  delay(5); // (so as not to completely flood the serial port)

}

// ---------------- Here there be Dragons ---------- 

int capacitiveRead(int pinA, int pinB, int number) {
  int capacitanceA = 0;
  int capacitanceB = 0;
  int capacitance = 0;

  for (int i = 0; i < number; i++) {
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT_PULLUP);
    ADMUX |=   0b11111;
    ADCSRA |= (1 << ADSC); //start conversion
    while (!(ADCSRA & (1 << ADIF))); //wait for conversion to finish
    ADCSRA |= (1 << ADIF); //reset the flag
    pinMode(pinB, INPUT);
    capacitanceB = analogRead(pinB);

    pinMode(pinB, INPUT);
    pinMode(pinA, INPUT_PULLUP);
    ADMUX |=   0b11111;
    ADCSRA |= (1 << ADSC); //start conversion
    while (!(ADCSRA & (1 << ADIF))); //wait for conversion to finish
    ADCSRA |= (1 << ADIF); //reset the flag
    pinMode(pinA, INPUT);
    capacitanceA = analogRead(pinA);

    capacitance = capacitance + capacitanceA + capacitanceB;

  }
  return capacitance;
}


int dualAnalogRead(int pinA, int pinB, int number) {
  int resistanceA = 0;
  int resistanceB = 0;
  int resistance = 0;

  for (int i = 0; i < number; i++) {
    pinMode(pinA, OUTPUT);
    digitalWrite(pinA, LOW);
    pinMode(pinB, INPUT_PULLUP);
    resistanceB = analogRead(pinB);

    pinMode(pinB, OUTPUT);
    digitalWrite(pinB, LOW);
    pinMode(pinA, INPUT_PULLUP);
    resistanceA = analogRead(pinA);

    resistance = resistance + (resistanceA + resistanceB) / 2;

  }
  return resistance / number;
}



