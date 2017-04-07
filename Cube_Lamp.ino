#include <Adafruit_NeoPixel.h>
#include <IRremote.h>
#include "buttons.h"

#define LED_PIN 2  //2 on Cube  //3 on proto
#define RECV_PIN 10 //10 on Cube //6 on proto

IRrecv irrecv(RECV_PIN);
decode_results results;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, LED_PIN, NEO_GRB + NEO_KHZ800);

int default_intensity = 25;
int input = 0; //records remote input
float input_rate = 250; //usually is 250 for remote
int rate = 1;

String interval;

enum AnimationMode {
  FULL_ON,       //On Button
  BREATHE,       //
  BLINK,         //Flash Button
  CYCLE,         //Strobe Button
  CYCLE_BREATHE, //Fade Button
  MIX,            //Smooth Button
  SIRENS
} mode = FULL_ON;

typedef struct {
  int R;
  int G;
  int B;
  int intensity;
} RGB;

RGB color, colorPrime;


void setup() {
  color = {0, 0, 0, default_intensity}; //initialize pixels to off with a pre-set intensity
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  irrecv.enableIRIn(); // Start the receiver
  Serial.begin(9600);
  //setup_mode();
}

int Clamp(int x) { //Clamp sets my color input to a value between
  if (x <= 0)      // 0 and 255, it won't allow it to go over or under.
    return 0;

  else if (x > 255)
    return 255;

  else
    return x;
}

int Check_Input(int y) { //Check makes sure that my remote input can't 
  if (y <= 0)      // go over or under a certain range. 
    return 1;      // I need both Clamp and Check because with only 'Clamp'
                   // certain values will screw up the end LED output. 
  else if (y >= 255)
    return 255;

  else
    return y;
}

void increaseIntensity() {
  color.intensity = Check_Input(color.intensity + 20);
}

void decreaseIntensity() {
  color.intensity = Check_Input(color.intensity - 20);
}

void loop(){
  //Serial.println("Take Input : ");
  TakeInput();
  Interpret();
  AnimationStep();
  SetPixels();
  
  //Serial.println("**********I MADE IT HERE!!!!**********");
}

void AnimationStep() {
  switch (mode) {
    case BREATHE:
      Serial.println("BREATHE");
      Breathe();
      break;

    case BLINK:
       Serial.println("BLINK");
      Blink();
      break;

    case CYCLE:
      Serial.println("CYCLE");
      Cycle();
      break;
      
    case MIX:
      Serial.println("MIX");
      Mix();
      break;      
      
    case CYCLE_BREATHE:
      Serial.println("CYCLE_BREATHE");
      CycleBreathe();
      break;
      
    case SIRENS:
      Serial.println("SIRENS");
      Sirens();
      break;
      
      
    case FULL_ON:
      //Serial.println("AnimationStep = default");
      Full_On();
      break;
  }
}

void TakeInput() {

    if (irrecv.decode(&results)) {
      Serial.print("got input: ");
      input = (results.value);
      Serial.println(input, HEX);
      delayMicroseconds(500);
    }
    
    else {
       //Serial.println("no input");
       //Note  should this say?
       //input = NULL;
    }
    
      irrecv.resume(); // Receive the next value
      delay(input_rate);
  
}


