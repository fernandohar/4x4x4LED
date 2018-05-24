/*
  4x4x4 LED Cube
  Columns (from +'ve of LED)
  [(x,y)-Pin] 
  (1,1)-13
  (1,2)-12
  (1,3)-11
  (1,4)-10
  (2,1)-9
  (2,2)-8
  (2,3)-7
  (2,4)-6
  (3,1)-5
  (3-2)-4
  (3-3)-3
  (3,4)-2
  (4,1)-1
  (4,2)-0
  (4,3)-A5
  (4,4)-A4
  
  Layers
  [layer-Pin] (from -'ve of LED)
  a-A0 //BOTTOM LAYER
  b-A1 //FIRST LAYER
  c-A2 //SECOND LAYER
  d-A3 //TOP LAYER
*/
#define LAYERON 0
#define LAYEROFF 1
#define COLUMNON 1
#define COLUMNOFF 0
#define ON 1
#define OFF 0

//initializing and declaring led rows
int column[16]={13,12,11,10,9,8,7,6,5,4,3,2,1,0,A5,A4};
//initializing and declaring led layers
int layer[4]={A0,A1,A2,A3};
  
//mapping of LED in [x,y] positioning to corresponding arduino PIN 
int x_y[4][4] = { {13,12,11,10}, {9,8,7,6},{5,4,3,2}, {1,0,A5,A4}};
    
// xyPORT_mask is used for port manipulation, it maps LED in [x,y] positioning to corresponding BIT in PORTX 
byte xyPORT_mask[4][4] = {
                          {B00100000, B00010000, B00001000, B00000100}, //PORTB, PORTB, PORTB, PORTB
                          {B00000010, B00000001, B10000000, B01000000}, //PORTB, PORTB, PORTD, PORTD
                          {B00100000, B00010000, B00001000, B00000100}, //PORTD, PORTD, PORTD, PORTD
                          {B00000010, B00000001, B00100000, B00010000}  //PORTD, PORTD, PORTC, PORTC
                         };
                         
byte byteMask_PORTD [4] = {0,0,0,0}; //array index = layer
byte byteMask_PORTC [4] = {0,0,0,0};
byte byteMask_PORTB [4] = {0,0,0,0};

// setLEDStatus updates the BYTE MASK that is used to set PORTX in updateLEDs()
// the physical LED does not change status until updateLEDs() is called
void setLedStatus(int x, int y, int layer, int status){
  if(layer > 3 || x > 3 || y > 3 || layer < 0 || x < 0 || y < 0) //Prevent array out of bounce problem
  {
    return;
  }
  
  int arduinoPin = x_y[x][y]; //find out the arduino PIN of the x,y  The pin is used to find the PORT in the next step
  if(arduinoPin >= 8 && arduinoPin <= 13 ){ //PORTB
    if(status){
      byteMask_PORTB[layer] = byteMask_PORTB[layer] | xyPORT_mask[x][y];
    }else{
      byteMask_PORTB[layer] = byteMask_PORTB[layer] & ~xyPORT_mask[x][y];
    }
  }else if(arduinoPin >=0 && arduinoPin <= 7){ //PORTD
    if(status){
      byteMask_PORTD[layer] = byteMask_PORTD[layer] | xyPORT_mask[x][y];
    }else{
      byteMask_PORTD[layer] = byteMask_PORTD[layer] & ~xyPORT_mask[x][y];
    }
  }else{ //PORTC
    if(status){
      byteMask_PORTC[layer] = byteMask_PORTC[layer] | xyPORT_mask[x][y];
    }else{
      byteMask_PORTC[layer] = byteMask_PORTC[layer] & ~xyPORT_mask[x][y];
    }    
  }
}
void updateLEDs(){ //use port manipulation to update all LEDs
  PORTC = PORTC | B00001111; //turn off all layers
  for(int i = 0; i < 4; i++){
    //PORTC = PORTC & ~_BV(i); //turn on the layer //1 = OFF LAYER, 0 = ON LAYER   
    analogWrite(i+14, 0);
    PORTD = byteMask_PORTD[i]; //update column pins
    PORTB = byteMask_PORTB[i]; //update column pins
    PORTC =  (PORTC & B11001111) | byteMask_PORTC[i]; //update column pins
   
    delay(1); //too large a value result in flickers, too small a value result in too dim   
    //PORTC = PORTC | _BV(i); //TURN OFF THE LAYER//1 = OFF LAYER, 0 = ON LAYER   
    analogWrite(i+14, 255);
  }
}



