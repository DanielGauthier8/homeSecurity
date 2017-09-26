#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Password.h>


const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

int buzzer = 13;

Password password = Password( "0288" );
int going = true;
int pinLocation = 0;
bool result;
char key;
bool lockDown = true;
bool noise = true;
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
int mode = 2;
//modes 0 (sysOK), 1(enterPIN), 2 (system is armed),3  (alarm)

void setup()
{
  lcd.init(); //initialize the lcd
  lcd.backlight(); //turn on the backlight
  pinMode (buzzer, OUTPUT);
}

void loop()
{
  if (mode == 0) //Display System Ok message
  {
    lcd.setCursor(3, 0); // set the cursor
    lcd.print("AUTO MODE"); // Print a message to the LCD.
    lcd.setCursor(3, 1);
    lcd.print("SYSTEM OK");
    key = keypad.getKey();
    if (key) {
      lockDown = false;
      mode = 1;
      lcd.clear();
    }
  }
  else if (mode == 1)//Ask the user for the PIN
  {
    if (lockDown)
    {
      lcd.setCursor(0, 0); // set the cursor to column 15, line 0
      lcd.print("Please Enter Pin"); // Print a message to the LCD.
      result = enterPIN();
      if (result == true)
      {
        mode = 0;
        lcd.clear();
      }
      else if (result == false)
      {
        mode = 3;
        lcd.clear();
      }
    }
    else
    {
      lcd.setCursor(1, 0); // set the cursor
      lcd.print("PRESS # TO ARM"); // Print a message to the LCD.
      key = keypad.getKey();
      if (key == '#')
      {
        mode = 2;
        lockDown = true;
        lcd.clear();
      }
      else if (key == '*')
      {
        mode = 0;
        lcd.clear();
      }
    }
  }
  else if (mode == 2)//The system is armed
  {
    lcd.setCursor(3, 0); // set the cursor
    lcd.print("AUTO MODE "); // Print a message to the LCD.
    lcd.setCursor(1, 1);
    lcd.print("SYSTEM IS ARMED");

    key = keypad.getKey();
    if (key) {
      mode = 1;
      lcd.clear();
    }

  }
  else if (mode == 3)
  {
    alarm();
  }

  if (arm())
  {
    mode  = 2;
    lcd.clear();
  }
}


boolean enterPIN()
{
  password.reset();
  going = true;
  while (going) {
    key = keypad.getKey();
    if (key)
    {
      if (key == '#')
      {
        pinLocation = 0;
        going = false;
      }
      else
      {
        lcd.setCursor(pinLocation, 1);
        password.append(key);
        lcd.print("*");
        pinLocation++;
      }
    }
  }
  if (password.evaluate())
  {
    password.reset();
    return true;
  }
  else
  {
    password.reset();
    return false;
  }
}

void alarm()
{
  lcd.setCursor(1, 1); // set the cursor
  lcd.print("ALARM ACTIVATED"); // Print a message to the LCD.
  lcd.setCursor(3, 0);
  lcd.print("ATTENTION!");
  unsigned char i, j ;// define variables
  for (j = 0; j < 10; j++)
  {
    for (i = 0; i < 80; i++)
    {
      digitalWrite (buzzer, LOW) ; // Turn buzzer ON
      delay (1) ;// Delay 1ms
      digitalWrite (buzzer, HIGH) ;// turn buzzer OFF
      delay (1) ;// delay ms
    }
    for (i = 0; i < 100; i++) // new frequency
    {
      digitalWrite (buzzer, LOW) ;// turn buzzer ON
      delay (2) ;// delay 2ms
      digitalWrite (buzzer, HIGH) ;// turn buzzer OFF
      delay (2) ;// delay 2ms
    }
  }
  password.reset();
  mode = 2;
}

bool arm()
{
  return false;
}
