//Connected to motors, reives information about motor position
const int feedbackPin1 = A4;
const int feedbackPin2 = A5;

const int PWMpin1 = 5;
const int PWMpin2 = 4;
const int PWMpin3 = 2;
const int PWMpin4 = 3;

const int outPin1 = 31;
const int outPin2 = 33;
const int outPin3 = 27;
const int outPin4 = 29;

//Describe the motor positions
int fbPosition1 = 0;
int fbPosition2 = 0;

int goalPosition = 0;

int c1, c2, c3, c4; // Sensors
int dc1, dc2u, dc2d;
int mindc1, mindc2u, mindc2d;
int k2 = 3; // zenith multiplier
int k1 = 2; //azimuth multiplier
int k3 = 0.2; //  zenith position multiplier , might be flipped
int c;
bool turn;
bool zturn1;
bool zturn2;
bool aturn1;
bool aturn2;
bool up;
bool down;
int i;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  fbPosition1 = analogRead(feedbackPin1); //azimuth
  fbPosition2 = analogRead(feedbackPin2); //zenith

  pinMode(PWMpin1, OUTPUT);
  pinMode(PWMpin2, OUTPUT);
  pinMode(PWMpin3, OUTPUT);
  pinMode(PWMpin4, OUTPUT);

  pinMode(outPin1, OUTPUT);
  pinMode(outPin2, OUTPUT);
  pinMode(outPin3, OUTPUT);
  pinMode(outPin4, OUTPUT);

  digitalWrite(PWMpin1, LOW);
  digitalWrite(PWMpin2, LOW);
  digitalWrite(PWMpin3, LOW);
  digitalWrite(PWMpin4, LOW);

  ///Pins connected to the sensors, the resolution in 10 bits
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  c = 0;
  i = 0;
}

void loop() {
  c = c + 1;

  /////////// Find sensor values, turn --> true if the opposing sensors are not equal

  c1 = analogRead(A0);
  //  c1 = map(c1, 0, 1023, 0, 127);
  c2 = analogRead(A1);
  // c2 = map(c2, 0, 1023, 0, 127);
  c3 = analogRead(A2);
  // c3 = map(c3, 0, 1023, 0, 127);
  c4 = analogRead(A3);
  //c4 = map(c4, 0, 1023, 0, 127);
  fbPosition1 = analogRead(feedbackPin1); //azimuth
  fbPosition2 = analogRead(feedbackPin2); //zenith
  c3 = c3 + 4;
  turn = abs(c1 - c2) != 0 || abs(c3 - c4) != 0;


  //////////////////Every 350 cycle (0.1ms?) print each sensor value//////////////
  if (c == 350)
  {
    Serial.println("Start Printing");
    Serial.println(fbPosition1);
    Serial.println(fbPosition2);
    Serial.println(c1);
    Serial.println(c2);
    Serial.println(c3);
    Serial.println(c4);
    Serial.println("End Printing");
    c = 0;
  }
  i = 0;
  while (turn)
  {
    i = i + 1;
    if (c1 == c2)
    {
      analogWrite(PWMpin1, LOW);
      analogWrite(PWMpin2, LOW);
    }
    if (c3 == c4)
    {
      analogWrite(PWMpin3, LOW);
      analogWrite(PWMpin4, LOW);
    }
    fbPosition1 = analogRead(feedbackPin1); //azimuth
    fbPosition2 = analogRead(feedbackPin2); //zenith


    zturn1 = true;
    zturn2 = true;
    aturn1 = true;
    aturn2 = true;
    setDC();
    limitCheck();
    if (c4 > c3 && zturn1 == true)
    {
      moveUp();
    }
    else if (c4 < c3 && zturn2 == true)
    {
      moveDown();
    }


    if (c1 > c2 && aturn1 == true)
    {
      moveAntiClockwise();
    }
    else if (c1 < c2 && aturn2 == true)
    {
      moveClockwise();
    }


    c1 = analogRead(A0);
    //c1 = map(c1, 0, 1023, 0, 127);
    c2 = analogRead(A1);
    //c2 = map(c2, 0, 1023, 0, 127);
    c3 = analogRead(A2);
    //c3 = map(c3, 0, 1023, 0, 127);
    c4 = analogRead(A3);
    //c4 = map(c4, 0, 1023, 0, 127);
    c3 = c3 + 4;

    if (c1 == c2)
    {
      analogWrite(PWMpin1, LOW);
      analogWrite(PWMpin2, LOW);
    }
    if (c3 == c4)
    {
      analogWrite(PWMpin3, LOW);
      analogWrite(PWMpin4, LOW);
    }
    //c3 = c3 - 20;
    turn = abs(c1 - c2) != 0 || abs(c3 - c4) != 0;

    if (i == 100)
    {
      Serial.println("StartP");
      Serial.println(fbPosition1);
      Serial.println(fbPosition2);
      Serial.println(c1);
      Serial.println(c2);
      Serial.println(c3);
      Serial.println(c4);
      Serial.println("EndP");
      i = 0;
    }
    //  delay(50);
  }

  i = 0;
  switchOff();


}