////////////////////////////////////////////////////////////////////////////
void setup()
{
  //PORTD maps to arduino digital pins 0 - 7
  DDRD =  B11111111; //sets Arduino pin 0 - 7 as outputs
  PORTD = B00000000; //sets Arduino pin 0 - 7 LOW
  
  //PORTB maps to arduino digital pins 8 - 13
  DDRB =  B00111111; //sets Arduino pin 8 - 13 as outputs (high two bits are not usable)
  PORTB = B00000000; //set Arduino pin 8 - 13 LOW

  //PORTC maps to Arduino analog pins 0 - 5
  DDRC =  B00111111;  //set Arduino pin A0 - A5 as output
  PORTC = B00001111; //set Arduino pin A4, A5 as LOW, A0 - A3 as high
  
  randomSeed(analogRead(A0));
  allOFF();
  //Serial.begin(9600); //This causes the pin 0 constantly high and pin 1 at around 1/2 HIGH
}


int currentAnimation = -1;
unsigned long animationTimer = 0;
int animationCounter = 0; //used within animation function to step forward animation
bool animationEnded = true;
unsigned int totalAnimation  = 1; //THIS CONFIG MUST BE UPDATED FOR NEW ADDED ANIMATION

void loop()
{
  //if animation has ended, choose a new animation to play. Ensuring we don't pick the same one as before
  if(animationEnded){ 
    int temp = random(100) % totalAnimation;
//    while(currentAnimation == temp){
//      temp = random(100) % totalAnimation;
//    }
    //Reset animation controling variables
    currentAnimation = temp;
    animationCounter = 0;
    animationEnded = false;
    animationTimer = millis();
  }
  switch(currentAnimation){
    case 0:
        rainAnimation(random(1, 3));
//        setLedStatus(0, 0, 0, ON);
//        setLedStatus(0, 1, 0, ON);
//        setLedStatus(0, 2, 0, ON);
//        setLedStatus(0, 3, 0, ON);
//        setLedStatus(1, 0, 0, ON);
//        setLedStatus(1, 1, 0, ON);
//        setLedStatus(1, 2, 0, ON);
//        setLedStatus(1, 3, 0, ON);
//        setLedStatus(2, 0, 0, ON);
//        setLedStatus(2, 1, 0, ON);
//        setLedStatus(2, 2, 0, ON);
//        setLedStatus(2, 3, 0, ON);
//        setLedStatus(3, 0, 0, ON);
//        setLedStatus(3, 1, 0, ON);
//        setLedStatus(3, 2, 0, ON);
//        setLedStatus(3, 3, 0, ON);
        
      break;
    case 1:      
      oneColumnAtATimeAnimation(random(50, 500));
      break;
    case 2:
      oneAtATimeAnimation(random(50, 500));
      break;
    case 3:
      allLEDOnAnimation(3000);
      break;
    case 4:
      flickerAnimation(random(10), 5000);
      break;
    case 5:
      flickerAnimation(random(100), 3000);
      break;
    case 6:
      cubeEdgeAnimation(5000);
      break;
    case 7:
      rainAnimation(random(1, 3));
      break;
    default:
    break;
  }
  
  updateLEDs();
}
////////////////////////////////////////////////////////////////////
//ANIMATIONS
///////////////////////////////////////////////////////////////////

//helper Function to set all LED status to ON 
void allON(){
  for(int layer = 0; layer < 4; ++layer){
    for(int x = 0; x < 4; ++x){
      for(int y  = 0; y < 4; ++y){
        setLedStatus(x, y, layer, ON);
      }
    }
  }
}

