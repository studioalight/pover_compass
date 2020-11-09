/* Compass example for the Nano 33 BLE Sense
 * A compass direction is calculated from the magnetic readings X and Y of the LSM9DS1 chip.
 * A higher value of maxcount increases accuracy but slows the measurement cycle.
 * The compass must be calibrated for the magnetic disturbance of the environment.
 * A push button should be connected between D3 (buttonpin) and GND.
 * The onboard led connected to D13 is used as indicator.
 * Keep the button pressed till the led goes ON
 * Releasing it starts the calibration indicated by the fast flashing led.
 * During calibration the board must at least be turned over a full 360 deg.
 * To finish push the button again. This stores the values and restarts the compass with the new value.
 * The printed offsetvalues can be used in the program as initial values of offsetX and offsetY
 * see https://forum.arduino.cc/index.php?topic=668641.0
 * Written by Femme Verbeek 2020 for educational purposes
*/

#include <Arduino_LSM9DS1.h>
#define buttonpin 3
#define ledpin 13
//New offset X= -21.41 Y= 37.69
//New offset X= 10.21 Y= -2.84
//New offset X= -18.96 Y= 8.60
//New offset X= -22.28 Y= 7.81


float   Declination = +4.2833; 
float   Mag_x_dampened,       Mag_y_dampened,       Mag_z_dampened;
String Headingstrold="                              ";


// Modify these offset numbers at will with the outcome of the calibration values 
float offsetX = -22.28;//0;   
float offsetY = 7.81 ;//0;
const int maxcount = 50;       //higher number = more accurate but slower

void setup() {

     pinMode(ledpin, OUTPUT);
     pinMode(buttonpin, INPUT);
     digitalWrite(ledpin, LOW);     
     Serial.begin(115200);
     while(!Serial);                     // wait till the serial monitor connects
     //Serial.println("Starting Compass");
     if (!IMU.begin()) Serial.println("Failed to initialize IMU!");
}

void loop() {
String Headingstr="";
float x, y, z;
float averX=0 , averY =0;
     //Serial.print("Measuring ");
     for (int i=1;i<=maxcount;i++){
       while (!IMU.magneticFieldAvailable());
       IMU.readMagneticField(x, y, z);
       averX += x/maxcount;
       averY += y/maxcount;
       digitalWrite(ledpin, LOW);
     //}
     
     Mag_x_dampened = Mag_x_dampened * 0.9 + x * 0.1;
     Mag_y_dampened = Mag_y_dampened * 0.9 + y * 0.1;

     float Heading= atan2(Mag_y_dampened-offsetY,Mag_x_dampened-offsetX)*180/PI +180;
     
     Heading += Declination;               // Geographic North
     if (Heading > 360.0) Heading -= 360.0;
     if (Heading < 0.0) Heading += 360.0;

     // ----- Allow for under/overflow
     if (Heading < 0) Heading += 360;
     if (Heading >= 360) Heading -= 360;
     Headingstr=String(Heading,0);
     if (Headingstr.compareTo(Headingstrold)!=0)
     {
      Headingstrold=Headingstr;
      digitalWrite(ledpin, HIGH);
     //   Serial.print(" Compass direction ");
      Serial.print("{\"heading\":\"");
      Serial.print(Headingstr);
      Serial.println("\"}");
     }
     }
          float heading= atan2(averY-offsetY,averX-offsetX)*180/PI +180;
     heading += Declination;
     //Serial.print("{\"headinggggggggggg\":\"");
     //Serial.print(heading,0);
     //Serial.println("\"}");
     if (digitalRead(buttonpin)==0) recalibrate();   //button pressed
}
 
void recalibrate(){
float x, y, z, Xmin, Xmax, Ymin, Ymax  ;
boolean ledIsOn = true;
       digitalWrite(ledpin, ledIsOn);
       Serial.println("Recalibrating");
       while (!IMU.magneticFieldAvailable());
       IMU.readMagneticField(Xmin, Ymin, z);     //find initial values
       Xmax = Xmin; Ymax = Ymin;
       //while (digitalRead(buttonpin)==0); //wait till button not pressed       
       //delay(5);
       while (digitalRead(buttonpin)==0)   {              //keep measuring  till button pressed           
          while (!IMU.magneticFieldAvailable());  // wait till new magnetic reading is available
          IMU.readMagneticField(x, y, z);
          Xmax = max (Xmax, x); Xmin = min (Xmin, x);
          Ymax = max (Ymax, y); Ymin = min (Ymin, y);
          Serial.print("xmax= "); 
          Serial.print(Xmax);
          Serial.print(" xmin= "); 
          Serial.print(Xmin);
          Serial.print(" ymax= "); 
          Serial.print(Ymax);
          Serial.print(" ymin= "); 
          Serial.println(Ymin);

          digitalWrite(ledpin, ledIsOn = !ledIsOn);  //reverse on/off led
       }
       offsetX= (Xmax + Xmin)/2;
       offsetY= (Ymax + Ymin)/2;
       Serial.print("New offset X= "); 
       Serial.print(offsetX);
       Serial.print(" Y= ");
       Serial.println(offsetY);
}   
