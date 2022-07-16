
/*                                                                ,----,                ,--,
                                                              ,/   .`|             ,---.'|
                                           ,--,     ,--,    ,`   .'  : ,---,       |   | :
          ,--,  ,---,                      |'. \   / .`|  ;    ;     /'  .' \      :   : |
        ,'_ /|,---.'|               __  ,-.; \ `\ /' / ;.'___,/    ,'/  ;    '.    |   ' :
   .--. |  | :|   | :             ,' ,'/ /|`. \  /  / .'|    :     |:  :       \   ;   ; '
  ,'_ /| :  . |:   : :      ,---.  '  | |' | \  \/  / ./ ;    |.';  ;:  |   /\   \  '   | |__
  |  ' | |  . .:     |,-.  /     \ |  |   ,'  \  \.'  /  `----'  |  ||  :  ' ;.   : |   | :.'|
  |  | ' |  | ||   : '  | /    /  |'  :  /     \  ;  ;       '   :  ;|  |  ;/  \   \'   :    ;
  :  | | :  ' ;|   |  / :.    ' / ||  | '     / \  \  \      |   |  ''  :  | \  \ ,'|   |  ./
  |  ; ' |  | ''   : |: |'   ;   /|;  : |    ;  /\  \  \     '   :  ||  |  '  '--'  ;   : ;
  :  | : ;  ; ||   | '/ :'   |  / ||  , ;  ./__;  \  ;  \    ;   |.' |  :  :        |   ,/
  '  :  `--'   \   :    ||   :    | ---'   |   : / \  \  ;   '---'   |  | ,'        '---'
  :  ,      .-./    \  /  \   \  /         ;   |/   \  ' |           `--''
  `--`----'   `-'----'    `----'          `---'     `--`


  A joint project by VajskiDs and SpankyToot
  Combining existing projects by VajskiDs, PSXTAL and tehUberChip.
  PSX modchip and with the combination of a DFO board from VajskiDs, also an automatic video output clock switcher
*/

//*****************REGION SELECT****************************
// EUROPE / AUSTRALIA / NEWZEALAND = 1    (SCEE)
// USA = 2                                (SCEA)
// JP = 3                                 (SCEI)
int MAGICKEY = 1;                                               //   <----------------------------------------------------------------------------------REGION SELECT!!
//**********************************************************

//*****************REVISION SELECT**************************
// PU-20 or lower = 1                 (SCPH-100X through SCPH-590X)
// PU-22 or higher = 2                (SCPH-700X through SCPH-9000X and SCPH-10X)
int REVISION = 1;                                               //   <----------------------------------------------------------------------------------REVISION SELECT!!
//**********************************************************


#define DRIVE_LID_SENS 8 // D8 to Lid switch on PSX
#define GATE 2 // D2 to GATE on PSX
#define DATA 12 // D12 to DATA on PSX
#define mchipregister PIND  //register for monitoring high/low states (read only)
#define mchipregwrite PORTD //register to write pin states to in setup
#define DRIVE_LID_SENS_REG PINB //the register with the DATA line and our drive lid sensor line
#define SENSOR_PIN 14 //A0 from GPU pin 157
#define NTSC 15 // A1 Power output to DFO NTSC Crystal
#define PAL 16 // A2 Power output to DFO PAL Crystal
int bitdelay (3974); //delay between bits sent to drive controller (MS)
int stringdelay (67); //delay between string injections
const int Lid_Open = (0B000001); //so we don't have to refer to the states in bits further on
const int Lid_Closed = (0B000000);
int bitcounter = 0; //variable for injection loop storage
const int BENCHMARK = 500; // Value sensed between PAL and NTSC values. PAL=0, NTSC=750 and Off-250
//int XTALMODE = 00; // A Reminder for later to setup a synth clock for later revision boards
int VideoRegion = 0; // Value of GPU output

// Region injection strings
char SCEE[] = "S10011010100100111101001010111010010101110100";
char SCEA[] = "S10011010100100111101001010111010010111110100";
char SCEI[] = "S10011010100100111101001010111010010110110100";

