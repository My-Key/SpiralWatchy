#include "SpiralWatchy.h"

const float VOLTAGE_MIN = 3.5f;
const float VOLTAGE_MAX = 4.2f;
const float VOLTAGE_WARNING = 3.6f;
const float VOLTAGE_RANGE = VOLTAGE_MAX - VOLTAGE_MIN;

const int DENSITY = 1;
const int VECTOR_SIZE = 60 * DENSITY;
const float STEP_ANGLE = 360 / VECTOR_SIZE;

const int STEP_MINUTE = DENSITY;
const int STEP_HOUR = VECTOR_SIZE/12;

const Vector CENTER = {99.5f, 99.5f};
const int RADIUS = 99;
const int RIM_SIZE = 20;
const int FACE_RADIUS = 260 - RIM_SIZE;

const float BATTERY_MIN = 0.5f;
const float BATTERY_RANGE = 1.0f - BATTERY_MIN;
const float BATTERY_WARNING = BATTERY_MIN + ((VOLTAGE_WARNING - VOLTAGE_MIN) / VOLTAGE_RANGE) * BATTERY_RANGE;

const Vector SHADOW_CORNER_1 = {66.0f,66.0f};
const Vector SHADOR_CORNER_2 = {133.0f,66.0f};
const Vector SHADOR_CORNER_3 = {133.0f,133.0f};
const Vector SHADOR_CORNER_4 = {66.0f,133.0f};

const float LOOP_SCALE = 0.45f;

const Vector HAND[] =
{{0.0f, -1.0f},
 {0.0f, -0.8f}, 
 {0.1f, -0.8f},
{0.0f, 0.0f},
 {0.05f, 0.15f},
{-0.05f, 0.15f},
 {-0.1f, -0.8f}};

const Vector HAND_NORMAL[] =
{{0.5f, -0.85f}, {0.2f, -0.2f}, {0.85f, -0.5f},
{0.3f, -0.1f}, {0.96f, -0.1f}, {0.3f, 0.1f}, {0.96f, 0.1f},
{0.0f, 0.3f}, {0.1f, 0.96f}, {-0.1f, 0.96f},
{-0.3f, -0.1f}, {-0.96f, -0.1f}, {-0.3f, 0.1f}, {-0.96f, 0.1f},
{-0.5f, -0.85f}, {-0.2f, -0.2f}, {-0.85f, -0.5f}};

const int HAND_POS_INDEX[] = 
{0,1,2,
1,2,3,
2,3,4,
3,4,5,
3,5,6,
3,6,1,
6,1,0};

const int HAND_POS_LEN = 7;

const int HAND_NORMAL_INDEX[] = 
{0,1,2,
3,4,5,
4,5,6,
7,8,9,
10,11,13,
10,13,12,
14,15,16};

const int HAND_OUTLINE_INDEX[] = {0,2,4,5,6};

const int HAND_OUTLINE_LEN = 5;

Vector EDGE_NORMAL[VECTOR_SIZE];

float SCALE[VECTOR_SIZE * 4];

SpiralWatchy::SpiralWatchy(const watchySettings& s) : Watchy(s)
{
  Vector up = {-1.0f, 0.0f};
  
  for (int i = 0; i < VECTOR_SIZE; i++)
  {
    EDGE_NORMAL[i] = Vector::rotateVector(up, i * STEP_ANGLE);
    EDGE_NORMAL[i].normalize();
  }

  for (int i = 0; i < VECTOR_SIZE * 4; i++)
  {
    SCALE[i] = pow(LOOP_SCALE, i / (float)VECTOR_SIZE);
  }
}

static float lerp(float a, float b, float f)
{
    return a * (1.0f - f) + (b * f);
}

