#define LED D4 
#define LED2 D8
#define LED3 1
void setup() {
  pinMode(LED, OUTPUT); // set the digital pin as output.
  pinMode(LED2, OUTPUT); // set the digital pin as output.
  pinMode(LED3, OUTPUT); // set the digital pin as output.

}
void loop() {
  digitalWrite(LED3, LOW);
  digitalWrite(LED, HIGH);// turn the LED off.(Note that LOW is the voltage level but actually
  delay(1000);          // wait for 1 second.
  digitalWrite(LED, LOW); // turn the LED on.
  digitalWrite(LED2, HIGH);
  delay(1000);         // wait for 1 second.
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
  delay(1000);         // wait for 1 second.

}