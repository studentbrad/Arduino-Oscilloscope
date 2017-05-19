/*
 * Written by Bradley Kohler
 * Mechatronics Engineering Student at McMaster University
 * kohlerba@mcmaster.ca
 * GitHub: https://github.com/studentbrad
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ecg_logo_32_32.h"

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//definitions
#define OFFSET 3
#define TEXTSIZE 10
#define CHARWIDTH TEXTSIZE/2
#define CHARNUM 5

//variables
struct plot{
  uint8_t* pvalues;
  uint8_t pstart;
  uint8_t psize;
};

plot myplot;

unsigned long acc_time;
unsigned int sample_time;

void setup() {
  //serial
  Serial.begin(9600);

  //display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  //startup
  display.drawBitmap(display.width()/2 - 16,display.height()/2 - 16,ecg_logo_32_32,32,32,WHITE);
  display.display();
  delay(3000);
  display.clearDisplay();

  //text
  display.setTextSize(TEXTSIZE/10);
  display.setTextColor(WHITE);

  //analogReference(EXTERNAL);
  
  myplot.pvalues = (uint8_t*) malloc(sizeof(uint8_t)*(display.width() - OFFSET));
  myplot.psize = display.width() - OFFSET;
  for(int i = 0 ; i < myplot.psize ; i++){
    myplot.pvalues[i] = 0;
  }
  myplot.pstart = 0;

  acc_time = micros();
  sample_time = 0;
}

void drawAxis() {
  for(int i = OFFSET - 1 ; i > -1 ; i--) {
  display.drawPixel(i,(display.height()-1) * 0.0,WHITE);
  display.drawPixel(i,(display.height()-1) * 0.2,WHITE);
  display.drawPixel(i,(display.height()-1) * 0.4,WHITE);
  display.drawPixel(i,(display.height()-1) * 0.6,WHITE);
  display.drawPixel(i,(display.height()-1) * 0.8,WHITE);
  display.drawPixel(i,(display.height()-1) * 1.0,WHITE);
  }
}

void drawSampleTime(){
  display.setCursor(display.width() * 1/2 - (CHARWIDTH*CHARNUM+(CHARNUM-1))/2, display.height() - TEXTSIZE);
  display.println(sample_time);
}

bool drawPlot(plot* input) {
  for(int i = 0 ; i < input->psize ; i++){
  display.drawPixel(OFFSET + i,display.height()- 1 - input->pvalues[(i + input->pstart) % input->psize],WHITE);
  }
  return true;
}

void updateValues(plot* input) {
  input->pvalues[(input->pstart) % input->psize] = (analogRead(0) >> 4);
  input->pstart++;
  input->pstart = input->pstart % input->psize;
}

void loop() { 
  updateValues(&myplot);
  if(myplot.pstart == myplot.psize - 1){
    //update time
    sample_time = micros() - acc_time;
    acc_time += sample_time;
    //change display
    display.clearDisplay();
    drawAxis();
    drawPlot(&myplot);
    drawSampleTime();
    display.display();
  }
}