void SpiralWatchy::drawWatchFace()
{
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);

  int hour = currentTime.Hour;
  int minute = currentTime.Minute;
  
  float minuteNormalized = minute / 60.0f;
  float hourNormalized = (hour + minuteNormalized) / 24.0f;

  float batteryFill = getBatteryFill();
  float batteryFillScale = BATTERY_MIN + BATTERY_RANGE * batteryFill;
  float rimSize = RIM_SIZE * batteryFillScale;

  for (int i = minute; i < VECTOR_SIZE * 3 + minute; i++)
  {
    int index = i % VECTOR_SIZE;
    int nextIndex = (i + 1) % VECTOR_SIZE;

    int scaleIndex = i - minute;
    int scaleNextIndex = scaleIndex + 1;

    float currentLoopSCale = SCALE[scaleIndex];

    float scale1 = FACE_RADIUS * currentLoopSCale;
    Vector v1 = EDGE_NORMAL[index] * scale1 + CENTER;
    Vector uv1 = EDGE_NORMAL[index] * RADIUS + CENTER;

    float nextLoopScale = SCALE[scaleNextIndex];
    float scale2 = FACE_RADIUS * nextLoopScale;
    Vector v2 = EDGE_NORMAL[nextIndex] * scale2 + CENTER;
    Vector uv2 = EDGE_NORMAL[nextIndex] * RADIUS + CENTER;

    float scale3 = scale1 * LOOP_SCALE;
    Vector v1a = EDGE_NORMAL[index] * scale3 + CENTER;
    Vector uv1a = EDGE_NORMAL[index] * RADIUS * LOOP_SCALE + CENTER;

    float scale4 = scale2 * LOOP_SCALE;
    Vector v2a = EDGE_NORMAL[nextIndex] * scale4 + CENTER;
    Vector uv2a = EDGE_NORMAL[nextIndex] * RADIUS * LOOP_SCALE + CENTER;

    fillTriangle2(v1a, uv1a, v1, uv1, v2, uv2, SpiralFaceWithShadow, 200, 200);
    fillTriangle2(v2a, uv2a, v1a, uv1a, v2, uv2, SpiralFaceWithShadow, 200, 200);

    Vector v4 = EDGE_NORMAL[index] * (scale1 + rimSize * currentLoopSCale) + CENTER;
    Vector uv3 = EDGE_NORMAL[index] * -RADIUS + CENTER;
    Vector uv4 = EDGE_NORMAL[index] * RADIUS + CENTER;

    Vector v6 = EDGE_NORMAL[nextIndex] * (scale2 + rimSize * nextLoopScale) + CENTER;
    Vector uv5 = EDGE_NORMAL[nextIndex] * -RADIUS + CENTER;
    Vector uv6 = EDGE_NORMAL[nextIndex] * RADIUS + CENTER;

    fillTriangle2(v1, uv3, v4, uv4, v2, uv5, MatCapSource, 200, 200);
    fillTriangle2(v4, uv4, v2, uv5, v6, uv6, MatCapSource, 200, 200);

    display.drawLine(v1.x, v1.y, v2.x, v2.y, GxEPD_BLACK);
    display.drawLine(v4.x, v4.y, v6.x, v6.y, GxEPD_BLACK);
  }

  for (int i = VECTOR_SIZE * 3 + minute; i < VECTOR_SIZE * 4 + minute - 1; i++)
  {
    int index = i % VECTOR_SIZE;
    int nextIndex = (i + 1) % VECTOR_SIZE;

    int scaleIndex = i - minute;
    int scaleNextIndex = scaleIndex + 1;

    float currentLoopSCale = SCALE[scaleIndex];

    float scale1 = FACE_RADIUS * currentLoopSCale;
    Vector v1 = EDGE_NORMAL[index] * scale1 + CENTER;

    float nextLoopScale = SCALE[scaleNextIndex];
    float scale2 = FACE_RADIUS * nextLoopScale;
    Vector v2 = EDGE_NORMAL[nextIndex] * scale2 + CENTER;

    Vector v4 = EDGE_NORMAL[index] * (scale1 + rimSize * currentLoopSCale) + CENTER;

    Vector v6 = EDGE_NORMAL[nextIndex] * (scale2 + rimSize * nextLoopScale) + CENTER;

    display.drawTriangle(v1.x, v1.y, v4.x, v4.y, v2.x, v2.y, GxEPD_BLACK);
    display.drawTriangle(v4.x, v4.y, v2.x, v2.y, v6.x, v6.y, GxEPD_BLACK);
  }

  fillTriangle(SHADOW_CORNER_1, SHADOW_CORNER_1, SHADOR_CORNER_2, SHADOR_CORNER_2, SHADOR_CORNER_3, SHADOR_CORNER_3, SpiralFaceShadowCenter, 200, 200, GxEPD_BLACK);
  fillTriangle(SHADOR_CORNER_3, SHADOR_CORNER_3, SHADOR_CORNER_4, SHADOR_CORNER_4, SHADOW_CORNER_1, SHADOW_CORNER_1, SpiralFaceShadowCenter, 200, 200, GxEPD_BLACK);

  float hourAngle = ((float)(hour % 12) + minuteNormalized) * 30;

  DrawHand(hourAngle, 70);
  DrawHand(minute * 6, 90);
}

