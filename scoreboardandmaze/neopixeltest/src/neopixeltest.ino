//#define OFFLINE

#include "Particle.h"
#include "neomatrix.h"

#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B

const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

const size_t UART_TX_BUF_SIZE = 20;
const size_t SCAN_RESULT_COUNT = 20;

BleCharacteristic peerTxCharacteristic;
BleCharacteristic peerRxCharacteristic;
BlePeerDevice peer;


#ifdef OFFLINE
SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);
#endif

#include "MQTT.h"



Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8,8,2,2, PIXEL_PIN,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_ZIGZAG +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  PIXEL_TYPE);
enum MazeColors {
  d,w,g,t,e,s,q
};
int xsample = 0;
int ysample = 0;
int zsample = 0;

SerialLogHandler logHandler(LOG_LEVEL_TRACE);

#include "Wire.h"
#include "adafruit-led-backpack.h"




int getNthDigit(unsigned long number, int digit) {
  for (int place = 0; place < digit; place++) number = number / 10;
  return number % 10;
}
Adafruit_AlphaNum4 midScoreDisplay = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 firstScoreDisplay = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 helperDisplay = Adafruit_AlphaNum4();
void writeNum(unsigned long num) {
  firstScoreDisplay.writeDigitAscii(3, getNthDigit(num, 0) + '0');
  firstScoreDisplay.writeDigitAscii(2, getNthDigit(num, 1) + '0');
  firstScoreDisplay.writeDigitAscii(1, getNthDigit(num, 2) + '0');
  firstScoreDisplay.writeDigitAscii(0, getNthDigit(num, 3) + '0');
  midScoreDisplay.writeDigitAscii(3, getNthDigit(num, 4) + '0');
  midScoreDisplay.writeDigitAscii(2, getNthDigit(num, 5) + '0');
  midScoreDisplay.writeDigitAscii(1, getNthDigit(num, 6) + '0');
  midScoreDisplay.writeDigitAscii(0, getNthDigit(num, 7) + '0');
  firstScoreDisplay.writeDisplay();
  midScoreDisplay.writeDisplay();
  
}
void setupLCDs() {
  Wire.setSpeed(CLOCK_SPEED_400KHZ);
  Wire.begin();
  

  firstScoreDisplay.begin(0x71);  // pass in the address
  midScoreDisplay.begin(0x73);
  helperDisplay.begin(0x74);

}


void callback(char* topic, byte* payload, unsigned int length) 
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  
  helperDisplay.writeDigitAscii(0, p[0]);
  helperDisplay.writeDigitAscii(1, p[1]);
  helperDisplay.writeDigitAscii(2, p[2]);
  helperDisplay.writeDigitAscii(3, p[3]);
  midScoreDisplay.writeDigitAscii(0, p[4]);
  midScoreDisplay.writeDigitAscii(1, p[5]);
  midScoreDisplay.writeDigitAscii(2, p[6]);
  midScoreDisplay.writeDigitAscii(3, p[7]);
  firstScoreDisplay.writeDigitAscii(0, p[8]);
  firstScoreDisplay.writeDigitAscii(1, p[9]);
  firstScoreDisplay.writeDigitAscii(2, p[10]);
  firstScoreDisplay.writeDigitAscii(3, p[11]);
  firstScoreDisplay.writeDisplay();
  midScoreDisplay.writeDisplay();
  helperDisplay.writeDisplay();

  Serial.printf("%s",p);
  Serial.println();
  //delay(5000);
}

#ifndef OFFLINE
MQTT client("lab.thewcl.com", 1883, callback);
#endif