//helper function to set all LED status to OFF
void allOFF(){
  for(int layer = 0; layer < 4; ++layer){
    for(int x = 0; x < 4; ++x){
      for(int y  = 0; y < 4; ++y){
        setLedStatus(x, y, layer, OFF);
      }
    }
  }
}
void allLEDOnAnimation(int totalTime){
  if((millis() - animationTimer) < totalTime){
    allON();
  }else{
    animationEnded = true;
  }
}


//animation to turn on one column at a time
void oneColumnAtATimeAnimation(int speed){
  if((millis() - animationTimer) > speed){
    allOFF();
    int x = animationCounter / 4;
    int y = animationCounter % 4;
    setLedStatus(x , y , 0, ON);
    setLedStatus(x , y,  1, ON);
    setLedStatus(x , y , 2, ON);
    setLedStatus(x , y , 3, ON);
    animationCounter++;
    if(animationCounter > 16){
      animationCounter = 0;
      animationEnded = true;
    }
    animationTimer = millis();
  }
}

//animation to turn on one LED at a time
void oneAtATimeAnimation( int speed){
  if((millis() - animationTimer) > speed){
    allOFF();
    int layer = animationCounter / 16;
    int x = animationCounter / 4 % 4;
    int y = animationCounter % 4;
    setLedStatus(x , y , layer, ON);
    animationCounter++;
    
    if(animationCounter > 64){
      animationCounter = 0;
      animationEnded = true;
    }
    animationTimer = millis();
  }
}

bool flickerAnimationStatus_ledON = false;
unsigned long flickerAnimationEndTime = 0;
void flickerAnimation(int animationSpeed, int duration){ 
  if(animationCounter == 0){
    flickerAnimationEndTime = millis() + duration;
  }
  
  if((millis() - animationTimer) > animationSpeed){
    (flickerAnimationStatus_ledON)? allOFF() :  allON();
    flickerAnimationStatus_ledON = !(flickerAnimationStatus_ledON);
    animationCounter++;
    animationTimer = millis();
  }
  if(millis() > flickerAnimationEndTime){
      animationEnded = true;
      animationCounter = 0;
  }
}
void cubeEdgeAnimation(int duration){
  if((millis() - animationTimer) < duration){
    allOFF();
    for(int x = 0; x < 4; ++x){
      setLedStatus(x, 0, 0, ON); //a
      setLedStatus(x, 3, 0, ON);
      setLedStatus(x, 0, 3, ON);
      setLedStatus(x, 3, 3, ON);
    }
    for(int y = 0; y < 4; ++y){
      setLedStatus(0, y, 0, ON);
      setLedStatus(3, y, 0, ON);
      setLedStatus(0, y, 3, ON);
      setLedStatus(3, y, 3, ON);
    }
    for(int z = 0; z < 4; ++z){
      setLedStatus(0, 0, z, ON);
      setLedStatus(0, 3, z, ON);
      setLedStatus(3, 0, z, ON);
      setLedStatus(3, 3, z, ON);
    }
  }else{
    animationEnded = true;
  }
}

void remove1By1(int speed){
  allON();
  speed = abs(1000 - speed);
  for(int i = 0; i < 4; i++){
    digitalWrite(layer[i], LAYERON);
      for(int i = 0; i < 16; i++){
        digitalWrite(column[i], COLUMNOFF);
        delay(speed);       
      } 
    digitalWrite(layer[i], LAYEROFF);
  }
  allOFF();
}



