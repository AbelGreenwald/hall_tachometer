#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <LedControl.h>
#include <TimerOne.h>

#define LCD_ADDR 0x3F

const int sensorPin = 3;
const long sample_length = 1000000; //us

volatile unsigned long sensorCount;
volatile unsigned int rpm;
volatile boolean updated;
unsigned long curMillis;
unsigned long oldMillis;
unsigned long total_count;

LiquidCrystal_I2C lcd(LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void rev() {
    sensorCount++;
    total_count++;
}

void timerIsr() {
    // Toggle LED
    digitalWrite( 13, digitalRead( 13 ) ^ 0x1 );
    // sensorCount (rev) * 1000 ms * 60 sec = sensorCount * 60000 rev
    // duration (ms)       1   sec   1  min                       min
    curMillis = millis();
    rpm = ( sensorCount * 60000 ) / (curMillis - oldMillis) ; //revs per minute (from us)
    updated = true;
    sensorCount = 0;
    oldMillis = curMillis;
}

void setup() {
    //globally disable inturrupts for setup
    cli();

    pinMode(13, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(sensorPin, INPUT);
    Timer1.initialize(sample_length);
    // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
    Timer1.attachInterrupt( timerIsr );
    wdt_enable(WDTO_2S);
    attachInterrupt(digitalPinToInterrupt(sensorPin), rev, FALLING);
    Serial.begin(115200);
    digitalWrite(4, HIGH);
    // enable inturrupts, Here we go!
    sei();
    lcd.begin(16,2); // initialize the lcd
    lcd.home (); // go home
    lcd.setCursor (0, 0 );
    lcd.print("Hello User!");
    lcd.setCursor (0, 1 );
    lcd.print("Starting up...");
    delay(750);

}

void loop() {

    if (updated) 
    {
        lcd.setCursor (0, 0 );
        lcd.print("RPM:   ");
        lcd.print(rpm);
        lcd.print("        ");
    }
        lcd.setCursor (0,1);
        lcd.print("Count: ");
        lcd.print(total_count);
        lcd.print("        ");

wdt_reset();  // Reset WDT timer 
}