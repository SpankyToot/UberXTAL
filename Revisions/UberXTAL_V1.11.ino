//                                                                 ,----,                ,--,    
//                                                               ,/   .`|             ,---.'|    
//                                            ,--,     ,--,    ,`   .'  : ,---,       |   | :    
//           ,--,  ,---,                      |'. \   / .`|  ;    ;     /'  .' \      :   : |    
//         ,'_ /|,---.'|               __  ,-.; \ `\ /' / ;.'___,/    ,'/  ;    '.    |   ' :    
//    .--. |  | :|   | :             ,' ,'/ /|`. \  /  / .'|    :     |:  :       \   ;   ; '    
//  ,'_ /| :  . |:   : :      ,---.  '  | |' | \  \/  / ./ ;    |.';  ;:  |   /\   \  '   | |__  
//  |  ' | |  . .:     |,-.  /     \ |  |   ,'  \  \.'  /  `----'  |  ||  :  ' ;.   : |   | :.'| 
//  |  | ' |  | ||   : '  | /    /  |'  :  /     \  ;  ;       '   :  ;|  |  ;/  \   \'   :    ; 
//  :  | | :  ' ;|   |  / :.    ' / ||  | '     / \  \  \      |   |  ''  :  | \  \ ,'|   |  ./  
//  |  ; ' |  | ''   : |: |'   ;   /|;  : |    ;  /\  \  \     '   :  ||  |  '  '--'  ;   : ;    
//  :  | : ;  ; ||   | '/ :'   |  / ||  , ;  ./__;  \  ;  \    ;   |.' |  :  :        |   ,/     
//  '  :  `--'   \   :    ||   :    | ---'   |   : / \  \  ;   '---'   |  | ,'        '---'      
//  :  ,      .-./    \  /  \   \  /         ;   |/   \  ' |           `--''                     
//   `--`----'   `-'----'    `----'          `---'     `--`                                      
//
//
//  A joint project by VajskiDs and SpankyToot
//  Combining existing projects by VajskiDs, PSXTAL and tehUberChip.
//  PSX modchip and with the combination of a DFO board from VajskiDs, also an automatic video output clock switcher
//
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

void NewDisc() { // runs in game multidisc change injection

    if (bitcounter == 0); {
      delay (500);
      if (MAGICKEY == 1) {injectSCEE();}
      if (MAGICKEY == 2) {injectSCEA();}
      if (MAGICKEY == 3) {injectSCEI();}
    }
}

void DriveLidStatus() {
  if (DRIVE_LID_SENS_REG == Lid_Closed) { //state of this register after initial injection routine with lid closed
    bitcounter = 0; //This reset is in case of multidisc/ multiple injection routines
    NewDisc();
  }
  else if (DRIVE_LID_SENS_REG == Lid_Open) {  //monitor register state with this pin for sensing purposes, data idles low after injection, only need to look for a change in one bit (lid sens pin)
    do{
    }
  while (DRIVE_LID_SENS_REG == Lid_Open);
    NewDisc();
  }
}

void PowerUpDelay() { // Patiently waits to inject at the precise moment its needed.
  delay (7200);
  
  if (MAGICKEY == 1) {injectSCEE();}
  if (MAGICKEY == 2) {injectSCEA();}
  if (MAGICKEY == 3) {injectSCEI();}
}

void injectSCEE() { //Injects SCEE String
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
}

void injectSCEA() { // Injects SCEA String
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
}

void injectSCEI() { // Injects SCEI String
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
}

void senseregion() { //Checks current GPU output and sets IO accordingly
  VideoRegion = analogRead(SENSOR_PIN);
  
  if (VideoRegion <= BENCHMARK){ // Power PAL XTAL
    pinMode (PAL, OUTPUT);
    pinMode (NTSC, INPUT);
    digitalWrite(PAL, HIGH);
  }
  if (VideoRegion >= BENCHMARK){ // Power NTSC XTAL
    pinMode (NTSC, OUTPUT);
    pinMode (PAL, INPUT);
    digitalWrite(NTSC, HIGH);
  }
}

void setup() {
  pinMode (GATE, OUTPUT);
  pinMode (DATA, OUTPUT);
  pinMode (DRIVE_LID_SENS, INPUT);  
  //sets true states of other unused pins in this register
  digitalWrite (DRIVE_LID_SENS_REG, 0B000000);

  if (MAGICKEY == 1) {
    pinMode (PAL, OUTPUT);
    pinMode (NTSC, INPUT);
    digitalWrite(PAL, HIGH);
  }
  else {
    pinMode (NTSC, OUTPUT);
    pinMode (PAL, INPUT);
    digitalWrite(NTSC, HIGH);
  }    //Set boot video region
  
  if (DRIVE_LID_SENS_REG == Lid_Closed) {
  PowerUpDelay(); // Only needs this to run once
  } 
}

void loop() { // Does it over and over again
  DriveLidStatus();
  senseregion();
}
