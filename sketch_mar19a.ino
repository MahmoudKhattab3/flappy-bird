#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>
#include "TouchScreen.h"
#include <Fonts/Org_01.h>    //Include a different font
#include <EEPROM.h> 
MCUFRIEND_kbv tft;  
/*______End of Libraries_______*/

/*______Define LCD pins (I have asigned the default values)_______*/
#define YP A3  // (A3) must be an analog pin, use "An" notation!
#define XM A2  // (A2)must be an analog pin, use "An" notation!
#define YM 9   // (9)can be a digital pin
#define XP 8   // (8)can be a digital pin
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4


#define REDBAR_MINX 50
#define GREENBAR_MINX 130
#define BLUEBAR_MINX 180
#define BAR_MINY 30
#define BAR_HEIGHT 250
#define BAR_WIDTH 30

/*_______End of defanitions______*/

/*______Assign names to colors and pressure_______*/
#define BLACK   0x0000
int LIME = tft.color565(50, 50, 255);   // --->>>><<<<
#define DARKBLUE 0x0010
#define VIOLET 0x8888
#define BLUE     0xF800
#define  MAGENTA   0x07E0   // --->>> green
#define RED    0x07FF     // --->>>  cyan
#define GREEN 0xF81F   // --->>>>>
#define  CYAN  0xFFE0   // -->>>
#define WHITE   0xFFFF
#define GREY   tft.color565(64, 64, 64);
#define GOLD 0xFEA0
#define BROWN 0xA145
#define SILVER 0xC618
#define YELLOW 0x07E0

int currentpcolour;
bool backsensed = false;
bool resetsensed = false;

#define MINPRESSURE 10
#define MAXPRESSURE 1000                  //       =======>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<

/*_______Assigned______*/

/*____Calibrate TFT LCD_____*/

#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92 
#define TS_MAXY 952  //               ======>>>>><<<<<======

/*______End of Calibration______*/

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //300 is the sensitivity


#define DRAW_LOOP_INTERVAL 50  //The interval used instead of delay(); 

//Define the address used in the eeprom memory to store highscores
#define addr 0
int currentpage;

int currentWing;        //Used to flap the wings
int flX, flY, fallRate; //Used to calculete and store the bird's position
int pillarPos, gapPosition;  //Used tho draw the pillars
int score;              //Store the score
int highScore = 0;      //Store the highscore
bool running = false;   //Store weather flying or not
bool crashed = false;   //Store weather crashed or not
bool scrPress = false;  //Store weather sensed some touch and store
long nextDrawLoopRunTime;
int redval;
int greenval;
int blueval;

int redpos = BAR_MINY + 12;
int greenpos = BAR_MINY + 12;
int bluepos = BAR_MINY + 12;

int oldrpos = redpos;
int oldgpos = greenpos;
int oldbpos = bluepos;
int x, y;



void drawHome()
{

  tft.fillScreen(BLACK);
  tft.drawRoundRect(0, 0, 479, 319, 8, WHITE);     //Page border   //

  tft.fillRoundRect(140, 180, 200, 40, 8, RED);
  tft.drawRoundRect(140, 180, 200, 40, 8, WHITE);  //Game


  tft.setCursor(150, 100);
  tft.setTextSize(3);
  tft.setFont();
  tft.setTextColor(WHITE);
  tft.print("FLAPPY GAME");
  tft.setCursor(150, 30);
  tft.setTextSize(2);
  tft.setTextColor(LIME);
  tft.print("Mahmoud Khattab");
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(190, 190);
  tft.print("START");

  //  delay(500);

}

void setup() {
  Serial.begin(9600); //Use serial monitor for debugging
  tft.reset(); //Always reset at start
  tft.begin(0x9486); // My LCD uses LIL9341 Interface driver IC                                                     =====>>>>>>>
  tft.setRotation(1); // I just roated so that the power jack faces up - optional
  tft.invertDisplay(1);

  currentpage = 0;
tft.fillScreen(BLACK);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
 
  tft.setCursor(50, 10);
  tft.print("This Project");
 tft.setCursor(90, 50);
 tft.print( "Done By");
tft.setTextSize(2);

  tft.setTextColor(tft.color565(255, 255, 0));
  tft.setCursor(50, 110);
  tft.print("Mahmoud Ahmed");
tft.setTextSize(3);
 tft.setCursor(5, 160);
   tft.setTextColor(GREEN);
  tft.print("Loading...");

  for (int i=0; i < 470; i++)
  {
    tft.fillRect(0, 210, i, 10, RED);
    delay(2);
  }

  drawHome();
}

