#include "Math/Vector2.h"

#include <format>

#include "SDL3/SDL_stdinc.h"

Vector2 Vector2::operator+(const Vector2& otherVector) const
{
    return {.x = x + otherVector.x, .y = y + otherVector.y};
}

Vector2 Vector2::operator-(const Vector2& otherVector) const
{
    return {.x = x - otherVector.x, .y = y - otherVector.y};
}

Vector2 Vector2::operator*(const float number) const
{
    return {.x = x * number, .y = y * number};
}

Vector2 Vector2::operator*(const Vector2& otherVector) const
{
    return {.x = x * otherVector.x, .y = y * otherVector.y};
}

Vector2 Vector2::operator/(const float number) const
{
    return {.x = x / number, .y = y / number};
}

Vector2& Vector2::operator+=(const Vector2& otherVector)
{
    Vector2& thisRef = *this;
    thisRef = thisRef + otherVector;

    return thisRef;
}

Vector2& Vector2::operator-=(const Vector2& otherVector)
{
    Vector2& thisRef = *this;
    thisRef = thisRef - otherVector;

    return thisRef;
}

Vector2& Vector2::operator*=(const float number)
{
    Vector2& thisRef = *this;
    thisRef = thisRef * number;

    return thisRef;
}

Vector2& Vector2::operator/=(const float number)
{
    Vector2& thisRef = *this;
    thisRef = thisRef / number;

    return thisRef;
}

bool Vector2::operator==(const Vector2& vector2) const
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    return std::abs(x - vector2.x) < epsilon && std::abs(y - vector2.y) < epsilon;
}

float Vector2::dot(const Vector2& otherVector) const
{
    return x * otherVector.x + y * otherVector.y;
}

float Vector2::size() const
{
    return SDL_sqrtf(squaredSize());
}

float Vector2::squaredSize() const
{
    return x*x + y*y;
}

bool Vector2::isNormalized() const
{
    return std::abs(squaredSize() - 1.f) < std::numeric_limits<float>::epsilon();
}

bool Vector2::isAlmostZero(const float tolerance) const
{
    return squaredSize() < tolerance;
}

bool Vector2::normalize()
{
    float outSize;
    return normalize(outSize);
}

bool Vector2::normalize(float& outSize)
{
    outSize = size();
    if (outSize < 0.000001f) return false;

    x /= outSize;
    y /= outSize;

    return true;
}

Vector2 Vector2::getNormalized() const
{
    Vector2 vec = *this;
    float outSize;
    if (vec.normalize(outSize)) return vec;
    
    return {.x = 0.0f, .y = 0.0f};
}

std::string Vector2::toString() const
{
    return std::format("X: {:.5f}\n Y: {:.5f}\n", x, y);
}
