#include <FastLED.h>

#define UNDERGLOW_DATA_PIN  7
#define NUM_LEDS 8

uint16_t frame = 0;      //I think I might be able to move this variable to the void loop() scope and save some CPU
uint16_t animateSpeed = 100;            //Number of frames to increment per loop
uint8_t  animation = 0;    //Active animation
uint8_t brightness = 23;    //Global brightness percentage

#define MAX_BRIGHTNESS 42
#define MAX_INT_VALUE 65536

const int brilloMedio = 42;
const int brilloAlto = 123;

CRGB leds[NUM_LEDS];

void setup() {
  //FastLED.addLeds<PIXEL_TYPE, DATE_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, UNDERGLOW_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  //RingPair(leds, frame);

  //DoubleChaser(leds, frame);

  //TripleBounce(leds, frame);

  //WaveInt(leds, frame, 180);

  //Wave(leds, frame, 180);

  //Spark(leds, frame, 255, 188); //Overloaded version of "Spark" with Hue value, 255 for fade is the slowest fade possible. 256 is on/off
  //delay(2);       //Slow things down a bit more for Slow Spark

  //Spark(leds, frame, 246, 188); //Overloaded version of "Spark" with Hue value, 246 fade is faster which makes for a sharper dropoff
  //White spark (Slow)
  //Spark(leds, frame, 255);   //"Spark" function without hue make a white spark, 255 for fade is the slowest fade possible.
  //delay(2);       //Slow things down a bit more for Slow Spark
  //White spark (fast)      //"Spark" function without hue make a white spark, 246 fade is faster which makes for a sharper dropoff
  //Spark(leds, frame, 245);

  RainbowSpark(leds, frame, 240);  //240 for dropoff is a pretty sharp fade, good for this animation

  FastLED.show();         //All animations are applied!..send the results to the leds(s)
  frame += animateSpeed;
  /*
    ida();
    respira();
    regreso();
    respira();
    pong();
    respira();
  */
}

void ida() {
  poneMedio();
  for (int i = 0; i < NUM_LEDS; i++) {
    poneMedio();
    leds[i] = CRGB(0, brilloAlto, 0);
    FastLED.show();
    delay(50);
  }
}

void regreso() {
  poneMedio();
  for (int i = NUM_LEDS; i > 0; i--) {
    poneMedio();
    leds[i] = CRGB(0, brilloAlto, 0);
    FastLED.show();
    delay(50);
  }
}

void poneMedio() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(0, brilloMedio, 0);
  }
  FastLED.show();
}

void respira() {
  for (int i = 0; i < brilloAlto; i++) {
    poneBrilloTodos(i);
    FastLED.show();
    delay(25);
  }
  for (int i = brilloAlto; i > 0; i--) {
    poneBrilloTodos(i);
    FastLED.show();
    delay(25);
  }
}

void poneBrilloTodos(int brillo) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(0, brillo, 0);
  }
}

void pong() {
  ida();
  regreso();
}

void TripleBounce(CRGB leds[], uint16_t frame)    //3 chaser animations offset by 120 degrees each
{
  FastLED.clear();    //Clear previous buffer
  Bounce(leds,frame,0);
  Bounce(leds,frame+(MAX_INT_VALUE/3),100);
  Bounce(leds,frame+(MAX_INT_VALUE/3)*2,150);
}

void DoubleChaser(CRGB leds[], uint16_t frame)   //2 chaser animations offset 180 degrees
{
  FastLED.clear();    //Clear previous buffer
  frame = frame * 2;
  Ring(leds, frame, 0);
  Ring(leds, frame + (MAX_INT_VALUE / 2), 150);
}

void RingPair(CRGB leds[], uint16_t frame)     //2 rings animations at inverse phases
{
  FastLED.clear();    //Clear previous buffer
  Ring(leds, frame, 30);
  Ring(leds, MAX_INT_VALUE - frame, 150);
}


void RainbowSpark(CRGB targetStrip[], uint16_t animationFrame,uint8_t fade){    //Color spark where hue is function of frame
  Spark(targetStrip,animationFrame,fade,animationFrame/255);
  delay(20);
}




//#######################################################################################################
//##                                PRIMATIVE ANIMATION FUNCTIONS                                      ##
//#######################################################################################################



//*********************     Bounce      ***************************
// Linear "Larson scanner" (or knight rider effect) with anti-aliasing
// Color is determined by "hue"
//*****************************************************************
void Bounce(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue)
{
  uint16_t pos16;
  if (animationFrame < (MAX_INT_VALUE / 2))
  {
    pos16 = animationFrame * 2;
  
  }else{
    pos16 = MAX_INT_VALUE - ((animationFrame - (MAX_INT_VALUE/2))*2);
  }

  int position = map(pos16, 0, MAX_INT_VALUE, 0, ((NUM_LEDS) * 16));
  drawFractionalBar(targetStrip, position, 3, hue,0);
}




