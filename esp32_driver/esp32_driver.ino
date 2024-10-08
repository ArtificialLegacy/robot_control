#include <WiFi.h>

#include "secrets.h"
#include "proto.h"

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

class Motor
{
public:
    Motor(int pwm, int fwd, int bak, int minspd, int maxspd, float threshold)
    {
        this->pwm = pwm;
        this->fwd = fwd;
        this->bak = bak;

        this->minspd = minspd;
        this->maxspd = maxspd;
        this->threshold = threshold;
    }

    void init()
    {
        pinMode(this->pwm, OUTPUT);
        pinMode(this->fwd, OUTPUT);
        pinMode(this->bak, OUTPUT);
    }

    void enable()
    {
        this->enabled = true;
    }

    void disable()
    {
        this->enabled = false;
        this->prevSpeed = 0;
        this->startSpeed = 0;
        this->targetSpeed = 0;
        this->disableMotor();
    }

    void speedSet(int dir, int speed)
    {
        float target = speed;

        if (dir == MOTOR_BAK)
        {
            target *= -1;
        }

        this->targetSpeed = target;
        this->startSpeed = this->prevSpeed;
    }

    bool atSpeed()
    {
        return this->prevSpeed == this->targetSpeed;
    }

    void update()
    {
        if (this->atSpeed())
        {
            return;
        }

        if (this->prevSpeed == this->startSpeed)
        {
            if (this->targetSpeed > this->startSpeed)
            {
                this->prevSpeed = this->startSpeed + 1;
            }
            else
            {
                this->prevSpeed = this->startSpeed - 1;
            }
        }

        float p = 1 - (abs(this->prevSpeed - this->targetSpeed) / abs(this->targetSpeed - this->startSpeed));
        float mu = min(max((1 - cos(p * PI)) / 2, 0.003), 0.075);

        float speedSmoothed = (this->targetSpeed - this->prevSpeed) * mu + this->prevSpeed;
        this->prevSpeed = speedSmoothed;

        float range = this->maxspd - this->minspd;

        int speed = (speedSmoothed / 255 * range) + this->minspd * sgn(speedSmoothed);
        int speedTarget = (this->targetSpeed / 255 * range) + this->minspd * sgn(this->targetSpeed);

        if (this->targetSpeed == 0)
        {
            speedTarget *= sgn(speedSmoothed);
        }

        if (abs(speedTarget - speed) <= this->threshold)
        {
            speedSmoothed = this->targetSpeed;
            this->prevSpeed = speedSmoothed;
            speed = speedTarget;
        }
        speed = abs(speed);

        Serial.println(speed * sgn(speedSmoothed));

        if (speedSmoothed == 0)
        {
            this->disableMotor();
        }
        else if (speedSmoothed > 0)
        {
            this->enableMotor(this->fwd, this->bak, speed);
        }
        else
        {
            this->enableMotor(this->bak, this->fwd, speed);
        }
    }

private:
    int pwm;
    int fwd;
    int bak;

    bool enabled;

    float prevSpeed;
    float targetSpeed;
    float startSpeed;

    int minspd;
    int maxspd;
    float threshold;

    void disableMotor()
    {
        digitalWrite(this->pwm, LOW);
        digitalWrite(this->fwd, LOW);
        digitalWrite(this->bak, LOW);
    }

    void enableMotor(int diron, int diroff, int speed)
    {
        digitalWrite(diroff, LOW);
        digitalWrite(diron, HIGH);
        analogWrite(this->pwm, speed);
    }
};

const char largestMotor = 2;
Motor motors[2] = {
    {27, 16, 17, 50, 255, 10},
    {26, 18, 19, 50, 255, 10}};

WiFiClient client;

void setup()
{
    Serial.begin(115200);
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

    for (int i = 0; i < largestMotor; i++)
    {
        motors[i].init();
    }
}

void stopRobot()
{
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    for (int i = 0; i < largestMotor; i++)
    {
        motors[i].disable();
    }

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

    for (int i = 0; i < largestMotor; i++)
    {
        motors[i].update();
    }
}

void processSignal(int len)
{
    char *packet = new char[len];

    for (int i = 0; i < len; i++)
    {
        packet[i] = client.read();
    }

    Serial.println("signal");

    switch (packet[0])
    {
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

void processCMD(char *packet)
{
    int len = packet[1];

    Serial.println("cmd");

    for (int i = 0; i < len; i++)
    {
        int pos = i + 2;
        switch (packet[pos])
        {
        case ACTION_NIL:
            break;
        case ACTION_MSP:
            processActionMSP(packet[pos + 1], packet[pos + 2]);
            i += 2;
            break;
        default:
            Serial.print("unknown action: ");
            Serial.println(packet[pos]+0);
            stopRobot();
        }
    }
}

void processActionMSP(char mtr, char speed)
{
    int motorId = mtr & MOTOR_MID;
    int motorDir = mtr & MOTOR_DIR;

    if (motorId > largestMotor)
    {
        Serial.print("invalid motor id: ");
        Serial.println(motorId);
        stopRobot();
    }

    Motor * m = &motors[motorId - 1];
    m->speedSet(motorDir, speed);
}