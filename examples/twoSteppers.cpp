#include <Arduino.h>

// Include for Register Enums
#include <imxrt.h>

#define HARDCODE

#define STEPPER_PUL 28
#define STEPPER_DIR 29

/* ---------------------------------- STEPPER STUFF ---------------------------------- */
#define STEPS_PER_REV_1 200
#define STEPS_PER_REV_2 200

constexpr float stepsPerDeg1 = STEPS_PER_REV_1 / 360.0f;
constexpr float stepsPerDeg2 = STEPS_PER_REV_2 / 360.0f;

int targetAngle1, targetAngle2;
int currentAngle1, currentAngle2;

int totalSteps1, totalSteps2;
int currentStep1, currentStep2;

bool stepDir1 = 1, stepDir2 = 1;

bool step1 = 0, step2 = 0;

bool moving1 = false, moving2 = false;

// Service routine for writing pulses to the driver
void ISR_STEP();
// Run once at the end of setup, configures GPT
void GPT_CONFIG(uint32_t us);

// Move to specified angle, call in loop whenever moving required.
void moveToAngle(float angle1, float angle2);
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
    targetAngle1 = 90;
    targetAngle2 = 45;
    Serial.printf("Angle1: %d, Angle2: %d\n", targetAngle1, targetAngle2);
    moveToAngle(targetAngle1, targetAngle2);

    delay(2000);

    targetAngle1 = -90;
    targetAngle2 = -45;
    Serial.printf("Angle1: %d, Angle2: %d\n", targetAngle1, targetAngle2);
    moveToAngle(targetAngle1, targetAngle2);

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
    GPT1_SR |= GPT_SR_OF1;

    if (!moving1 && !moving2)
        return;

    if (moving1) {
        step1 = !step1;
        digitalWrite(STEPPER_PUL, step1);
        currentStep1++;

        if (currentStep1 >= (totalSteps1 * 2)) {
            currentStep1 = 0;
            moving1 = 0;
        }
    }

    if (moving2) {
        step2 = !step2;
        digitalWrite(STEPPER_PUL, step2);
        currentStep2++;

        if (currentStep2 >= (totalSteps2 * 2)) {
            currentStep2 = 0;
            moving2 = 0;
        }
    }
}

void moveToAngle(float angle1, float angle2)
{
    // Below lines were for testing purposes UwU
    // angle1 = constrain(angle1, -90, 90);
    // angle2 = constrain(angle2, -90, 90);

    // steps to move = delta(angle) * steps per deg
    totalSteps1 = abs((angle1 - currentAngle1) * stepsPerDeg1);
    totalSteps2 = abs((angle2 - currentAngle2) * stepsPerDeg2);

    // direction determine
    stepDir1 = angle1 > currentAngle1 ? 1 : 0;
    stepDir2 = angle2 > currentAngle2 ? 1 : 0;

    // write direction beforehand
    digitalWrite(STEPPER_DIR, stepDir1);
    digitalWrite(STEPPER_DIR, stepDir2);

    // reset step counter to keep track of steps moved in current target
    currentStep1 = 0;
    currentStep2 = 0;
    currentAngle1 = angle1; // update for future purposes.
    currentAngle2 = angle2; // update for future purposes.

    // After direction & totalSteps are determined, then set moving to true else won't behave accurately.
    moving1 = true;
    moving2 = true;
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