void SpiralWatchy::DrawHand(float angle, float size)
{
  float radians = angle * DEG_TO_RAD;
  float sinAngle = sin(radians);
  float cosAngle = cos(radians);

  for (int i = 0; i < HAND_POS_LEN; i++)
  {
    Vector v1 = Vector::rotateVector(HAND[HAND_POS_INDEX[i * 3]], sinAngle, cosAngle) * size + CENTER;
    Vector v2 = Vector::rotateVector(HAND[HAND_POS_INDEX[i * 3 + 1]], sinAngle, cosAngle) * size + CENTER;
    Vector v3 = Vector::rotateVector(HAND[HAND_POS_INDEX[i * 3 + 2]], sinAngle, cosAngle) * size + CENTER;

    Vector uv1 = Vector::rotateVector(HAND_NORMAL[HAND_NORMAL_INDEX[i * 3]], sinAngle, cosAngle) * 99 + CENTER;
    Vector uv2 = Vector::rotateVector(HAND_NORMAL[HAND_NORMAL_INDEX[i * 3 + 1]], sinAngle, cosAngle) * 99 + CENTER;
    Vector uv3 = Vector::rotateVector(HAND_NORMAL[HAND_NORMAL_INDEX[i * 3 + 2]], sinAngle, cosAngle) * 99 + CENTER;

    fillTriangle2(v1, uv1, v2, uv2, v3, uv3, MatCapSource, 200, 200);
  }

  Vector currentPoint = Vector::rotateVector(HAND[HAND_OUTLINE_INDEX[0]], sinAngle, cosAngle) * size + CENTER;

  for (int i = 0; i < HAND_OUTLINE_LEN; i++)
  {
    Vector nextPoint = Vector::rotateVector(HAND[HAND_OUTLINE_INDEX[(i + 1) % HAND_OUTLINE_LEN]], sinAngle, cosAngle) * size + CENTER;

    display.drawLine(currentPoint.x, currentPoint.y, nextPoint.x, nextPoint.y, GxEPD_BLACK);

    currentPoint = nextPoint;
  }
}

float SpiralWatchy::getBatteryFill()
{
  float VBAT = getBatteryVoltage();

  // 12 battery states
  float batState = ((VBAT - VOLTAGE_MIN) / VOLTAGE_RANGE);
  if (batState > 1.0f)
    batState = 1.0f;
  if (batState < 0.0f)
    batState = 0.0f;

  return batState;
}

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

