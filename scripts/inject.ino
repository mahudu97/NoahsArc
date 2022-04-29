/*
 * Can either run in Arduino Serial monitor or through
 * the script stepper.py
 * 
 * this script is inspired by 
 * https://learn.adafruit.com/adafruit-arduino-lesson-16-stepper-motors/arduino-code.
 *
 *
 * Currently, this works with an arudino uno (https://store.arduino.cc/products/arduino-uno-rev3/)
 * and controls a single stepper motor (specifically 28BYJ-48 5V Stepper Motor + ULN2003 Motor Driver Board)
 * via the pins below.
 *
 * This motor controls a syringe pump, the best way to describe it is through a video here:
 * https://drive.google.com/file/d/1NBVGI3rdMyDDZzPux7d7JAyAq1FNVAgx/view?usp=sharing
 *
 * We would like to control more syringe pumps from the same arduino, ie connect multiple stepper motors to a single arduino
 * and be able to control them independently in the same way as the current one is.
 * 
 * To anyone looking at this willing to help out, thank you!
*/

#include <Stepper.h>
#include <array>

class StepperMotor() : public Stepper
{
public:
  using Pins = std::array<unsigned int, 4>;

  StepperMotor(unsigned int stepsPerRevolution, const Pins& inPins)
    : Stepper(stepsPerRevolution, inPins[0], inPins[1], inPins[2], inPins[3])
    , _inPins(inPins)
    , _stepsPerRevolution(stepsPerRevolution)
  {}

  void setupMotor(unsigned int speed)
  {
    pinMode(_inPins[0], OUTPUT);
    pinMode(_inPins[1], OUTPUT);
    pinMode(_inPins[2], OUTPUT);
    pinMode(_inPins[3], OUTPUT);

    setSpeed(speed);
  }

private:
  Pins _inPins;
  unsigned int _stepsPerRevolution;
};


// check order (as of 5.2.22 it was 2,3,5,4) - however it seems 2,5,3,4 is working
// (cable dependent obviously, and with opposite permutation giving opposite motion)
StepperMotor motor(512, {2, 5, 3, 4});

/* Another motor is made like (using pins 6,7,8,9 and the same 512 steps per revolution):
StepperMotor anotherMotor(512, {6, 7, 8, 9});
*/

/**
 * Matches a motor number to one of the above motors.
 * @return a pointer to the motor if found, otherwise nullptr.
 */
StepperMotor* SelectMotor(unsigned int motorNumber)
{
  if (motorNumber == 1)
  {
    return &motor;
  }
  else if (motorNumber == 2)
  {
    // return &anotherMotor;
    return nullptr; // change this to the above line if anotherMotor is made above.
  }
  // else if (motorNumber == 3)
  // {
  //   return &yetAnotherMotor;
  // }
  else
  {
    // always have this final else
    return nullptr; // did not find motor
  }
}

void setup()
{
  // this line is for Leonardo's, it delays the serial interface
  // until the terminal window is opened
  //while (!Serial);
  Serial.begin(9600);

  // might need to adjust this if not working
  // 10 seems like a reasonable (and working) default value; will need to
  // check flow rates etc.
  // NB 5-20 seems like a working range
  motor.setupMotor(5);

  // anotherMotor.setupMotor(10);
}

void loop()
{
  if (Serial.available())
  {
    Serial.print("Enter motor number:");
    int motorNumber = Serial.parseInt();

    StepperMotor* motor = SelectMotor(motorNumber);
    if (motor == nullptr)
    {
      Serial.print("Did not find motor for motor number entered");
      return;
    }

    Serial.print("Enter number of steps to turn the motor:");
    int steps = Serial.parseInt();

    Serial.print("Moving this many steps: ");
    Serial.println(steps);

    motor->step(steps);

    Serial.print("Motor moved by this many steps: ");
    Serial.println(steps);
  }
}