void setDC()
{
  mindc2u = 2080;//130;
  mindc1 = 960;//65;
  mindc2d = 1600;//100;

  dc1 = mindc1 + k1 * abs(c3 - c4); // azimuth but shouldn't it look at c1 and c2
  dc2u = mindc2u + k2 * abs(c1 - c2) + k3 * fbPosition2; // power to go up (zenith)
  dc2d = mindc2d + k2 * abs(c1 - c2); //

  Serial.println("MotorValues");
  Serial.println(dc1);
  Serial.println(dc2u);
  Serial.println(dc2d);
  Serial.println("EndMotorValues");

}
void limitCheck()
{
  if (fbPosition2 > 170 )
  {
    analogWrite(PWMpin3, LOW);
    analogWrite(PWMpin4, LOW);
    zturn1 = false;

  }

  if (fbPosition2 < 70)
  {
    digitalWrite(outPin3, HIGH);
    digitalWrite(outPin4, LOW);
    analogWrite(PWMpin3, LOW);
    analogWrite(PWMpin4, LOW);
    zturn2 = false;
  }

  if (fbPosition1 < 100 )
  {
    analogWrite(PWMpin1, LOW);
    analogWrite(PWMpin2, LOW);
    aturn2 = false;
  }

  if ( fbPosition1 > 630)
  {
    analogWrite(PWMpin1, LOW);
    analogWrite(PWMpin2, LOW);
    aturn1 = false;
  }

}

void moveUp()
{


  //move up (extend)

  digitalWrite(outPin3, HIGH);
  digitalWrite(outPin4, LOW);
  analogWriteResolution(12);
  analogWrite(PWMpin3, dc2u);
  analogWriteResolution(12);
  analogWrite(PWMpin4, dc2u);
  // Serial.println("Moving Up");

}


void moveDown()
{
  //move down (retract)
  digitalWrite(outPin3, LOW);
  digitalWrite(outPin4, HIGH);
  analogWriteResolution(12);
  analogWrite(PWMpin3, dc2d);
  analogWriteResolution(12);
  analogWrite(PWMpin4, dc2d);
  //Serial.println("Moving Down");

}

void moveAntiClockwise()
{
  //move anticlockwise (extend)
  digitalWrite(outPin1, HIGH);
  digitalWrite(outPin2, LOW);
  analogWriteResolution(12);
  analogWrite(PWMpin1, dc1);
  analogWriteResolution(12);
  analogWrite(PWMpin2, dc1);
  // Serial.println("AntiClock");

}

void moveClockwise()
{
  //move clockwise(retract)
  digitalWrite(outPin1, LOW);
  digitalWrite(outPin2, HIGH);
  analogWriteResolution(12);
  analogWrite(PWMpin1, dc1);
  analogWriteResolution(12);
  analogWrite(PWMpin2, dc1);
  // Serial.println("Clock");
}

void switchOff()
{
  digitalWrite(outPin1, LOW);
  digitalWrite(outPin2, LOW);
  digitalWrite(PWMpin1, LOW);
  digitalWrite(PWMpin2, LOW);
  digitalWrite(outPin3, LOW);
  digitalWrite(outPin4, LOW);
  digitalWrite(PWMpin3, LOW);
  digitalWrite(PWMpin4, LOW);
  //delay(2000);
}


