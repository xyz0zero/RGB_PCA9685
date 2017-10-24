//zero. 2017/10/21.

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
//Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(); Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41); Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(0x42); Adafruit_PWMServoDriver pwm4 = Adafruit_PWMServoDriver(0x43);

Adafruit_PWMServoDriver pca[4] = {0x40, 0x41, 0x42, 0x43};

//Use point
//int  Floor[3][3] = {{1,1,1},{2,2,2},{3,3,3},};
int I[] = {255, 13, 0}; // 1
int II[] = {254, 74, 0}; //2
//int III[]={0,255,0}; // led problem..
int III[] = {254, 143, 0}; //3
int IV[] = {254, 205, 0}; //4
int V[] = {246, 237, 2}; //5
int VI[] = { 201, 240, 16}; //6
int VII[] = { 122, 246, 39}; //7
int VIII[] = { 30, 253, 66}; //8
int IX[] = {0, 240, 98 }; //9
int X[] = { 0, 212, 142}; //10
int XI[] = {0, 176, 198 }; //11
int XII[] = {1, 135, 251 }; //12
int XIII[] = {12, 103, 214 }; //13
int XIV[] = {22, 73, 179 }; //14
int XV[] = { 30, 51, 154}; //15
int XVI[] = {42, 33, 137 }; //16
int XVII[] = { 66, 30, 145}; //17
int XVIII[] = { 107, 25, 158}; //18
int XIX[] = {152, 20, 172 }; //19
int* COLORS[] = {I, II, III, IV, V, VI, VII,
                 VIII, IX, X, XI, XII, XIII,
                 XIV, XV, XVI, XVII, XVIII, XIX
                }; // 1st, 2nd, 3rd
int *bigSmall; // it can search max, min function
uint16_t spd = 3; //light pwm speed..
int valueBig;
boolean finish = false;
int now = 0;
int next = 0;
int total = 15; //need change..11
void setup() {
  Serial.begin(9600);
  pca[0].begin(); pca[1].begin(); pca[2].begin(); pca[3].begin();
  pca[0].setPWMFreq(1600);  pca[1].setPWMFreq(1600);
  pca[2].setPWMFreq(1600);  pca[3].setPWMFreq(1600);
  uint8_t twbrbackup = TWBR; // save I2C bitrate
  TWBR = 12; // upgrade to 400KHz! // must be changed after calling Wire.begin() (inside pwm.begin())

}

void loop() {
  //allOFF();
  //Serial.print("bigger[0] = "); Serial.println(bigSmall[0]);// max    //Serial.print("bigger[1] = "); Serial.println(bigSmall[1]);// min

  //pca[3].setPWM(12,0,4095); // red!!  //pca[3].setPWM(13, 0, 0); // green!! 

  if (now == 3)     total = 11;
  else    total = 15;

  for (int iNum = 0;  iNum < total; iNum += 3) { //0-14 or 0-11
    //Serial.print("next_outside:"); Serial.println(next);
    bigSmall = rgbBiggest(COLORS[next], next);
    finish = setColor(COLORS[next], bigSmall[0], iNum, now); // set 1.COLORS && 2.bigValue = [0] && 3.now floor && 4.pca
    //Serial.print("finish:"); Serial.print(finish); Serial.print(", next: "); Serial.println(next);
    if (finish == true) { // floor light finish : pwm finish.
      //Serial.print("??????:"); Serial.println(finish);
      next++; // inner counter....outside:COLOR
      finish = false;
      if (next % 5 == 0) {
        now++; //I-V,VI-X,XI-XV,XVI-XIX : pca[0] > pca[1] >pca[2]>pca[3]
       // Serial.print("now:"); Serial.println(now);
      }
      if (next == 19) {
        next = 0;
        now = 0;
        delay(9000);
        allOFF();  // off all light.
        //Serial.println("finish............");
      }
    }
  }




}