void Interpret() {
  switch (input) {
    //Serial.println(input, HEX);
      case RED:
        color.R = 5,color.G = color.B = 0;
        break;

      case GREEN:
        color.R = color.B = 0, color.G = 5;
        break;

      case BLUE:
        color.R = color.G = 0, color.B = 5;
        break;
        
      case WHITE:
        color.R = color.G = color.B = 3;
        break;
        
      case DARKORANGE:
        color.R = 5, color.G = 3, color.B = 0;
        break;

      case TURQUOISE:
        color.R = 0, color.G = 10, color.B = 5;
        break;       
         
      case DARKBLUE:
        color.R = 0, color.G = 0, color.B = 5;
        break;
        
      case ORANGE:
        color.R = 10, color.G = 5, color.B = 0;
        break;
       
      case CYAN:
        color.R = 0, color.G = color.B = 5;
        break;

      case PURPLE:
        color.R = 3, color.G = 0, color.B = 5;
        break;
        
      case YELLOWORANGE:
        color.R = 10, color.G = 7, color.B = 5;
        break;
        
      case LIGHTBLUE:
        color.R = 0, color.G = 5, color.B = 10;
        break;

      case LIGHTPURPLE:
        color.R = 5, color.G = 0, color.B = 10;
        break;

      case YELLOW:
        color.R = 4, color.G = 6, color.B = 0;
        break;

      case SKYBLUE:
        color.R = 0, color.G = 0, color.B = 5;
        break;

      case PINK:
        color.R = 10, color.G = 0, color.B = 5;
        break;

      case UP:
        increaseIntensity();
        break;

      case DOWN:
        decreaseIntensity();
        break;

      case OFF:
        color.R = color.G = color.B = 0;
        break;
        
      case ON:
        mode = FULL_ON;
        break; 

      case FLASH:
        mode = BLINK;      
        break; 
               
      case STROBE:
        mode = CYCLE;
        break; 
        
      case FADE:
        mode = CYCLE_BREATHE;
        break;   
                     
      case SMOOTH:
        mode = MIX;
        break;

      case SIRENS:
        mode = SIRENS;
        break;

      case SOMEBUTTON:   //made up place holder for this option
        mode = BREATHE;
        break;
    }
    input = 0;
}


void Full_On() {
  colorPrime = color;
}


void Breathe() {

  static enum {
    inhale,
    exhale
  } respiration = exhale;
  
  static int brightness = 255;

  if (brightness > color.intensity) {
    brightness = color.intensity;
    respiration = exhale;
  }

  if (respiration == exhale) {
    if (brightness == 0) {
      respiration = inhale;
      brightness++;
    }
    else
      brightness--;
  }

  else {
    if (brightness == color.intensity) {
      respiration = exhale;
      brightness--;
    }
    else brightness++;
  }

  // use the value of colorPrime for output
  colorPrime = color;
  colorPrime.intensity = brightness;
  
  Serial.print("brightness = ");
  Serial.println(brightness);

  //Serial.print("color.intensity = ");
 // Serial.println(color.intensity);
  
  //rate = .07; 
}


void Blink() {
  static int brightness = color.intensity;
  static enum {
    on,
    off,
    middle
  } type = on;

  if (type == on) {
    delay(200 * rate); //500
    brightness = color.intensity;
    type = middle;
  }

  else if (type == middle) {
    type = off;
  }

  else {
    delay(200 * rate);
    brightness = 0;
    type = on;
  }

  colorPrime = color;
  colorPrime.intensity = brightness;
}


void Cycle(){
  ColorChange();
  colorPrime = color;
  delay(50*rate); //100
}


void Mix(){
 
  static enum {
  count_down
} RGB_value = count_down;

int Max_count = 768;
static int brightness = Max_count;

if (RGB_value == count_down) {
    if (brightness == 0) {
      brightness = Max_count;
    }
    else
      brightness--;
  }

if (brightness <= (Max_count/3))                         // <256    // Red to Yellow
  {
    color.R = ((Max_count/3) - brightness);                         // red goes from on to off
    color.G = brightness;                                           // green goes from off to on
    color.B = 0;                                                    // blue is always off
  }

else if (brightness <= ((Max_count/3)*2))                // <512    // Green to Blue
  {
    color.R = 0;                                                    // red is always off
    color.G = ((Max_count/3) - (brightness - (Max_count/3)));       // green on to off
    color.B = (brightness - (Max_count/3));                         // blue off to on
  }

else if (brightness >= ((Max_count/3)*2))                // <768    // Purple to Red
  {
    color.R = (brightness - ((Max_count/3)*2));                     // red off to on
    color.G = 0;                                                    // green is always off
    color.B = 255 - (brightness - ((Max_count/3)*2));               // blue on to off
  }
  
  Serial.print("brightness = ");
  Serial.println(brightness);
  
  colorPrime = color;
  colorPrime.intensity = 1;
  input_rate = 1;
  delay(20);
}