void loop() {


  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();     // Read touchscreen
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (currentpage == 0)
  {
    if (p.z > 10 && p.z < 1000)
    {
      Serial.print(p.x);
      Serial.print(" ");
      Serial.println(p.y);
      if (p.x > 300 && p.x < 750 && p.y > 660 && p.y < 750  && p.z > MINPRESSURE && p.z < MAXPRESSURE)
      {
        Serial.println("FlappyBird");

        tft.fillRoundRect(60, 180, 200, 40, 8, WHITE);

        delay(70);

        tft.fillRoundRect(60, 180, 200, 40, 8, RED);
        tft.drawRoundRect(60, 180, 200, 40, 8, WHITE);
        tft.setCursor(65, 195);

       tft.setTextSize(3);
  tft.setCursor(115, 190);
  tft.print("START");


        currentpage = 1;

        nextDrawLoopRunTime = millis() + DRAW_LOOP_INTERVAL;
        crashed = false;
        running = false;
        scrPress = false;

               startGame();
      }

    }

  }

  if (currentpage == 1) //Flappy bird
  {
    
    if (millis() > nextDrawLoopRunTime && !crashed)
    {
      drawLoop();
      delay(15);
      checkCollision();
      delay(15);
      nextDrawLoopRunTime += DRAW_LOOP_INTERVAL;
      delay(15);
    }

Serial.println(p.z);
    // Process user input   If sensed a touch anywere on the screen eccept the area of the reset button 
    if (p.z>0 && !scrPress && !resetsensed && !backsensed) {                                                 //  ===========>>>>>>><<<<<<<<<<<<<<<<================
      if (crashed) {
        // restart game
        startGame();
      }
      else
      {
        // Go up
        fallRate = -8;
        scrPress = true;
      }
    }
    else if (p.z == 0 && scrPress) {
      scrPress = false;
    }
  }

}

void drawLoop() { //Flappy bird functions
  // clear moving items
  clearPillar(pillarPos, gapPosition);   //Clear pillar
  clearFlappy(flX, flY);            //Clear bird

  // Move items
  if (running) {
    flY += fallRate;
    fallRate++;

    pillarPos -= 5;
    if (pillarPos == 0) {
      score=score+5;
    }
    else if (pillarPos < -50) {
      pillarPos = 460;
      gapPosition = random(20, 160);    //Random gap position
    }
  }

  // draw moving items & animate
  drawPillar(pillarPos, gapPosition);   //Pillars
  drawFlappy(flX, flY);            //Bird
  switch (currentWing) {     //Make it's flapping wings
    case 0: case 1: drawWing1(flX, flY); break;  //Wing down
    case 2: case 3: drawWing2(flX, flY); break;  //Wing middle
    case 4: case 5: drawWing3(flX, flY); break;  //Wing up

  }

  currentWing++;   //flap the wing
  if (currentWing == 6  ) currentWing = 0;  //reset the wing

}

void checkCollision() {

  // Collision with ground
  if (flY > 250 ||flY <0 ) crashed = true;    //    it will be changed 

  // Collision with pillar
  if (flX + 34 > pillarPos && flX < pillarPos + 50)
    if (flY < gapPosition || flY + 24 > gapPosition + 90)
      crashed = true;

  if (crashed) {      //If it crashes somewere
    tft.setTextColor(RED);   //Set the text colour to red
    tft.setTextSize(5);      //Set the text size to 5
    tft.setCursor(20, 75);   //Set the cursor on 20,75
    tft.print("Game Over!"); //Print "Game Over"

    tft.setTextSize(4);      //Set the text size to 4
    tft.setCursor(75, 125);  //Set the cursor on 75,125
    tft.print("Score:");     //Print "Score:"

    tft.setCursor(220, 125); //Set the cursor to 220,125
    tft.setTextSize(5);      //Set the text size to 5
    tft.setTextColor(WHITE);
    
    tft.print(score);        //Print the score

       tft.setTextColor(BLACK);   //Set the text colour to red
    tft.setTextSize(5);      //Set the text size to 5
    tft.setCursor(20, 225);   //Set the cursor on 20,75
    tft.print("tab to restart");

    running = false;      // Stop animation

    // Delay to stop any last minute clicks from restarting immediately
    delay(1000);
  }
}

