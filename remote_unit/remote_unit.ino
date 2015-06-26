
#include <UTFT.h>
// #include <UTFT_Geometry.h>
#include <SerialCommand.h>
#include<Wire.h>
const int MPU=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

extern uint8_t SevenSegNumFont[];
extern uint8_t SmallFont[];
extern uint8_t DotMatrix_M_Slash[];
extern uint8_t Ubuntu[];
extern uint8_t BigFont[];

SerialCommand SCmd; 

#define COLOR_PURPLE 0x04DF // 0,153,255 
#define COLOR_BLACK 0x0000 
#define COLOR_WHITE 0xFFFF // 255,255,255
#define COLOR_RED 0xF800 // 255,0,0
#define COLOR_GREEN 0x07E0 //0,255,0
#define COLOR_BLUE 0x001F // 0,0,255
#define COLOR_SAND 0xDD01 // 218,163,8
#define COLOR_GREY 0x94B2

#define BOX_COLOR_RAND COLOR_SAND
#define BOX_COLOR_FILL COLOR_BLACK

UTFT myGLCD(ITDB32WD,38,39,40,41);

// UTFT_Geometry geo(&myGLCD);

// extern unsigned int compass_bmp[0x3E04];

#define STRING_LENGHT 50

struct mpd_s{
  char s[STRING_LENGHT];
  int x;
  int y;
  int color;
  int bcolor;
};

#define COUNT_MPD 3
#define MPD_MARGIN_TOP 30

mpd_s mpd[COUNT_MPD] = {{"Artist + Titel",CENTER,0+MPD_MARGIN_TOP,COLOR_WHITE,COLOR_BLACK},
                        {"Status1",CENTER,25+MPD_MARGIN_TOP,COLOR_GREY,COLOR_BLACK},
                        {"Status2",CENTER,45+MPD_MARGIN_TOP,COLOR_GREY,COLOR_BLACK}};

struct string_s{
  char s[STRING_LENGHT];
  int x;
  int y;
  int color;
  int bcolor;
  int seite;
};

#define COUNT_TEXT 4
#define AOR_MARGIN_TOP 0

string_s text[COUNT_TEXT] = {{"ALLGAEU ORIENT 2015",CENTER,AOR_MARGIN_TOP,COLOR_BLACK,COLOR_SAND,0},
                             {"MPD Playlist",CENTER,AOR_MARGIN_TOP,COLOR_BLACK,COLOR_PURPLE,1},
                             {"MPD Files",CENTER,AOR_MARGIN_TOP,COLOR_BLACK,COLOR_PURPLE,2},
                             {"Kompass",CENTER,AOR_MARGIN_TOP,COLOR_BLACK,COLOR_PURPLE,3}};

/*
                            {"OEL1",35,53+VALUES_MARGIN_TOP,COLOR_BLACK,COLOR_SAND},
                            {"OEL2",170,53+VALUES_MARGIN_TOP,COLOR_BLACK,COLOR_SAND},
                            {"RPI",315,53+VALUES_MARGIN_TOP,COLOR_BLACK,COLOR_SAND},
                            {"Motor",25,123+VALUES_MARGIN_TOP,COLOR_BLACK,COLOR_SAND},
                            {"Aussen",155,123+VALUES_MARGIN_TOP,COLOR_BLACK,COLOR_SAND},
                            {"Innen",295,123+VALUES_MARGIN_TOP,COLOR_BLACK,COLOR_SAND}}; */

// 4LCD   
//  drawBox(0,0,100,60);
//  drawBox(99,0,100,60);
//  drawBox(199,0,100,60);
//  drawBox(299,0,100,60);

// 3LCD
//  drawBox(0,0,133,60);
//  drawBox(133,0,133,60);
//  drawBox(266,0,133,60);


struct value_s{
  double v;
  char text[STRING_LENGHT];
  int warn;
  int alarm;
};
  
struct value_box_s{
  int vid;
  int x;  
  int y;
  int padding_left; // text padding in box
  int padding_top;  // text padding in box
  int width;   // der box
  int height;  // der box
  int background_color;
  int value_color;
  int alarm_color; 
  int text_color;
  int box_color;
  int seite;
};

#define VALUES_MARGIN_TOP 100
#define COUNT_VALUE 6
#define COUNT_BOX 6
#define VALUE_COLOR_SCHEMA COLOR_BLACK,COLOR_PURPLE,COLOR_RED,COLOR_BLACK,COLOR_SAND

value_s values[COUNT_VALUE] = { {11.1,"OEL",    500.0,  600.0},
                                {22.2,"Wasser", 500.0,  600.0},
                                {33.3,"RPI",    55.0,   60.0},
                                {44.4,"Motor",  500.0,  600.0},
                                {55.5,"Aussen", 30.0,  35.0},
                                {66.6,"Innen",  30.0,   35.0}};

