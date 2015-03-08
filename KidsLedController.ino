
/*
  Led controller for arduino uno.
 
  This was created to control RGB strip lights for a child's playhouse.  

  Christopher Gannon
  3/8/2015
  
  This code is in the public domain.

  Thanks to adafruit for inspiring the project and providing the basis of the circuit board.  https://learn.adafruit.com/rgb-led-strips/
  

*/



//Define RGB Pin Outputs
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3

//Define Button Inputs
#define REDBUTTON A0
#define ORANGEBUTTON 4
#define GREENBUTTON A2
#define YELLOWBUTTON 8
#define BLUEBUTTON A1
#define PURPLEBUTTON 7
#define WHITEBUTTON A3
#define BLACKBUTTON A4
#define RANDOMBUTTON A5

//Define Time until lights automatically turn off
#define AUTOOFFSECONDS 10

//Delay between changes in LED color.
#define FADESPEED 50 

//Number of steps system will use to go from color A to color B.
#define NUMSTEPS 20

//Playing around with FADEFEED and NUMSTEPS should give you the effect you want.


struct ButtonAssignment
{
   int pin;
   int red;
   int green;
   int blue;
};

//Structure to handle RGB values with = overload.
struct RGB {
  
     RGB& operator =(const RGB& a) 
     {
        red = a.red;
        green = a.green;
        blue = a.blue;
        return *this;
    }

  int red;
  int green;
  int blue;
};


//Define RGB light currentColor and target colors and initial steprate.
RGB currentColor = {255,255,255};
RGB targetColor = {255,255,255};
RGB StepRateforColor = {0,0,0};




ButtonAssignment buttonColors[9];
int isPushed[9];
int enableUpdates=1;
int timeuntilshutoff = AUTOOFFSECONDS*1000;
int isShutoff = 0;
int flashSwitch=0;


 //Setup is run once as the Arduino boots.
void setup() {
  
  	//Set the pinmode of the LED lights to output
  	pinMode(REDPIN, OUTPUT);
  	pinMode(GREENPIN, OUTPUT);
  	pinMode(BLUEPIN, OUTPUT);

  	//Set the pinmode of the buttons to INPUT_PULLUP
  	pinMode(REDBUTTON, INPUT_PULLUP);
  	pinMode(ORANGEBUTTON, INPUT_PULLUP);
  	pinMode(GREENBUTTON, INPUT_PULLUP);  
  	pinMode(YELLOWBUTTON, INPUT_PULLUP);
  	pinMode(BLUEBUTTON, INPUT_PULLUP);
  	pinMode(PURPLEBUTTON, INPUT_PULLUP);
  	pinMode(WHITEBUTTON, INPUT_PULLUP);  
  	pinMode(BLACKBUTTON, INPUT_PULLUP);  
  	pinMode(RANDOMBUTTON, INPUT_PULLUP);  
  
  
    buttonColors[0]= {REDBUTTON, 255,0, 0};
    buttonColors[1]= {BLUEBUTTON, 0, 0, 255};
    buttonColors[2]= {GREENBUTTON, 0, 255, 0};
    buttonColors[3]= {ORANGEBUTTON, 255,102, 0};
    buttonColors[4]= {YELLOWBUTTON,255, 255, 0};
    buttonColors[5]= {PURPLEBUTTON, 255, 0, 255};
    buttonColors[6]= {WHITEBUTTON, 255, 255, 255};
    buttonColors[7]= {BLACKBUTTON, 0, 0, 0};
    buttonColors[8]= {RANDOMBUTTON, random(0,255),random(0,255),random(0,255)};

	#define NUMBEROFBUTTONS 9
    
}


//Resets the shutoff values if a button is pressed.
void doResetShutoff()
{
   	isShutoff=0;
   	timeuntilshutoff = AUTOOFFSECONDS*1000;     
}


//Updates the target color of the LED lights to the given color
void updateTargetColor(int r1, int g1, int b1)