void drawPillar(int x, int gap) {  //Draw the pillar
int xx = 180 ;
  tft.fillRect(x + 2, 2, 46, gap - 4, currentpcolour);
  tft.fillRect(x + 2, gap + 92, 46, xx - gap, currentpcolour);

  tft.drawRect(x, 0, 50, gap, BLACK);
  tft.drawRect(x + 1, 1, 48, gap -4 - 2, BLACK);
  tft.drawRect(x, gap + 90, 50, xx  - gap, BLACK);
  tft.drawRect(x + 1, gap + 91 , 48, xx -2 - gap, BLACK);
  /*
    if (flX + 34 > pillarPos && flX < pillarPos + 50)
    if (flY < gapPosition || flY + 24 > gapPosition + 90)
  */
}

void clearPillar(int x, int gap) {  //Clear the pillar

int xx = 185 ;
  //Clear pillars by printing blue colour
  tft.fillRect(x + 45, 0, 5, gap, BLUE);
  tft.fillRect(x + 45, gap + 90, 5, xx - gap, BLUE);
}

void clearFlappy(int x, int y) {  //Clear the bird
  tft.fillRect(x, y, 34, 24, BLUE);
}

void drawFlappy(int x, int y) {  //Draw the bird
  // Upper and lower body
  tft.fillRect(x + 2, y + 8, 2, 10, BLACK);
  tft.fillRect(x + 4, y + 6, 2, 2, BLACK);
  tft.fillRect(x + 6, y + 4, 2, 2, BLACK);
  tft.fillRect(x + 8, y + 2, 4, 2, BLACK);
  tft.fillRect(x + 12, y, 12, 2, BLACK);
  tft.fillRect(x + 24, y + 2, 2, 2, BLACK);
  tft.fillRect(x + 26, y + 4, 2, 2, BLACK);
  tft.fillRect(x + 28, y + 6, 2, 6, BLACK);
  tft.fillRect(x + 10, y + 22, 10, 2, BLACK);
  tft.fillRect(x + 4, y + 18, 2, 2, BLACK);
  tft.fillRect(x + 6, y + 20, 4, 2, BLACK);

  // Body fill
  tft.fillRect(x + 12, y + 2, 6, 2, YELLOW);
  tft.fillRect(x + 8, y + 4, 8, 2, YELLOW);
  tft.fillRect(x + 6, y + 6, 10, 2, YELLOW);
  tft.fillRect(x + 4, y + 8, 12, 2, YELLOW);
  tft.fillRect(x + 4, y + 10, 14, 2, YELLOW);
  tft.fillRect(x + 4, y + 12, 16, 2, YELLOW);
  tft.fillRect(x + 4, y + 14, 14, 2, YELLOW);
  tft.fillRect(x + 4, y + 16, 12, 2, YELLOW);
  tft.fillRect(x + 6, y + 18, 12, 2, YELLOW);
  tft.fillRect(x + 10, y + 20, 10, 2, YELLOW);

  // Eye
  tft.fillRect(x + 18, y + 2, 2, 2, BLACK);
  tft.fillRect(x + 16, y + 4, 2, 6, BLACK);
  tft.fillRect(x + 18, y + 10, 2, 2, BLACK);
  tft.fillRect(x + 18, y + 4, 2, 6, WHITE);
  tft.fillRect(x + 20, y + 2, 4, 10, WHITE);
  tft.fillRect(x + 24, y + 4, 2, 8, WHITE);
  tft.fillRect(x + 26, y + 6, 2, 6, WHITE);
  tft.fillRect(x + 24, y + 6, 2, 4, BLACK);

  // Beak
  tft.fillRect(x + 20, y + 12, 12, 2, BLACK);
  tft.fillRect(x + 18, y + 14, 2, 2, BLACK);
  tft.fillRect(x + 20, y + 14, 12, 2, RED);
  tft.fillRect(x + 32, y + 14, 2, 2, BLACK);
  tft.fillRect(x + 16, y + 16, 2, 2, BLACK);
  tft.fillRect(x + 18, y + 16, 2, 2, RED);
  tft.fillRect(x + 20, y + 16, 12, 2, BLACK);
  tft.fillRect(x + 18, y + 18, 2, 2, BLACK);
  tft.fillRect(x + 20, y + 18, 10, 2, RED);
  tft.fillRect(x + 30, y + 18, 2, 2, BLACK);
  tft.fillRect(x + 20, y + 20, 10, 2, BLACK);
}