void CycleBreathe(){ //NEed to work on this code, as of now resperatio = inhale doesn't do anything. 

  static enum {
   inhale,
   exhale 
  }respiration = exhale;
  
  static int brightness = 25;
 //Serial.println(color.intensity);
  //Serial.println(brightness);


if (brightness > color.intensity){
    brightness = color.intensity;
    respiration = exhale;
}

if (respiration == exhale){
  if (brightness == 0){
    respiration = inhale;
        
    Serial.print("Change Color = ");
    ColorChange();
    brightness++;
  }
  
  else 
    brightness--;
}

else{
  if (brightness == color.intensity){
    respiration = exhale;
    brightness--;
  }
  else brightness++;
}
 
  colorPrime = color;
  colorPrime.intensity = brightness;
  delayMicroseconds(50);
    //Serial.print("brightness = ");
    //Serial.println(brightness);
    
  //Serial.println(brightness);
  //delay(10*rate); //100
  
}

void Sirens(){
 
int siren_time = 100;

  for(int i = 0; i <= 2; i++){
    color.R = 5,color.G = color.B = 0;
    delay(siren_time);
    
    color.R = color.G = color.B = 0;
    delay(siren_time);
  }

  for(int i = 0; i <= 2; i++){
    color.R = color.G = 0, color.B = 5;
    delay(siren_time);
    
    color.R = color.G = color.B = 0;
    delay(siren_time);
  }

    colorPrime = color;
}


int ColorChange(){
   static enum{
    red,
    green,
    blue,
    yellow,
    purple,
    cyan,
    white
  }interval = yellow;
  
    if (interval == red)
    {
      Serial.println("red");
      input = RED;
      interval = green;
    }
  
    else if (interval == green)
    {
      Serial.println("green");
      input = GREEN;
      interval = blue;
    }
  
    else if (interval == blue)
    {
      Serial.println("blue");
      input = BLUE;
      interval = yellow;
    }
  
      else if (interval == yellow)
    {
      Serial.println("yellow");
      input = YELLOW;
      interval = purple;
    }
  
      else if (interval == purple)
    {
      Serial.println("purple");
      input = PURPLE;
      interval = cyan;
    }
  
      else if (interval == cyan)
    {
      Serial.println("cyan");
      input = CYAN;
      interval = white;
    }
  
      else if (interval == white)
    {
      Serial.println("white");
      input = WHITE;
      interval = red;
    }
  return interval;
 }//end colorchange


void SetPixels() {
  //Serial.println("colorPrime.intensity");
 // Serial.println(colorPrime.intensity);
  
  int rr = colorPrime.R * colorPrime.intensity;
  int gg = colorPrime.G * colorPrime.intensity;
  int bb = colorPrime.B * colorPrime.intensity;
  
  rr = Clamp(rr);
  gg = Clamp(gg);
  bb = Clamp(bb);
  
  SetLED(strip.Color(rr, gg, bb), 0);
//  Serial.println();
//  Serial.print("Red = ");
//  Serial.println(rr);
//  
//  
//  Serial.print("Green = ");
//  Serial.println(gg);
//  
//  Serial.print("Blue = ");
//  Serial.println(bb);
}


void SetLED(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
  }
}


void setup_mode(){
//color.R = color.B = 0, color.G = 5;

//color.R=30, color.G=50, color.B=0;
default_intensity = 1;

  mode = SIRENS;
//mode = CYCLEBREATHE;
//mode = FULL_ON;
}

