#pragma once

#include <Arduino.h>

struct VectorInt
{
    int x;
    int y;

public:
    static float dotProduct(const VectorInt& v1, const VectorInt& v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    static float crossProduct(const VectorInt& v1, const VectorInt& v2)
    {
        return v1.x * v2.y - v1.y * v2.x;
    }

    static VectorInt rotateVectorByRightAngle(const VectorInt& vector, const int& angle)
    {
        int tempAngle = angle % 4;

        switch (tempAngle)
        {
            case 1:
            return {-vector.y, vector.x};
            case 2:
            return {-vector.x, -vector.y};
            case 3:
            return {vector.y, -vector.x};
            default:
            return vector;
        }
    }

    static VectorInt rotateVector(const VectorInt& vector, const float& angle)
    {
        float radians = angle * DEG_TO_RAD;
        float sinAngle = sin(radians);
        float cosAngle = cos(radians);

        VectorInt newVector = {0,0};
        newVector.x = cosAngle * vector.x - sinAngle * vector.y;
        newVector.y = sinAngle * vector.x + cosAngle * vector.y;
        return newVector;
    }

    void normalize()
    {
        float magnitude = sqrt(x * x + y * y);

        if (magnitude <= 0.0)
            return;

        x /= magnitude;
        y /= magnitude;
    }

    void scale(const float& scale)
    {
        x *= scale;
        y *= scale;
    }

    VectorInt operator+(const VectorInt& a) const
    {
        return {x + a.x, y + a.y};
    }

    VectorInt operator-(const VectorInt& a) const
    {
        return {x - a.x, y - a.y};
    }

    VectorInt operator*(const float scale) const
    {
        return {x * scale, y * scale};
    }
};