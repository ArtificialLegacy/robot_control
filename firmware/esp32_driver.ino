#include <WiFi.h>

const int A_PWM = 27;
const int A_FWD = 16;
const int A_BAK = 17;
const int A_STL = 30;

const int B_PWM = 26;
const int B_FWD = 18;
const int B_BAK = 19;
const int B_STL = 30;

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

WiFiClient client;

void setup()
{
    Serial.begin(115200);

    pinMode(A_PWM, OUTPUT);
    pinMode(A_FWD, OUTPUT);
    pinMode(A_BAK, OUTPUT);

    pinMode(B_PWM, OUTPUT);
    pinMode(B_FWD, OUTPUT);
    pinMode(B_BAK, OUTPUT);

    WiFi.begin(SSID, PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("...");
    }

    Serial.print("WiFi connected with IP:");
    Serial.println(WiFi.localIP());

    while (!client.connect(SERVER, PORT))
    {
        Serial.println("Connection to host failed");
        delay(1000);
    }

    client.write(FLAG_ACK);
    while (!client.available())
    {
        Serial.println("waiting for ack...");
    }

    char response = client.read();
    if (response != FLAG_ACK)
    {
        Serial.println("ack failed");
        stopRobot();
    }
    client.flush();
}

void stopRobot()
{
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    motorDisable(A_FWD, A_BAK, A_PWM);
    motorDisable(B_FWD, B_BAK, B_PWM);

    for (;;)
        ;
}

void loop()
{
    int len = client.available();
    if (len > 0)
    {
        processSignal(len);
    }

    if (!client.connected())
    {
        stopRobot();
    }
}

void processSignal(int len)
{
    char *packet = new char[len];

    for (int i = 0; i < len; i++)
    {
        packet[i] = client.read();
    }

    switch (packet[0])
    {
    case NIL:
        Serial.println("recieved nil response flag.");
        stopRobot();
        break;
    case FLAG_CMD:
        Serial.println("recieved cmd response flag.");
        processCMD(packet);
        break;
    }

    client.write(FLAG_ACK);
    client.flush();
    delete[] packet;
}

void processCMD(char *packet)
{
    int len = packet[1];
    Serial.print("cmd len: ");
    Serial.println(len);

    for (int i = 0; i < len; i++)
    {
        int pos = i + 2;
        switch (packet[pos])
        {
        case NIL:
            break;
        case ACTION_MSP:
            processActionMSP(packet[pos + 1], packet[pos + 2]);
            i += 2;
            break;
        default:
            Serial.print("unknown action: ");
            Serial.println(packet[pos]);
            stopRobot();
        }
    }
}

void processActionMSP(char motor, char speed)
{
    int motorId = motor & MOTOR_MID;
    int motorDir = motor & MOTOR_MDR;

    if (motorId == 1)
    {
        if (speed < A_STL)
        {
            motorDisable(A_FWD, A_BAK, A_PWM);
            return;
        }

        if (motorDir == MOTOR_FWD)
        {
            motorEnable(A_FWD, A_BAK, A_PWM, speed);
        }
        else if (motorDir == MOTOR_BAK)
        {
            motorEnable(A_BAK, A_FWD, A_PWM, speed);
        }
    }
    else if (motorId == 2)
    {
        if (speed < B_STL)
        {
            motorDisable(B_FWD, B_BAK, B_PWM);
            return;
        }

        if (motorDir == MOTOR_FWD)
        {
            motorEnable(B_FWD, B_BAK, B_PWM, speed);
        }
        else if (motorDir == MOTOR_BAK)
        {
            motorEnable(B_BAK, B_FWD, B_PWM, speed);
        }
    }
}

void motorEnable(int diron, int diroff, int pwm, char speed)
{
    digitalWrite(diroff, LOW);
    digitalWrite(diron, HIGH);
    analogWrite(pwm, speed);
}

void motorDisable(int dira, int dirb, int pwm)
{
    digitalWrite(dira, LOW);
    digitalWrite(dirb, LOW);
    digitalWrite(pwm, LOW);
}