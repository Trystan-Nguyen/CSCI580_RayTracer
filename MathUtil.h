#pragma once

#include	"Gz.h"
#include <cstdlib>

class Vector3
{
public:
	float base[3] = { 0, 0, 0 };
	Vector3(float x, float y, float z);
	Vector3(float* a);
	Vector3() { base[0] = -1; base[1] = -1; base[2] = -1; };
	Vector3 Subtract(Vector3 v2);
	Vector3 Add(Vector3 v2);
	Vector3 Mult(float s);
	Vector3 Crossproduct(Vector3 v2);
	float DotProduct(Vector3 v2);
	Vector3 Normalize();
	float Length();

	Vector3& operator+=(const Vector3& v) {
		base[0] += v.base[0];
		base[1] += v.base[1];
		base[2] += v.base[2];
		return *this;
	}
};

class Vector4
{

public:
	float base[4] = { 0,0,0,0 };

	Vector4(float x, float y, float z);
	Vector4(float x, float y, float z, float l);

	void Add(Vector4 v2);
	void Subtract(Vector4 v2);
	void Multiply(float s);

	float* ToVector3F();
	Vector3 ToVector3();

	void InvertDirection();
	Vector4 Crossproduct(Vector4);

	float GetSlopeXY() { return base[0] / base[1]; };
	float GetSlopeZY() { return base[2] / base[1]; };
};

void MatrixIdentity(GzMatrix a);
void MatrixMult(GzMatrix a, GzMatrix b, GzMatrix result);
//void MatrixMult(GzMatrix a, Vector4 b, Vector4 result);
void MatrixMultNorm(GzMatrix a, GzMatrix b, GzMatrix result);
Vector4 MatrixMult(GzMatrix a, Vector4 b);

Vector3 interpolateVector(Vector3 a, Vector3 b, float weightA);

Vector3 interpolateVector3(Vector3* pos, Vector3 ref, Vector3* interpolVals);



class Triangle
{
public:
	float* data;
	bool useTexture = false;
	Triangle::Triangle() {
		data = new float[37]();
	}
	void Triangle::SetPositions(int index, Vector3 xyz) {
		data[3 * index + 0] = xyz.base[0];
		data[3 * index + 1] = xyz.base[1];
		data[3 * index + 2] = xyz.base[2];
	}
	Vector3 Triangle::GetPosition(int index) {
		return Vector3(data[3 * index + 0], data[3 * index + 1], data[3 * index + 2]);
	}
	void Triangle::SetNorms(int index, Vector3 xyz) {
		data[9 + 3 * index + 0] = xyz.base[0];
		data[9 + 3 * index + 1] = xyz.base[1];
		data[9 + 3 * index + 2] = xyz.base[2];
	}
	Vector3 Triangle::GetNorms(int index) {
		return Vector3(data[9 + (3 * index) + 0], data[9 + (3 * index) + 1], data[9 + (3 * index) + 2]);
	}
	void Triangle::SetUV(int index, Vector3 xyz) {
		data[18 + (3 * index) + 0] = xyz.base[0];
		data[18 + (3 * index) + 1] = xyz.base[1];
		data[18 + (3 * index) + 2] = 0;

		float a = xyz.base[0];
		float b = xyz.base[1];

		return;
	}
	Vector3 Triangle::GetUV(int index) {
		Vector3 ret = Vector3(data[18 + (3 * index) + 0], data[18 + (3 * index) + 1], 0);

		float a = data[18 + (3 * index) + 0];
		float b = data[18 + (3 * index) + 1];

		return ret;
	}

	void Triangle::SetKa(float r, float g, float b) {
		data[27 + 0] = r;
		data[27 + 1] = g;
		data[27 + 2] = b;
	}
	Vector3 Triangle::GetKa() {
		return Vector3(data[27 + 0], data[27 + 1], data[27 + 2]);
	}
	void Triangle::SetKd(float r, float g, float b) {
		data[30 + 0] = r;
		data[30 + 1] = g;
		data[30 + 2] = b;
	}
	Vector3 Triangle::GetKd() {
		return Vector3(data[30 + 0], data[30 + 1], data[30 + 2]);
	}
	void Triangle::SetKs(float r, float g, float b) {
		data[33 + 0] = r;
		data[33 + 1] = g;
		data[33 + 2] = b;
	}
	Vector3 Triangle::GetKs() {
		return Vector3(data[33 + 0], data[33 + 1], data[33 + 2]);
	}
	void Triangle::SetSpec(float s) {
		data[36] = s;
	}
	float Triangle::GetSpec() { return data[36]; }


};

float intersection(Vector3* rayOrigin, Vector3 ray, Vector3 planeOrigin, Vector3 planeNormal, Vector3* intersection, bool test = false);
bool positionInTriangle(Vector3* triangleCoords, Vector3 position);

inline double random_double() {
	// Returns a random real in [0,1).
	return std::rand() / (RAND_MAX + 1.0);
}

Vector3 sample_square();