


// Motor A

int enA = 9;
int in1 = 8;
int in2 = 7;

// Motor B

int enB = 3;
int in3 = 5;
int in4 = 4;

void setup(){
  
  Serial.begin(9600);
  
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  }

  void loop(){
    
    digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  analogWrite(enA,255);
  analogWrite(enB,255);
    Serial.println("A and B");
  delay(10000);

  //Now both off
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  analogWrite(enA,0);
  analogWrite(enB,0);
  Serial.println("Off");
  delay(2000);  
    
    
    }
