//Connected to motors, retrieves information about motor position
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

////////////offsets////////////

const int PosNegPin = 53;

const int offsetPin4 = 51;
const int offsetPin3 = 49;
const int offsetPin2 = 47;
const int offsetPin1 = 45;

const int c1offset = 30;
const int c2offset = 15;
const int c3offset = 90;
const int c4offset = 10;

// 0< accuracy < inf . Lower is more accurate. 
const int accuracy = 5;

int offsetHolder1  ;
int offsetHolder2  ;
int offsetHolder3  ;
int offsetHolder4  ;

int PosNegHolder ;
int sensorOffset4 = 0;
int sensorOffset3 = 0;
int sensorOffset2 = 0;
int sensorOffset1 = 0;


//Describe the motor positions
int fbPosition1 = 0; //100 <fbPosition1< 630
int fbPosition2 = 0; // 70 <fbPosition2< 170

int c1, c2, c3, c4; // Sensors

/*
sensor position reference
                  c4
     |-----------------------------|
     |                             |
     |                             |
     |                             |
c2   |         front plane         |  c1
     |                             |
     |                             |
     |-----------------------------|
                  c3


                  ground
----------------------------------------------
*/

int dc1, dc2u, dc2d;

int offset_trial = 0;


int mindc1, mindc2u, mindc2d;
int k2 = 8; // zenith multiplier
int k1 = 4; //azimuth multiplier
int k3 = 1; //  zenith position multiplier , might be flipped
int c; ////turn direction indicator variables
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

  pinMode(offsetPin4, INPUT);
  digitalWrite(offsetPin4, LOW);
  pinMode(offsetPin3, INPUT);
  digitalWrite(offsetPin3, LOW);
  pinMode(offsetPin2, INPUT);
  digitalWrite(offsetPin2, LOW);
  pinMode(offsetPin1, INPUT);
  digitalWrite(offsetPin1, LOW);

  pinMode(PosNegPin, INPUT);
  digitalWrite(PosNegPin, LOW);

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

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////MAIN LOOP////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  c = c + 1;



  /////////// Find sensor values, turn --> true if the opposing sensors are not equal
    c1 = analogRead(A0);
  c2 = analogRead(A1);
  c3 = analogRead(A2);
  c4 = analogRead(A3);


//Base offsets , they don't change
  c1= c1 - c1offset;
  c2= c2 - c2offset;
  c3= c3 - c3offset;
  c4= c4 - c4offset;