#ifndef _swap_vector
#define _swap_vector(a, b)                                                    \
  {                                                                            \
    Vector t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

#ifndef _swap_vector_int
#define _swap_vector_int(a, b)                                                    \
  {                                                                            \
    VectorInt t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

static float clamp(float val, float min, float max)
{
  if (val > max)
    val = max;
  
  if (val < min)
    val = min;

  return val;
}

static void barycentric(VectorInt p, VectorInt v0, VectorInt v1, VectorInt a, float invDen, float &u, float &v, float &w)
{
    VectorInt v2 = p - a;
    // ToDo: Premultiply v0 and v1 by invDen?
    v = (v2.x * v1.y - v1.x * v2.y) * invDen;
    w = (v0.x * v2.y - v2.x * v0.y) * invDen;
    u = 1.0 - v - w;
}


static bool getColor(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h) 
{
  int16_t byteWidth = (w + 7) / 8;
  return (pgm_read_byte(bitmap + y * byteWidth + x / 8) & (128 >> (x & 7)));
}

void SpiralWatchy::drawLine(int x, int y, int w, VectorInt v0, Vector uv0, VectorInt a, Vector uv1, VectorInt b, Vector uv2, float invDen, const uint8_t *bitmap, int16_t bw, int16_t bh)
{
  for (int i = 0; i < w; i++)
  {
    float ua, va, wa;
    VectorInt pointA = {x + i, y};
    barycentric(pointA, a, b, v0, invDen, ua, va, wa);

    Vector uv = uv0 * ua + uv1 * va + uv2 * wa;

    bool white = getColor(uv.x, uv.y, bitmap, bw, bh);
    display.drawPixel(x + i, y, white ? GxEPD_WHITE : GxEPD_BLACK);
  }
}

void SpiralWatchy::fillTriangle(VectorInt v0, Vector uv0, VectorInt v1, Vector uv1, VectorInt v2, Vector uv2, const uint8_t bitmap[], int w, int h)
{
  int16_t a, b, y, last;
  Vector uvA, uvB;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (v0.y > v1.y) {
    _swap_vector_int(v0, v1);
    _swap_vector(uv0, uv1);
  }
  if (v1.y > v2.y) {
    _swap_vector_int(v2, v1);
    _swap_vector(uv2, uv1);
  }
  if (v0.y > v1.y) {
    _swap_vector_int(v0, v1);
    _swap_vector(uv0, uv1);
  }

  display.startWrite();
  if (v0.y == v2.y) { // Handle awkward all-on-same-line case as its own thing
    a = b = v0.x;
    uvA = uv0;
    uvB = uv0;

    if (v1.x < a)
    {
      a = v1.x;
      uvA = uv1;
    }
    else if (v1.x > b)
    { 
      b = v1.x;
      uvB = uv1;
    }
    if (v2.x < a)
    {
      a = v2.x;
      uvA = uv2;
    }
    else if (v2.x > b)
    {
      b = v2.x;
      uvB = uv2;
    }

    writeFastHLineUV(a, v0.y, b - a + 1, uvA, uvB, bitmap, w, h);
    display.endWrite();
    return;
  }

  int16_t dx01 = v1.x - v0.x, dy01 = v1.y - v0.y, 
          dx02 = v2.x - v0.x, dy02 = v2.y - v0.y,
          dx12 = v2.x - v1.x, dy12 = v2.y - v1.y;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (v1.y == v2.y)
    last = v1.y; // Include y1 scanline
  else
    last = v1.y - 1; // Skip it

    
  VectorInt aa = v1 - v0, bb = v2 - v0;
  float invDen = 1 / VectorInt::crossProduct(aa, bb);

  int startY = v0.y;

  if (startY < 0)
  {
    sa += -startY * dx01;
    sb += -startY * dx02;
    startY = 0;
  }

  for (y = startY; y <= last && y < 200; y++) {
    a = v0.x + sa / dy01;
    b = v0.x + sb / dy02;

    sa += dx01;
    sb += dx02;

    
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);

    drawLine(a, y, b - a + 1, v0, uv0, aa, uv1, bb, uv2, invDen, bitmap, w, h);
  }

  startY = last + 1;

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (startY - v1.y);
  sb = (int32_t)dx02 * (startY - v0.y);
  
  if (startY < 0)
  {
    sa += -startY * dx12;
    sb += -startY * dx02;
    startY = 0;
  }
 
  int endY = v2.y;

  if (endY > 200)
    endY = 200;

  for (y = startY; y <= endY; y++) {
    a = v1.x + sa / dy12;
    b = v0.x + sb / dy02;

    sa += dx12;
    sb += dx02;

    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);

    drawLine(a, y, b - a + 1, v0, uv0, aa, uv1, bb, uv2, invDen, bitmap, w, h);
  }
  display.endWrite();
}

void SpiralWatchy::writeFastHLineUV(int16_t x, int16_t y, int16_t w, Vector uvA, Vector uvB, const uint8_t bitmap[], int bw, int bh)
{
  display.startWrite();

  for (int i = 0; i < w; i++)
  {
    float lerpVal = i / (w + 1.0);
    Vector uv = (uvA * lerpVal) + (uvB * (1.0 - lerpVal));
    bool white = getColor(uv.x, uv.y, bitmap, bw, bh);
    display.drawPixel(x + i, y, white ? GxEPD_WHITE : GxEPD_BLACK);
  }

  display.endWrite();
}

static bool getColor2(int16_t x, int16_t y, int16_t xUv, int16_t yUv, const uint8_t *bitmap, int16_t w, int16_t h) 
{
  return bitmap[yUv * w + xUv] > BlueNoise200[y * 200 + x];
}

