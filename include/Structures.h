#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)
#endif

#include <math.h>

/*

Represnets an ARGB (alpha, red, green, blue) color.

*/
struct Color
{
	Color(unsigned char a_ = 255, unsigned char r_ = 255, unsigned char g_ = 255, unsigned char b_ = 255)
	{
		this->a = a_;
		this->r = r_;
		this->g = g_;
		this->b = b_;
	}

	static Color ToRGB(unsigned long hex)
	{
		Color c;
		c.a = ((hex >> 24) & 0xff);
		c.r = ((hex >> 16) & 0xff);
		c.g = ((hex >> 8) & 0xff);
		c.b = ((hex) & 0xff);
		return c;
	}

	static unsigned long ToHex(Color rgb)
	{
		return (unsigned long)((((rgb.a) & 0xff) << 24) | (((rgb.r) & 0xff) << 16) | (((rgb.g) & 0xff) << 8) | ((rgb.b) & 0xff));
	}

	Color operator + (const Color & c) const
	{
		return Color(a + c.a, r + c.r, g + c.g, b + c.b);
	}

	Color operator - (const Color & c) const
	{
		return Color(a - c.a, r - c.r, g - c.g, b - c.b);
	}

	bool operator == (const Color & c) const
	{
		return ((a == c.a) && (r == c.b) && (g == c.g) && (b == c.b));
	}

	unsigned char a, r, g, b;
};

/*

Represents an HSV (hue, saturation, value) color.

*/
struct HSV
{
	float h;
	float s;
	float v;
};

/*

Represents an ordered pair of integer x- and y-coordinates that defines a point in a two-dimensional plane.

*/
struct Point
{
	Point(int x_ = 0, int y_ = 0)
	{
		this->x = x_;
		this->y = y_;
	}

	Point operator + (const Point & p) const
	{
		return Point(x + p.x, y + p.y);
	}

	Point operator - (const Point & p) const
	{
		return Point(x - p.x, y - p.y);
	}

	bool operator == (const Point & p) const
	{
		return ((x == p.x) && (y == p.y));
	}

	int x, y;
};

/*

2D Vector

*/
struct Vector2
{
	Vector2(Point p)
	{
		this->x = p.x;
		this->y = p.y;
	}

	Vector2(float x_ = 0.0f, float y_ = 0.0f)
	{
		this->x = x_;
		this->y = y_;
	}

	static float Angle(const Vector2 & from, const Vector2 & to)
	{
		float dot = Dot(from.Normalized(), to.Normalized());

		if (dot < -1.0f) { dot = -1.0f; }

		if (dot > 1.0f) { dot = 1.0f; }

		return acos(dot) * 57.29578f;
	}

	float Magnitude() const
	{
		return sqrt(x * x + y * y);
	}

	float SqrMagnitude() const
	{
		return x * x + y * y;
	}

	Vector2 Normalized() const
	{
		Vector2 result = Vector2(x, y);
		result.Normalize();
		return result;
	}

	void Normalize()
	{
		float m = Magnitude();

		if (m)
		{
			const float il = 1.0f / m;
			x *= il;
			y *= il;
		}
		else
		{
			x = y = 0.0f;
		}
	}

	float Distance(const Vector2 & v) const
	{
		float dx = x - v.x;
		float dy = y - v.y;
		return sqrt(dx * dx + dy * dy);
	}

	static float Dot(const Vector2 & lhs, const Vector2 & rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	float Dot(const Vector2 & v) const
	{
		return x * v.x + y * v.y;
	}

	static Vector2 Lerp(Vector2 a, Vector2 b, float t)
	{
		if (t < 0.0f) { t = 0.0f; }

		if (t > 1.0f) { t = 1.0f; }

		return Vector2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
	}

	static Vector2 LerpUnclamped(Vector2 a, Vector2 b, float t)
	{
		return Vector2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
	}

	const Vector2 & operator = (const Vector2 & v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	float & operator [] (int index)
	{
		return (&x)[index];
	}

	float operator [] (int index) const
	{
		return (&x)[index];
	}

	bool operator == (const Vector2 & v) const
	{
		return ((x == v.x) && (y == v.y));
	}

	bool operator != (const Vector2 & v) const
	{
		return ((x != v.x) || (y != v.y));
	}

	Vector2 operator - () const
	{
		return Vector2(-x, -y);
	}

	Vector2 operator + (const Vector2 & v) const
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 operator - (const Vector2 & v) const
	{
		return Vector2(x - v.x, y - v.y);
	}

	Vector2 operator * (float f) const
	{
		return Vector2(x * f, y * f);
	}

	Vector2 operator / (float f) const
	{
		f = 1.0f / f; return Vector2(x * f, y * f);
	}

	Vector2 & operator += (const Vector2 & v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2 & operator -= (const Vector2 & v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2 & operator *= (float f)
	{
		x *= f;
		y *= f;
		return *this;
	}

	Vector2 & operator /= (float f)
	{
		f = 1.0f / f;
		x *= f;
		y *= f;
		return *this;
	}

	float x, y;
};

/*

Stores an odered pair of integers, which specify a Height and Width.

*/
struct Size
{
	Size(int w_ = 0, int h_ = 0)
	{
		this->w = w_;
		this->h = h_;
	}

	Size operator + (const Size & s) const
	{
		return Size(w + s.w, h + s.h);
	}

	Size operator - (const Size & s) const
	{
		return Size(w - s.w, h - s.h);
	}

	bool operator == (const Size & s) const
	{
		return ((w == s.w) && (h == s.h));
	}

	int w, h;
};

/*

Stores a set of four integers that represent the location and size of a rectangle.

*/
struct Rect
{
	Rect(int x_ = 0, int y_ = 0, int w_ = 0, int h_ = 0)
	{
		this->x = x_;
		this->y = y_;
		this->w = w_;
		this->h = h_;
	}

	Rect(Point pos, Size size)
	{
		this->x = pos.x;
		this->y = pos.y;
		this->w = size.w;
		this->h = size.h;
	}

	Rect operator + (const Rect & r) const
	{
		return Rect(x + r.x, y + r.y, w + r.w, h + r.h);
	}

	Rect operator - (const Rect & r) const
	{
		return Rect(x - r.x, y - r.y, w - r.w, h - r.h);
	}

	bool operator == (const Rect & r) const
	{
		return ((x == r.x) && (y == r.y) && (w == r.w) && (h == r.h));
	}

	int x, y, w, h;
};

#endif