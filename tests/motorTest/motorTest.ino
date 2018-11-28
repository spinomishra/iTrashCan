//
// testing motor for forward movement, left, right turns and pivots
//


// Motor A - right
int enA = 3;
int in1 = 5;
int in2 = 4;

// Motor B - left
int enB = 11;
int in3 = 7;
int in4 = 8;
bool doonce ;

void PivotRight()
{
  Serial.println(" Setting motors for RIGHT PIVOT |||***");
  Serial.println("  ");
  Serial.println("  ");

  analogWrite(enB, 200);
  analogWrite(enA, 160);  // lowering the speed of right motor by providing less pwms
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(500) ;
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(20) ;
}

void PivotLeft()
{
  Serial.println(" Setting motors for LEFT PIVOT ***|||");
  Serial.println("  ");
  Serial.println("  ");

  analogWrite(enB, 150); // lowering the speed of left motor by providing less pwms
  analogWrite(enA, 150);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void setup()
{
  doonce = true ;
  Serial.begin(9600);
  
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop()
{
  // Turn on Motor A forwards
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  analogWrite(enA,45);
  Serial.println("Right Motor - forward");
  delay(2000);


  //Turn Off
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  analogWrite(enA,0);
  Serial.println("Off");
  delay(2000);

  // Now Backwards
    digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  analogWrite(enA,45);
  Serial.println("Right Motor - Backwards");
  delay(2000);

// Now off again
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  analogWrite(enA,0);
  Serial.println("Off");
  delay(2000);

// Turn on Motor B forwards
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  analogWrite(enB,45);
  Serial.println("Left Motor - Forward");
  delay(2000);

  //Turn Off
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(enB,0);
  Serial.println("Off");
  delay(2000);

  // Now Backwards
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(enB,45);
  Serial.println("Left motor backwards");
  delay(2000);

// Now off again
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  analogWrite(enB,0);
  Serial.println("Off");
  delay(2000);

  // Now together

  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  analogWrite(enA,45);
  analogWrite(enB,45);
  Serial.println("Right and Left motors forward");
  delay(25000);

  //Now both off
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  analogWrite(enA,0);
  analogWrite(enB,0);
  Serial.println("Off");
  delay(25000);
}