boolean LoadAtBoot = true;



void NewDisc() {

  do {
    ;
  } while (DRIVE_LID_SENS_REG == Lid_Open);  // Do nothing until the lid is closed again, once closed, detect region and jump to corresponding inject routine

  delay (2500);
  if (MAGICKEY == 1) {
    injectSCEE();
  }
  if (MAGICKEY == 2) {
    injectSCEA();
  }
  if (MAGICKEY == 3) {
    injectSCEI();
  }
}



void DriveLidStatus() {

  boolean LoadAtBoot = false;
  bitcounter = 0; // reset counter for injections

  do {
    ;
  }
  while (DRIVE_LID_SENS_REG == Lid_Closed); // while the lid is closed, lock into a do nothing loop, it's only broken by lid being opened.


  NewDisc();  // If the lid isn't closed anymore, you must be switching discs, jump to newdisc()

}



void PowerUpDelay() { // Patiently waits to inject at the precise moment its needed.
  Serial.println(F("PowerUpDelay")); //DEBUG
  delay (7200);

  if (MAGICKEY == 1) {
    injectSCEE();
  }
  if (MAGICKEY == 2) {
    injectSCEA();
  }
  if (MAGICKEY == 3) {
    injectSCEI();
  }
}


void injectSCEE() { //Injects SCEE String
  Serial.println(F("injectSCEE")); //DEBUG
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
        senseregion();
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
  while (bitcounter < 4400 && LoadAtBoot == true || bitcounter < 99999999999 && LoadAtBoot == false);
}

void injectSCEA() { // Injects SCEA String
  Serial.println(F("injectSCEA")); //DEBUG
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
        senseregion();
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
  while (bitcounter < 4400 && LoadAtBoot == true || bitcounter < 99999999999 && LoadAtBoot == false);
}

void injectSCEI() { // Injects SCEI String
  Serial.println(F("injectSCEI")); //DEBUG
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
        senseregion();
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
  while (bitcounter < 4400 && LoadAtBoot == true || bitcounter < 99999999999 && LoadAtBoot == false);
}

void senseregion() { //Checks current GPU output and sets IO accordingly
  Serial.println(F("senseregion")); //DEBUG
  VideoRegion = analogRead(SENSOR_PIN);

  if (VideoRegion <= BENCHMARK) { // Power PAL XTAL
    pinMode (PAL, OUTPUT);
    pinMode (NTSC, INPUT);
    digitalWrite(PAL, HIGH);
    Serial.println(F("PAL SET")); //DEBUG
  }
  if (VideoRegion >= BENCHMARK) { // Power NTSC XTAL
    pinMode (NTSC, OUTPUT);
    pinMode (PAL, INPUT);
    digitalWrite(NTSC, HIGH);
    Serial.println(F("NTSC SET")); //DEBUG
  }
}

void setup() {
  //************DEBUG SHIZ*************
  Serial.begin(115200); //dev monitor
  Serial.println(F("Starting")); //DEBUG
  //***********************************

  pinMode (GATE, OUTPUT);
  pinMode (DATA, OUTPUT);
  pinMode (DRIVE_LID_SENS, INPUT);
  //sets true states of other unused pins in this register
  digitalWrite (DRIVE_LID_SENS_REG, 0B000000);

  if (MAGICKEY == 1) {
    pinMode (PAL, OUTPUT);
    pinMode (NTSC, INPUT);
    digitalWrite(PAL, HIGH);
    Serial.println(F("Booting as PAL")); //DEBUG
  }
  else {
    pinMode (NTSC, OUTPUT);
    pinMode (PAL, INPUT);
    digitalWrite(NTSC, HIGH);
    Serial.println(F("Booting as NTSC")); //DEBUG
  }    //Set boot video region

  if (DRIVE_LID_SENS_REG == Lid_Closed) {
    PowerUpDelay(); // Only needs this to run once
  }
}

void loop() { // Does it over and over again
  Serial.println(F("looping")); //DEBUG
  senseregion();
  DriveLidStatus();
}

