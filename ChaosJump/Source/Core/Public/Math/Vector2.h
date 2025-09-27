#pragma once
#include <string>

struct Vector2
{
    float x = 0, y = 0;

    Vector2 operator+(const Vector2& otherVector) const;
    Vector2 operator-(const Vector2& otherVector) const;
    Vector2 operator*(float number) const;
    Vector2 operator*(const Vector2& otherVector) const;
    Vector2 operator/(const Vector2& otherVector) const;
    Vector2 operator/(float number) const;
    Vector2& operator+=(const Vector2& otherVector);
    Vector2& operator-=(const Vector2& otherVector);
    Vector2& operator*=(float number);
    Vector2& operator/=(float number);
    Vector2& operator*=(const Vector2& otherVector);
    Vector2& operator/=(const Vector2& otherVector);
    bool operator==(const Vector2& vector2) const;

    float dot(const Vector2& otherVector) const;
    float size() const;
    float squaredSize() const;
    bool isNormalized() const;
    bool isAlmostZero(float tolerance = std::numeric_limits<float>::epsilon()) const;

    bool normalize();
    bool normalize(float& outOldSize);
    Vector2 getNormalized() const;

    std::string toString() const;
};

inline Vector2 operator*(float number, const Vector2& otherVector)
{
    return otherVector * number;
}
inline Vector2 operator/(float number, const Vector2& otherVector)
{
    return otherVector / number;
}

inline Vector2 operator-(const Vector2& otherVector)
{
    return -1.f * otherVector;
}



