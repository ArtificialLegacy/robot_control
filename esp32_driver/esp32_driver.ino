#include <WiFi.h>

#include "secrets.h"
#include "proto.h"

class Motor {
public:
    Motor(int pwm, int fwd, int bak, int stl) {
        this->pwm = pwm;
        this->fwd = fwd;
        this->bak = bak;
        this->stl = stl;
    }

    void init() {
        pinMode(this->pwm, OUTPUT);
        pinMode(this->fwd, OUTPUT);
        pinMode(this->bak, OUTPUT);
    }

    void disable() {
        this->motorDisable();
    }

    void enable(char dir, char speed) {
        if (speed <= this->stl) {
            this->motorDisable();
            return;
        }

        switch (dir) {
        case MOTOR_FWD:
            this->forward(speed);
            break;
        case MOTOR_BAK:
            this->backward(speed);
            break;
        }
    }

private:
    int pwm;
    int fwd;
    int bak;
    int stl;

    void forward(char speed) {
        this->motorEnable(this->fwd, this->bak, speed);
    }

    void backward(char speed) {
        this->motorEnable(this->bak, this->fwd, speed);
    }

    void motorEnable(int diron, int diroff, char speed) {
        digitalWrite(diroff, LOW);
        digitalWrite(diron, HIGH);
        analogWrite(this->pwm, speed);
    }

    void motorDisable() {
        digitalWrite(this->fwd, LOW);
        digitalWrite(this->bak, LOW);
        digitalWrite(this->pwm, LOW);
    }
};

const char largestMotor = 2;
Motor motors[2] = {
    { 27, 16, 17, 50 },
    { 26, 18, 19, 50 }
};

WiFiClient client;

void setup() {
    Serial.begin(115200);
    WiFi.begin(SSID, PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("...");
    }

    Serial.print("WiFi connected with IP:");
    Serial.println(WiFi.localIP());

    while (!client.connect(SERVER, PORT)) {
        Serial.println("Connection to host failed");
        delay(1000);
    }

    client.write(FLAG_ACK);
    while (!client.available()) {
        Serial.println("waiting for ack...");
    }

    char response = client.read();
    if (response != FLAG_ACK) {
        Serial.println("ack failed");
        stopRobot();
    }
    client.flush();

    for (int i = 0; i < largestMotor; i++) {
        motors[i].init();
    }
}

void stopRobot() {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    for (int i = 0; i < largestMotor; i++) {
        motors[i].disable();
    }

    for(;;)
        ;
}

void loop() {
    int len = client.available();
    if (len > 0) {
        processSignal(len);
    }

    if (!client.connected()) {
        stopRobot();
    }
}

void processSignal(int len) {
    char* packet = new char[len];

    for (int i = 0; i < len; i++) {
        packet[i] = client.read();
    }

    switch (packet[0]) {
        case FLAG_NIL:
            Serial.println("recieved nil response flag.");
            stopRobot();
            break;
        case FLAG_CMD:
            processCMD(packet);
            break;
    }

    client.write(FLAG_ACK);
    client.flush();
    delete[] packet;
}

void processCMD(char* packet) {
    int len = packet[1];

    for (int i = 0; i < len; i++) {
        int pos = i+2;
        switch (packet[pos]) {
            case ACTION_NIL:
                break;
            case ACTION_MSP:
                processActionMSP(packet[pos+1], packet[pos+2]);
                i += 2;
                break;
            default:
                Serial.print("unknown action: ");
                Serial.println(packet[pos]);
                stopRobot();
        }
    }
}

void processActionMSP(char mtr, char speed) {
    int motorId = mtr & MOTOR_MID;
    int motorDir = mtr & MOTOR_MDR;

    if (motorId > largestMotor) {
        Serial.print("invalid motor id: ");
        Serial.println(motorId);
        stopRobot();
    }

    Motor m = motors[motorId-1];
    m.enable(motorDir, speed);
}