// First wing : down
void drawWing1(int x, int y) {
  tft.fillRect(x, y + 14, 2, 6, BLACK);
  tft.fillRect(x + 2, y + 20, 8, 2, BLACK);
  tft.fillRect(x + 2, y + 12, 10, 2, BLACK);
  tft.fillRect(x + 12, y + 14, 2, 2, BLACK);
  tft.fillRect(x + 10, y + 16, 2, 2, BLACK);
  tft.fillRect(x + 2, y + 14, 8, 6, WHITE);
  tft.fillRect(x + 8, y + 18, 2, 2, BLACK);
  tft.fillRect(x + 10, y + 14, 2, 2, WHITE);
}

// Second wing: middle
void drawWing2(int x, int y) {
  tft.fillRect(x + 2, y + 10, 10, 2, BLACK);
  tft.fillRect(x + 2, y + 16, 10, 2, BLACK);
  tft.fillRect(x, y + 12, 2, 4, BLACK);
  tft.fillRect(x + 12, y + 12, 2, 4, BLACK);
  tft.fillRect(x + 2, y + 12, 10, 4, WHITE);
}

// Third wing: up
void drawWing3(int x, int y) {
  tft.fillRect(x + 2, y + 6, 8, 2, BLACK);
  tft.fillRect(x, y + 8, 2, 6, BLACK);
  tft.fillRect(x + 10, y + 8, 2, 2, BLACK);
  tft.fillRect(x + 12, y + 10, 2, 4, BLACK);
  tft.fillRect(x + 10, y + 14, 2, 2, BLACK);
  tft.fillRect(x + 2, y + 14, 2, 2, BLACK);
  tft.fillRect(x + 4, y + 16, 6, 2, BLACK);
  tft.fillRect(x + 2, y + 8, 8, 6, WHITE);
  tft.fillRect(x + 4, y + 14, 6, 2, WHITE);
  tft.fillRect(x + 10, y + 10, 2, 4, WHITE);
}

void startGame() {
  //  Serial.println("Starting game!");
  //Set the default x and y cordinates of the bird
  flX = 50;
  flY = 25;
  fallRate = 0; //Set the fallrate to 1
  pillarPos = 460;  //Set the pillar on position 320
  gapPosition = 60; //Set the pillar's gap on position 60
  crashed = false;  //Not crashed
  score = 0;       //Score to 0
  highScore = EEPROM.read(addr);    //Set the highscore variable to the data read from the EEPROM position 0
  tft.setFont(&Org_01);              //Set the font
  tft.fillScreen(BLUE);              //Fill the screen blue
  tft.setTextColor(YELLOW);          //Set the text colour to yellow
  tft.setTextSize(3);                //Text size to 3
  tft.setCursor(5, 20);              //Cursor on 5,20
  tft.println("Flappy Bird");
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.println(" let's go !");

  // Draw Ground
  int ty = 275; int tx = 0;
  for ( tx = 0; tx <= 460; tx += 20) {
    tft.fillTriangle(tx, ty, tx + 10, ty, tx, ty + 10, GREEN);
    tft.fillTriangle(tx + 10, ty + 10, tx + 10, ty, tx, ty + 10, YELLOW);
    tft.fillTriangle(tx + 10, ty, tx + 20, ty, tx + 10, ty + 10, YELLOW);
    tft.fillTriangle(tx + 20, ty + 10, tx + 20, ty, tx + 10, ty + 10, GREEN);
  }
delay(1000);
  nextDrawLoopRunTime = millis() + DRAW_LOOP_INTERVAL;
}

