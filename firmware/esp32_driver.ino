#include <WiFi.h>

const char *SSID = "wifi-network";
const char *PASS = "wifi-pass";

const IPAddress SERVER = IPAddress(0, 0, 0, 0);
const int PORT = 3131;

const unsigned char NIL = 0;

const unsigned char FLAG_ACK = 1;
const unsigned char FLAG_CMD = 2;
const unsigned char FLAG_RPT = 3;

const unsigned char ACTION_MDI = 1;
const unsigned char ACTION_MSP = 2;

const unsigned char MOTOR_BAK = 64;
const unsigned char MOTOR_FWD = 128;
const unsigned char MOTOR_MDR = 192;
const unsigned char MOTOR_MID = 63;

const unsigned char REPORT_MEN = 1;
const unsigned char REPORT_MSP = 2;

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
        this->motorDisable(this->fwd, this->bak, this->pwm);
    }

    void enable(char dir, char speed) {
        if (speed <= this->stl) {
            this->motorDisable(this->fwd, this->bak, this->pwm);
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
        this->motorEnable(this->fwd, this->bak, this->pwm, speed);
    }

    void backward(char speed) {
        this->motorEnable(this->bak, this->fwd, this->pwm, speed);
    }

    void motorEnable(int diron, int diroff, int pwm, char speed) {
        digitalWrite(diroff, LOW);
        digitalWrite(diron, HIGH);
        analogWrite(pwm, speed);
    }

    void motorDisable(int dira, int dirb, int pwm) {
        digitalWrite(dira, LOW);
        digitalWrite(dirb, LOW);
        digitalWrite(pwm, LOW);
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
        case NIL:
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
            case NIL:
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