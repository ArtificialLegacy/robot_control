#include <WiFi.h>

#include "secrets.h"
#include "proto.h"

float ease(float x) {
    return -(cos(3.141592 * x) - 1) / 2;
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class Motor {
public:
    int stall;
    int easefactor;

    Motor(int pwm, int fwd, int bak, int stall, int easefactor) {
        this->pwm = pwm;
        this->fwd = fwd;
        this->bak = bak;
        this->stall = stall;
        this->easefactor = easefactor;
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
        Serial.println(this->currentSpeed);
        this->targetSpeed = speed;
        if (dir == MOTOR_BAK) this->targetSpeed *= -1;
    }

    void update() {
        Serial.println(this->currentSpeed);

        if (this->targetSpeed == this->currentSpeed) {
            this->acc = 0;
            return;
        }

        if (abs(this->currentSpeed) + this->acc >= abs(this->targetSpeed)) {
            this->currentSpeed = this->targetSpeed;
        }

        this->currentSpeed += ease(this->acc) * sgn(this->currentSpeed) * this->easefactor;
        if (this->acc < 1) this->acc += 0.05;

        if (this->currentSpeed == 0) {
            this->motorDisable();
        } else if (this->currentSpeed > 0) {
            this->motorEnable(this->fwd, this->bak, this->currentSpeed);
        } else if (this->currentSpeed < 0) {
            this->motorEnable(this->bak, this->fwd, abs(this->currentSpeed));
        }
    }

private:
    int pwm;
    int fwd;
    int bak;

    int currentSpeed;
    int targetSpeed;
    float acc;

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
    { 27, 16, 17, 50, 1 },
    { 26, 18, 19, 50, 1 }
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

    for (int i = 0; i < largestMotor; i++) {
        motors[i].update();
    }
}

void processSignal(int len) {
    char* packet = new char[len];

    for (int i = 0; i < len; i++) {
        packet[i] = client.read();
    }

    Serial.println("signal");

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

    Serial.println("cmd");

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
    int motorDir = mtr & MOTOR_DIR;

    if (motorId > largestMotor) {
        Serial.print("invalid motor id: ");
        Serial.println(motorId);
        stopRobot();
    }

    Motor m = motors[motorId-1];
    m.enable(motorDir, speed);
}