const MazeColors easyMaze[16][16] = {
    {d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d},
    {d,g,g,g,g,g,w,g,g,w,g,g,g,g,g,d},
    {d,g,w,g,g,w,w,g,g,g,g,w,w,w,t,d},
    {d,g,w,g,w,g,w,g,w,w,g,g,g,g,w,d},
    {d,g,w,g,g,g,g,g,g,w,w,g,w,g,g,d},
    {d,g,g,g,w,w,g,w,g,g,w,g,g,w,g,d},
    {d,g,w,g,g,g,g,g,g,g,g,g,g,g,g,d},
    {d,g,t,w,g,g,w,g,w,w,g,w,w,g,g,d},
    {d,w,w,g,g,w,w,g,g,g,g,g,g,w,g,d},
    {d,t,g,g,w,t,g,g,w,g,w,g,w,g,g,d},
    {d,g,g,g,g,w,g,g,g,g,w,w,g,g,w,d},
    {d,g,w,w,g,g,g,w,w,g,g,g,w,g,w,d},
    {d,g,w,g,w,g,w,g,g,g,w,g,g,g,g,d},
    {d,g,w,g,g,g,w,g,w,w,g,w,w,w,g,d},
    {d,g,g,g,w,g,g,g,g,t,w,e,g,g,g,d},
    {d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d}
};
const MazeColors hardMaze[16][16] = {
  
{d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d},
{d,w,g,g,g,w,g,g,g,g,w,g,g,g,g,d},
{d,g,t,w,g,g,g,w,w,g,g,g,w,g,g,d},
{d,g,w,w,g,g,w,w,w,g,w,g,w,g,w,d},
{d,g,g,g,g,g,g,w,w,g,g,g,g,g,g,d},
{d,w,g,g,g,g,g,w,w,g,w,g,w,g,g,d},
{d,g,g,w,g,w,w,w,w,t,w,g,w,w,g,d},
{d,g,w,w,w,w,w,w,w,w,w,w,w,w,g,d},
{d,g,w,w,g,g,g,w,w,g,g,g,w,g,g,d},
{d,g,g,g,g,g,g,w,w,g,w,g,g,g,w,d},
{d,g,w,g,g,g,g,w,w,g,w,s,w,g,g,d},
{d,g,w,g,g,g,g,w,w,t,g,w,g,g,g,d},
{d,g,g,w,g,w,g,w,w,w,g,g,g,g,g,d},
{d,g,g,g,g,w,g,w,w,g,g,w,w,w,g,d},
{d,w,g,w,g,g,g,g,g,g,g,g,w,t,g,d},
{d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d}

};

const MazeColors maxMaze[16][16] = {
  {d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d},
  {d,g,w,w,w,w,w,w,w,w,w,w,g,g,g,d},
  {d,g,t,w,g,g,g,w,g,g,g,w,g,w,g,d},
  {d,g,w,w,g,w,g,w,g,w,g,w,g,w,g,d},
  {d,g,w,w,g,w,g,w,g,w,g,w,g,w,g,d},
  {d,g,w,w,g,w,g,g,g,w,g,g,g,w,g,d},
  {d,g,g,g,g,w,w,w,w,w,w,w,w,w,g,d},
  {d,w,w,w,w,w,g,g,g,g,g,g,g,g,g,d},
  {d,g,g,g,g,g,t,w,w,w,w,w,w,w,g,d},
  {d,g,w,w,w,w,w,w,g,g,g,g,g,g,g,d},
  {d,g,g,g,g,w,w,w,g,w,w,w,w,w,g,d},
  {d,w,w,w,g,w,g,w,g,w,g,t,w,w,g,d},
  {d,w,g,g,g,w,g,w,g,g,g,w,w,w,g,d},
  {d,w,g,w,w,w,g,w,w,w,w,w,g,g,g,d},
  {d,g,g,g,s,w,g,g,g,g,g,t,t,w,g,d},
  {d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d}
};

const MazeColors lookUp[16][16] = {
        {d, d, d, d, d, d, g, g, g, g, g, g, g, g, g, g},
        {g, g, g, g, g, d, g, g, g, g, g, g, g, g, g, g},
        {g, g, g, g, g, d, g, g, w, w, w, w, w, w, g, g},
        {g, g, g, g, g, d, g, g, g, g, g, g, g, w, w, g},
        {g, g, g, g, d, g, g, g, g, g, g, g, g, g, w, g},
        {g, g, g, d, g, d, g, g, g, g, g, g, g, g, w, g},
        {g, g, g, g, d, g, g, g, g, g, g, g, g, w, w, g},
        {g, g, g, g, g, g, g, g, w, w, w, w, w, w, g, g},
        {g, g, g, g, d, g, g, g, g, g, g, g, g, g, g, g},
        {g, g, g, d, g, d, g, g, g, g, g, g, g, g, g, g},
        {g, g, g, g, d, g, g, g, w, w, w, w, w, w, w, g},
        {g, g, g, g, g, g, g, g, w, g, g, g, w, g, g, g},
        {g, g, d, d, d, d, g, g, w, w, g, w, w, g, g, g},
        {g, g, g, g, d, g, g, g, g, w, w, w, g, g, g, g},
        {g, g, g, d, g, d, g, g, g, g, g, g, g, g, g, g},
        {g, g, g, g, g, g, g, g, g, g, g, g, g, g, g, g},
};