int rainAnimationLED_X[16];
int rainAnimationLED_Y[16];
int rainAnimation_amount = 0;
void rainAnimation(int rainAmount)
{
 if(animationCounter == 0){
    rainAmount = min(16, rainAmount);
    animationCounter ++;
    animationTimer = millis();
    rainAnimation_amount = rainAmount;
    
    for(int i = 0; i < rainAnimation_amount; i++){
      rainAnimationLED_X[i] = random(0, 4);
      rainAnimationLED_Y[i] = random(0, 4);
    }
 }
 unsigned long duration = (millis() - animationTimer);
 if( duration < 100){  //from 0 - 99 milli second
    allOFF();
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 3, ON);   //layer 3
    }
 }else if(duration < 150){ //from 101 - 149 milli second
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 2,  ON);  // layer 3 + 2
    }
 }else if(duration < 200){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 3,  OFF); //layer 2
    }
 }else if(duration < 250){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 1, ON);  //layer 2 + 1
    }
 }else if(duration < 300){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 2, OFF); //layer 1
    }
 }else if(duration < 350){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 0, ON); //layer 1 + 0
    }
 }else if(duration < 400){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 1, OFF); //layer 0
    }
 }else if(duration < 450){
  animationCounter = 0;
  animationEnded = true;
 }
}


void layerFallDownAnimation(){
  if(animationCounter == 0){
    animationCounter ++;
    animationTimer = millis();
 }
 unsigned long duration = (millis() - animationTimer);
 if( duration < 100){  //from 0 - 99 milli second
    allOFF();
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 3, ON);   //layer 3
    }
 }else if(duration < 150){ //from 101 - 149 milli second
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 2,  ON);  // layer 3 + 2
    }
 }else if(duration < 200){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 3,  OFF); //layer 2
    }
 }else if(duration < 250){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 1, ON);  //layer 2 + 1
    }
 }else if(duration < 300){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 2, OFF); //layer 1
    }
 }else if(duration < 350){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 0, ON); //layer 1 + 0
    }
 }else if(duration < 400){
    for(int i = 0; i < rainAnimation_amount; ++i){
      setLedStatus(rainAnimationLED_X[i], rainAnimationLED_Y[i], 1, OFF); //layer 0
    }
 }else if(duration < 450){
  animationCounter = 0;
  animationEnded = true;
 }
}

//void timedAnimation()
//{
// if(animationCounter == 0){
//    rainAnimationLED_XY = random(0, 16);
//    animationCounter ++;
// }
// unsigned long duration = (millis() - animationTimer);
// if( duration < 100){  //from 0 - 99 milli second
//    allOFF();
//   
// }else if(duration < 150){ //from 101 - 149 milli second
//  
// }else if(duration < 250){
//  
// }else if(duration < 400){
//  
// }else{
//  animationEnded = true;
// }
//}

//xxxxxxxxxxxxxxxxxxxxFUNCTIONSxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


