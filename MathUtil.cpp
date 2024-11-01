#include "stdafx.h"
#include "MathUtil.h"


Vector4::Vector4(float x, float y, float z) 
{
	base[0] = x;
	base[1] = y;
	base[2] = z;
	base[3] = 1;
}

Vector4::Vector4(float x, float y, float z, float l)
{
	float len = sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));

	base[0] = l * x / len;
	base[1] = l * y / len;
	base[2] = l * z / len;
	base[3] = 1;
}

void Vector4::Add(Vector4 v2)
{
	base[0] += v2.base[0];
	base[1] += v2.base[1];
	base[2] += v2.base[2];
	base[3] += v2.base[3];
}

void Vector4::Subtract(Vector4 v2)
{
	base[0] -= v2.base[0];
	base[1] -= v2.base[1];
	base[2] -= v2.base[2];
	base[3] -= v2.base[3];
}

void Vector4::Multiply(float s)
{
	base[0] *= s;
	base[1] *= s;
	base[2] *= s;
	base[3] *= s;
}


float* Vector4::ToVector3F()
{
	float vec3[3] = { base[0] / base[3], base[1] / base[3] , base[2] / base[3] };
	return vec3;
}

Vector3 Vector4::ToVector3()
{
	return Vector3(base[0] / base[3], base[1] / base[3], base[2] / base[3]);
}

Vector4 Vector4::Crossproduct(Vector4 cross) 
{
	Vector4 ret = Vector4(0,0,0);
	float* v1 = this->ToVector3F();
	float* v2 = cross.ToVector3F();

	ret.base[0] = v1[1] * v2[2] - v1[2] * v2[1];
	ret.base[1] = -v1[0] * v2[2] + v1[2] * v2[0];
	ret.base[2] = v1[0] * v2[1] - v1[1] * v2[0];

	float x = v1[1] * v2[2] - v1[2] * v2[1];
	float y = -v1[0] * v2[2] + v1[2] * v2[0];
	float z = v1[0] * v2[1] - v1[1] * v2[0];

	return ret;
}

void Vector4::InvertDirection()
{
	this->base[0] *= -1;
	this->base[1] *= -1;
	this->base[2] *= -1;
}



//------------------------------------------------------------------------------------------------------------


Vector3::Vector3(float x, float y, float z) {
	base[0] = x;
	base[1] = y;
	base[2] = z;
}
Vector3::Vector3(float* b) {
	base[0] = b[0];
	base[1] = b[1];
	base[2] = b[2];
}

Vector3 Vector3::Subtract(Vector3 v2) {
	return Vector3(base[0] - v2.base[0], base[1] - v2.base[1], base[2] - v2.base[2]);
}
Vector3 Vector3::Add(Vector3 v2) {
	return Vector3(base[0] + v2.base[0], base[1] + v2.base[1], base[2] + v2.base[2]);
}
Vector3 Vector3::Mult(float s) {
	return Vector3(base[0] * s, base[1] * s, base[2] * s);
}
Vector3 Vector3::Crossproduct(Vector3 vec2) {
	Vector3 ret = Vector3(0, 0, 0);
	float* v1 = base;
	float* v2 = vec2.base;

	ret.base[0] = v1[1] * v2[2] - v1[2] * v2[1];
	ret.base[1] = -v1[0] * v2[2] + v1[2] * v2[0];
	ret.base[2] = v1[0] * v2[1] - v1[1] * v2[0];

	return ret;
}
float Vector3::DotProduct(Vector3 v2) {
	return base[0] * v2.base[0] + base[1] * v2.base[1] + base[2] * v2.base[2];
}

Vector3 Vector3::Normalize() {
	float d = sqrt(pow(base[0], 2) + pow(base[1], 2) + pow(base[2], 2));

	return Vector3(base[0] / d, base[1] / d, base[2] / d);
}

float Vector3::Length() {
	float d = sqrt(pow(base[0], 2) + pow(base[1], 2) + pow(base[2], 2));
	return d;
}


//------------------------------------------------------------------------------------------------------------



