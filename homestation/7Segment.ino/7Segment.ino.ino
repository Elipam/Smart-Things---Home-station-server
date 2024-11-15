int RES_pin = 5;
int RCK_pin = 0;
int SCK_pin = 4;
int RES_pin2 = 13;
int RCK_pin2 = 14;
int SCK_pin2 = 12;

int dec_digits[12] = {3, 159, 37, 13, 153, 73, 65, 31, 1, 9, 254,0};

void setup() {
    pinMode(RES_pin, OUTPUT);
    pinMode(RCK_pin, OUTPUT);
    pinMode(SCK_pin, OUTPUT);
    pinMode(RES_pin2, OUTPUT);
    pinMode(RCK_pin2, OUTPUT);
    pinMode(SCK_pin2, OUTPUT);    
}

void loop() { 
 
  for (int i=0;i<12;i++){
    digitalWrite(RCK_pin,LOW);
    shiftOut(RES_pin, SCK_pin, LSBFIRST,dec_digits[i]);
    digitalWrite(RCK_pin, HIGH);
    digitalWrite(RCK_pin2,LOW);
    shiftOut(RES_pin2, SCK_pin2, LSBFIRST,dec_digits[i]);
    digitalWrite(RCK_pin2, HIGH);
    delay(1000);

  }
}