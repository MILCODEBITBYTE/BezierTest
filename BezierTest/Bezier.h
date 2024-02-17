#pragma once
typedef struct tagPVector
{
	float x;
	float y;

	tagPVector()
	{
		x = y = 0;
	}

	tagPVector(const float in_x, const float in_y)
	{
		x = in_x;
		y = in_y;
	}

	static tagPVector Lerp(const tagPVector a, const tagPVector b, float rate)
	{
		float _x, _y;
		_x = a.x * (1 - rate) + b.x * rate;
		_y = a.y * (1 - rate) + b.y * rate;


		return tagPVector(_x, _y);
	}
}PVector;

static PVector operator + (const PVector& a, const PVector& b)
{
	return PVector(a.x + b.x, a.y + b.y);
}

static PVector operator - (const PVector& a, const PVector& b)
{
	return PVector(a.x - b.x, a.y - b.y);
}

static PVector operator * (const PVector& a, const float& b)
{
	return PVector(a.x * b, a.y * b);
}

static PVector operator / (const PVector& a, const float& b)
{
	return PVector(a.x / b, a.y / b);
}

static PVector& operator += (PVector& a, const PVector& b)
{
	a.x += b.x;
	a.y += b.y;

	return a;
}

static PVector& operator -= (PVector& a, const PVector& b)
{
	a.x -= b.x;
	a.y -= b.y;

	return a;
}

static PVector& operator *= (PVector& a, const float& b)
{
	a.x *= b;
	a.y *= b;

	return a;
}

static PVector& operator /= (PVector& a, const float& b)
{
	a.x /= b;
	a.y /= b;

	return a;
}

typedef struct tagMatrix3x3
{
	float m[3][3];
	float& operator()(int i, int j)
	{
		return m[i][j];
	}
	const float operator()(int i, int j) const
	{
		return m[i][j];
	}
	float* operator[](int i)
	{
		return m[i];
	}

	void Identity()
	{
		m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
		m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
	}

	void Translate(float x, float y)
	{
		m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
		m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
		m[2][0] = x; m[2][1] = y; m[2][2] = 1;
	}

	void Scale(float x, float y)
	{
		m[0][0] = x; m[0][1] = 0; m[0][2] = 0;
		m[1][0] = 0; m[1][1] = y; m[1][2] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
	}

	void Rotate(float angle)
	{
		float radian = angle * 3.141592f / 180.f;
		float c = cosf(radian);
		float s = sinf(radian);

		m[0][0] = c; m[0][1] = -s; m[0][2] = 0;
		m[1][0] = s; m[1][1] = c; m[1][2] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
	}

	void Shear(float x, float y)
	{
		m[0][0] = 1; m[0][1] = x; m[0][2] = 0;
		m[1][0] = y; m[1][1] = 1; m[1][2] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
	}




}MATRIX3x3;

inline MATRIX3x3 MatrixMultiply(const MATRIX3x3& a, const MATRIX3x3& b)
{
	MATRIX3x3 m;

	m[0][0] = a(0, 0) * b(0, 0) + a(0, 1) * b(1, 0) + a(0, 2) * b(2, 0);
	m[0][1] = a(0, 0) * b(0, 1) + a(0, 1) * b(1, 1) + a(0, 2) * b(2, 1);
	m[0][2] = a(0, 0) * b(0, 2) + a(0, 1) * b(1, 2) + a(0, 2) * b(2, 2);

	m[1][0] = a(1, 0) * b(0, 0) + a(1, 1) * b(1, 0) + a(1, 2) * b(2, 0);
	m[1][1] = a(1, 0) * b(0, 1) + a(1, 1) * b(1, 1) + a(1, 2) * b(2, 1);
	m[1][2] = a(1, 0) * b(0, 2) + a(1, 1) * b(1, 2) + a(1, 2) * b(2, 2);

	m[2][0] = a(2, 0) * b(0, 0) + a(2, 1) * b(1, 0) + a(2, 2) * b(2, 0);
	m[2][1] = a(2, 0) * b(0, 1) + a(2, 1) * b(1, 1) + a(2, 2) * b(2, 1);
	m[2][2] = a(2, 0) * b(0, 2) + a(2, 1) * b(1, 2) + a(2, 2) * b(2, 2);

	return m;
}

inline PVector BezierPoint2(PVector p, PVector p2, PVector cp1, PVector cp2, float t)
{
	PVector q0 = PVector::Lerp(p, cp1, t);
	PVector q1 = PVector::Lerp(cp1, cp2, t);
	PVector q2 = PVector::Lerp(cp2, p2, t);

	PVector r0 = PVector::Lerp(q0, q1, t);
	PVector r1 = PVector::Lerp(q1, q2, t);

	PVector r = PVector::Lerp(r0, r1, t);

	return r;
}

inline PVector BezierPoint(PVector p, PVector p2, PVector cp, float t)
{
	PVector q0 = PVector::Lerp(p, cp, t);
	PVector q1 = PVector::Lerp(cp, p2, t);

	PVector r = PVector::Lerp(q0, q1, t);

	return r;
}

inline PVector VectorTransformCoord(const PVector& p, const MATRIX3x3& m)
{
	PVector result;
	result.x = p.x * m(0, 0) + p.y * m(1, 0) + m(2, 0);
	result.y = p.x * m(0, 1) + p.y * m(1, 1) + m(2, 1);

	return result;
}

inline PVector VectorTransform(const PVector& p, const MATRIX3x3& m)
{
	PVector result;
	result.x = p.x * m(0, 0) + p.y * m(1, 0);
	result.y = p.x * m(0, 1) + p.y * m(1, 1);

	return result;
}

inline float Distance(const PVector& a, const PVector& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return sqrtf(dx * dx + dy * dy);
}

typedef struct tagArgLength
{
	PVector s;
	PVector e;
	float t;
	float Length()
	{
		float dx = (e.x - s.x);
		float dy = (e.y - s.y);
		return sqrtf(dx * dx + dy * dy);
	}
}ARC_LENGTH;