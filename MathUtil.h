#pragma once

#include	"Gz.h"

class Vector3
{
public:
	float base[3] = { 0, 0, 0 };
	Vector3(float x, float y, float z);
	Vector3(float* a);
	Vector3 Subtract(Vector3 v2);
	Vector3 Add(Vector3 v2);
	Vector3 Mult(float s);
	Vector3 Crossproduct(Vector3 v2);
	float DotProduct(Vector3 v2);
	Vector3 Normalize();
	float Length();
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
		return Vector3(data[9 + 3 * index + 0], data[9 + 3 * index + 1], data[9 + 3 * index + 2]);
	}
	void Triangle::SetUV(int index, Vector3 xyz) {
		data[18 + 3 * index + 0] = xyz.base[0];
		data[18 + 3 * index + 1] = xyz.base[1];
		data[18 + 3 * index + 2] = xyz.base[2];
	}
	Vector3 Triangle::GetUV(int index) {
		return Vector3(data[18 + 3 * index + 0], data[18 + 3 * index + 1], data[18 + 3 * index + 2]);
	}
};

float intersection(Vector3 rayOrigin, Vector3 ray, Vector3 planeOrigin, Vector3 planeNormal, Vector3* intersection, bool test = false);
bool positionInTriangle(Vector3* triangleCoords, Vector3 position);


