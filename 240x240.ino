#define TFT_MOSI            D7
#define TFT_SCLK            D5
#define TFT_CS              D3
#define TFT_DC              D2
#define TFT_RST             D4
 
#define TFT_BL              D1

#include <TFT_eSPI.h> // 需要从管理库中安装
#include <SPI.h>
#include <Wire.h>
#include "bmp1.h"
#include "bmp2.h"
 
#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif
 
#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif
 
#define TFT_BL D1
 
TFT_eSPI tft = TFT_eSPI(240, 240); // Invoke custom library
 
void showImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data);
 
void setup()
{
    Serial.begin(9600);
    Serial.println("Start");
    tft.init();
    tft.setRotation(0); //显示方向设定
    tft.fillScreen(TFT_BLACK);
 
}


 
void loop()
{
     
   // tft.setSwapBytes(true);  //用这一句红色变成绿色
    tft.setSwapBytes(false);   //用这一句正常
    showImage(0, 0,  240, 240, bmp1);
    delay(1000);
    showImage(0, 0,  240, 240, bmp2);
    delay(1000);
    
}
#define PI_BUF_SIZE 128 //

void showImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data){
  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w;
  int32_t dh = h*2;
 
  if (x < 0) { dw += x; dx = -x; x = 0; }
  if (y < 0) { dh += y; dy = -y; y = 0; }
 
  if (dw < 1 || dh < 1) return;
 
 // CS_L;
 
  data += dx + dy * w;
 
  uint16_t  buffer[PI_BUF_SIZE];
  uint16_t* pix_buffer = buffer;
  uint16_t  high,low;
 
  tft.setWindow(x, y, x + dw - 1, y + dh - 1);
 
  // Work out the number whole buffers to send  计算出要发送的整个缓冲区的数字
  uint16_t nb = (dw * dh) / (2 * PI_BUF_SIZE);   //240x2x240/2x128= 450,没有余数，如果60x60x2%2x128 是32 
 
  // Fill and send "nb" buffers to TFT
  for (int32_t i = 0; i < nb; i++) {               //  i < 450 发送450个128
    
    for (int32_t j = 0; j < PI_BUF_SIZE; j++) {    //发送一个128
      high = pgm_read_word(&data[(i * 2 * PI_BUF_SIZE) + 2 * j + 1]);
      low = pgm_read_word(&data[(i * 2 * PI_BUF_SIZE) + 2 * j ]);
      pix_buffer[j] = (high<<8)+low;

     // 内含128个像素数据的pix_buffer准备完成
    }
    tft.pushPixels(pix_buffer, PI_BUF_SIZE); //发送准备好的128个数据，然后返回i=1，准备第二个128个数据并发送。
  }
 //直到发送完整数个128数据
 //下面发送剩余的数据，剩余的已经不够128个了
  // Work out number of pixels not yet sent  计算出尚未发送的像素数
  uint16_t np = (dw * dh) % (2 * PI_BUF_SIZE);   //np是32，实际还有64个8位字节，合成后还有32个16位字节。
 
  // Send any partial buffer left over
  if (np) {                       
    for (int32_t i = 0; i < np; i++)
    {
      
      //   发送60x60 的图片         28 * 2 *128             
      high = pgm_read_word(&data[(nb * 2 * PI_BUF_SIZE) + 2 * i + 1]);
      low = pgm_read_word(&data[(nb * 2 * PI_BUF_SIZE) + 2 * i ]);
      pix_buffer[i] = (high<<8)+low;
    }
    tft.pushPixels(pix_buffer, np);
  }
 
 // CS_H;

 
}
