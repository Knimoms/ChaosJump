#pragma once

struct Vector2
{
    float x = 0, y = 0;

    Vector2 operator+(const Vector2& otherVector) const;
    Vector2 operator-(const Vector2& otherVector) const;
    Vector2 operator*(float number) const;
    Vector2 operator/(float number) const;
    Vector2& operator+=(const Vector2& otherVector);
    Vector2& operator-=(const Vector2& otherVector);
    Vector2& operator*=(float number);
    Vector2& operator/=(float number);

    float dot(const Vector2& otherVector) const;
    float size() const;
    float squaredSize() const;
    
    bool normalize();
    Vector2 getNormalized() const;
    
};

inline Vector2 operator*(float number, const Vector2& otherVector)
{
    return otherVector * number;
}
inline Vector2 operator/(float number, const Vector2& otherVector)
{
    return otherVector / number;
}


