// #include <Arduino.h>
// #include <imxrt.h>

// #define LATCH 15
// #define CLK 19
// #define DATA 18

// byte solenoidTracker = 0b11111111;
// byte rsolenoidTracker = solenoidTracker;

// volatile int ticks;

// void ISR_SIPO()
// {
//     GPT1_SR |= GPT_SR_OF3; // clear all set bits
//     digitalWrite(CLK, HIGH);
//     if (ticks < 8) {
//         digitalWrite(DATA, rsolenoidTracker & 0x01);
//         rsolenoidTracker = rsolenoidTracker >> 1;
//     } else {
//         digitalWrite(LATCH, HIGH);
//         digitalWrite(LATCH, LOW);
//         rsolenoidTracker = solenoidTracker;
//         ticks = -1;
//     }
//     digitalWrite(CLK, LOW);
//     ticks++;
//     while (GPT1_SR & GPT_SR_OF1)
//         ; // wait for clear
// }

// void gpt1_init(uint32_t us)
// {
//     CCM_CCGR1 |= CCM_CCGR1_GPT(CCM_CCGR_ON); // enable GPT1 module
//     GPT1_CR = 0;
//     GPT1_PR = 23; // prescale+1
//     GPT1_OCR1 = us - 1; // compare
//     GPT1_SR = 0x3F; // clear all prior status
//     GPT1_IR = GPT_IR_OF1IE;
//     GPT1_CR = GPT_CR_EN | GPT_CR_CLKSRC(1); // 1 ipg 24mhz  4 32khz
//     attachInterruptVector(IRQ_GPT1, ISR_SIPO);
//     NVIC_ENABLE_IRQ(IRQ_GPT1);
// }

// void setup()
// {
//     Serial.begin(115200);
//     pinMode(LED_BUILTIN, OUTPUT);
//     digitalWrite(LED_BUILTIN, HIGH);
//     gpt1_init(10);
//     Serial.println("Booting...");
//     // pinMode(VCC, OUTPUT);
//     pinMode(CLK, OUTPUT);
//     pinMode(LATCH, OUTPUT);
//     pinMode(DATA, OUTPUT);
//     // digitalWrite(VCC, 1);
// }

// void loop()
// {
//     Serial.println(ticks);
//     solenoidTracker = ~solenoidTracker;
//     delay(1000); // Otherwise Serial would overflow at least for my system
// }