value_box_s value_boxes[COUNT_BOX] = {  {0,0,    VALUES_MARGIN_TOP,     8,13,133,50,VALUE_COLOR_SCHEMA,0},
                                        {1,133,  VALUES_MARGIN_TOP,     8,13,133,50,VALUE_COLOR_SCHEMA,0},
                                        {2,266,  VALUES_MARGIN_TOP,     8,13,133,50,VALUE_COLOR_SCHEMA,0},
                                        {3,0,    70+VALUES_MARGIN_TOP,  8,13,133,50,VALUE_COLOR_SCHEMA,0},
                                        {4,133,  70+VALUES_MARGIN_TOP,  8,13,133,50,VALUE_COLOR_SCHEMA,0},
                                        {5,266,  70+VALUES_MARGIN_TOP,  8,13,133,50,VALUE_COLOR_SCHEMA,0}};
                                        
                                        
int seite = 0;

int i,k;

void stringHandler(){
  int a,nr;  
  char *arg; 
  arg = SCmd.next();
  a = 0; 
  if (arg != NULL){
    nr = atoi(arg);
    arg = SCmd.next();
    while(arg != NULL && nr < COUNT_TEXT){
      while(*arg != NULL){
        text[nr].s[a] = *arg;
        a++;
        arg++;
      }
      text[nr].s[a] = ' ';
      a++;
      arg = SCmd.next();
    }
    text[nr].s[a] = 0x00;
  }
}

void mpdHandler(){
  int a,b,nr;  
  char *arg; 
  arg = SCmd.next();
  a = 0; 
  b = 0;
  nr = 0;
  if (arg != NULL){
    nr = atoi(arg);
    arg = SCmd.next();
    if(arg != NULL){
      for(b=0;b<STRING_LENGHT;b++){
         mpd[nr].s[b] = 0x00;
      }
    }   
    while(arg != NULL && nr < COUNT_MPD){
      while(*arg != NULL){
        mpd[nr].s[a] = *arg;
        a++;
        arg++;
      }
      mpd[nr].s[a] = ' ';
      a++;
      arg = SCmd.next();
    }
    if(nr < COUNT_MPD){
      myGLCD.setColor(mpd[nr].bcolor);
      myGLCD.fillRect( 0, mpd[nr].y, 399, mpd[nr].y+15);
      myGLCD.setColor(mpd[nr].color);
      myGLCD.setBackColor(mpd[nr].bcolor);
      myGLCD.setFont(BigFont);
      myGLCD.print(mpd[nr].s,CENTER, mpd[nr].y); 
      if(a>0)
        mpd[nr].s[a-1] = 0x00;
      mpd[nr].s[a] = 0x00;
      mpd[nr].s[a+1] = 0x00;
    }
  }
}

void unrecognized(){
}

void seitenHandler(){
  int a,c,nr;
  double d;  
  char *arg; 
  arg = SCmd.next();
  a = 0; 
  if (arg != NULL){
    seite = atoi(arg);
    drawSeite();
  } 
}

void valueHandler(){
  int a,c,nr;
  double d;  
  char *arg; 
  arg = SCmd.next();
  a = 0; 
  if (arg != NULL){
    nr = atoi(arg);
    arg = SCmd.next();
    if(arg != NULL && nr < COUNT_VALUE){
       values[nr].v = atof(arg);
    }
  }
}

void inputHandler(){
  SCmd.readSerial();
}


void updateValues(){
  static int c,id;
  char str[10];
  if(value_boxes[i].seite == seite){
    id = value_boxes[c].vid;
    dtostrf(values[id].v, 5, 1, &str[0]);
    if(values[id].v > values[id].alarm)
      myGLCD.setColor(value_boxes[c].alarm_color); 
    else 
      myGLCD.setColor(value_boxes[c].value_color);
    myGLCD.setBackColor(value_boxes[c].background_color);
    myGLCD.setFont(Ubuntu);
    myGLCD.print(str, value_boxes[c].x+value_boxes[c].padding_left, value_boxes[c].y+value_boxes[c].padding_top);
    myGLCD.setColor(value_boxes[c].text_color);
    myGLCD.setBackColor(value_boxes[c].box_color);
    myGLCD.setFont(BigFont);
    myGLCD.print(values[id].text,value_boxes[c].x, value_boxes[c].y + value_boxes[c].height + 2); 
  } 
  c++;
  if(c>=COUNT_VALUE)
     c = 0;
   
}

