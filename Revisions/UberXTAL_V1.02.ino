//Define the used pins
#define DRIVE_LID_SENS 8
#define GATE 2
#define DATA 12
#define P_SENS 4
#define mchipregister PIND  //register for monitoring high/low states (read only)
#define mchipregwrite PORTD //register to write pin states to in setup
#define DRIVE_LID_SENS_REG PINB //the register with the DATA line and our drive lid sensor line
int bitdelay (3974); //delay between bits sent to drive controller (MS)
int stringdelay (67); //delay between string injections
const int Lid_Open = (0B000001); //so we don't have to refer to the states in bits further on
const int Lid_Closed = (0B000000);
int bitcounter = 0; //variable for injection loop storage
const int SENSOR_PIN = A0; //A0 from GPU pin 157
const int NTSC = A1; // Power output to DFO NTSC Crystal
const int PAL = A2; // Power output to DFO PAL Crystal
const int BENCHMARK = 500; // Value sensed between PAL and NTSC values. PAL=0, NTSC=750 and Off-250
//Video frequency switching shiz



//*****************REGION SELECT****************************            
// EUROPE/AUSTRALIA/NEWZEALAND = 1    (SCEE)
// USA = 2                            (SCEA)
// JP= 3                              (SCEI)
int MAGICKEY = 1;                                               //   <----------------------------------------------------------------------------------REGION SELECT!!
//**********************************************************

//*****************REVISION SELECT**************************            
// PU-20 or lower = 1                 (SCPH-100X through SCPH-590X)
// PU-22 or higher = 2                (SCPH-700X through SCPH-9000X and SCPH-10X)
int REVISION = 1;                                               //   <----------------------------------------------------------------------------------REVISION SELECT!!
//**********************************************************



char SCEE[] = "S10011010100100111101001010111010010101110100";
char SCEA[] = "S10011010100100111101001010111010010111110100";
char SCEI[] = "S10011010100100111101001010111010010110110100";




//DEFAULT 'SCEE' INJECTION, CHANGE IN INJECT() TO SUIT YOUR REGION (x 3 references to character string)

void setup() {

  Serial.begin(115200); //dev monitor
  Serial.println("Booted");
  //Set up the used pins
  pinMode (GATE, OUTPUT);
  pinMode (DATA, OUTPUT);
  pinMode (P_SENS, INPUT);
  pinMode (DRIVE_LID_SENS, INPUT);
  //and sets true states of other unused pins in this register
  digitalWrite (DRIVE_LID_SENS_REG, 0B000000);

  if (MAGICKEY == 1) {
    digitalWrite(PAL, HIGH);
    digitalWrite(NTSC, LOW);
    Serial.println("Boot PAL set")
  }
  else {
    digitalWrite(NTSC, HIGH);
    digitalWrite(PAL, LOW);
    Serial.println("Boot NTSC set")
  }
    //Set boot video region
}

void NewDisc() {
  Serial.println("NewDisc");

  do
  {
    delay (1500);
  }
  while (DRIVE_LID_SENS_REG == Lid_Open);
  
  if (MAGICKEY == 1) {injectSCEE();} //ready to inject
  if (MAGICKEY == 2) {injectSCEA();}
  if (MAGICKEY == 3) {injectSCEI();}
}

void DriveLidStatus() {
  Serial.println("DriveLidStatus");
    senseregion();
  if (DRIVE_LID_SENS_REG == Lid_Closed) { //state of this register after initial injection routine with lid closed
    bitcounter = 0;                  //This reset is in case of multidisc/ multiple injection routines
  }
  else if (DRIVE_LID_SENS_REG == Lid_Open) {  //monitor register state with this pin for sensing purposes, data idles low after injection, only need to look for a change in one bit (lid sens pin)
    delay (500);
    NewDisc();
  }
  DriveLidStatus();                           //forms a conditional if loop
}

void PowerUpDelay() {
  Serial.println("PowerUpDelay");
  delay (7200);
  
  if (MAGICKEY == 1) {injectSCEE();} //ready to inject
  if (MAGICKEY == 2) {injectSCEA();}
  if (MAGICKEY == 3) {injectSCEI();}
}

void injectSCEE() {
  Serial.println("injectSCEE");
  do
  {
    for (byte i = 0; i < sizeof(SCEE) - 1; i++)

      if (SCEE[i] == '1')
      {
        digitalWrite (DATA, HIGH);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
      else if (SCEE[i] == 'S')
      {
        delay(stringdelay); //string delay when it hits 's' (end of injection string bits)
      }
      else if (DRIVE_LID_SENS_REG == Lid_Open)
      {
        DriveLidStatus();
      }
      else
      {
        digitalWrite (DATA, LOW);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
  }
  while (bitcounter < 4400);
  DriveLidStatus();
}


void injectSCEA() {
  Serial.println("injectSCEA");
  do
  {
    for (byte i = 0; i < sizeof(SCEA) - 1; i++)

      if (SCEA[i] == '1')
      {
        digitalWrite (DATA, HIGH);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
      else if (SCEA[i] == 'S')
      {
        delay(stringdelay); //string delay when it hits 's' (end of injection string bits)
      }
      else if (DRIVE_LID_SENS_REG == Lid_Open)
      {
        DriveLidStatus();
      }
      else
      {
        digitalWrite (DATA, LOW);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
  }
  while (bitcounter < 4400);
  DriveLidStatus();
}


void injectSCEI() {
  Serial.println("injectSCEI");
  do
  {
    for (byte i = 0; i < sizeof(SCEI) - 1; i++)

      if (SCEI[i] == '1')
      {
        digitalWrite (DATA, HIGH);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
      else if (SCEI[i] == 'S')
      {
        delay(stringdelay); //string delay when it hits 's' (end of injection string bits)
      }
      else if (DRIVE_LID_SENS_REG == Lid_Open)
      {
        DriveLidStatus();
      }
      else
      {
        digitalWrite (DATA, LOW);
        delayMicroseconds (bitdelay);
        bitcounter = (bitcounter + 1);
      }
  }
  while (bitcounter < 4400);
  DriveLidStatus();
}

void senseregion() {
  Serial.println("senseregion");
  // 1. reading the input from the analog pin to which sensor is connected
  int input = analogRead(SENSOR_PIN);
  
  // 2. if sensor value less than "benchmark" - powere PAL Crystal
  if (input <= BENCHMARK){
    digitalWrite(PAL, HIGH);
    digitalWrite(NTSC, LOW);
    Serial.println("PAL SET");
  }
  //2bis. if sensor value is more than "benchmark" - Power the NTSC Crystal
  if (input >= BENCHMARK){
    digitalWrite(NTSC, HIGH);
    digitalWrite(PAL, LOW);
    Serial.println("NTSC SET");
  }
}



void loop() {
  Serial.println("loop");
  
  if  (bitcounter >= 4400)
  {
    DriveLidStatus();
  }
  PowerUpDelay();             // else jump to here
  
}
