/*
PROJECT: "SIMON" MEMORIZING GAME USING ARDUINO UNO
CREATOR: KELVIN LE (MINH NGUYEN LE)
CONTACT:
      email: nguyenleminh1002@gmail.com
             minhnguyen.le@qut.edu.au
      mobile: AUS: (+61) 423686585
              VN:  (+84) 929801193
LINKDIN: https://www.linkedin.com/in/kelvin-le-06a50b261/
GITHUB:  https://github.com/bocho0600
*/


#include <stdint.h>

//TONES
#define E4 363;
#define C4 305;
#define a4 484;
#define E3 181;
volatile uint32_t tone_Step1 = E4;  // >> (octave - 1);
volatile uint32_t tone_Step2 = C4;
volatile uint32_t tone_Step3 = a4;
volatile uint32_t tone_Step4 = E3;

volatile uint32_t state_lfsr1 = 0x11429984;  //student number
volatile uint32_t state_lfsr = 0x11429984;   //initial seed
volatile uint8_t step;                       // where the program stores the step
volatile uint8_t button1_pressed = 0;
volatile uint8_t button2_pressed = 0;
volatile uint8_t button3_pressed = 0;
volatile uint8_t button4_pressed = 0;

//PIN
const uint8_t pinLED1 = 5;  //pin for LED corresponding to each number
const uint8_t pinLED2 = 4;
const uint8_t pinLED3 = 3;
const uint8_t pinLED4 = 2;
const uint8_t pinBuzzer = 13;  //pin for buzzer
const uint8_t pinButt1 = 12;
const uint8_t pinButt2 = 11;
const uint8_t pinButt3 = 10;
const uint8_t pinButt4 = 9;
int sensorValue;
volatile uint16_t PlaybackDelay = 500;  //ms = 10^-3 s

//7-SEGMENT DISPLAY:
int pin[] = { A1, A2, A3, A4, A5, 8, 7, 6 };  //arduino pin array
int number[10][8] = {
  //number array
  { 1, 1, 1, 0, 1, 1, 1, 0 },  //0
  { 0, 0, 1, 0, 1, 0, 0, 0 },  //1
  { 1, 1, 0, 0, 1, 1, 0, 1 },  //2
  { 0, 1, 1, 0, 1, 1, 0, 1 },  //3
  { 0, 0, 1, 0, 1, 0, 1, 1 },  //4
  { 0, 1, 1, 0, 0, 1, 1, 1 },  //5
  { 1, 1, 1, 0, 0, 1, 1, 1 },  //6
  { 0, 0, 1, 0, 1, 1, 0, 0 },  //7
  { 1, 1, 1, 0, 1, 1, 1, 1 },  //8
  { 0, 1, 1, 0, 1, 1, 1, 1 }   //9

};


typedef enum {
  STEPS,
  RESPONSE,
  SUCCESS,
  FAIL
} state_t;
state_t state = STEPS;  //initial state
uint16_t SequenceLength = 1;
uint16_t StepLength = 0;
uint8_t ButtonsRecord = 0;
void next_step() {
  // calculate next step of sequence
  volatile uint32_t bit = (state_lfsr & 0b01);
  state_lfsr = state_lfsr >> 1;
  if (bit == 1) {
    state_lfsr = state_lfsr ^ 0xE2023CAB;
  }
  step = (state_lfsr & 0b11) + 1;
}
void all_off() {
  digitalWrite(pinLED1, LOW);  //turn LEDs off
  digitalWrite(pinLED2, LOW);
  digitalWrite(pinLED3, LOW);
  digitalWrite(pinLED4, LOW);
  tone(pinBuzzer, 0);  //turn buzzer off
}

void all_on() {
  digitalWrite(pinLED1, ~LOW);  //turn LEDs off
  digitalWrite(pinLED2, ~LOW);
  digitalWrite(pinLED3, ~LOW);
  digitalWrite(pinLED4, ~LOW);
  tone(pinBuzzer, 0);  //turn buzzer off
}

void PlayStep(uint8_t a) {
  switch (a) {
    case 1:
      digitalWrite(pinLED1, HIGH);
      tone(pinBuzzer, tone_Step1);
      break;
    case 2:
      digitalWrite(pinLED2, HIGH);
      tone(pinBuzzer, tone_Step2);
      break;
    case 3:
      digitalWrite(pinLED3, HIGH);
      tone(pinBuzzer, tone_Step3);
      break;
    case 4:
      digitalWrite(pinLED4, HIGH);
      tone(pinBuzzer, tone_Step4);
      break;
  }
}

