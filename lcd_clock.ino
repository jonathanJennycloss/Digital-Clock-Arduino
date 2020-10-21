
#include <Wire.h>
#include <DS3231.h>
#include <pitches.h>
/*Please change the timer used by the pitches library.*/
#include <IRremote.h>
#include <IRremoteInt.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8,9,10,11,12,13);

int receive = 2;
IRrecv irrecv(receive); 
decode_results result;

const long power =  0xFFA25D;
const long func = 0xFFE21D;
const long volUp =  0xFF629D;
const long back = 0xFF22DD;
const long pause = 0xFF02FD;
const long forward = 0xFFC23D;
const long down = 0xFFE01F;
const long volDown = 0xFFA857;
const long up = 0xFF906F;
const long eq = 0xFF9867;
const long rept = 0xFFB04F;
const long zero = 0xFF6897;
const long one = 0xFF30CF;
const long two = 0xFF18E7;
const long three = 0xFF7A85;
const long four = 0xFF10EF;
const long five = 0xFF38C7;
const long six = 0xFF5AA5;
const long seven = 0xFF42BD;
const long eight = 0xFF4AB5;
const long nine = 0xFF52AD;
const long repeat = 0xFFFFFFFF;


DS3231 clock; //class in header file
RTCDateTime t; //struct in header file

const int pinTemp = 0;

bool go = false;
int hr, min;

const int buzzerPin = 5;
//notes in the melody:
int melody[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
int duration = 500; //500 ms


byte lowerArms[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010
};
byte raiseArms[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};

void setup() {
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  irrecv.enableIRIn();
  clock.begin();
  clock.setDateTime(__DATE__,__TIME__);
  lcd.createChar(1, lowerArms);
  lcd.createChar(2, raiseArms);
}

void loop() {
 if (irrecv.decode(&result)) // have we received an IR signal?
  {
    lcd.clear();
    lcd.setCursor(0,0);
    IR();
    irrecv.resume();
  }
  tempRead();
  clock_time();
  if (t.hour == hr and t.minute == min and t.second == 0){
    alarm();
  }
}


void tempRead(){
  int temperature = analogRead(pinTemp);
  //convert analog reading to a temp in K.
  double kelvin = log(10000.0 * (1024.0 / temperature - 1)); 
  kelvin = 1/(0.001129148 +(0.000234125 + (0.0000000876741*kelvin*kelvin))*kelvin);
  float degC = kelvin - 273.15;
  //float degF = (degC*9.0)/5.0 + 32.0;
  lcd.setCursor(10,0);
  lcd.print(degC, 1); // 2s.f.
  lcd.print("oC");
  delay(100);  
}

void clock_time(){
  t = clock.getDateTime();
  lcd.setCursor(0,0);
  lcd.print(t.year); lcd.print("/");
  lcd.print(t.month);lcd.print("/");
  lcd.print(t.day);lcd.setCursor(0,1);
  lcd.print(t.hour);lcd.print(":");
  if (t.minute<10){
    lcd.setCursor(3,1);
    lcd.print("0");
  }
  lcd.print(t.minute);
  lcd.print(":");
  if (t.second<10){
    lcd.setCursor(6,1);
    lcd.print("0");
  }
  lcd.print(t.second);
  delay(900);
}

void IR(){
  switch(result.value){
  case pause:  go = !go;  break;
  case one: timer(); break;
  case two: stopwatch();  break;
  case three: setAlarmHour();    break;
  case four:   setAlarmMin(); break;
  case five:  character();  break;
   default:
   Serial.println(" other button ");
  }
  delay(500);
}

void timer(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("timer");
  int timers = 1;
  go = true;
  delay(2000);
  while(go){
    lcd.clear();
    lcd.print(timers);
    delay(1000);
    timers = timers + 1;
    if (irrecv.decode(&result)){
    lcd.clear();
    lcd.setCursor(0,0);
    switch(result.value){
    case power: go = !go; break;
    case pause:  go = !go; break;  
    }
    irrecv.resume();
  }
  }
}

void stopwatch(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Stopwatch: ");
  delay(1000);
  int i = 0;
  go = true;
  while(go){
   if (irrecv.decode(&result)) // have we received an IR signal?
  {
    switch(result.value){
    case up: i++;   break;
    case down: i--;   break;
    case pause: go = !go; break;  
    }
    irrecv.resume(); // receive the next value
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(i);
  delay(200);
  lcd.cursor();
  delay(300);
  // Turn on the cursor:
  lcd.noCursor();
  }
  lcd.clear();
  for (i=i; i>0; i--){
    if (irrecv.decode(&result)){
        switch(result.value){
        case power: i=0; break;  
        }
        irrecv.resume(); // receive the next value
        }
    lcd.clear();
    lcd.print(i);
    delay(1000);
  }
}


void setAlarmHour(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set alarm hour");
  delay(1000);
  hr = 0;
  go = true;
  while(go){
   if (irrecv.decode(&result)){
    switch(result.value){
    case up: hr++;   break;
    case down: hr--;   break;
    case pause: go = !go; break; 
    case power: go = !go; break; 
    }
    irrecv.resume(); 
  }
  lcd.clear();
  lcd.setCursor(0,0);
  if (hr < 0){
    hr = 0;
  }else if(hr > 23){
    hr = 23;
  }
  lcd.print(hr);
  delay(200);
  lcd.cursor();
  delay(300);
  // Turn on the cursor:
  lcd.noCursor();
  }
  lcd.clear();
}

void setAlarmMin(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set alarm minute");
  delay(1000);
  min = 0;
  go = true;
  while(go){
   if (irrecv.decode(&result)){
    switch(result.value){
    case up: min++;   break;
    case down: min--;   break;
    case pause: go = !go; break; 
    case power: go = !go; break; 
    }
    irrecv.resume();
  }
  lcd.clear();
  lcd.setCursor(0,0);
  if (min < 0){
    min = 0;
  }else if(min > 59){
    min = 59;
  }
  lcd.print(min);
  delay(200);
  lcd.cursor();
  delay(300);
  // Turn on the cursor:
  lcd.noCursor();
  }
  lcd.clear();
}

void alarm(){
  lcd.clear();
  int thisNote;
  go = true;
  int j = 1;
  while(go){
    if (j%2 == 0){
      thisNote = 0; 
    }else{
      thisNote = 1;
    }
    if (irrecv.decode(&result)){
        switch(result.value){
        case power: thisNote=9; go = !go; break;  
        }
        irrecv.resume();
        }
    tone(buzzerPin, melody[thisNote], duration);
    character();
  j++;
  //delay(500); 
  }
  lcd.clear();
}

void character(){
    int sensor = analogRead(A0);
    int delayT = map(sensor, 0, 1023, 200, 1000);
    lcd.setCursor(4, 1);
    // draw the little man, arms down:
    lcd.write(1);
    lcd.setCursor(4, 0);
    lcd.write(1);
    lcd.setCursor(11, 1);
    lcd.write(1);
    lcd.setCursor(11, 0);
    lcd.write(1);
    delay(delayT);
    lcd.setCursor(4, 1);
    // draw him arms up:
    lcd.write(2);
    lcd.setCursor(4, 0);
    lcd.write(2);
    lcd.setCursor(11, 1);
    lcd.write(2);
    lcd.setCursor(11, 0);
    lcd.write(2);
    delay(delayT);
}