void SpiralWatchy::drawLine2(int x, int y, int w, VectorInt v0, Vector uv0, VectorInt a, Vector uv1, VectorInt b, Vector uv2, float invDen, const uint8_t *bitmap, int16_t bw, int16_t bh)
{
  for (int i = 0; i < w; i++)
  {
    float ua, va, wa;
    VectorInt pointA = {x + i, y};
    barycentric(pointA, a, b, v0, invDen, ua, va, wa);

    Vector uv = uv0 * ua + uv1 * va + uv2 * wa;

    bool white = getColor2(x + i, y, uv.x, uv.y, bitmap, bw, bh);
    display.drawPixel(x + i, y, white ? GxEPD_WHITE : GxEPD_BLACK);
  }
}

void SpiralWatchy::fillTriangle2(VectorInt v0, Vector uv0, VectorInt v1, Vector uv1, VectorInt v2, Vector uv2, const uint8_t bitmap[], int w, int h)
{
  int16_t a, b, y, last;
  Vector uvA, uvB;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (v0.y > v1.y) {
    _swap_vector_int(v0, v1);
    _swap_vector(uv0, uv1);
  }
  if (v1.y > v2.y) {
    _swap_vector_int(v2, v1);
    _swap_vector(uv2, uv1);
  }
  if (v0.y > v1.y) {
    _swap_vector_int(v0, v1);
    _swap_vector(uv0, uv1);
  }

  display.startWrite();
  if (v0.y == v2.y) { // Handle awkward all-on-same-line case as its own thing
    a = b = v0.x;
    uvA = uv0;
    uvB = uv0;

    if (v1.x < a)
    {
      a = v1.x;
      uvA = uv1;
    }
    else if (v1.x > b)
    { 
      b = v1.x;
      uvB = uv1;
    }
    if (v2.x < a)
    {
      a = v2.x;
      uvA = uv2;
    }
    else if (v2.x > b)
    {
      b = v2.x;
      uvB = uv2;
    }

    writeFastHLineUV2(a, v0.y, b - a + 1, uvA, uvB, bitmap, w, h);
    display.endWrite();
    return;
  }

  int16_t dx01 = v1.x - v0.x, dy01 = v1.y - v0.y, 
          dx02 = v2.x - v0.x, dy02 = v2.y - v0.y,
          dx12 = v2.x - v1.x, dy12 = v2.y - v1.y;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (v1.y == v2.y)
    last = v1.y; // Include y1 scanline
  else
    last = v1.y - 1; // Skip it

    
  VectorInt aa = v1 - v0, bb = v2 - v0;
  float invDen = 1 / VectorInt::crossProduct(aa, bb);

  int startY = v0.y;

  if (startY < 0)
  {
    sa += -startY * dx01;
    sb += -startY * dx02;
    startY = 0;
  }

  for (y = startY; y <= last && y < 200; y++) {
    a = v0.x + sa / dy01;
    b = v0.x + sb / dy02;

    sa += dx01;
    sb += dx02;

    
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);

    if (y >= 0 && y < 200)
      drawLine2(a, y, b - a + 1, v0, uv0, aa, uv1, bb, uv2, invDen, bitmap, w, h);
  }

  startY = last + 1;

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (startY - v1.y);
  sb = (int32_t)dx02 * (startY - v0.y);
  
  if (startY < 0)
  {
    sa += -startY * dx12;
    sb += -startY * dx02;
    startY = 0;
  }
 
  int endY = v2.y;

  if (endY > 200)
    endY = 200;

  for (y = startY; y <= endY; y++) {
    a = v1.x + sa / dy12;
    b = v0.x + sb / dy02;

    sa += dx12;
    sb += dx02;

    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);

    drawLine2(a, y, b - a + 1, v0, uv0, aa, uv1, bb, uv2, invDen, bitmap, w, h);
  }
  display.endWrite();
}

void SpiralWatchy::writeFastHLineUV2(int16_t x, int16_t y, int16_t w, Vector uvA, Vector uvB, const uint8_t bitmap[], int bw, int bh)
{
  display.startWrite();

  for (int i = 0; i < w; i++)
  {
    float lerpVal = i / (w + 1.0);
    Vector uv = (uvA * lerpVal) + (uvB * (1.0 - lerpVal));
    bool white = getColor2(x + i, y, uv.x, uv.y, bitmap, bw, bh);
    display.drawPixel(x + i, y, white ? GxEPD_WHITE : GxEPD_BLACK);
  }

  display.endWrite();
}