// led , pca[]change
boolean setColor(int* color, int maxValue, int i, int now_) {
  // Serial.print("r: "); Serial.print(color[0]); Serial.print(", g: ");  Serial.print(color[1]); Serial.print(", b: ");   Serial.println(color[2]); Serial.print("maxValue: "); Serial.println(maxValue);
  int r = color[0];//0-255
  int g = color[1];//0-255
  int b = color[2];//0-255
  boolean state = false; // call outside it finish
  int R, G, B, MAX;
  R = map(r, 0, 255, 0, 4095);
  G = map(g, 0, 255, 0, 4095);
  B = map(b, 0, 255, 0, 4095);
  MAX = map(maxValue, 0, 255, 0, 4095);
  int minR = R, minG = G, minB = B;
  //Serial.print("R: "); Serial.print(color[0]); Serial.print(", G: ");   Serial.print(color[1]); Serial.print(", B: ");   Serial.println(color[2]);  Serial.print("MAX:  "); Serial.print(MAX);


  //for (int i = 0; i < 16; i += 3) { // all lemp..0,3,6,9,12,15
  for (int j = 0; j <= MAX; j += spd) {
    //for (int j = 0; j <= 4095; j += spd) {
    if (R == MAX) {
      valueBig = j * 8;
      if (valueBig <= R)  pca[now_].setPWM(i, 0, valueBig);
    } else {
      if (j <= R ) pca[now_].setPWM(i, 0, j); //r
    }
    if (G == MAX) {
      valueBig = j * 8;
      if (valueBig <= G)  pca[now_].setPWM(i + 1, 0 , valueBig);
    } else {
      if (j <= G)  pca[now_].setPWM(i + 1, 0, j); // g
    }
    if (B == MAX) {
      valueBig = j * 8;
      if (valueBig <= B) pca[now_].setPWM(i + 2, 0, valueBig);
    } else {
      if (j <= B)  pca[now_].setPWM(i + 2, 0, j); //b
    }
  }
  state = true;
  // Serial.print("-------------");    Serial.println(i);
  return state;

  /*
    //pwm(lewss..)
    for (int j = 0; j <= MAX; j += spd) {
    minR = R - j;
    minG = G - j;
    minB = B - j;

    if (minR >=  0)  {
      pca[now_].setPWM(i, 0, minR);
      Serial.print("downR: "); Serial.print(minR);
    }
    if (minG >=  0)  {
      pca[now_].setPWM(i + 1, 0, minG);
      Serial.print(", downG: "); Serial.print(minG);
    }
    if (minB >=  0)  {
      pca[now_].setPWM(i + 2, 0, minB);
      Serial.print(", downB: "); Serial.print(minB);
    }
    Serial.println("...");
    }
    pca[now_].setPWM(i, 0, 0); // if want to off light need this ..
  */
  //}

  //return  state;
}

int* rgbBiggest(int* rgb, int next_) {  // choice bigest Value
  int maxNum = rgb[0]; // if it is max
  int minNum = rgb[0]; // if it is mini
  int innerN[2];
  for (int i; i < 3 ; i++) {
    if (rgb[i] > maxNum) maxNum = rgb[i];
    if (rgb[i] < minNum) minNum = rgb[i];
    //bigSmall[i] = &rgb[i];
  }
  innerN[0] = maxNum; innerN[1] = minNum;
  //info(maxNum, minNum, innerN[0], innerN[1], next ); //print information..
  return innerN;
}

void allOFF() {
  for (int j = 0 ; j < 15 ; j++) {
    pca[0].setPWM(j, 0, 0);     pca[1].setPWM(j, 0, 0);
    pca[2].setPWM(j, 0, 0);     pca[3].setPWM(j, 0, 0);
  }
}


void info(int maxNum_ , int minNum_ , int innerN_0 , int innerN_1, int nextNum) {
  Serial.print("next : "); Serial.print(next);
  Serial.print(":  max = "); Serial.print(maxNum_); Serial.print(" , "); Serial.print("min = "); Serial.print(minNum_);
  Serial.print(" , ");  Serial.print("innerN[0]= "); Serial.print(innerN_0);
  Serial.print(" , ");  Serial.print("innerN[1]= "); Serial.print(innerN_1);
  Serial.println("");
}