void MatrixIdentity(GzMatrix a) {
	for(int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			a[i][j] = i == j ? 1 : 0;
		}
	}
}
void MatrixMult(GzMatrix a, GzMatrix b, GzMatrix result) {
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			double entry = 0.0;
			for (int k = 0; k < 4; k++)
				entry += a[i][k] * b[k][j];
			result[i][j] = entry;
		}
	}
}
Vector4 MatrixMult(GzMatrix a, Vector4 b) {
	Vector4 result(0, 0, 0);
	for (int i = 0; i < 4; ++i) {
		result.base[i] = 0;
		for (int j = 0; j < 4; ++j) {
			result.base[i] += a[i][j] * b.base[j];
		}
	}
	return result;
}

void MatrixMultNorm(GzMatrix a, GzMatrix b, GzMatrix result) {
	GzMatrix normTransform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	for (int i = 0; i < 3; ++i) {
		float total = 0;
		for (int j = 0; j < 3; ++j) {
			total += pow(b[i][j], 2);
		}
		float scale = sqrtf(total);
		for (int j = 0; j < 3; ++j) {
			normTransform[i][j] = b[i][j] / scale;
		}
	}

	MatrixMult(a, normTransform, result);
}

Vector3 interpolateVector(Vector3 a, Vector3 b, float weightA)
{
	return a.Mult(weightA).Add(b.Mult(1.0 - weightA));
}

Vector3 interpolateVector3(Vector3* pos, Vector3 ref, Vector3* interpolVals)
{
	float a_weight = ((pos[1].Subtract(ref)).Crossproduct((pos[2].Subtract(ref)))).Length() / (1 + (pos[0].base[2] / (MAXINT - pos[0].base[2])));
	float b_weight = ((pos[0].Subtract(ref)).Crossproduct((pos[2].Subtract(ref)))).Length() / (1 + (pos[1].base[2] / (MAXINT - pos[1].base[2])));
	float c_weight = ((pos[0].Subtract(ref)).Crossproduct((pos[1].Subtract(ref)))).Length() / (1 + (pos[2].base[2] / (MAXINT - pos[2].base[2])));

	float total_weight = a_weight + b_weight + c_weight;

	Vector3 result = interpolVals[0].Mult(a_weight / total_weight).Add(interpolVals[1].Mult(b_weight / total_weight)).Add(interpolVals[2].Mult(c_weight / total_weight));
	return result;
}

float intersection(Vector3 rayOrigin, Vector3 ray, Vector3 planeOrigin, Vector3 planeNormal, Vector3* position, bool test)
{
	float LeftEq_t = 0;
	float RightEq = 0;

	for (int i = 0; i < 3; ++i)
	{
		LeftEq_t += planeNormal.base[i] * ray.base[i];
		RightEq -= planeNormal.base[i] * (rayOrigin.base[i] - planeOrigin.base[i]);
	}

	if (LeftEq_t == 0) return -1;

	float t = RightEq / LeftEq_t;
	if (t < 0) return -1;

	Vector3 intersection = rayOrigin.Add(ray.Mult(t));
	float mag = 0;
	for (int i = 0; i < 3; ++i) {
		position->base[i] = intersection.base[i];
		mag += pow(rayOrigin.base[i] + intersection.base[i], 2);
	}
	return sqrtf(mag);
}

bool positionInTriangle(Vector3* triangleCoords, Vector3 position) {
	float a_weight = ((triangleCoords[1].Subtract(position)).Crossproduct((triangleCoords[2].Subtract(position)))).Length();
	float b_weight = ((triangleCoords[0].Subtract(position)).Crossproduct((triangleCoords[2].Subtract(position)))).Length();
	float c_weight = ((triangleCoords[0].Subtract(position)).Crossproduct((triangleCoords[1].Subtract(position)))).Length();

	float total_weight = a_weight + b_weight + c_weight;

	float triArea = ((triangleCoords[1].Subtract(triangleCoords[0])).Crossproduct((triangleCoords[2].Subtract(triangleCoords[0])))).Length();

	return fabs(triArea - total_weight) < 0.001;
}