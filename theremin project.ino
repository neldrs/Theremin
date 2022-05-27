#define USPERCM 29       //speed of sound in microseconds to centimeters
#define CUTOFFDIST 35   //max distance ultrasonic sensor will read is 35cm
#define TRIGTIME 10      //Time to pulse trigger pin to generate sound burst
#define UD 11     //Up/Down (U/D) (used to control direction of wiper and counter increment/decrement) connected to pin 11 on arduino
#define CS 7      //Chip select connected to pin 7 on arduino
#define INC 9     //Increment (INC) connected to pin 9 on arduino
#define VW A13    //VW (wiper terminal) connected to analog 13 on arduino
#define TDI 10    //Time to wait after setting U/D voltage to high to toggle INC
#define TIL 5     //Time to wait after setting INC to low voltage
#define TIH 5     //time to wait after setting INC to high voltage
#define BUZZER 12    //Buzzer is connected to pin 12
#define NOTELENGTH 250  //How long tone plays for
#define TRIG 3    //Trigger pin set to 3
#define ECHO 5    //Echo pin set to 5
char count = 0;   //current volume

//used to set volume controlled by digital potentiometer
char setCount(char c){
  if (count == c)                 //checks to see if volume is already at the volume we want to set it at
    return count;                 //returns same value because we don't need to change the volume
  if (count < c){                 //checks if current volume is less than the volume we want, if true:
    digitalWrite(UD, HIGH);           //sets U/D pin voltage to high, this allows us to increase volume when we pulse INC
    delayMicroseconds(TDI);           //waits TDI (5 microseconds)
    while(count < c){                 //loops until current volume is the desired volume (by pulsing INC)
      digitalWrite(INC, LOW);             //sets INC voltage to LOW
      delayMicroseconds(TIL);             //waits TIL (5 microseconds) before next toggle
      digitalWrite(INC, HIGH);            //sets INC voltage to LOW
      delayMicroseconds(TIH);             //waits TIH (5 microseconds) before next toggle
      count++;                            //increases count by 1 (used to track current volume)
      }
    }
  if (count > c){               //checks if current volume is higher than the volume we want, if true:
    digitalWrite(UD, LOW);      //sets U/D to LOW voltage, this allows us to decrease volume when we pulse INC
    delayMicroseconds(TDI);     //starts pulsing INC after delay
    while(count > c){
      digitalWrite(INC, LOW);   
      delayMicroseconds(TIL);
      digitalWrite(INC, HIGH);
      delayMicroseconds(TIH);
      count--;
    }
  }
}

//calculates distance from ultrasonic sensor
int calcDist(int trigPin, int echoPin){       //need access to trigPin and echoPin
  int dist;      //distance stored in here
  long time = 0;  //initializing time variable to 0
  digitalWrite(trigPin, LOW);   //verifying pin is low to make sure we get a positive pulse 
  delayMicroseconds(2);         //delay before setting pin to high and starting pulse which will send out soundwave
  digitalWrite(trigPin, HIGH); //need positive pulse to send out ultrasonic sound
  delayMicroseconds(TRIGTIME);  //time to wait before setting trig pin to low for pulse
  digitalWrite(trigPin, LOW);   //trig pin low
  time = pulseIn(echoPin, HIGH);  //finds time it takes for pulse to return using echo pin
  dist = time / USPERCM / 2;      //calculates distance from ultrasonic sensor: time / speed of sound / 2 (divide by two because we're only concerned with the distance one way)
  delayMicroseconds(USPERCM * CUTOFFDIST); // time it takes for sound wave to propogate and come back before we transmit again
  return dist;      //returns distance from sensor
}

void setup() {
  Serial.begin(9600);     //starts communication between arduino and serial monitor at 9600 baud rate

  //digital potentiometer setup
  pinMode(INC, OUTPUT);   //sets pin for INC as an OUTPUT which allows us to use the arduino to control voltage
  digitalWrite(INC, HIGH);  //sets pin for INC to high voltage
  pinMode(CS, OUTPUT);      //sets pin for CS as an OUTPUT
  digitalWrite(CS, LOW);    //sets CS voltage to LOW to select chip (more useful when using multiple chips)
  pinMode(UD, OUTPUT);      //sets pin for U/D as ab OUTPUT

  
  // Potentiometer appears to store volume in nonvolatile memory on chip,
  // so volume will not always be at zero when we start the arduino, 
  // following lines and loop allow us to make sure our potentiometer volume begins at zero    

  digitalWrite(UD, LOW);    //preparing to pulse INC in loop, to bring down volume
  delayMicroseconds(TDI);   //waits 5 microseconds (time needed to wait before using INC)
  for(int i = 0; i < 100; i++){   //loops 100 times to ensure volume starts at 0
    digitalWrite(INC, LOW); 
    delayMicroseconds(TIL);
    digitalWrite(INC, HIGH);
    delay(1);
  }

  //ultrasonic sensor setup
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  //buzzer setup
  pinMode(BUZZER, OUTPUT);    //sets buzzer pin as an OUTPUT which allows us to use the arduino to control voltage
  
  Serial.println("Hello, world.");  //print to terminal check
}

void loop() {
  char vol;
  int pitch;
  int dist;
  
  dist = calcDist(TRIG, ECHO);

  //won't set volume or tone if sensor distance is greater than cutoff distance 
  if (dist <= CUTOFFDIST){
    vol = map(dist, 0, CUTOFFDIST, 20, 31);  //max 31
    pitch = map(dist, 0, CUTOFFDIST, 31, 4978);
    setCount(vol);
    tone(BUZZER, pitch);

  }

}