const MazeColors mazeOne[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, g, g, g, g, g, g, t, g, g, g, g, g, t, d},
        {d, g, g, g, g, g, w, w, w, w, w, w, w, w, w, d},
        {d, w, w, g, g, w, w, g, g, g, g, g, g, g, g, d},
        {d, w, g, g, w, w, g, g, g, g, g, g, g, g, g, d},
        {d, w, g, w, w, w, g, g, g, g, g, g, g, g, g, d},
        {d, w, g, w, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, w, g, w, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, w, g, w, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, w, t, w, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, w, g, w, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, w, g, w, w, w, w, w, w, g, g, g, g, g, g, d},
        {d, w, g, g, t, g, g, g, w, w, w, g, g, g, g, d},
        {d, w, w, w, w, w, w, g, g, g, w, w, w, w, w, d},
        {d, w, w, w, w, w, w, w, w, g, g, t, g, g, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors mazeTwo[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, g, g, g, t, g, g, g, w, d, d, d, d, d, d},
        {d, w, w, w, w, w, w, w, g, w, d, d, d, d, d, d},
        {d, d, d, d, d, d, d, w, g, w, w, w, w, w, w, w},
        {d, d, d, d, d, d, d, w, g, g, g, g, g, g, t, w},
        {w, w, w, w, w, w, w, w, g, w, w, w, w, w, w, w},
        {w, t, g, g, g, g, g, g, g, w, d, d, d, d, d, d},
        {w, w, w, w, w, w, w, w, g, w, d, d, d, d, d, d},
        {d, d, d, d, d, d, d, w, g, w, d, d, d, d, d, d},
        {w, w, w, w, w, w, w, w, g, w, w, w, w, w, w, w},
        {w, t, g, g, g, g, g, g, g, g, g, g, g, g, t, w},
        {w, w, w, w, w, w, w, w, g, w, w, w, w, w, w, w},
        {d, d, d, d, d, d, d, w, g, w, d, d, d, d, d, d},
        {d, d, d, d, d, d, d, w, g, w, w, w, w, w, w, d},
        {d, d, d, d, d, d, d, w, g, g, g, g, g, g, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors thirdMaze[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, w, w, w, w, w, w, w, w, w, w, w, w, g, d},
        {d, g, w, d, d, d, d, d, d, d, d, d, d, d, g, d},
        {d, g, w, d, w, w, w, w, w, w, w, w, w, d, g, d},
        {d, g, w, d, w, d, d, d, d, d, d, d, w, d, g, d},
        {d, g, w, d, w, d, w, w, w, w, w, d, w, d, g, d},
        {d, g, w, d, w, d, w, t, t, t, w, d, w, d, g, d},
        {d, g, w, d, w, d, w, t, g, g, w, d, w, d, g, d},
        {d, g, w, d, w, d, w, t, g, g, w, d, w, d, g, d},
        {d, g, w, d, w, d, w, w, w, g, g, d, w, d, g, d},
        {d, g, w, d, w, d, d, d, d, d, g, g, w, d, g, d},
        {d, g, w, d, w, w, w, w, w, w, w, g, g, d, g, d},
        {d, g, w, d, d, d, d, d, d, d, d, d, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors fourthMaze[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, w, w, w, w, w, w, w, w, w, w, w, w, w, g, d},
        {d, g, g, t, g, g, g, t, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, t, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, t, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, t, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors fifthMaze[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, w, g, w, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, w, g, w, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, w, g, w, g, g, g, g, g, g, d},
        {d, g, g, g, g, t, w, g, w, t, g, g, g, g, g, d},
        {d, g, w, w, w, w, w, g, w, w, w, w, w, w, g, d},
        {d, g, g, g, g, g, g, t, g, g, g, g, g, g, g, d},
        {d, g, w, w, w, w, w, g, w, w, w, w, w, w, g, d},
        {d, g, g, g, g, t, w, g, w, t, g, g, g, g, g, d},
        {d, g, g, g, g, g, w, g, w, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, w, g, w, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, w, g, w, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, w, g, w, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors sixthMaze[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, t, d},
        {d, g, w, w, w, w, w, w, w, w, w, w, w, w, g, d},
        {d, g, w, g, g, g, g, g, g, g, g, g, g, w, g, d},
        {d, g, w, g, g, g, g, g, g, g, g, g, g, w, g, d},
        {d, g, w, g, g, g, g, g, g, g, g, g, g, w, g, d},
        {d, g, w, g, w, w, w, w, w, g, g, g, g, w, g, d},
        {d, g, w, g, g, g, g, g, w, w, w, g, w, w, g, d},
        {d, g, w, w, w, w, w, g, w, w, w, g, w, w, g, d},
        {d, g, w, g, g, g, w, g, w, w, w, g, w, w, g, d},
        {d, g, w, g, w, g, w, g, w, g, g, g, g, w, g, d},
        {d, g, w, g, w, g, w, g, w, g, g, g, g, w, g, d},
        {d, g, g, g, w, g, g, g, w, t, g, g, t, w, g, d},
        {d, g, w, w, w, w, w, w, w, w, w, w, w, w, g, d},
        {d, t, g, g, g, g, g, g, g, g, g, g, g, g, t, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

// const MazeColors sixthMaze[16][16] = {
//         {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
//         {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
//         {w, w, w, w, w, w, w, w, w, w, w, w, g, g, g, d},
//         {w, w, w, w, w, w, w, w, w, w, w, w, g, g, g, d},
//         {w, w, w, w, w, w, w, w, w, w, w, w, g, g, g, d},
//         {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
//         {d, g, d, d, d, d, d, d, d, d, d, d, d, g, g, d},
//         {d, g, g, g, d, g, g, g, d, g, g, g, d, g, g, d},
//         {d, g, d, g, d, g, d, g, d, g, d, g, d, g, g, d},
//         {d, g, d, g, d, g, d, g, d, g, d, g, d, g, g, d},
//         {d, t, d, g, d, g, d, g, d, g, d, g, d, g, g, d},
//         {d, t, d, g, d, g, d, g, d, g, d, g, d, g, g, d},
//         {d, t, d, g, d, g, d, g, d, g, d, g, d, g, g, d},
//         {d, t, d, g, d, g, d, g, d, g, d, g, d, g, g, d},
//         {d, t, d, g, g, g, d, g, g, g, d, g, g, g, g, d},
//         {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
// };

const MazeColors seventhMaze[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, g, g, g, g, w, g, g, g, g, g, g, g, g, d},
        {d, g, g, g, g, g, w, w, w, g, g, g, w, w, g, d},
        {d, g, g, g, g, g, g, g, w, w, g, g, g, w, g, d},
        {d, g, g, g, g, g, g, g, g, w, w, g, g, g, g, d},
        {d, g, g, g, g, t, g, g, g, w, w, w, g, g, g, d},
        {d, w, w, g, g, g, g, g, g, g, g, w, w, g, w, d},
        {d, g, w, w, w, g, g, w, w, g, g, g, w, g, g, d},
        {d, g, w, w, w, g, g, w, t, g, g, g, w, w, w, d},
        {d, g, w, g, g, g, g, g, g, g, g, g, g, g, w, d},
        {d, w, w, g, g, w, w, g, g, g, t, g, w, g, w, d},
        {d, w, g, g, g, g, w, w, g, g, g, g, g, g, g, d},
        {d, g, g, w, g, g, g, w, w, g, g, w, g, t, g, d},
        {d, g, g, w, g, w, g, g, w, w, g, g, g, g, g, d},
        {d, t, g, g, g, w, g, g, g, w, w, g, g, g, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors eighthMaze[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, w, w, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, g, w, g, w, g, d, g, g, w, w, w, g, g, w, d},
        {d, g, w, g, g, g, g, t, g, w, t, w, g, w, w, d},
        {d, g, w, g, d, d, g, g, g, w, g, w, g, w, t, d},
        {d, g, w, g, g, d, d, g, g, w, g, w, g, g, g, d},
        {d, g, w, g, g, g, d, g, g, w, g, w, g, g, g, d},
        {d, g, g, g, g, g, g, g, g, w, g, w, w, w, g, d},
        {d, g, w, g, g, g, g, g, g, w, g, w, w, w, g, d},
        {d, g, w, g, g, g, d, d, g, g, g, g, g, g, g, d},
        {d, g, w, w, w, t, g, d, g, g, g, g, d, d, g, d},
        {d, g, w, g, t, w, g, d, g, g, g, d, d, g, g, d},
        {d, g, w, g, g, g, g, d, g, g, g, g, g, g, g, d},
        {d, g, w, w, w, w, w, w, w, w, w, w, w, w, w, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors ninthMaze[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, t, t, t, t, t, g, g, g, g, g, g, g, g, d},
        {d, w, w, w, w, w, w, w, w, w, w, w, w, w, g, d},
        {d, g, g, g, g, g, g, g, g, g, g, g, g, w, g, d},
        {d, g, w, w, w, w, w, w, w, w, w, w, g, w, g, d},
        {d, g, w, g, g, g, g, w, g, g, g, g, g, w, g, d},
        {d, g, w, g, w, w, g, w, g, w, w, w, g, w, g, d},
        {d, g, w, g, w, w, g, g, g, w, w, w, g, w, g, d},
        {d, g, w, g, w, w, w, w, w, w, w, w, g, g, g, d},
        {d, g, w, g, g, g, g, g, g, g, w, w, g, w, g, d},
        {d, g, w, w, w, w, w, w, w, g, w, w, g, w, g, d},
        {d, g, w, g, g, w, w, g, g, g, w, w, g, w, g, d},
        {d, g, w, w, g, g, g, g, w, w, w, w, g, w, g, d},
        {d, g, w, w, w, w, w, w, w, w, w, w, g, w, g, d},
        {d, g, w, g, g, g, g, g, g, g, g, g, g, w, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors finalMaze[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, w, d, t, w, d, t, w, d, w, d, t, w, d, g, d},
        {d, t, w, d, t, w, d, t, w, d, w, d, t, d, g, d},
        {d, d, t, w, d, t, w, d, t, w, d, w, d, d, g, d},
        {d, w, d, t, w, d, t, w, d, t, w, d, w, d, g, d},
        {d, t, w, d, t, w, d, t, w, d, t, w, d, d, g, d},
        {d, d, t, w, d, t, w, d, t, w, d, t, w, d, g, d},
        {d, w, d, t, w, d, t, w, d, t, w, d, t, d, g, d},
        {d, t, w, d, t, w, d, t, w, d, t, w, d, d, g, d},
        {d, d, t, w, d, t, w, d, t, w, d, t, w, d, g, d},
        {d, w, d, t, w, d, t, w, d, t, w, d, t, d, g, d},
        {d, t, w, d, t, w, d, t, w, d, t, w, d, d, g, d},
        {d, d, t, w, d, t, w, d, t, w, d, t, w, d, g, d},
        {d, w, d, t, w, d, t, w, d, t, w, d, t, d, g, d},
        {d, t, w, d, t, w, d, t, w, d, t, w, d, d, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors youWin[16][16] = {
        {d, g, g, d, g, g, g, g, g, g, g, g, w, g, g, g},
        {d, g, g, d, d, d, g, g, g, g, g, g, w, w, w, g},
        {d, t, g, g, g, d, d, g, g, g, g, g, g, g, w, w},
        {d, t, g, g, g, g, d, d, d, d, d, g, g, g, g, w},
        {d, t, g, g, g, d, d, g, g, g, g, g, g, g, w, w},
        {d, t, g, d, d, d, g, g, g, g, g, g, w, w, w, g},
        {d, t, g, d, g, g, g, d, d, d, g, g, w, g, g, g},
        {d, g, g, g, g, g, d, g, g, g, d, g, g, g, g, g},
        {d, g, g, g, g, g, d, g, g, g, d, g, w, g, w, w},
        {d, g, g, g, g, g, d, g, g, g, d, g, g, g, g, g},
        {d, g, g, g, g, g, g, d, d, d, g, g, g, g, g, g},
        {d, g, g, g, g, g, g, g, g, g, g, g, w, w, w, g},
        {d, g, g, g, g, g, g, d, d, d, g, g, g, w, g, g},
        {d, g, g, g, g, g, g, g, g, g, d, g, w, g, g, g},
        {d, g, g, g, g, g, g, g, g, g, d, g, w, g, g, g},
        {d, g, g, g, g, g, g, d, d, d, g, g, g, w, w, g},
};

const MazeColors lindLine[16][16] = {
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
        {d, g, g, g, g, g, t, g, t, g, t, g, g, g, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, g, d},
        {d, w, w, w, w, g, g, g, g, g, g, w, g, d, g, d},
        {d, g, g, g, w, g, w, w, w, w, g, w, w, d, g, d},
        {d, g, g, g, g, g, g, g, g, w, g, g, w, d, t, d},
        {d, w, g, w, w, g, g, g, g, g, g, w, g, d, g, d},
        {d, g, g, g, g, g, w, g, w, w, g, w, w, d, g, d},
        {d, g, w, w, w, g, g, g, g, g, g, g, w, d, g, d},
        {d, g, g, w, g, g, g, w, w, w, g, w, g, d, t, d},
        {d, g, g, g, w, g, g, g, w, g, g, w, w, d, g, d},
        {d, g, g, g, g, g, g, g, g, w, g, g, w, d, g, d},
        {d, g, g, w, w, g, g, g, g, g, g, w, g, d, g, d},
        {d, w, w, w, w, g, g, g, w, g, g, w, w, d, g, d},
        {d, g, g, g, g, g, g, w, g, w, g, g, w, d, g, d},
        {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
};

const MazeColors secretMaze[16][16] = {
        {q, g, q, q, q, q, q, q, q, q, q, q, q, q, q, q},
        {q, g, g, g, g, g, g, g, g, g, g, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, g, g, g, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, q, q, g, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, g, q, q, g, g, g, g, q},
        {q, g, g, g, q, q, q, g, g, g, q, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, g, g, q, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, g, g, q, g, g, g, g, q},
        {q, g, g, g, q, q, q, g, g, g, q, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, g, q, q, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, q, q, g, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, q, g, g, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, g, g, g, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, g, g, g, g, g, g, g, q},
        {q, g, g, g, g, g, g, g, g, g, g, g, g, g, g, g},
        {q, q, q, q, q, q, q, q, q, q, q, q, q, q, q, q},
};

// const MazeColors secretMaze[16][16] = {
//         {d, g, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
//         {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, d},
//         {d, g, g, g, g, g, g, d, d, d, g, g, g, g, g, d},
//         {d, g, g, g, g, d, d, d, g, g, d, d, g, g, g, d},
//         {d, g, g, g, d, d, g, g, g, g, g, d, g, g, g, d},
//         {d, g, g, d, d, g, g, g, g, g, w, g, d, g, g, d},
//         {d, g, g, d, d, g, t, t, t, g, w, g, d, g, g, d},
//         {d, g, g, d, d, g, g, g, g, g, w, g, d, g, g, d},
//         {d, g, g, d, d, g, t, t, g, g, w, g, d, g, g, d},
//         {d, g, g, d, g, g, g, g, g, g, w, g, d, g, g, d},
//         {d, g, g, d, d, g, g, g, g, g, g, d, g, g, g, d},
//         {d, g, g, g, d, d, g, g, g, g, g, d, g, g, g, d},
//         {d, g, g, g, g, d, d, d, g, d, d, g, g, g, g, d},
//         {d, g, g, g, g, g, g, d, d, g, g, g, g, g, g, d},
//         {d, g, g, g, g, g, g, g, g, g, g, g, g, g, g, g},
//         {d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d},
// };

#define BALL_SPEED 0.25

signed int mazenum = 69;

MazeColors getMazePixel(int x, int y) {
  switch(mazenum) {
    case -1:
      return lindLine[x][y];
      break;
    case 0:
      return mazeOne[x][y];
      break;
    case 1:
      return mazeTwo[x][y];
      break;
    case 2:
      return thirdMaze[x][y];
      break;
    case 3:
      return fourthMaze[x][y];
      break;
    case 4:
      return fifthMaze[x][y];
      break;
    case 5:
      return sixthMaze[x][y];
      break;
    case 6:
      return seventhMaze[x][y];
      break;
    case 7:
      return eighthMaze[x][y];
      break;
    case 8:
      return ninthMaze[x][y];
      break;
    case 9:
      return finalMaze[x][y];
      break;
    case 10:
      return youWin[x][y];
      break;
    case 69:
      return secretMaze[x][y];
    default:
      return d;
  }
}

int mazebright = 255;


double ballx = 14.0;
double bally = 14.0;
double ballxspeed = 0.0;
double ballyspeed = 0.0;

unsigned long startTime = millis();


int reachedTargets[5][3] = {
  {0, 0, 1}, // {target x, target y, 1 if reached and 0 if not}
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1},
  {0, 0, 1}
};

void populateReachedTargets() {
  if (mazenum != 9) {
    int targetCount = 0;
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        if (getMazePixel(i, j) == t) {
          reachedTargets[targetCount][0] = i;
          reachedTargets[targetCount][1] = j;
          reachedTargets[targetCount][2] = 0;

          Serial.print(i);
          Serial.print(", ");
          Serial.print(j);
          Serial.println();
          targetCount++;
        }
      }
    }
  }
}

bool checkIfTargetsReached() {
  bool targetsReached = true;
  for (int i = 0; i < 5; i++) {
    if (reachedTargets[i][2] != 1) {
      targetsReached = false;
    }
  }
  if (mazenum == 9) {
    targetsReached = true;
  }
  return targetsReached;
}


int counter = 0;
int numColors = 255;

long HSBtoRGB(float _hue, float _sat, float _brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    
    if (_sat == 0.0) {
        red = _brightness;
        green = _brightness;
        blue = _brightness;
    } else {
        if (_hue == 360.0) {
            _hue = 0;
        }

        int slice = _hue / 60.0;
        float hue_frac = (_hue / 60.0) - slice;

        float aa = _brightness * (1.0 - _sat);
        float bb = _brightness * (1.0 - _sat * hue_frac);
        float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
        
        switch(slice) {
            case 0:
                red = _brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = _brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = _brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = _brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = _brightness;
                break;
            case 5:
                red = _brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    long ired = red * 255.0;
    long igreen = green * 255.0;
    long iblue = blue * 255.0;
    
    return long((ired << 16) | (igreen << 8) | (iblue));
}

int red;
int green;
int blue;

void tickRGBEnd() {
  if (checkIfTargetsReached()) {
      float colorNumber = counter > numColors ? counter - numColors: counter;
    
    // Play with the saturation and brightness values
    // to see what they do
    float saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
    float brightness = 1; // Between 0 and 1 (0 = dark, 1 is full brightness)
    float hue = (colorNumber / float(numColors)) * 360; // Number between 0 and 360
    long color = HSBtoRGB(hue, saturation, brightness); 
    
    // Get the red, blue and green parts from generated color
    red = color >> 16 & 255;
    green = color >> 8 & 255;
    blue = color & 255;

    //Serial.println(green);
    if (mazenum != 9) {
      matrix.drawPixel(1, 1, matrix.Color(red, green, blue));
    } else {
      matrix.drawPixel(1, 14, matrix.Color(red, green, blue));
    }
    
    // Counter can never be greater then 2 times the number of available colors
    // the colorNumber = line above takes care of counting backwards (nicely looping animation)
    // when counter is larger then the number of available colors
    counter = (counter + 1) % (numColors * 2);
  }
}

void doBallPhysics() {  
  
  ballx += ballxspeed;
  if (getMazePixel((int)ballx, (int) bally) == w || getMazePixel((int)ballx, (int) bally) == d || (getMazePixel((int)ballx, (int) bally) == t && mazenum == 69)) {
    ballx -= ballxspeed;
  }
  bally += ballyspeed;
  if (getMazePixel((int)ballx, (int) bally) == w || getMazePixel((int)ballx, (int) bally) == d || (getMazePixel((int)ballx, (int) bally) == t && mazenum == 69)) {
    bally -= ballyspeed;
  }
  ballx = constrain(ballx, 0.0, 15.0);
  bally = constrain(bally, 0.0, 15.0);
  int tempx = (int) ballx;
  int tempy = (int) bally;
  for (int i = 0; i < 5; i++) {
    if (reachedTargets[i][0] == tempx && reachedTargets[i][1] == tempy) {
      reachedTargets[i][2] = 1;
    }
  }

}

void checkIfLevelFinished() {
  if (((int) ballx == 1 && (int) bally == 1 && checkIfTargetsReached()) || ((int) ballx == 1 && (int) bally == 14 && mazenum == 9)) {
    levelTransition();
    mazenum++;
    #ifndef OFFLINE
    if (mazenum == 10) {
      if (!client.isConnected()) {
        client.connect(System.deviceID());
        client.subscribe("lindenline/leaderboardmsg");
      }
      unsigned long temp;
      Serial.println("timer stopped");
      if (millis() > startTime) {
        temp = millis() - startTime;
      } else {
        temp = 1337; // in case of a rollover
      }
      Serial.println("Publishing:");
      Serial.println(String(temp));
      client.publish("lindenline/leaderboard", String(temp));
    
    }
    #endif
    if (mazenum == 0) {
      startTime = millis();
      Serial.println("timer started");
    }
    if (mazenum == 11) {
      mazenum = -1;
    }

    ballx = 14.0;
    bally = 14.0;
    
  }
}

void levelTransition() {
  for (int i = 0; i < 16; i++) {
      matrix.fillScreen(0);
      drawMaze(i, i, mazenum);
      int tempmazenum = mazenum + 1;
      if (mazenum == 10) {
        tempmazenum = -1;
      }
      drawMaze(i - 16, i - 16, tempmazenum);
      if (i == 8 && mazenum != 10) {
        mazenum++;
        populateReachedTargets();
        mazenum--;
      } else if (i == 8 && mazenum == 10) {
        mazenum = -1;
        populateReachedTargets();
        mazenum = 10;
      }
      matrix.show();
      delay(100);
  }
}

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
  char stuff[21];
    for (size_t ii = 0; ii < len; ii++) {
        stuff[ii] = data[ii];
    }
    String stuffconv = stuff;
    xsample = stuffconv.substring(0, stuffconv.indexOf(" ")).toInt();
    ysample = stuffconv.substring(stuffconv.indexOf(" ") + 1, stuffconv.lastIndexOf(" ")).toInt();
    ysample = -ysample; // reverse y axis
    zsample = stuffconv.substring(stuffconv.lastIndexOf(" ") + 1).toInt();
    //Serial.println(xsample);
}

void drawMaze(signed int offsetx, signed int offsety) {
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      switch(getMazePixel(x, y)) {
        case d:
          matrix.drawPixel(x + offsetx, y + offsety, matrix.Color(0, 0, mazebright));
          break;
        case w:
          matrix.drawPixel(x + offsetx, y + offsety, matrix.Color(mazebright, 0, 0));
          break;
        case g:
          //matrix.drawPixel(x, y, matrix.Color(0, 255, 0));
          break;
        case t:
          if (mazenum == 9) {
          matrix.drawPixel(x + offsetx, y + offsety, matrix.Color(0, 255, 0));
          } 
          break;
        case e:
          matrix.drawPixel(x + offsetx, y + offsety, matrix.Color(0, mazebright, 0));
          break;

        case q:
          matrix.drawPixel(x + offsetx, y + offsety, matrix.Color(red, green, blue));
          break;
      }
    }
  }
  if (mazenum != 9 && mazenum != 69) {
    for (int i = 0; i < 5; i++) {
      //Serial.print("checking pixel");
      //Serial.println(i);
      if (reachedTargets[i][2] != 1) {
        matrix.drawPixel(reachedTargets[i][0] + offsetx, reachedTargets[i][1] + offsety, matrix.Color(mazebright, mazebright, 0));
        //Serial.print("drawing pixel ");
        //Serial.println(i);
      } else {
        matrix.drawPixel(reachedTargets[i][0] + offsetx, reachedTargets[i][1] + offsety, matrix.Color(0, mazebright, 0));
      }
    }
  }
}

void drawMaze(signed int offsetx, signed int offsety, signed int levelNum) {
  signed int temp = mazenum;
  mazenum = levelNum;
  drawMaze(offsetx, offsety);
  mazenum = temp;
  
}

void connectui() {
    matrix.fillScreen(0);
  //drawMaze();
  matrix.setCursor(0, 0);
  matrix.setTextSize(1);
  matrix.print("con");
  matrix.show();
  while (!BLE.connected()) {
    Vector<BleScanResult> scanResults = BLE.scan();
      if (scanResults.size()) {
        Log.info("%d devices found", scanResults.size());

        for (int ii = 0; ii < scanResults.size(); ii++) {
            String name = scanResults[ii].advertisingData().deviceName();
            if (name.length() > 0  && name.equals("maximus")) {
                Log.info("Connecting to: %s", name.c_str());
                peer = BLE.connect(scanResults[ii].address());
                if (peer.connected()) {
                peer.getCharacteristicByUUID(peerTxCharacteristic, txUuid);
                peer.getCharacteristicByUUID(peerRxCharacteristic, rxUuid);

                // Could do this instead, but since the names are not as standardized, UUIDs are better
                // peer.getCharacteristicByDescription(peerTxCharacteristic, "tx");
              }
            }
        }
    }
  }
  matrix.fillScreen(0);
  //drawMaze();
  matrix.setCursor(0, 0);
  matrix.setTextSize(1);
  matrix.print("ct");
  matrix.show();
}

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void setup() {
  setupLCDs();
  firstScoreDisplay.writeDigitAscii(1, 'A');
  firstScoreDisplay.writeDisplay();
  matrix.begin();
  matrix.setTextWrap(true);
  matrix.setBrightness(25);
  matrix.setTextColor(matrix.Color(80,255,0));



  BLE.on();
  peerTxCharacteristic.onDataReceived(onDataReceived, &peerTxCharacteristic);
  connectui();
  // delay(2000);
  populateReachedTargets();
  
  //BLE.on();
  
}


void loop() {
  doBallPhysics();
  ballxspeed = map((double) xsample, -16000.0, 16000.0, -BALL_SPEED, BALL_SPEED);
  ballyspeed = map((double) ysample, -16000.0, 16000.0, -BALL_SPEED, BALL_SPEED);
  matrix.fillScreen(0);
  //drawMaze();
  matrix.setCursor(0, 0);
  //matrix.print(xsample);
  drawMaze(0, 0);
  tickRGBEnd();
  matrix.drawPixel((int) ballx, (int) bally, matrix.Color(255, 0, 255));
  checkIfLevelFinished();
  matrix.show();
  #ifndef OFFLINE
    if (client.isConnected()) {
    client.loop();
    } else {
      client.connect(System.deviceID());
      client.subscribe("lindenline/leaderboardmsg");
    }
  #endif
  if (!BLE.connected()) {
    connectui();
  }
  if (mazenum == -1) {
    writeNum(0);
  } else if (mazenum != 10) {
    writeNum(millis() - startTime);
    helperDisplay.writeDigitAscii(0, '0');
    helperDisplay.writeDigitAscii(1, '0');
    helperDisplay.writeDigitAscii(2, '0');
    helperDisplay.writeDigitAscii(3, '0');
    helperDisplay.writeDisplay();
  }
  
}


