
const int pwm = 13;
const int fwd = 12;
const int bak = 11;

const int FORWARD = 1;
const int BACKWARD = 2;

int sgn(float a)
{
    if (a < 0)
    {
        return -1;
    }

    return 1;
}

class Motor
{
public:
    Motor(int pwm, int fwd, int bak)
    {
        this->pwm = pwm;
        this->fwd = fwd;
        this->bak = bak;

        this->minspd = 40;
        this->maxspd = 255;
        this->threshold = 10;
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

        if (dir == BACKWARD)
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

Motor motor = {pwm, fwd, bak};

void setup()
{
    Serial.begin(9600);

    motor.init();
    motor.enable();
}

int dir = BACKWARD;
int speed = 0;

void loop()
{
    if (motor.atSpeed() && speed == 0)
    {
        speed = 255;
        if (dir == FORWARD)
        {
            dir = BACKWARD;
            motor.speedSet(dir, 255);
        }
        else
        {
            dir = FORWARD;
            motor.speedSet(dir, 255);
        }
    }
    else if (motor.atSpeed() && speed == 255)
    {
        speed = 0;
        motor.speedSet(dir, 0);
    }
    motor.update();
    // delay(100);
}