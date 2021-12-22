/*
Robo_CORE_neck -- This is for the head movement
*/
#include <Servo.h>

Servo x;
Servo y;
int c=0;
int xpos = 90, ypos = 90;
char data;
int speed = 1;
void setup() {
  Serial.begin(9600);
  x.attach(9);
  y.attach(10);

  delay(10);

  x.write(xpos);
  y.write(ypos);
  delay(100);

}

void loop() {
  data = Serial.read();
  if (data == 'X') {
    xpos += speed;
  }

  if (data == 'Y') {
    ypos += speed;
  }

  if (data == 'x') {
    xpos -= speed;
  }

  if (data == 'y') {
    ypos -= speed;
  }

  x.write(xpos);
  y.write(ypos);

}