////////////////turn everything on and off by layer up and down NOT TIMED
//void turnOnAndOffAllByLayerUpAndDownNotTimed()
//{
//  int x = 75;
//  for(int i = 5; i != 0; i--)
//  {
//    turnEverythingOn();
//    for(int i = 4; i!=0; i--)
//    {
//      digitalWrite(layer[i-1], 0);
//      delay(x);
//    }
//    for(int i = 0; i<4; i++)
//    {
//      digitalWrite(layer[i], 1);
//      delay(x);
//    }
//      for(int i = 0; i<4; i++)
//    {
//      digitalWrite(layer[i], 0);
//      delay(x);
//    }
//    for(int i = 4; i!=0; i--)
//    {
//      digitalWrite(layer[i-1], 1);
//      delay(x);
//    }
//  }
//}
////////////////////////////turn everything on and off by column sideways
//void turnOnAndOffAllByColumnSideways()
//{
//  int x = 75;
//  turnEverythingOff();
//  //turn on layers
//  for(int i = 0; i<4; i++)
//  {
//    digitalWrite(layer[i], 1);
//  }
//  for(int y = 0; y<3; y++)
//  {
//    //turn on 0-3
//    for(int i = 0; i<4; i++)
//    {
//      digitalWrite(column[i], 0);
//      delay(x);
//    }
//    //turn on 4-7
//    for(int i = 4; i<8; i++)
//    {
//      digitalWrite(column[i], 0);
//      delay(x);
//    }
//    //turn on 8-11
//    for(int i = 8; i<12; i++)
//    {
//      digitalWrite(column[i], 0);
//      delay(x);
//    }
//    //turn on 12-15
//    for(int i = 12; i<16; i++)
//    {
//      digitalWrite(column[i], 0);
//      delay(x);
//    }
//    //turn off 0-3
//    for(int i = 0; i<4; i++)
//    {
//      digitalWrite(column[i], 1);
//      delay(x);
//    }
//    //turn off 4-7
//    for(int i = 4; i<8; i++)
//    {
//      digitalWrite(column[i], 1);
//      delay(x);
//    }
//    //turn off 8-11
//    for(int i = 8; i<12; i++)
//    {
//      digitalWrite(column[i], 1);
//      delay(x);
//    }
//    //turn off 12-15
//    for(int i = 12; i<16; i++)
//    {
//      digitalWrite(column[i], 1);
//      delay(x);
//    }
//    //turn on 12-15
//    for(int i = 12; i<16; i++)
//    {
//      digitalWrite(column[i], 0);
//      delay(x);
//    }
//    //turn on 8-11
//    for(int i = 8; i<12; i++)
//    {
//      digitalWrite(column[i], 0);
//      delay(x);
//    }
//    //turn on 4-7
//    for(int i = 4; i<8; i++)
//    {
//      digitalWrite(column[i], 0);
//      delay(x);
//    }
//    //turn on 0-3
//    for(int i = 0; i<4; i++)
//    {
//      digitalWrite(column[i], 0);
//      delay(x);
//    }
//    //turn off 12-15
//    for(int i = 12; i<16; i++)
//    {
//      digitalWrite(column[i], 1);
//      delay(x);
//    }
//    //turn off 8-11
//    for(int i = 8; i<12; i++)
//    {
//      digitalWrite(column[i], 1);
//      delay(x);
//    }
//    //turn off 4-7
//    for(int i = 4; i<8; i++)
//    {
//      digitalWrite(column[i], 1);
//      delay(x);
//    }
//    //turn off 0-3
//    for(int i = 0; i<4; i++)
//    {
//      digitalWrite(column[i], 1);
//      delay(x);
//    }
//  }
//}
///////////////////////////////////////////up and down single layer stomp
//void layerstompUpAndDown()
//{
//  int x = 75;
//  for(int i = 0; i<4; i++)
//  {
//    digitalWrite(layer[i], 0);
//  }
//  for(int y = 0; y<5; y++)
//  {
//    for(int count = 0; count<1; count++)
//    { 
//      for(int i = 0; i<4; i++)
//      {
//        digitalWrite(layer[i], 1);
//        delay(x);
//        digitalWrite(layer[i], 0);
//      }
//      for(int i = 4; i !=0; i--)
//      {
//        digitalWrite(layer[i-1], 1);
//        delay(x);
//        digitalWrite(layer[i-1], 0);
//      }
//    }
//    for(int i = 0; i<4; i++)
//    {
//      digitalWrite(layer[i], 1);
//      delay(x);
//    }
//    for(int i = 4; i!=0; i--)
//    {
//      digitalWrite(layer[i-1], 0);
//      delay(x);
//    }
//  }
//}


