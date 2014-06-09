#include <Arduino.h>
extern HardwareSerial Serial;

// Constants


const int accelSPEED = 3;
const int accelA = 2;
const int accelB = 4;

// steering pins
const int steerSPEED = 5;
const int steerA = 6;
const int steerB = 7;
const int posA = 10;
const int posB = 12;

const int ping = 9;



#define LEFT   B01
#define RIGHT  B10
#define CENTER B00

#define FORWARD  B10
#define BACKWARD B01

#define MAX_THROTTLE 4


// bit wise register
#define STATE_NOOP     B00000000
#define STATE_MOVE     B00000001
#define STATE_STEER    B00000010

//Global Variables
int iRobotStatus;
long lDuration;
int a, b;
int iThrottle = 0;

void getDistance() {
    pinMode(ping, OUTPUT);
    digitalWrite(ping, LOW);
    delayMicroseconds(2);
    digitalWrite(ping, HIGH);
    delayMicroseconds(5);
    digitalWrite(ping, LOW);

    // The same pin is used to read the signal from the PING))): a HIGH
    // pulse whose lDuration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    pinMode(ping, INPUT);
    lDuration = pulseIn(ping, HIGH);

}

void driveBreak() {
    digitalWrite(accelA, HIGH);
    digitalWrite(accelB, HIGH);
}

void driveForward() {
    digitalWrite(accelA, FORWARD >> 1);
    digitalWrite(accelB, FORWARD & 1);
}

void driveBackward() {
    digitalWrite(accelA, BACKWARD >> 1);
    digitalWrite(accelB, BACKWARD & 1);
}

void turnBreak() {
    digitalWrite(steerA, LOW);
    digitalWrite(steerB, LOW);
}

void turnRelease() {
    digitalWrite(steerA, HIGH);
    digitalWrite(steerB, HIGH);

}

void turnRight(int delays = 0) {
    digitalWrite(steerA, RIGHT >> 1);
    digitalWrite(steerB, RIGHT & 1);
    
    delay(delays);
}

void turnLeft(int delays = 0) {
    digitalWrite(steerA, LEFT >> 1);
    digitalWrite(steerB, LEFT & 1);
    
    delay(delays);
}

void report();

void center() {
    a = digitalRead(posA);
    b = digitalRead(posB);

    while (1) {
        
        if (a != b) {
            if (b)
                turnRight();
            else
                turnLeft();
        }

        if (a == b) {
            // just right!! Stop (ちょうどいいです、止る！）
            turnRelease();
            break;
        }
        report();
    }
}

long microsecondsToCentimeters(long microseconds) {
    // The speed of sound is 340 m/s or 29 microseconds per centimeter.
    // The ping travels out and back, so to find the distance of the
    // object we take half of the distance travelled.
    return microseconds / 29 / 2;
}

void report() {

    a = digitalRead(posA);
    b = digitalRead(posB);

    Serial.print("a: ");
    Serial.print(a);
    Serial.print(" b: ");
    Serial.print(b);
    Serial.print(" d: ");
    Serial.print(microsecondsToCentimeters(lDuration));
    Serial.print(" t: ");
    Serial.print(iThrottle);
    Serial.println();
}

void throttle(int i) {
    analogWrite(accelSPEED, map(i, 0, MAX_THROTTLE, 0, 255));
}

void tick() {
    getDistance();
    center();
}

void setup() {
    Serial.begin(9600);
    Serial.read(); // press key to start

    Serial.write("Start");

    //  servo.attach(2);
    pinMode(posA, INPUT);
    pinMode(posB, INPUT);

    pinMode(accelSPEED, OUTPUT);
    pinMode(accelA, OUTPUT);
    pinMode(accelB, OUTPUT);

    pinMode(steerSPEED, OUTPUT);
    pinMode(steerA, OUTPUT);
    pinMode(steerB, OUTPUT);

    // stop driving
    throttle(0);
    driveBreak();

    // half speed for steering
    analogWrite(steerSPEED, 0xFF);
    turnBreak();
    
    turnLeft(2000);
    turnRight(2000);
    center();


}

void loop() {
    report();
    delay(1000);
    center();
}