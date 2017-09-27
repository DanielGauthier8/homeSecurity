//Import libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Password.h>

//Global Variables:
//Resource: https://playground.arduino.cc/Main/KeypadTutorial
const byte ROWS = 4; //four rows
const byte COLS = 4; //chnaged to four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//the row pinouts of the keypad
byte rowPins[ROWS] = {2, 3, 4, 5};
//the column pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//The current password for the keypad
//Resource: https://playground.arduino.cc/Code/Password
Password password = Password( "ABCD" );
//Initialize the looping variable the looping while loops checking for keypad input
int going = true;
//The location of where the user is entering the pin
int pinLocation = 0;
//Declaring the variable to see if the PIN is T or F
bool result;
//The pin out of the buzzer
int buzzer = 13;
//Part of the key capture
char key;
//Start the program off armed
bool lockDown = true;
//Start system armed
int mode = 2;
//modes 0 (sysOK), 1(enterPIN), 2 (system is armed),3  (alarm)
// initialize the library with the numbers of the interface pins
//Resource: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup()
{
  //initialize the lcd
  lcd.init();
  //turn on the backlight
  lcd.backlight();
  //initialize the buzzer
  pinMode (buzzer, OUTPUT);
}

void loop()
{
  if (mode == 0) //Display System Ok message
  {
    lcd.setCursor(3, 0); // set the cursor
    lcd.print("AUTO MODE"); // Print a message to the LCD.
    lcd.setCursor(3, 1); // set the cursor
    lcd.print("SYSTEM OK"); // Print a message to the LCD.
    //Get a keypress
    key = keypad.getKey();
    if (key) {
      //Arming boolean
      lockDown = false;
      //PIN mode
      mode = 1;
      lcd.clear();
    }
  }
  else if (mode == 1)//Ask the user for the PIN
  {
    if (lockDown)
    {
      lcd.setCursor(0, 0); // set the cursor to column 0, line 0
      lcd.print("Please Enter Pin"); // Print a message to the LCD.
      //See if user entered the correct PIN
      result = enterPIN();
      //If correct then go to "System OK"
      if (result == true)
      {
        mode = 0;
        lcd.clear();
      }
      //Have the alarm go off
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
      //Start checking for keypad input
      key = keypad.getKey();
      if (key == '#')
      {
        //Arm the system
        mode = 2;
        lockDown = true;
        lcd.clear();
      }
      else if (key == '*')
      {
        //Go back
        mode = 0;
        lcd.clear();
      }
    }
  }
  else if (mode == 2)//The system is armed
  {
    lcd.setCursor(3, 0); // set the cursor
    lcd.print("AUTO MODE "); // Print a message to the LCD.
    lcd.setCursor(1, 1); // set the cursor
    lcd.print("SYSTEM IS ARMED");// Print a message to the LCD.
    //Start checking for key
    key = keypad.getKey();
    if (key) {
      //Go to PIN mode
      mode = 1;
      lcd.clear();
    }

  }
  else if (mode == 3)//SOUND THE ALARM
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
  //Resest the password entered
  password.reset();
  //start the loop
  going = true;
  while (going) {
    //Look for a # indicating the pin was fully entered
    key = keypad.getKey();
    if (key)
    {
      if (key == '#')
      {
        //Start entering *'s at the 0
        pinLocation = 0;
        going = false;
      }
      else
      {
        //Set the cursor to the pinLocation var
        lcd.setCursor(pinLocation, 1);
        //enter key
        password.append(key);
        //Print on screen
        lcd.print("*");
        //Move pin forward
        pinLocation++;
      }
    }
  }
  //Check if the password entered is correct
  if (password.evaluate())
  {
    //Correct
    password.reset();
    return true;
  }
  else
  {
    //Incorrect
    password.reset();
    return false;
  }
}

void alarm()
{
  lcd.setCursor(1, 1); // set the cursor
  lcd.print("ALARM ACTIVATED"); // Print a message to the LCD.
  lcd.setCursor(3, 0); // set the cursor
  lcd.print("ATTENTION!"); // Print a message to the LCD.
  ////////ˇˇˇˇˇˇˇˇˇˇˇˇˇCopied code from https://rydepier.wordpress.com/2015/05/24/active-buzzer-alarm/ˇˇˇˇˇˇˇˇˇˇ
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
//^^^^^^^^^^Copied code from https://rydepier.wordpress.com/2015/05/24/active-buzzer-alarm/^^^^^^^^^^^^^^^
bool arm()
{
  //Return false before implementation of connecting to ifttt
  return false;
}