//offsets changing based on switch inputs
  c4 = c4 + sensorOffset4;
  c3 = c3 + sensorOffset3;
  c2 = c2 + sensorOffset2;
  c1 = c1 + sensorOffset1;

  //  c1 = map(c1, 0, 1023, 0, 127);
  // c2 = map(c2, 0, 1023, 0, 127);
  // c3 = map(c3, 0, 1023, 0, 127);
  //c4 = map(c4, 0, 1023, 0, 127);

  fbPosition1 = analogRead(feedbackPin1); //azimuth
  fbPosition2 = analogRead(feedbackPin2); //zenith

  turn = abs(c1 - c2) >= accuracy || abs(c3 - c4) >= accuracy;


  //////////////////Every 1000 cycle (0.1ms?) print each sensor value//////////////
  if (c == 1000)
  {
    Serial.println("Start Printing");
    Serial.print("azimuth: ");
    Serial.println(fbPosition1);
    Serial.print("zenith: ");
    Serial.println(fbPosition2);
    Serial.print("c1: ");
    Serial.println(c1);
    Serial.print("c2: ");
    Serial.println(c2);
    Serial.print("c3: ");
    Serial.println(c3);
    Serial.print("c4: ");
    Serial.println(c4);
    Serial.print("sensorOffset4 ");
    Serial.println(sensorOffset4);
    Serial.print("sensorOffset3 ");
    Serial.println(sensorOffset3);
    Serial.print("sensorOffset2 ");
    Serial.println(sensorOffset2);
    Serial.print("sensorOffset1 ");
    Serial.println(sensorOffset1);
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
  offsetHolder4 = digitalRead(offsetPin4);
  offsetHolder3 = digitalRead(offsetPin3);
  offsetHolder2 = digitalRead(offsetPin2);
  offsetHolder1 = digitalRead(offsetPin1);
  
  PosNegHolder = digitalRead(PosNegPin);
  
/////////Switch control module/////////////
  
  if ( offsetHolder4 == HIGH){
    if (PosNegHolder == HIGH){
      sensorOffset4 = sensorOffset4 - 1;    
    } else {
      sensorOffset4 = sensorOffset4 + 1;
  }
    delay (1000);
  } else if ( offsetHolder3 == HIGH){
    if (PosNegHolder == HIGH){
      sensorOffset3 = sensorOffset3 - 1;    
    } else {
      sensorOffset3 = sensorOffset3 + 1;
  }
  delay (1000);
} else if ( offsetHolder2 == HIGH){
    if (PosNegHolder == HIGH){
      sensorOffset2 = sensorOffset2 - 1;    
    } else {
      sensorOffset2 = sensorOffset2 + 1;
  }
  delay (1000);
} else if ( offsetHolder1 == HIGH){
    if (PosNegHolder == HIGH){
      sensorOffset1 = sensorOffset1 - 1;    
    } else {
      sensorOffset1 = sensorOffset1 + 1;
  }
  delay (1000);
}

////////////////////////////

  c1 = analogRead(A0);
  c2 = analogRead(A1);
  c3 = analogRead(A2);
  c4 = analogRead(A3);

  c1= c1 - c1offset;
  c2= c2 - c2offset;
  c3= c3 - c3offset;
  c4= c4 - c4offset;
  
//offsets changing based on switch inputs
  c4 = c4 + sensorOffset4;
  c3 = c3 + sensorOffset3;
  c2 = c2 + sensorOffset2;
  c1 = c1 + sensorOffset1;

    //c1 = map(c1, 0, 1023, 0, 127);
    //c2 = map(c2, 0, 1023, 0, 127);
    //c3 = map(c3, 0, 1023, 0, 127);
    //c4 = map(c4, 0, 1023, 0, 127);
  
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
    turn = abs(c1 - c2) >= accuracy || abs(c3 - c4) >= accuracy ;

    if (i == 500)
    {
     Serial.println("Start Printing");
    Serial.print("azimuth: ");
    Serial.println(fbPosition1);
    Serial.print("zenith: ");
    Serial.println(fbPosition2);
    Serial.print("c1: ");
    Serial.println(c1);
    Serial.print("c2: ");
    Serial.println(c2);
    Serial.print("c3: ");
    Serial.println(c3);
    Serial.print("c4: ");
    Serial.println(c4);
    Serial.print("sensorOffset4 ");
    Serial.println(sensorOffset4);
    Serial.print("sensorOffset3 ");
    Serial.println(sensorOffset3);
    Serial.print("sensorOffset2 ");
    Serial.println(sensorOffset2);
    Serial.print("sensorOffset1 ");
    Serial.println(sensorOffset1);
    Serial.println("End Printing");
      i = 0;
    }
    //  delay(50);
  }

  i = 0;
  switchOff();


}

void setDC()
{

  //base speed
  //max is 4096
  mindc2u = 3080;//130;
  mindc1 = 1200;//65;
  mindc2d = 2500;//100;

  dc1 = mindc1 + k1 * abs(c1 - c2); // azimuth (left/right)
  dc2u =  mindc2u + k2 * abs(c3 - c4) + k3 * (300-fbPosition2); //power to go up (zenith)
  dc2d = mindc2d + k2 * abs(c3 - c4); //

 // Serial.println("MotorValues");
 // Serial.println(dc1);
 // Serial.println(dc2u);
  //Serial.println(dc2d);
 // Serial.println("EndMotorValues");

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