///////////////////////////////////////////////////////diagonal rectangle
//void diagonalRectangle()
//{
//  int x = 350;
//  turnEverythingOff();
//  for(int count = 0; count<5; count++)
//  {
//    //top left
//    for(int i = 0; i<8; i++)
//    {
//      digitalWrite(column[i], 0);
//    }
//    digitalWrite(layer[3], 1);
//    digitalWrite(layer[2], 1);
//    delay(x);
//    turnEverythingOff();
//    //middle middle
//    for(int i = 4; i<12; i++)
//    {
//      digitalWrite(column[i], 0);
//    }
//    digitalWrite(layer[1], 1);
//    digitalWrite(layer[2], 1);
//    delay(x);
//    turnEverythingOff();
//    //bottom right
//    for(int i = 8; i<16; i++)
//    {
//      digitalWrite(column[i], 0);
//    }
//    digitalWrite(layer[0], 1);
//    digitalWrite(layer[1], 1);
//    delay(x);
//    turnEverythingOff();
//    //bottom middle
//    for(int i = 4; i<12; i++)
//    {
//      digitalWrite(column[i], 0);
//    }
//    digitalWrite(layer[0], 1);
//    digitalWrite(layer[1], 1);
//    delay(x);
//    turnEverythingOff();
//    //bottom left
//    for(int i = 0; i<8; i++)
//    {
//      digitalWrite(column[i], 0);
//    }
//    digitalWrite(layer[0], 1);
//    digitalWrite(layer[1], 1);
//    delay(x);
//    turnEverythingOff();
//    //middle middle
//    for(int i = 4; i<12; i++)
//    {
//      digitalWrite(column[i], 0);
//    }
//    digitalWrite(layer[1], 1);
//    digitalWrite(layer[2], 1);
//    delay(x);
//    turnEverythingOff();
//    //top right
//    for(int i = 8; i<16; i++)
//    {
//      digitalWrite(column[i], 0);
//    }
//    digitalWrite(layer[2], 1);
//    digitalWrite(layer[3], 1);
//    delay(x);
//    turnEverythingOff();
//    //top middle
//    for(int i = 4; i<12; i++)
//    {
//      digitalWrite(column[i], 0);
//    }
//    digitalWrite(layer[2], 1);
//    digitalWrite(layer[3], 1);
//    delay(x);
//    turnEverythingOff();
//  }
//  //top left
//  for(int i = 0; i<8; i++)
//  {
//    digitalWrite(column[i], 0);
//  }
//  digitalWrite(layer[3], 1);
//  digitalWrite(layer[2], 1);
//  delay(x);
//  turnEverythingOff();
//}
////////////////////////////////////////////////////////////////propeller
//void propeller()
//{
//  turnEverythingOff();
//  int x = 90;
//  for(int y = 4; y>0; y--)
//  {
//    for(int i = 0; i<6; i++)
//    {
//      //turn on layer
//      digitalWrite(layer[y-1], 1);
//      //a1
//      turnColumnsOff();
//      digitalWrite(column[0], 0);
//      digitalWrite(column[5], 0);
//      digitalWrite(column[10], 0);
//      digitalWrite(column[15], 0);
//      delay(x);
//      //b1
//      turnColumnsOff();
//      digitalWrite(column[4], 0);
//      digitalWrite(column[5], 0);
//      digitalWrite(column[10], 0);
//      digitalWrite(column[11], 0);
//      delay(x);
//      //c1
//      turnColumnsOff();
//      digitalWrite(column[6], 0);
//      digitalWrite(column[7], 0);
//      digitalWrite(column[8], 0);
//      digitalWrite(column[9], 0);
//      delay(x);
//      //d1
//      turnColumnsOff();
//      digitalWrite(column[3], 0);
//      digitalWrite(column[6], 0);
//      digitalWrite(column[9], 0);
//      digitalWrite(column[12], 0);
//      delay(x);
//      //d2
//      turnColumnsOff();
//      digitalWrite(column[2], 0);
//      digitalWrite(column[6], 0);
//      digitalWrite(column[9], 0);
//      digitalWrite(column[13], 0);
//      delay(x);
//      //d3
//      turnColumnsOff();
//      digitalWrite(column[1], 0);
//      digitalWrite(column[5], 0);
//      digitalWrite(column[10], 0);
//      digitalWrite(column[14], 0);
//      delay(x);
//    }
//  }
//  //d4
//  turnColumnsOff();
//  digitalWrite(column[0], 0);
//  digitalWrite(column[5], 0);
//  digitalWrite(column[10], 0);
//  digitalWrite(column[15], 0);
//  delay(x);
//}
////////////////////////////////////////////////////////spiral in and out
//void spiralInAndOut()
//{
//  turnEverythingOn();
//  int x = 60;
//  for(int i = 0; i<6; i++)
//  {
//    //spiral in clockwise
//    digitalWrite(column[0], 1);
//    delay(x);
//    digitalWrite(column[1], 1);
//    delay(x);
//    digitalWrite(column[2], 1);
//    delay(x);
//    digitalWrite(column[3], 1);
//    delay(x);
//    digitalWrite(column[7], 1);
//    delay(x);
//    digitalWrite(column[11], 1);
//    delay(x);
//    digitalWrite(column[15], 1);
//    delay(x);
//    digitalWrite(column[14], 1);
//    delay(x);
//    digitalWrite(column[13], 1);
//    delay(x);
//    digitalWrite(column[12], 1);
//    delay(x);
//    digitalWrite(column[8], 1);
//    delay(x);
//    digitalWrite(column[4], 1);
//    delay(x);
//    digitalWrite(column[5], 1);
//    delay(x);
//    digitalWrite(column[6], 1);
//    delay(x);
//    digitalWrite(column[10], 1);
//    delay(x);
//    digitalWrite(column[9], 1);
//    delay(x);
//    ///////////////////////////////////////spiral out counter clockwise
//    digitalWrite(column[9], 0);
//    delay(x);
//    digitalWrite(column[10], 0);
//    delay(x);
//    digitalWrite(column[6], 0);
//    delay(x);
//    digitalWrite(column[5], 0);
//    delay(x);
//    digitalWrite(column[4], 0);
//    delay(x);
//    digitalWrite(column[8], 0);
//    delay(x);
//    digitalWrite(column[12], 0);
//    delay(x);
//    digitalWrite(column[13], 0);
//    delay(x);
//    digitalWrite(column[14], 0);
//    delay(x);
//    digitalWrite(column[15], 0);
//    delay(x);
//    digitalWrite(column[11], 0);
//    delay(x);
//    digitalWrite(column[7], 0);
//    delay(x);
//    digitalWrite(column[3], 0);
//    delay(x);
//    digitalWrite(column[2], 0);
//    delay(x);
//    digitalWrite(column[1], 0);
//    delay(x);
//    digitalWrite(column[0], 0);
//    delay(x);
//    ///////////////////////////////////////spiral in counter clock wise
//    digitalWrite(column[0], 1);
//    delay(x);
//    digitalWrite(column[4], 1);
//    delay(x);
//    digitalWrite(column[8], 1);
//    delay(x);
//    digitalWrite(column[12], 1);
//    delay(x);
//    digitalWrite(column[13], 1);
//    delay(x);
//    digitalWrite(column[14], 1);
//    delay(x);
//    digitalWrite(column[15], 1);
//    delay(x);
//    digitalWrite(column[11], 1);
//    delay(x);
//    digitalWrite(column[7], 1);
//    delay(x);
//    digitalWrite(column[3], 1);
//    delay(x);
//    digitalWrite(column[2], 1);
//    delay(x);
//    digitalWrite(column[1], 1);
//    delay(x);
//    digitalWrite(column[5], 1);
//    delay(x);
//    digitalWrite(column[9], 1);
//    delay(x);
//    digitalWrite(column[10], 1);
//    delay(x);
//    digitalWrite(column[6], 1);
//    delay(x);
//    //////////////////////////////////////////////spiral out clock wise
//    digitalWrite(column[6], 0);
//    delay(x);
//    digitalWrite(column[10], 0);
//    delay(x);
//    digitalWrite(column[9], 0);
//    delay(x);
//    digitalWrite(column[5], 0);
//    delay(x);
//    digitalWrite(column[1], 0);
//    delay(x);
//    digitalWrite(column[2], 0);
//    delay(x);
//    digitalWrite(column[3], 0);
//    delay(x);
//    digitalWrite(column[7], 0);
//    delay(x);
//    digitalWrite(column[11], 0);
//    delay(x);
//    digitalWrite(column[15], 0);
//    delay(x);
//    digitalWrite(column[14], 0);
//    delay(x);
//    digitalWrite(column[13], 0);
//    delay(x);
//    digitalWrite(column[12], 0);
//    delay(x);
//    digitalWrite(column[8], 0);
//    delay(x);
//    digitalWrite(column[4], 0);
//    delay(x);
//    digitalWrite(column[0], 0);
//    delay(x);
//  }
//}

