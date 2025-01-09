#include <Arduino.h>

// Include for Register Enums
#include <imxrt.h>

#define HARDCODE

#define STEPPER_PUL 28
#define STEPPER_DIR 29

/* ---------------------------------- STEPPER STUFF ---------------------------------- */
// Refer your motor driver for confirmation. Single step mode is 200.
#define STEPS_PER_REV 200

// 200 steps per revolution / 360 degrees per revolution = 0.5556 steps per degree
constexpr float stepsPerDeg = STEPS_PER_REV / 360.0f; // evaluate at compile time

// Angle to move to
int targetAngle;
// Temp Angle
int currentAngle;

// Total steps required to move to specified angle
int totalSteps;
// Counter for ISR
int currentStep;

// Direction flag for motor
bool stepDir = 1;

// pulse variable
bool step = 0;

// Flag to determine when to stop movement.
bool moving = false;

// Service routine for writing pulses to the driver
void ISR_STEP();
// Run once at the end of setup, configures GPT
void GPT_CONFIG(uint32_t us);

// Move to specified angle, call in loop whenever moving required.
void moveToAngle(float angle);
/* ---------------------------------------------------------------------------------------------------- */

void setup()
{
    Serial.begin(115200);
    Serial.println("Enter Angle to move to: ");

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    pinMode(STEPPER_DIR, OUTPUT);
    pinMode(STEPPER_PUL, OUTPUT);

    digitalWrite(STEPPER_DIR, HIGH);

    // Keep your serial prints before this line, otherwise uploading fucks up somehow -_-
    GPT_CONFIG(350); // argument is the interrupt interval us
    delay(1000);
}

void loop()
{
#ifdef HARDCODE
    targetAngle = 90;
    Serial.printf("Moving to Angle: %d\n", targetAngle);
    moveToAngle(targetAngle);

    delay(2000);

    targetAngle = -90;
    Serial.printf("Moving to Angle: %d\n", targetAngle);
    moveToAngle(targetAngle);

    delay(2000);
#endif
}
/* ---------------------------------------------------------------------------------------------------- */

void ISR_STEP()
{
    /*
       Service routine called every overflow of the GPT1.
       Writes a pulse to stepper pulse pin and stops once designated angle is reached.
       totalSteps are number of steps needed to reach the target angle.
    */

    // This must be done at the start of routine to tell the timer to start counting again.
    // if other returns are performed before this, execution fucks up cuz interrupt not served, OF still up.
    GPT1_SR |= GPT_SR_OF3;

    if (!moving)
        return;

    step = !step;
    digitalWrite(STEPPER_PUL, step);
    currentStep++;

    if (currentStep >= (totalSteps * 2)) {
        currentStep = 0;
        moving = 0;
        return;
    }
}

void moveToAngle(float angle)
{
    // Below line was for testing purposes UwU
    // angle = constrain(angle, -90, 90);

    // steps to move = delta(angle) * steps per deg
    totalSteps = abs((angle - currentAngle) * stepsPerDeg);

    // direction determine
    stepDir = angle > currentAngle ? 1 : 0;

    // write direction beforehand
    digitalWrite(STEPPER_DIR, stepDir);

    // reset step counter to keep track of steps moved in current target
    currentStep = 0;
    currentAngle = angle; // update for future purposes.

    // After direction & totalSteps are determined, then set moving to true else won't behave accurately.
    moving = true;
}

void GPT_CONFIG(uint32_t us)
{
    CCM_CCGR1 |= CCM_CCGR1_GPT(CCM_CCGR_ON);

    GPT1_CR = 0; // turn off GPT1 timer
    GPT1_PR = 24 - 1; // prescale from 24 MHz to 1 MHz
    GPT1_OCR1 = us - 1; // interrupt interval in microseconds
    GPT1_IR = GPT_IR_OF1IE; // enable output compare 1
    GPT1_SR = 0x03; // clear status register
    GPT1_CR = GPT_CR_EN | GPT_CR_CLKSRC(1); // enable with ipg_clk (peripheral clock for general purpose timers)

    attachInterruptVector(IRQ_GPT1, ISR_STEP);
    NVIC_ENABLE_IRQ(IRQ_GPT1);
}
