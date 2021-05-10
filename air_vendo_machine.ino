#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27,20,4);


const int pressureInput = A15; //select the analog input pin for the pressure transducer
const int pressureZero = 102.4; //analog reading of pressure transducer at 0psi
const int pressureMax = 921.6; //analog reading of pressure transducer at 100psi
const int pressuretransducermaxPSI = 100; //psi value of transducer being used

float pressureValue = 0.0;
int coinInserted = 0;
int interruptPin = 2;
int solenoidControl = 3;
int incomingByte = 0;
int targetPressure = 20;


const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {8, 9, 10, 11}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init();
  lcd.clear();         
  lcd.backlight(); 

  pinMode(solenoidControl, OUTPUT);

  closeSolenoid();
  attachInterrupt(digitalPinToInterrupt(interruptPin), coinIncrement, FALLING);
  
  constantScreen();
}

void loop() {
  // put your main code here, to run repeatedly:

  readPressure();
  printToLCD(0,1, "Pressure: "+(String)pressureValue + " psi");
  printToLCD(0,2, "Coins: "+(String)coinInserted);


  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);

    if(incomingByte == 49){
      openSolenoid();  
    }

    if(incomingByte == 50){
      closeSolenoid();
    }
  }


  char key = keypad.getKey();

  if (key){
    Serial.println(key);
  }

  
  delay(250);
  
}


void constantScreen(){
  printToLCD(0,0, "Air Vendo Machine");
  printToLCD(0,1, "Pressure: "+(String)pressureValue + " psi");
  printToLCD(0,2, "Coins: "+(String)coinInserted);
  printToLCD(0,3, "Target PSI: "+(String)targetPressure);
}

void printToLCD(int x, int y, String text){
  lcd.setCursor(x,y);
  lcd.print(text);
}

void coinIncrement(){
  coinInserted++;
}

void readPressure(){
    pressureValue = analogRead(pressureInput); //reads value from input pin and assigns to variable
    pressureValue = ((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
}

void openSolenoid(){
  digitalWrite(solenoidControl, LOW);  
}

void closeSolenoid(){
  digitalWrite(solenoidControl, HIGH);  
}
