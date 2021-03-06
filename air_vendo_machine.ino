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
int targetPressure = 0;
boolean askingPresureScreen = false;
String keyInput = "";
int requiredCoins = 0;


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
  lcd.init();
  lcd.clear();         
  lcd.backlight(); 

  pinMode(solenoidControl, OUTPUT);

  closeSolenoid();
  attachInterrupt(digitalPinToInterrupt(interruptPin), coinIncrement, RISING);
  
  constantScreen();
}

void loop() {
    // put your main code here, to run repeatedly:

    if(!askingPresureScreen){
      readPressure();
      printToLCD(0,1, "Pressure: "+(String)pressureValue + " psi ");
      printToLCD(0,2, "Coins: "+(String)coinInserted + "/" + (String)requiredCoins);
      printToLCD(0,3, "Target PSI: "+(String)targetPressure);

    }

    if(targetPressure > 0 ){
        if(pressureValue < targetPressure && coinInserted >= requiredCoins){
          openSolenoid();
        }
        
        if(pressureValue > targetPressure && pressureValue < (targetPressure + 5) ){
          closeSolenoid();
        }
    }
  

    //process keypad press
    char key = keypad.getKey();
    if (key){
      processKeyInput(key);     
    }

  
}


void processKeyInput(char key){
  switch(key){
    case 'A':
      askingPresureScreen = true;
      askTargetPressureScreen();
      break;
    case 'B':
      askingPresureScreen = false;
      setTargetPressure();
      break;
    case 'C':
      closeSolenoid();
      break;
    case 'D':
      openSolenoid();
      break;
    case '#':
      break;
    case '*':
      break;
    default:
      keyInput.concat(key);  
      askTargetPressureScreen();
      break;
  }
}

void setTargetPressure(){
  targetPressure = keyInput.toInt();
  requiredCoins = ceil( (targetPressure - pressureValue) / 10);
  keyInput = "";
  constantScreen();
}


void constantScreen(){
  printToLCD(0,0, "Air Vendo Machine");
  printToLCD(0,1, "Pressure: "+(String)pressureValue + " psi ");
  printToLCD(0,2, "Coins: "+(String)coinInserted);
  printToLCD(0,3, "Target PSI: "+(String)targetPressure);
}

void askTargetPressureScreen(){
  lcd.clear();
  printToLCD(0,0, "Please set");
  printToLCD(0,1, "target pressure...");
  printToLCD(0,2, (String)keyInput);
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
