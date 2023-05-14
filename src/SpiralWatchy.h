#pragma once

#include <Watchy.h>
#include "../include/images.h"
#include "../include/MatCapSource.h"
#include "Vector.h"
#include "../include/BlueNoise200.h"
#include "../include/SpiralFaceShadow.h"
#include "../include/SpiralFaceShadowCenter.h"
#include "../include/SpiralFaceWithShadow.h"

class SpiralWatchy : public Watchy
{
public:
  SpiralWatchy(const watchySettings& s);
  void drawWatchFace();

  float getBatteryFill();

  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                              int16_t w, int16_t h, uint16_t color, bool even);


  void DrawHand(float angle, float size);                           
  void fillTriangle(VectorInt v0, Vector uv0, VectorInt v1, Vector uv1, VectorInt v2, Vector uv2, const uint8_t bitmap[], int w, int h);
  
   void drawLine(int x, int y, int w, VectorInt v0, Vector uv0, VectorInt a, Vector uv1, VectorInt b, Vector uv2, float den, const uint8_t *bitmap, int16_t bw, int16_t bh);

                                
  void writeFastHLineUV(int16_t x, int16_t y, int16_t w, Vector uvA, Vector uvB, const uint8_t bitmap[], int bw, int bh);

  void fillTriangle2(VectorInt v0, Vector uv0, VectorInt v1, Vector uv1, VectorInt v2, Vector uv2, const uint8_t bitmap[], int w, int h);
  
  void drawLine2(int x, int y, int w, VectorInt v0, Vector uv0, VectorInt a, Vector uv1, VectorInt b, Vector uv2, float den, const uint8_t *bitmap, int16_t bw, int16_t bh);
            
  void writeFastHLineUV2(int16_t x, int16_t y, int16_t w, Vector uvA, Vector uvB, const uint8_t bitmap[], int bw, int bh);

  

  void fillTriangle(VectorInt v0, Vector uv0, VectorInt v1, Vector uv1, VectorInt v2, Vector uv2, const uint8_t bitmap[], int w, int h, uint16_t color);
  
  void drawLine(int x, int y, int w, VectorInt v0, Vector uv0, VectorInt a, Vector uv1, VectorInt b, Vector uv2, float den, const uint8_t *bitmap, int16_t bw, int16_t bh, uint16_t color);
            
  void writeFastHLineUV(int16_t x, int16_t y, int16_t w, Vector uvA, Vector uvB, const uint8_t bitmap[], int bw, int bh, uint16_t color);
};
