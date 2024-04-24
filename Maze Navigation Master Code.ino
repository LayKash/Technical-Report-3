#include <Wire.h>
#define I2C_SLAVE_ADDR 0x04  // 4 in hexadecimal
#include<LiquidCrystal.h>
#include <Keypad.h>
#define A 13          // enableA command line
const int rs = 19, en = 23, d4 = 18, d5 = 17, d6 = 16, d7 = 15;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 32, 33, 25, 26 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 27, 14, 4 };       //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String path;
char input[50];
int distance = 0;
int direction = 90;
int encL = 0;
int a = 0;
int Motorspeed = 0;
int preb = 0;  //previous encoder
int diff = 0;  //distance traveled, diffrence between previous and new encoder value
int g=0;
int c=0;

void setup() {
  Serial.begin(9600);
  pinMode(A, OUTPUT);
  Wire.begin();  // join i2c bus (address optional for the master) - on the ESP32 the default I2C pins are 21 (SDA) and 22 (SCL)
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("  Input Keypad  ");
  lcd.setCursor(0, 1);
}

// three integer values are sent to the slave device
int x = 0;
int y = 0;


void loop() {
 
  int16_t b;
  // two 16-bit integer values are requested from the slave
  while (a <= 0) {
    if (digitalRead(A) == HIGH) {
      a = 1;
      Serial.println("a");
    }
    char key = keypad.getKey();

    if (key) {
      path += key;
        Serial.println(key);
    
        lcd.print(key);
      if (key == '*') {
        path = "";
        lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("  Input Keypad  ");
        lcd.setCursor(0, 1);
      }
      path.toCharArray(input, 50);
      Serial.println(input);
    }
  }
  for (int i = 0; i < 51; i++) {
   
    b = 0;
    uint8_t bytesReceived = Wire.requestFrom(I2C_SLAVE_ADDR, 4);  // 4 indicates the number of bytes that are expected
    uint8_t b16_9 = Wire.read();                                  // receive bits 16 to 9 of b (one byte)
    uint8_t b8_1 = Wire.read();                                   // receive bits 8 to 1 of b (one byte)

    b = (b16_9 << 8) | b8_1;  // combine the two bytes into a 16 bit number


    Serial.print("\t");
    Serial.println(b);
     if(g==1){
     diff=b+preb;
     Serial.println("difference: ");
     Serial.print(diff);
      }
      else{
    diff = b - preb;  //calculate distance bot has traveled so far
    Serial.println("difference: ");
    Serial.print(diff);
      }
    Serial.print("\t");
    Serial.println(diff);
    Serial.print("\t");
    Serial.println(i);
    // do //if the command is complete send next message
    // {


    if (diff < encL||(diff>encL && g==1)) {
      i--;
     // preb=0;
      delay(10);
    }
    else{
    preb = b;
    }
    //check what that value says
    if (input[i] == '1') {
      distance = 10;
    }
    if (input[i] == '2') {
      distance = 20;
    }
    if (input[i] == '3') {
      distance = 30;
    }
    if (input[i] == '4') {
      distance = 40;
    }
    if (input[i] == '5') {
      distance = 50;
    }
    if (input[i] == '6') {
      distance = 60;
    }
    if (input[i] == '7') {
      direction = 0;
      Motorspeed = 160;
      g=0;
    }
    if (input[i] == '8') {
      direction = 88;
      Motorspeed = 140;
      g=0;
    }
    if (input[i] == '9') {
      direction = 140;
      Motorspeed = 140;
      g=0;
    }
    if (input[i] == '0') {
      direction = 90;
      Motorspeed = -220;
      g=1;
    }
    if (input[i] == '#') {
      direction = 90;
      Motorspeed = 0;
      distance = 0;
      g=0;
    }
    Serial.println("Send");
    Serial.print(distance);
    Serial.println("\t");
    Serial.print(direction);
    Serial.println("\t");
    Serial.println(Motorspeed);
    Serial.println("\t");
    Serial.println(b);
    //calcuclate encoder value left
    encL = distance*2.3;
    x = Motorspeed;
    y = Motorspeed+5;//right motor
   

    //encR=28*(distance/6*3.14);//right
    Serial.println("\t");
    Serial.println(encL);
    //Get new encoder value
    //send to main
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    //direction
    Wire.write((byte)((direction & 0x0000FF00) >> 8));  // first byte of x, containing bits 16 to 9
    Wire.write((byte)(direction & 0x000000FF));         // second byte of x, containing the 8 LSB - bits 8 to 1
                                                        //distance encL
    //speed
    Wire.write((byte)((x & 0x0000FF00) >> 8));  // first byte of x, containing bits 16 to 9
    Wire.write((byte)(x & 0x000000FF));         // second byte of x, containing the 8 LSB - bits 8 to 1

    //speed
    Wire.write((byte)((y & 0x0000FF00) >> 8));  // first byte of x, containing bits 16 to 9
    Wire.write((byte)(y & 0x000000FF));         // second byte of x, containing the 8 LSB - bits 8 to 1
    Wire.endTransmission();
    
    //make it stop moving
    if (i == 50) {
      for (int n; 51; ++n) {
        input[n] = '#';
        diff=1000;
        encL=1;
      }
    }
    if(input[i]=='0'){
      c=input[i+1];
      c=c*20;
      delay(c);
      i=++i;
    }
    else{
      delay(500);
    }
    
  }
}