void SpiralWatchy::drawLine(int x, int y, int w, VectorInt v0, Vector uv0, VectorInt a, Vector uv1, VectorInt b, Vector uv2, float invDen, const uint8_t *bitmap, int16_t bw, int16_t bh, uint16_t color)
{
  for (int i = 0; i < w; i++)
  {
    float ua, va, wa;
    VectorInt pointA = {x + i, y};
    barycentric(pointA, a, b, v0, invDen, ua, va, wa);

    Vector uv = uv0 * ua + uv1 * va + uv2 * wa;

    bool draw = getColor2(x + i, y, uv.x, uv.y, bitmap, bw, bh);

    if (!draw)
      display.drawPixel(x + i, y, color);
  }
}

void SpiralWatchy::fillTriangle(VectorInt v0, Vector uv0, VectorInt v1, Vector uv1, VectorInt v2, Vector uv2, const uint8_t bitmap[], int w, int h, uint16_t color)
{
  int16_t a, b, y, last;
  Vector uvA, uvB;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (v0.y > v1.y) {
    _swap_vector_int(v0, v1);
    _swap_vector(uv0, uv1);
  }
  if (v1.y > v2.y) {
    _swap_vector_int(v2, v1);
    _swap_vector(uv2, uv1);
  }
  if (v0.y > v1.y) {
    _swap_vector_int(v0, v1);
    _swap_vector(uv0, uv1);
  }

  display.startWrite();
  if (v0.y == v2.y) { // Handle awkward all-on-same-line case as its own thing
    a = b = v0.x;
    uvA = uv0;
    uvB = uv0;

    if (v1.x < a)
    {
      a = v1.x;
      uvA = uv1;
    }
    else if (v1.x > b)
    { 
      b = v1.x;
      uvB = uv1;
    }
    if (v2.x < a)
    {
      a = v2.x;
      uvA = uv2;
    }
    else if (v2.x > b)
    {
      b = v2.x;
      uvB = uv2;
    }

    writeFastHLineUV(a, v0.y, b - a + 1, uvA, uvB, bitmap, w, h, color);
    display.endWrite();
    return;
  }

  int16_t dx01 = v1.x - v0.x, dy01 = v1.y - v0.y, 
          dx02 = v2.x - v0.x, dy02 = v2.y - v0.y,
          dx12 = v2.x - v1.x, dy12 = v2.y - v1.y;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (v1.y == v2.y)
    last = v1.y; // Include y1 scanline
  else
    last = v1.y - 1; // Skip it

    
  VectorInt aa = v1 - v0, bb = v2 - v0;
  float invDen = 1 / VectorInt::crossProduct(aa, bb);

  int startY = v0.y;

  if (startY < 0)
  {
    sa += -startY * dx01;
    sb += -startY * dx02;
    startY = 0;
  }

  for (y = startY; y <= last && y < 200; y++) {
    a = v0.x + sa / dy01;
    b = v0.x + sb / dy02;

    sa += dx01;
    sb += dx02;

    
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);

    drawLine(a, y, b - a + 1, v0, uv0, aa, uv1, bb, uv2, invDen, bitmap, w, h, color);
  }

  startY = last + 1;

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (startY - v1.y);
  sb = (int32_t)dx02 * (startY - v0.y);

  if (startY < 0)
  {
    sa += -startY * dx12;
    sb += -startY * dx02;
    startY = 0;
  }

  int endY = v2.y;

  if (endY > 200)
    endY = 200;

  for (y = startY; y <= endY; y++) {
    a = v1.x + sa / dy12;
    b = v0.x + sb / dy02;

    sa += dx12;
    sb += dx02;

    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);

   drawLine(a, y, b - a + 1, v0, uv0, aa, uv1, bb, uv2, invDen, bitmap, w, h, color);
  }
  display.endWrite();
}

void SpiralWatchy::writeFastHLineUV(int16_t x, int16_t y, int16_t w, Vector uvA, Vector uvB, const uint8_t bitmap[], int bw, int bh, uint16_t color)
{
  display.startWrite();

  for (int i = 0; i < w; i++)
  {
    float lerpVal = i / (w + 1.0);
    Vector uv = (uvA * lerpVal) + (uvB * (1.0 - lerpVal));
    bool draw = getColor2(x + i, y, uv.x, uv.y, bitmap, bw, bh);

    if (!draw)
      display.drawPixel(x + i, y, color);
  }

  display.endWrite();
}