{

      targetColor = {r1,g1,b1};
      //The steprate is different for each color channel so the colors transition smoothly
      StepRateforColor = {((targetColor.red-currentColor.red)/NUMSTEPS), ((targetColor.green-currentColor.green)/NUMSTEPS), ((targetColor.blue-currentColor.blue)/NUMSTEPS)};
}

void updateLiveColors()
{
       
     if (abs(targetColor.red-currentColor.red) <= abs(StepRateforColor.red))
      { 
        currentColor.red=targetColor.red;
      }
      else
      {
        currentColor.red=currentColor.red+StepRateforColor.red; 
      }
      
     if (abs(targetColor.blue-currentColor.blue) <= abs(StepRateforColor.blue))
      { 
        currentColor.blue=targetColor.blue;
      }
      else
      {
        currentColor.blue=currentColor.blue+StepRateforColor.blue; 
      }
      
     if (abs(targetColor.green-currentColor.green) <= abs(StepRateforColor.green))
      { 
        currentColor.green=targetColor.green;
      }
      else
      {
        currentColor.green=currentColor.green+StepRateforColor.green; 
      }
}

void loop() {

  //Reset values
  int count=0;
  for (int i=0;i<NUMBEROFBUTTONS;i++)
  {
     isPushed[i]=0; 
  }

  //Checks to see if any buttons are pressed.  Multiple button presses are supported.
  for (int i=0;i<NUMBEROFBUTTONS;i++)
  {
      if (digitalRead(buttonColors[i].pin)==LOW)
      {
         count=count+1;
         isPushed[i]=1;
      }
  }

  //If any buttons have been pressed, reset the shutoff.
  if (count>0) doResetShutoff();

  //Since button 8 is the "random color" button, this resets the random color if it was pressed.
  if (isPushed[8]==1)
  {
      buttonColors[8]= {RANDOMBUTTON, random(0,255),random(0,255),random(0,255)};
  }
        
  //If only a single button is pressed, set a new target value and allow the LEDs to transtion to new color.
  if (count==1)
  {
    enableUpdates=1;
    for ( int i=0;i<NUMBEROFBUTTONS;i++)
    {
        if (isPushed[i]==1)
        {
           updateTargetColor(buttonColors[i].red,buttonColors[i].green,buttonColors[i].blue);
        }
    }
  }
  
   
  //If multple buttons are pressed, this code flashes between the pressed colors without a transition. 
  if (count>1)
  {
    enableUpdates=0;
     if (flashSwitch>=count) flashSwitch=0;
     int flashColors[10];
     int j=0;
     for ( int i=0;i<NUMBEROFBUTTONS;i++)
      {
        if (isPushed[i]==1)
        {
           flashColors[j] =  i;
           j++;
        }
      }
     currentColor.red = targetColor.red = buttonColors[flashColors[flashSwitch]].red;
     currentColor.green = targetColor.green = buttonColors[flashColors[flashSwitch]].green;
     currentColor.blue = targetColor.blue = buttonColors[flashColors[flashSwitch]].blue;
     flashSwitch++; 
  }
  
   //Update the colors of the LEDs.
   if (enableUpdates==1)  { updateLiveColors();}
   
  //If the shutoff time has passed, set the target color to black (off).
   if (timeuntilshutoff<0 && isShutoff==0)
   {
      isShutoff=1;
      enableUpdates=1;
      updateTargetColor(0,0,0);
   } 
    else
   {
    timeuntilshutoff=timeuntilshutoff-FADESPEED;
   }

    //These lines actually update the LEDs themselves with the current colors.
    analogWrite(REDPIN, currentColor.red);
    analogWrite(GREENPIN, currentColor.green);       
    analogWrite(BLUEPIN, currentColor.blue);

    //Delay according to fadespeed.  During flash mode, an additional fade is added.
    delay(FADESPEED);
    if (count>1) delay(FADESPEED*5);
    
}
