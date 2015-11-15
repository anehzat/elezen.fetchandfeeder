#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>  // Include SPI if you're using SPI
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#include <Servo.h>          // include Servo library
#include <Adafruit_NeoPixel.h> //include neopixel library
Servo feederServo;
Servo launcherServo;

//////////////////////////
// MicroOLED Definition //
//////////////////////////
#define PIN_RESET 8  // Connect RST to pin 9
#define PIN_DC    7  // Connect DC to pin 8
#define PIN_CS    9 // Connect CS to pin 10
#define DC_JUMPER 0

//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
MicroOLED oled(PIN_RESET, PIN_DC, PIN_CS); // SPI declaration
//MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration

//////////////////////////////////
//   Sonar Object Declaration   //
//////////////////////////////////
#define echoPin 4 // Echo Pin
#define trigPin 5 // Trigger Pin



//////////////////////////////////
//   Neopixel     Declaration   //
//////////////////////////////////
#define PIN 10
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);
    



//#define PressureStatusLEDPin 10  // Pressure Status LED
//#define ObjectStatusLEDPin 12    // Object Status LED
#define MotorStatusLEDPin 6      // Motor Status LED used to drive relay

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

////////////////////////////////////////
// Pressure Sensor Object Declaration //
////////////////////////////////////////
long pressValue;

////////////////////////////////////////
//     Counter Object Declaration     //
////////////////////////////////////////
int countValue, feedValue, ratio;


////////////////////////////////////////
//               Setup                //
////////////////////////////////////////
void setup() {
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //pinMode(PressureStatusLEDPin, OUTPUT); // Use LED indicator (if required)
  //pinMode(ObjectStatusLEDPin, OUTPUT); // Use LED indicator (if required)
  pinMode(MotorStatusLEDPin, OUTPUT); // Use LED indicator (if required)
  
  countValue = 0;
  feedValue = 0;

  feederServo.attach(2);        // servo control pin at D6 
  launcherServo.attach(3);        // servo control pin at D6 
  
  // Start serial at 9600 baud
  Serial.begin(9600); 

 //set servo default positions
 launcherServo.write(0);
 feederServo.write(0);

 //set neopixel
 strip.begin();
  strip.setBrightness(30); //adjust brightness here
 strip.show(); // Initialize all pixels to 'off'

}


////////////////////////////////////////
//           Functions                //
////////////////////////////////////////
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

//pressure is red
void redNeo(){
 colorWipe(strip.Color(255, 0, 0), 50); // Red
 strip.show(); 
}

//pressure is green
void greenNeo(){
 colorWipe(strip.Color(0, 255, 0), 50); // Green
 strip.show(); 
}

//pressure is blue
void blueNeo(){
 colorWipe(strip.Color(0, 0, 255), 50); // Blue
 strip.show(); 
}



void sonarValue(){
   /* The following trigPin/echoPin cycle is used to determine the
   distance of the nearest object by bouncing soundwaves off of it. */ 
   digitalWrite(trigPin, LOW); 
   delayMicroseconds(2); 
  
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10); 
   
   digitalWrite(trigPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   
   //Calculate the distance (in cm) based on the speed of sound.
   distance = duration/58.2;
   
   if (distance >= maximumRange || distance <= minimumRange){
   /* Send a negative number to computer 
   to indicate "out of range" */
   Serial.print("Distance: ");
   Serial.println("-1");
   }
   else {
   /* Send the distance to the computer using Serial protocol, and
   indicate successful reading. */
   Serial.print("Dist: ");
   Serial.println(distance); 
   }
}

void pressureValue(){
   // Read the input pressure on analog pin 0:
   pressValue = analogRead(A1);
  
   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
   float voltage = pressValue * (5.0 / 1023.0);  
   Serial.print("Press: ");
   Serial.println(pressValue);
}


void calculateValue(){
   countValue ++;
   Serial.print("Count: ");
   Serial.println(countValue); 
} 
  
void updateDisplay()
{
  // Demonstrate font 2. 10x16. Only numbers and '.' are defined. 
  // This font looks like 7-segment displays.
  // Lets use this big-ish font to display readings from the
  // analog pins.
 
  
  for (int i=0; i<25; i++)
  {
    oled.clear(PAGE);           // Clear the display
    oled.setCursor(0, 0);       // Set cursor to top-left
    oled.setFontType(0);        // Smallest font
    oled.print("Press:");       // Print "A0"
    oled.setFontType(0);        // 7-segment font
    oled.print(pressValue);     // Print a0 reading

    oled.setCursor(0, 10);      // Set cursor to top-middle-left
    oled.setFontType(0);        // Repeat
    oled.print("Dist:");
    oled.setFontType(0);
    oled.print(distance);

    oled.setCursor(0, 20);
    oled.setFontType(0);
    oled.print("Count:");
    oled.setFontType(0);
    oled.print(countValue);

    oled.setCursor(0, 30);
    oled.setFontType(0);
    oled.print("Feed:");
    oled.setFontType(0);
    oled.print(feedValue);

    oled.display();
    //delay(100);
  }
  
  
  
}




void loop() {
 // Read Sonar Value
 sonarValue();
 
 // Read Pressure Value
 pressureValue();

 //clear status LEDS
 colorWipe(strip.Color(0, 0, 0), 50); // turn off Neo strip
 //digitalWrite(PressureStatusLEDPin, LOW);
 digitalWrite(MotorStatusLEDPin, LOW);
 //digitalWrite(ObjectStatusLEDPin, LOW);
 
 // check for ball
 if (pressValue > 1020) {
   greenNeo();
   //digitalWrite(PressureStatusLEDPin, HIGH); //red
   //ball is in device and ready to launch  
   //check for objects
   if (distance < 10) {
     redNeo();
     //digitalWrite(ObjectStatusLEDPin, HIGH); //green
   }
   else {
     //no objects and clear to launch
     //digitalWrite(ObjectStatusLEDPin, LOW);
     //turn on motors relay
     blueNeo();
     digitalWrite(MotorStatusLEDPin, HIGH); //drive motor relay
     delay (2000);
     //release servo
     launcherServo.write(100);;
     delay(1000);
     launcherServo.write(0);

     delay(1000);     
     //incriment counter
     countValue ++;
     //turn off motor
     digitalWrite(MotorStatusLEDPin, LOW); //drive motor relay
     colorWipe(strip.Color(0, 0, 0), 50); // turn off Neo strip

   }

 }
 else {
   colorWipe(strip.Color(0, 0, 0), 50); // turn off Neo strip
   //digitalWrite(PressureStatusLEDPin, LOW);
 }

 // number of times ball must be laucnehd before feeder is activated
 ratio = countValue / 2;
 
 //if ratio is greater than previous feed count then activate feeder
 if (feedValue < ratio) {
    feederServo.write(50);
    delay(1000);
    feederServo.write(0);  
 }

 Serial.print("Ratio: ");
 Serial.println(ratio);

 // set new feed value to last calculated ratio
 feedValue = ratio;

 // Update Display Text
 updateDisplay();  

}




// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
void printTitle(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;
  
  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (title.length()/2)),
                 middleY - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
  delay(1500);
  oled.clear(PAGE);
}