//************************          Ring           ******************************
// Anti-aliased cyclical chaser, 3 pixels wide
// Color is determined by "hue"
//*****************************************************
void Ring(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue)
{
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  int pos16 = map(animationFrame, 0, MAX_INT_VALUE, 0, ((stripLength) * 16));
  drawFractionalBar(targetStrip, pos16, 3, hue,1);
}

//***************************   Wave [Float Math]  *******************************
// Squeezed sine wave  
// Uses slow, Arduino sin() function
// Squeezing achieved by using an exponential (^8) sin value
// Color is determined by "hue"
//***********************************************************************************
void Wave(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue){
  FastLED.clear();    //Clear previous buffer
  float deg; 
  float value; 
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  for(uint8_t i=0;i<stripLength;i++)
  {
    deg=float(animationFrame+((MAX_INT_VALUE/stripLength)*i))/(float(MAX_INT_VALUE)) * 360.0;
    value = pow(sin(radians(deg)),8);    //Squeeeeeeze

    if(value>=0){   //Chop sine wave (no negative values)
      targetStrip[i] += CHSV(hue,255,value*256);
    }
  } 
}


//***************************   Wave [Integer Math]  *******************************
// unadulterated sine wave.  
// Uses FastLED sin16() and no float math for efficiency. 
// Since im stuck with integer values, exponential wave-forming is not possible (unless i'm wrong???)
// Color is determined by "hue"
//***********************************************************************************
void WaveInt(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue){
  FastLED.clear();
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  uint8_t value;
  for(uint8_t i=0;i<stripLength;i++)
  {
    value=(sin16(animationFrame+((MAX_INT_VALUE/stripLength)*i)) + (MAX_INT_VALUE/2))/256;   
    if(value>=0){   
      targetStrip[i] += CHSV(hue,255,value);
    }
  } 
}


//********************************   Color Spark  ***********************************
// Color of the sparks is determined by "hue"
// Frequency of sparks is determined by global var "animateSpeed"
// "animateSpeed" var contrained from 1 - 255 (0.4% - 100%)
// "fade" parameter specifies dropoff (next frame brightness = current frame brightness * (x/256)
// fade = 256 means no dropoff, pixels are on or off
// NOTE: this animation doesnt clear the previous buffer because the fade/dropoff is a function of the previous LED state
//***********************************************************************************
void Spark(CRGB targetStrip[], uint16_t animationFrame,uint8_t fade, uint8_t hue){

  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  uint16_t rand = random16();

    for(int i=0;i<stripLength;i++)
    {   
      targetStrip[i].nscale8(fade);
    }


  if(rand < (MAX_INT_VALUE / (256 - (constrain(animateSpeed,1,256)))))  ;
  {
    targetStrip[rand % stripLength].setHSV(hue,255,255);
  }
}


//******************************       Spark       **********************************
// Same as color spark but no hue value, // in HSV white is any hue with 0 saturation
// Frequency of sparks is a percentage mapped to global var "animateSpeed"
// "animateSpeed" var contrained from 1 - 255 (0.4% - 100%)
// "fade" parameter specifies dropoff (next frame brightness = current frame brightness * (x/256)
// fade = 256 means no dropoff, pixels are on or off
// NOTE: this animation doesnt clear the previous buffer because the fade/dropoff is a function of the previous LED state
//***********************************************************************************
void Spark(CRGB targetStrip[], uint16_t animationFrame,uint8_t fade){
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  uint16_t rand = random16();
  
  for(int i=0;i<stripLength;i++)
    {   
      targetStrip[i].nscale8(fade);
    }
  

  if(rand < (MAX_INT_VALUE / (256 - (constrain(animateSpeed,1,255)))))
  {
    targetStrip[rand % stripLength].setHSV(0,0,255);
  }

}



//Anti-aliasing code care of Mark Kriegsman Google+: https://plus.google.com/112916219338292742137/posts/2VYNQgD38Pw
void drawFractionalBar(CRGB targetStrip[], int pos16, int width, uint8_t hue, bool wrap)
{
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  uint8_t i = pos16 / 16; // convert from pos to raw pixel number

  uint8_t frac = pos16 & 0x0F; // extract the 'factional' part of the position
  uint8_t firstpixelbrightness = 255 - (frac * 16);
  
  uint8_t lastpixelbrightness = 255 - firstpixelbrightness;

  uint8_t bright;
  for (int n = 0; n <= width; n++) {
    if (n == 0) {
      // first pixel in the bar
      bright = firstpixelbrightness;
    }
    else if (n == width) {
      // last pixel in the bar
      bright = lastpixelbrightness;
    }
    else {
      // middle pixels
      bright = 255;
    }

    targetStrip[i] += CHSV(hue, 255, bright );
    i++;
    if (i == stripLength)
    {
      if (wrap == 1) {
        i = 0; // wrap around
      }
      else{
        return;
      }
    }
  }
}
