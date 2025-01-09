// #include <Arduino.h>
// #include <imxrt.h>

// #define STEPPER_A 19
// #define STEPPER_B 18

// void ISR_STEP();

// bool state = LOW;
// unsigned long ticks;

// void GPT_CONFIG(uint32_t us);

// void setup()
// {
//     Serial.begin(115200);
//     pinMode(13, OUTPUT);
//     // digitalWrite(13, HIGH);

//     GPT_CONFIG(100);
// }

// void loop()
// {
//     Serial.println(ticks);

//     // read angle
//     delay(1000);
// }

// void ISR_STEP()
// {
//     GPT1_SR |= GPT_SR_OF3; // clear all set bits
//     ticks++;
//     state = !state;
//     digitalWriteFast(13, state);
//     // write steps
// }

// void GPT_CONFIG(uint32_t us)
// {
//     /*
//     1. Set clock source for gpt1. This is done throught the CCM_CCGR1
//             (Clock Control Module _ Clock Gating Register 1)
//     This register controls the clock input to different modules on the processor.
//     It serves the primary purpose of power management, disabling clock signal to unnecessary
//     peripherals preventing them from toggling flip-flops.

//     There are two concerned bit sets for GPT here,
//     1. 20-21: GPT1 bus clock (used for register access and bus transactions)
//     2. 22-23: GPT1 serial clock (used for GPT timer functionality)
//     GPT needs atleast one of the clock to be operational. if both are off, GPT1 won't function.
//     */

//     // Set 20 and 21 bits to enable the bus clock can be done in following ways:
//     // 1. CCM_CCGR1 |= (1 << 20) | (1 << 21);

//     // 2. The bus() sets bits 20-21 in provided combination. On sets both of them, i.e. clock will be provided
//     // in run, wait and stop modes.
//     CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON);

//     /*
//     Now that our GPT has a clock to work with, lets configure the GPT1.
//     To configure, GPT needs to be off at first. Set the enable bit to 0 in order to do that in the
//     GPT1_CR (control register)
//     Shorthand to do that is to reset the whole register.
//     */
//     GPT1_CR = 0;

//     /*
//     Set clock source for the gpt:
//     Let the gpt know that ipg_clk is to be used.
//     */
//     // Use Peripheral Clock (ipg_clk)

//     /*
//     Prescaling:
//         Timers work on clocks. Clocks are basically crystal oscillators which are set at a specific frequency.
//         For the imxrt1062 processor, the external xtal is of 24MHz.
//         Now it is not possible or needed in our case for the timer-counter to count  to 24 * 10^6

//         Hence, we take the input clock frequency and divide it by a factor, say 24, to reduce the input clock
//         frequency. Now the resulting clock frequency is
//             24 MHz / 24 = 1 MHz

//     To prescale the input, we must write a number in GPT1_PR register. The actual prescaler value is one more
//     than the value in the PR register. Thus, we will put value 23 in PR register.
//     */
//     GPT1_PR = 23;

//     /*
//     Output Compare Register:
//         Timers work the following way:
//         1. You set a value in the compare register to be matched with.
//         2. When the counter reaches that value, the timer interrupt is triggered.

//     Now GPT 1 has three of these Output Compare Registers.
//     Hence, it can interrupt at three different intervals.

//     Additionaly, there are modes on a timer- free run or reset.
//     When in free run mode, the timer keeps counting till it reaches absolute max value of the reg and then
//     resets to 0.
//     When in Restart mode, the timer resets to 0 upon starting reaching the compare value.

//     Free - running or restart mode is configured by the FRR bit in the GPT1_CR. Since it is set to 0,
//     our timer is in restart mode.
//     */
//     GPT1_OCR1 = us - 1; // since counting starts from 0.

//     /*
//     Enable interrupt:
//         GPT_IR is responsible for handling the interrupts of GPT1.
//         There are two types of interrupts: Input capture and output compare.
//         Input capture interrupt occures when the input line to be monitored is in desired state.
//         When it is in that state, the contents of the timer/counter are copied into another register.

//         Output capture is discussed above.

//         We set the bit OF1IE since we want the interrupt to be invoked periodically, and not based on external trigger.
//             Over Flow 1 Interrupt Enable
//         enable the interrupt allied with OCR1.
//     */
//     GPT1_IR = GPT_IR_OF1IE;

//     /*
//     Status Register:
//         This register contains the interrupt flags.
//         To configure the GPT, we must clear all overflow flags by writing a 1 to them.
//     */
//     GPT1_SR = 0x03; // write 1 to OF1, OF2, OF3 to reset them.

//     /*
//     Finally, enable the GPT now that it is configured by setting the en bit.
//     */
//     GPT1_CR = GPT_CR_EN | GPT_CR_CLKSRC(1);

//     // Attach the interrupt to GPT1
//     attachInterruptVector(IRQ_GPT1, ISR_STEP);

//     // Enable the Interrupt Request
//     NVIC_ENABLE_IRQ(IRQ_GPT1);
// }
