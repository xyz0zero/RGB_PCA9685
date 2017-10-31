//zero. 2017/10/21 V6.. 2017/10/30 v7 .. 1.ON led, 2. wait 9s,3.off led 
// ======================================================================== //
//  Pin setup:                                                              //
//                                                                          //
// -------                                                                  //
// ARDUINO|                           PCA9685|                              //
//     GND|                              GND |                              //
//      5V|                              VCC |                              //
//      19|A5                            SCL |      A5 Arduino              //
//      18|A4                          　SDA |      A4 Arduino              //
//      17|                             pwm0 |      R_1                     //
//      16|                             pwm1 |      G_1                     //
//      15|                             pwm2 |      B_1                     //
//      14|                             pwm3 |      R_2                     //
//      13|                             pwm4 |      G_2                     //
//      12|                             pwm5 |      B_2                     //
//      11|                             pwm6 |      R_3                     //
//      10|                             pwm7 |      G_3                     //
//       9|                             pwm8 |      B_3                     //
//       8|                             pwm9 |      R_4                     //
//       7|                            pwm10 |      G_4                     //
//       6|                            pwm11 |      B_4                     //
//       5|                            pwm12 |      R_5                     //
//       4|                            pwm13 |      G_5                     //
//       3|                            pwm14 |      B_5                     //
//       2|                            pwm15 |    < NO use                  //
//       1|                                                                 //
//       0|                                                                 //
// -------                                                                  //
//                                                                          //
//                                                                          //
//     notice: We use TIP 120 MOSFET: use 220 risistor                      //
//  -  Put the longer leg (anode) of the LEDs in VCC and the shorter leg    //
//          (cathode) in OUT(0-15). notice: we NO use pin 15                //
//                                                                          //
//  -  +5V from Arduino -> VCC   (VCC)                                      //
//  -  GND from Arduino ->       (GND)                                      //
//  -                                                                       //
//     use 4 Pca9685 0x40+0x41+0x42+0x43                                    //
//     setColor = 顏色設定 > setColor(顏色碼, 最大值, 第幾階, 哪個板子);  = set 1.COLORS && 2.bigValue = [0] && 3.now floor && 4.pca
//     rgbBiggest = 色碼判斷 最大值 > 色彩呈現才能在後面以倍速去追慢的色碼 = 色彩較接近當初的設定色，否則會有點色差
//     指標應用請參考 練習指標的程式..

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pca[4] = {0x40, 0x41, 0x42, 0x43};

//Use point .. 1-19
int I[] = {255, 5, 0}; // 1
int II[] = {254, 74, 0}; //2
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
uint16_t spd = 4; //light pwm speed..
int valueBig;
boolean finish = false;
int now = 0;
int next = 0;
int total = 15; //need change..11
int rise = 1 , reverse = 1;
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

  if (now == 3)     total = 11;  // 判斷 pca9685 到第4個板子 .. 只到 11
  else    total = 15;            // 判斷 pca9685 1-3個板子

  for (int iNum = 0;  iNum < total; iNum += 3) { //0-14 or 0-11
    bigSmall = rgbBiggest(COLORS[next], next);
    finish = setColor(COLORS[next], bigSmall[0], iNum, now, rise); // set 1.COLORS && 2.bigValue = [0] && 3.now floor && 4.pca
    if (finish == true) { // floor light finish : pwm finish.

      next++; // inner counter....outside:COLOR
      finish = false;
      if (next % 5 == 0) {
        now++; //I-V,VI-X,XI-XV,XVI-XIX : pca[0] > pca[1] >pca[2]>pca[3]
        // Serial.print("now:"); Serial.println(now);
      }
      if (next == 19) {
        next = 0;
        now = 0;
        delay(9000);                  // 完成時停止時間
        //allOFF();  // off all light.  //目前測試用以後要改
        rise = rise + reverse; // 1 or 2
        reverse = -reverse;

      }
    }
  }


}

// led , pca[]change
boolean setColor(int* color, int maxValue, int i, int now_, int rise) {
  int r = color[0];//0-255
  int g = color[1];//0-255
  int b = color[2];//0-255
  boolean state = false; // call outside it finish
  int R, G, B, MAX;
  R = map(r, 0, 255, 0, 4095);
  G = map(g, 0, 255, 0, 4095);
  B = map(b, 0, 255, 0, 4095);
  MAX = map(maxValue, 0, 255, 0, 4095);
  // int minR = R, minG = G, minB = B;

  Serial.print(rise);
  if (rise == 1) riseLed (i, now_, R, G, B, MAX); // 1
  else  downLed(i, now_, R, G, B,  MAX); //2

  state = true;
  return state;
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


void riseLed(int i, int now_, int R, int G, int B, int MAX) {
  //判斷 內色碼最大值 然後倍速去呈現.
  for (int j = 0; j <= MAX; j += spd) { // max .. 有點變速 .. 可改回 4095
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

}


void downLed(int i, int now_, int R, int G, int B, int MAX) {
  int minR = R, minG = G, minB = B;
  //pwm(lewss..)
  for (int j = 0; j <= MAX; j += spd) { // max .. 有點變速 .. 可改回 4095
    minR = R - j;
    minG = G - j;
    minB = B - j;

    if (minR >=  0)  {
      pca[now_].setPWM(i, 0, minR); 
      // Serial.print("downR: "); Serial.print(minR);
    }
    if (minG >=  0)  {
      pca[now_].setPWM(i + 1, 0, minG); 
      //Serial.print(", downG: "); Serial.print(minG);
    }
    if (minB >=  0)  {
      pca[now_].setPWM(i + 2, 0, minB);  
      //Serial.print(", downB: "); Serial.print(minB);
    }
    //Serial.println("...");
  }
  pca[now_].setPWM(i, 0, 0);     // Close R
  pca[now_].setPWM(i + 1, 0, 0); // Close G
  pca[now_].setPWM(i + 2, 0, 0); // Close B
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
