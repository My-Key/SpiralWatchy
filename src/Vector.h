#pragma once

#include <Arduino.h>
#include "VectorInt.h"

struct Vector
{
    float x;
    float y;

public:
    static float dotProduct(const Vector& v1, const Vector& v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    static float crossProduct(const Vector& v1, const Vector& v2)
    {
        return v1.x * v2.y - v1.y * v2.x;
    }

    static Vector rotateVectorByRightAngle(const Vector& vector, const int& angle)
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

    static Vector rotateVector(const Vector& vector, const float& angle)
    {
        float radians = angle * DEG_TO_RAD;
        float sinAngle = sin(radians);
        float cosAngle = cos(radians);

        Vector newVector = {0,0};
        newVector.x = cosAngle * vector.x - sinAngle * vector.y;
        newVector.y = sinAngle * vector.x + cosAngle * vector.y;
        return newVector;
    }

    static Vector rotateVector(const Vector& vector, const float& sinAngle, const float& cosAngle)
    {
        Vector newVector = {0,0};
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

    Vector operator+(const Vector& a) const
    {
        return {x + a.x, y + a.y};
    }

    Vector operator-(const Vector& a) const
    {
        return {x - a.x, y - a.y};
    }

    Vector operator*(const float scale) const
    {
        return {x * scale, y * scale};
    }

    operator VectorInt() const {return {x, y};}
};