void initValues(){
  for(i=0;i<COUNT_VALUE;i++){
      if(value_boxes[i].seite == seite){
        myGLCD.setColor(BOX_COLOR_RAND);
        myGLCD.drawRect(value_boxes[i].x, value_boxes[i].y, value_boxes[i].x+value_boxes[i].width, value_boxes[i].y+value_boxes[i].height);
        myGLCD.setColor(BOX_COLOR_FILL);
        myGLCD.fillRect(value_boxes[i].x+1, value_boxes[i].y+1, value_boxes[i].x+value_boxes[i].width-1, value_boxes[i].y+value_boxes[i].height-1);
        myGLCD.setColor(value_boxes[i].box_color);
        myGLCD.fillRect( value_boxes[i].x, 
                         value_boxes[i].y + value_boxes[i].height,
                         value_boxes[i].x + value_boxes[i].width, 
                         value_boxes[i].y + value_boxes[i].height+20); 
      }      
  }
}

void updateText(int c){
  if(text[c].seite == seite){
    myGLCD.setColor(text[c].bcolor);
    myGLCD.fillRect( 0, text[c].y, 399, 20+text[c].y);
    myGLCD.setColor(text[c].color);
    myGLCD.setBackColor(text[c].bcolor);
    myGLCD.setFont(BigFont);
    myGLCD.print(text[c].s,text[c].x, text[c].y+3); 
  }    
}

void updateTexte(){
  for(i=0;i<COUNT_TEXT;i++)
    updateText(i);
}

#define COMPASS_X 200-126/2
#define COMPASS_Y 135-126/2
#define COMPASS_RADIUS 100
#define COMPASS_X0 200
#define COMPASS_Y0 135-COMPASS_RADIUS

void drawCompass(int winkel){
  // myGLCD.setBackColor(COLOR_BLACK);
  // myGLCD.fillRect( 0, 20, 399, 239);
  // myGLCD.drawBitmap(COMPASS_X, COMPASS_Y, 126,126, compass_bmp, winkel, 63, 63,COLOR_BLACK); 
}

void drawSeite(){
  int x1,x2,x3,y1,y2,y3,r,as,ae;
  myGLCD.setColor(COLOR_BLACK);
  myGLCD.setBackColor(COLOR_BLACK);
  myGLCD.fillRect( 0, 0, 399, 239);

  initValues();
  
  if(0 == seite){
    // MPD Texte Initialisieren
    myGLCD.setFont(BigFont);
    for(i=0;i<COUNT_MPD;i++){
      myGLCD.setColor(mpd[i].color);
      myGLCD.setBackColor(mpd[i].bcolor);
      myGLCD.print(mpd[i].s,CENTER, mpd[i].y); 
    }
  }
  
  // Texte plotten
  updateTexte(); // updatet alle Texte!
}

void setup(){

  //randomSeed(analogRead(0));
  
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.setFont(Ubuntu);

  Serial.begin(9600);
  
  seite = 0;

  SCmd.addCommand("value",valueHandler);       // Turns LED on
  SCmd.addCommand("string",stringHandler);     // Turns LED off
  SCmd.addCommand("mpd",mpdHandler);     // Turns LED off
  SCmd.addCommand("seite",seitenHandler);     // Turns LED off
  SCmd.addDefaultHandler(unrecognized);        // Handler for command that isn't matched  (says "What?") 
  
  drawSeite();
  
  Wire.begin();
  Wire.beginTransmission(MPU);
      Wire.write(0x6B);  // PWR_MGMT_1 register
      Wire.write(0);     // set to zero (wakes up the MPU-6050)
      Wire.endTransmission(true);
}

void loop(){
  static int g = 0, a = 0;
  inputHandler();
  updateValues();
  updateTexte();
  
  
  if(seite == 3){
    delay(2000);
    drawCompass(g);
    g=g+10;
    if(g>=360)
      g = 0;
  }
  
  switch(a){
    case 0: 
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
      a++;
      break;
    case 1:
      AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)          
       a++;break;
    case 2:
      AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
        a++;break;
    case 3:
      AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
         a++;break;
    case 4:
      Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
      a++;break;
    case 5:
      GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
      a++;break;
    case 6:
      GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
      a++;break;
    case 7:
      GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
      a++;break;
    case 8:
      Serial.print("AcX = "); Serial.print(AcX);
     a++;break;
    case 9:
      Serial.print(" | AcY = "); Serial.print(AcY);
      a++;break;
    case 10:
      Serial.print(" | AcZ = "); Serial.print(AcZ);
      a++;break;
    case 11:
      Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
      a++;break;
    case 12:
      Serial.print(" | GyX = "); Serial.print(GyX);
      a++;break;
    case 13:
      Serial.print(" | GyY = "); Serial.print(GyY);
      a++;break;
    case 14:
      Serial.print(" | GyZ = "); Serial.println(GyZ);
      a=0;break;
      }
}