void ReadButtons() {
  button1_pressed = digitalRead(pinButt1);
  button2_pressed = digitalRead(pinButt2);
  button3_pressed = digitalRead(pinButt3);
  button4_pressed = digitalRead(pinButt4);
}
volatile int score;
void DisplayScore(uint16_t c) {
  if (c <= 0) {
    score = 0;
  }
  for (int a = 0; a < 10; a++) {
    for (int b = 0; b < 8; b++) {

      digitalWrite(pin[b], number[score][b]);  //display numbers
    }
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(pinLED1, OUTPUT);  //pins for LEDs to be OUTPUT
  pinMode(pinLED2, OUTPUT);
  pinMode(pinLED3, OUTPUT);
  pinMode(pinLED4, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);  //pins for BUZZER to be OUTPUT

  pinMode(pinButt1, INPUT);  //pins for BUTTONS to be INPUT
  pinMode(pinButt2, INPUT);
  pinMode(pinButt3, INPUT);
  pinMode(pinButt4, INPUT);
  pinMode(A0, INPUT);  //value of POT

  pinMode(A1, OUTPUT);  //Pins for 7-segment display.
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  next_step();
}

void DisplayScore(void) {
  for (int a = 0; a < 10; a++) {
    for (int b = 0; b < 8; b++) {
      digitalWrite(pin[b], number[SequenceLength - 1][b]);  //display numbers
    }
  }
}
void loop() {
  switch (state) {
    case STEPS:
    DisplayScore();
      sensorValue = analogRead(A0);
      PlaybackDelay = sensorValue >> 1;
      PlayStep(step);
      StepLength += 1;
      delay(PlaybackDelay / 2);
      all_off();  //turn off all LED for 1/2 PlayBack Delay
      delay(PlaybackDelay / 2);
      next_step();
      if (StepLength >= SequenceLength) {
        state = RESPONSE;
        Serial.println("Response:");
        state_lfsr = state_lfsr1;  //reset the seed for input from button
        StepLength = 0;            //reset step length
      }
      break;
    case RESPONSE:
      ReadButtons();  //check for rising edge of buttons

      while (button1_pressed) {  //rising edge
        PlayStep(1);
        ReadButtons();                 //check for falling edge
        if (button1_pressed == LOW) {  //falling edge
          next_step();                 //regenerate seed
          ButtonsRecord = 1;
          if (ButtonsRecord == step) {
            StepLength += 1;
            if (StepLength == SequenceLength) {
              state = SUCCESS;
            }
          } else if (ButtonsRecord != step) {
            state = FAIL;
          }
          all_off();
        }
      }
      while (button2_pressed) {
        PlayStep(2);
        ReadButtons();
        if (button2_pressed == LOW) {
          next_step();
          ButtonsRecord = 2;
          if (ButtonsRecord == step) {
            StepLength += 1;
            if (StepLength == SequenceLength) {
              state = SUCCESS;
            }
          } else if (ButtonsRecord != step) {
            state = FAIL;
          }
          all_off();
        }
      }
      while (button3_pressed) {
        PlayStep(3);
        ReadButtons();
        if (button3_pressed == LOW) {
          next_step();
          ButtonsRecord = 3;
          if (ButtonsRecord == step) {
            StepLength += 1;
            if (StepLength == SequenceLength) {
              state = SUCCESS;
            }
          } else if (ButtonsRecord != step) {
            state = FAIL;
          }

          all_off();
        }
      }
      while (button4_pressed) {
        PlayStep(4);
        ReadButtons();
        if (button4_pressed == LOW) {
          next_step();
          ButtonsRecord = 4;
          if (ButtonsRecord == step) {
            StepLength += 1;
            if (StepLength == SequenceLength) {
              state = SUCCESS;
            }

          } else if (ButtonsRecord != step) {
            state = FAIL;
          }
          all_off();
        }
      }


      break;
    case SUCCESS:
      Serial.println("SUCCESS!");
      DisplayScore();
      delay(200);
      all_on();
      tone(pinBuzzer, 164 << 2);  //E3
      delay(150);
      tone(pinBuzzer, 130 << 2);  //C3
      delay(150);
      tone(pinBuzzer, 164 << 2);  //E3
      delay(150);
      tone(pinBuzzer, 196 << 2);  //G3
      delay(150);
      all_off();
      delay(200);
      state_lfsr = state_lfsr1;
      StepLength = 0;
      SequenceLength += 1;

      next_step();
      state = STEPS;

      break;
    case FAIL:
      Serial.println("FAIL!");
      all_on();
      tone(pinBuzzer, 880 >> 3);  //G3
      delay(100);
      tone(pinBuzzer, 0);
      delay(100);
      tone(pinBuzzer, 880 >> 3);  //G3
      delay(300);
      all_off();
      delay(200);
      state_lfsr1 = state_lfsr;
      state_lfsr = state_lfsr1;
      StepLength = 0;
      SequenceLength = 1;
      next_step();
      state = STEPS;
      break;
  }
}