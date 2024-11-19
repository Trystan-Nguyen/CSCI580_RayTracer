/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include <cstdlib>

#define PI (float) 3.14159265358979323846
bool pushIdentityNorm = false;

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
	/* HW 3.1
	// Create rotate matrix : rotate along x axis
	// Pass back the matrix using mat value
	*/

	MatrixIdentity(mat);

	mat[1][1] = cos(degree * PI / 180);
	mat[1][2] = -sin(degree * PI / 180);

	mat[2][1] = sin(degree * PI / 180);
	mat[2][2] = cos(degree * PI / 180);

	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
	/* HW 3.2
	// Create rotate matrix : rotate along y axis
	// Pass back the matrix using mat value
	*/

	MatrixIdentity(mat);

	mat[0][0] = cos(degree * PI / 180);
	mat[0][2] = sin(degree * PI / 180);

	mat[2][0] = -sin(degree * PI / 180);
	mat[2][2] = cos(degree * PI / 180);

	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
	/* HW 3.3
	// Create rotate matrix : rotate along z axis
	// Pass back the matrix using mat value
	*/

	MatrixIdentity(mat);

	mat[0][0] = cos(degree * PI / 180);
	mat[0][1] = -sin(degree * PI / 180);

	mat[1][0] = sin(degree * PI / 180);
	mat[1][1] = cos(degree * PI / 180);

	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
	/* HW 3.4
	// Create translation matrix
	// Pass back the matrix using mat value
	*/

	MatrixIdentity(mat);

	mat[0][3] = translate[0];
	mat[1][3] = translate[1];
	mat[2][3] = translate[2];

	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
	/* HW 3.5
	// Create scaling matrix
	// Pass back the matrix using mat value
	*/

	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];

	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
	/* HW1.1 create a framebuffer for MS Windows display:
	 -- set display resolution
	 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
	 -- allocate memory for pixel buffer
	 */
	this->xres = xRes;
	this->yres = yRes;

	int bufferSize = xRes * yRes;
	pixelbuffer = new GzPixel[bufferSize]();
	framebuffer = new char[bufferSize * 3]();

	/* HW 3.6
	- setup Xsp and anything only done once
	- init default camera
	*/
	this->m_camera = GzCamera();

	this->m_camera.lookat[0] = 0;
	this->m_camera.lookat[1] = 0;
	this->m_camera.lookat[2] = 0;

	this->m_camera.position[0] = DEFAULT_IM_X;
	this->m_camera.position[1] = DEFAULT_IM_Y;
	this->m_camera.position[2] = DEFAULT_IM_Z;

	this->m_camera.worldup[0] = 0;
	this->m_camera.worldup[1] = 1;
	this->m_camera.worldup[2] = 0;

	this->m_camera.FOV = DEFAULT_FOV;

	matlevel = 0;
	MatrixIdentity(Ximage[0]);
	MatrixIdentity(Xnorm[0]);

	this->numlights = 0;

	numTriangles = 0;
	triangleList = new Triangle[MAX_TRIANGLES]();
}

GzRender::~GzRender()
{
	/* HW1.2 clean up, free buffer memory */
	delete[] pixelbuffer;
	delete[] framebuffer;
}

int GzRender::GzDefault()
{
	/* HW1.3 set pixel buffer to some default values - start a new frame */

	for (int i = 0; i < this->xres * this->yres; ++i)
	{
		pixelbuffer[i].red = 2055;
		pixelbuffer[i].green = 1798;
		pixelbuffer[i].blue = 1541;
		//pixelbuffer[i].alpha = 1;
		pixelbuffer[i].z = INT_MAX;
	}

	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
	/* HW 3.7
	- setup for start of each frame - init frame buffer color,alpha,z
	- compute Xiw and projection xform Xpi from camera definition
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw
	- now stack contains Xsw and app can push model Xforms when needed
	*/

	GzDefault();

	GzMatrix xsp = {
		this->xres / 2,		0.0,				0.0,	this->xres / 2,
		0.0,				-this->yres / 2,	0.0,	this->yres / 2,
		0.0,				0.0,				MAXINT,	0.0,
		0.0,				0.0,				0.0,	1.0
	};

	float invD = tan((m_camera.FOV * PI / 180) / 2);
	GzMatrix xpi = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, invD, 0.0,
		0.0, 0.0, invD, 1.0
	};

	Vector3 lookat = Vector3(m_camera.lookat[0], m_camera.lookat[1], m_camera.lookat[2]);
	Vector3 pos = Vector3(m_camera.position[0], m_camera.position[1], m_camera.position[2]);
	Vector3 worldUp = Vector3(m_camera.worldup[0], m_camera.worldup[1], m_camera.worldup[2]);

	Vector3 z = (lookat.Subtract(pos)).Normalize();
	Vector3 y = (worldUp.Subtract(z.Mult(worldUp.DotProduct(z)))).Normalize();
	Vector3 x = y.Crossproduct(z);
	GzMatrix xiw = {
		x.base[0], x.base[1], x.base[2], -x.DotProduct(pos),
		y.base[0], y.base[1], y.base[2], -y.DotProduct(pos),
		z.base[0], z.base[1], z.base[2], -z.DotProduct(pos),
		0.0, 0.0, 0.0, 1.0
	};
	pushIdentityNorm = true;
	//GzPushMatrix(xsp);
	//GzPushMatrix(xpi);
	pushIdentityNorm = false;
	GzPushMatrix(xiw);

	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
	/* HW 3.8
	/*- overwrite renderer camera structure with new camera definition
	*/

	this->m_camera.lookat[0] = camera.lookat[0];
	this->m_camera.lookat[1] = camera.lookat[1];
	this->m_camera.lookat[2] = camera.lookat[2];

	this->m_camera.position[0] = camera.position[0];
	this->m_camera.position[1] = camera.position[1];
	this->m_camera.position[2] = camera.position[2];

	this->m_camera.worldup[0] = camera.worldup[0];
	this->m_camera.worldup[1] = camera.worldup[1];
	this->m_camera.worldup[2] = camera.worldup[2];

	this->m_camera.FOV = camera.FOV;

	return GZ_SUCCESS;
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
	/* HW 3.9
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/

	if (matlevel < MATLEVELS - 1) {
		MatrixMult(Ximage[matlevel], matrix, Ximage[matlevel + 1]);
		if (pushIdentityNorm) MatrixMult(Xnorm[matlevel], Xnorm[0], Xnorm[matlevel + 1]);
		else MatrixMultNorm(Xnorm[matlevel], matrix, Xnorm[matlevel + 1]);
		++matlevel;
		return GZ_SUCCESS;
	}
	return GZ_FAILURE;
}

int GzRender::GzPopMatrix()
{
	/* HW 3.10
	- pop a matrix off the Ximage stack
	- check for stack underflow
	*/

	if (matlevel > 0)
		MatrixIdentity(Ximage[matlevel]);
	MatrixIdentity(Xnorm[matlevel]);
	--matlevel;
	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	/* HW1.4 write pixel values into the buffer */

	if (i < 0 || i >= this->xres)
	{
		return GZ_FAILURE;
	}
	else if (j < 0 || j >= this->yres)
	{
		return GZ_FAILURE;
	}

	int index = ARRAY(i, j);
	if (pixelbuffer[index].z < z || z < 0) return GZ_SUCCESS;

	pixelbuffer[index].red = r > 4095 ? 4095 : r;
	if (pixelbuffer[index].red < 0) pixelbuffer[index].red = 0;
	pixelbuffer[index].green = g > 4095 ? 4095 : g;
	if (pixelbuffer[index].green < 0) pixelbuffer[index].green = 0;
	pixelbuffer[index].blue = b > 4095 ? 4095 : b;
	if (pixelbuffer[index].blue < 0) pixelbuffer[index].blue = 0;

	//pixelbuffer[index].alpha;
	pixelbuffer[index].z = z;

	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity* r, GzIntensity* g, GzIntensity* b, GzIntensity* a, GzDepth* z)
{
	/* HW1.5 retrieve a pixel information from the pixel buffer */

	if (i < 0 || i >= this->xres)
	{
		return GZ_FAILURE;
	}
	else if (j < 0 || j >= this->yres)
	{
		return GZ_FAILURE;
	}

	GzPixel pixel = pixelbuffer[ARRAY(i, j)];
	*r = pixel.red;
	*g = pixel.green;
	*b = pixel.blue;
	*a = pixel.alpha;
	*z = pixel.z;

	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
	/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */

	if (doRayTrace) {
		RayTrace();
		doRayTrace = false;
	}

	fprintf(outfile, "P6 %d %d 255\r", this->xres, this->yres);
	for (int i = 0; i < this->xres * this->yres; ++i)
	{
		fputc((pixelbuffer[i].red >> 4) & 0xff, outfile);
		fputc((pixelbuffer[i].green >> 4) & 0xff, outfile);
		fputc((pixelbuffer[i].blue >> 4) & 0xff, outfile);
	}

	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
	/* HW1.7 write pixels to framebuffer:
		- put the pixels into the frame buffer
		- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red
		- NOT red, green, and blue !!!
	*/
	if (doRayTrace) {
		RayTrace();
		doRayTrace = false;
	}

	for (int i = 0; i < this->xres * this->yres; ++i)
	{
		framebuffer[i * 3 + 0] = (char)(pixelbuffer[i].blue >> 4) & 0xff;
		framebuffer[i * 3 + 1] = (char)(pixelbuffer[i].green >> 4) & 0xff;
		framebuffer[i * 3 + 2] = (char)(pixelbuffer[i].red >> 4) & 0xff;
	}

	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken* nameList, GzPointer* valueList)
{
	/* HW 2.1
	-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
	-- In later homeworks set shaders, interpolaters, texture maps, and lights
	*/

	for (int i = 0; i < numAttributes; ++i)
	{
		switch (nameList[i])
		{
		case GZ_RGB_COLOR:
		{
			GzPointer val = (GzPointer)valueList[i];
			flatcolor[0] = *(float*)val;
			flatcolor[1] = *((float*)val + 1);
			flatcolor[2] = *((float*)val + 2);
			break;
		}
		case GZ_INTERPOLATE:
		{
			GzPointer val = (GzPointer)valueList[i];
			this->interp_mode = *(int*)val;
			break;
		}
		case GZ_DIRECTIONAL_LIGHT:
		{
			if (this->numlights < MAX_LIGHTS - 1) {
				GzPointer val = (GzPointer)valueList[i];
				this->lights[this->numlights++] = *(GzLight*)val;
			}
			break;
		}
		case GZ_AMBIENT_LIGHT:
		{
			GzPointer val = (GzPointer)valueList[i];
			this->ambientlight = *(GzLight*)val;
			break;
		}
		case GZ_AMBIENT_COEFFICIENT:
		{
			GzPointer val = (GzPointer)valueList[i];
			this->Ka[0] = *(float*)val;
			this->Ka[1] = *((float*)val + 1);
			this->Ka[2] = *((float*)val + 2);
			break;
		}
		case GZ_DIFFUSE_COEFFICIENT:
		{
			GzPointer val = (GzPointer)valueList[i];
			this->Kd[0] = *(float*)val;
			this->Kd[1] = *((float*)val + 1);
			this->Kd[2] = *((float*)val + 2);
			break;
		}
		case GZ_SPECULAR_COEFFICIENT:
		{
			GzPointer val = (GzPointer)valueList[i];
			this->Ks[0] = *(float*)val;
			this->Ks[1] = *((float*)val + 1);
			this->Ks[2] = *((float*)val + 2);
			break;
		}
		case GZ_DISTRIBUTION_COEFFICIENT:
		{
			GzPointer val = (GzPointer)valueList[i];
			this->spec = *(float*)val;
			break;
		}
		case GZ_TEXTURE_MAP:
		{
			GzPointer val = (GzPointer)valueList[i];
			tex_fun = (GzTexture)val;
			break;
		}


		default:
		{
			printf("Error: GzPutAttribute doesn't recogonize code %i", nameList[i]);
			break;
		}
		}

	}

	return GZ_SUCCESS;
}



//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
Vector3 sumPos = Vector3(0,0,0);
int numPos = 0;


int GzRender::GzPutTriangle(int numParts, GzToken* nameList, GzPointer* valueList)
/* numParts - how many names and values */
{
	/* HW 2.2
	-- Pass in a triangle description with tokens and values corresponding to
		  GZ_NULL_TOKEN:		do nothing - no values
		  GZ_POSITION:		3 vert positions in model space
	-- Invoke the rastrizer/scanline framework
	-- Return error code
	*/

	Vector3 transformedCoords[] = {
		Vector3(0,0,0), Vector3(0,0,0), Vector3(0,0,0)
	};
	Vector3 transformedNorms[] = {
		Vector3(0,0,0), Vector3(0,0,0), Vector3(0,0,0)
	};
	Vector3 UVCoords[] = {
		Vector3(0,0,0), Vector3(0,0,0), Vector3(0,0,0)
	};

	for (int i = 0; i < numParts; ++i)
	{
		switch (nameList[i])
		{
		case(GZ_NULL_TOKEN):
		{
			break;
		}

		case(GZ_POSITION):
		{
			float* coords;
			coords = (float*)(GzPointer)valueList[i];
			for (int j = 0; j < 3; ++j) {
				Vector4 input = Vector4(coords[3 * j + 0], coords[3 * j + 1], coords[3 * j + 2]);
				Vector4 result = MatrixMult(Ximage[matlevel], input);
				Vector3 ret3 = result.ToVector3();

				transformedCoords[j] = ret3;
			}
			break;
		}

		case(GZ_NORMAL):
		{
			float* norms;
			norms = (float*)(GzPointer)valueList[i];

			for (int j = 0; j < 3; ++j) {
				Vector4 input = Vector4(norms[3 * j + 0], norms[3 * j + 1], norms[3 * j + 2], 1);
				Vector4 result = MatrixMult(Xnorm[matlevel], input);

				transformedNorms[j] = result.ToVector3().Normalize();
			}
			break;
		}

		case(GZ_TEXTURE_INDEX):
		{
			float* uvs;
			uvs = (float*)(GzPointer)valueList[i];

			for (int j = 0; j < 3; ++j) {
				UVCoords[j].base[0] = uvs[2 * j + 0];
				UVCoords[j].base[1] = uvs[2 * j + 1];
			}
			printf("");
		}

		default:
		{
			printf("Error: GzPutTriangle doesn't recogonize code %i", nameList[i]);
			break;
		}
		}
	}

	Triangle tri = Triangle();
	for (int i = 0; i < 3; ++i) {
		tri.SetPositions(i, transformedCoords[i]);
		tri.SetNorms(i, transformedNorms[i]);
		tri.SetUV(i, UVCoords[i]);

		sumPos = sumPos.Add(transformedCoords[i]);
		numPos++;
	}
	// TODO - Material specific parameters
	tri.SetKa(this->Ka[0], this->Ka[1], this->Ka[2]);
	tri.SetKd(this->Kd[0], this->Kd[1], this->Kd[2]);
	tri.SetKs(this->Ks[0], this->Ks[1], this->Ks[2]);
	tri.SetSpec(this->spec);

	triangleList[numTriangles++] = tri;

	return GZ_SUCCESS;
}

bool track = false;

struct AreaLight {
	Vector3 position;
	Vector3 color;
	float sideLength = 0;
	float samplePerSide = 0;
	AreaLight() {};
};
bool useAreaLight = false;
AreaLight aLight = AreaLight();

// Change based on what you are trying to do
void configureObject(GzRender* self) {
	for (int i = 0; i < self->numTriangles; ++i)
	{
		self->triangleList[i].useTexture = false;
		self->triangleList[i].SetKd(0.828125, 0.78515625, 0.7109375);
	}
	
	Vector3 avg = sumPos.Mult(1.0 / numPos);
	printf("");


	Vector3 a = Vector3(-1.03711438, -12.1594515, 32.2999420);
	Vector3 b = Vector3(-100.924759, 44.0511513, 143.932251);
	Vector3 c = Vector3(23.7079220, 99.5476532, 227.513214);
	Vector3 d = Vector3(123.595573, 43.3370552, 115.880905);

	Vector3 v1 = a.Subtract(b).Normalize();
	Vector3 v2 = c.Subtract(b).Normalize();
	Vector3 norm = v1.Crossproduct(v2).Normalize();

	float s = 2.5;
	a = a.Add(norm.Mult(s));
	b = b.Add(norm.Mult(s));
	c = c.Add(norm.Mult(s));
	d = d.Add(norm.Mult(s));
	Vector3 avgFloor = a.Add(b).Add(c).Add(d).Mult(1.0 / 4.0);


	Triangle floor1 = Triangle();
	floor1.SetPositions(0, a);
	floor1.SetPositions(1, b);
	floor1.SetPositions(2, c);
	floor1.SetKd(0.8, 0.8, 0.8);
	floor1.useTexture = false;

	Triangle floor2 = Triangle();
	floor2.SetPositions(0, a);
	floor2.SetPositions(1, d);
	floor2.SetPositions(2, c);
	floor2.SetKd(0.8, 0.8, 0.8);
	floor2.useTexture = false;

	for (int i = 0; i < 3; ++i) {
		floor1.SetNorms(i, norm);
		floor2.SetNorms(i, norm);
	}
	self->triangleList[(self->numTriangles)++] = floor1;
	self->triangleList[(self->numTriangles)++] = floor2;

	self->numlights = 0;

	useAreaLight = true;
	aLight.color = Vector3(0.7, 0.7, 0.7);
	aLight.position = Vector3(-80, -40, -250);
	aLight.sideLength = 25;
	aLight.samplePerSide = 5;
}




void GzRender::RayTrace()		
{
	configureObject(this);

	float fov = 60.0;
	float aspectRatio = float(xres) / yres;
	float left = -aspectRatio * tan(3.14159 * double(fov) / 2 / 180);
	float right = aspectRatio * tan(3.14159 * double(fov) / 2 / 180);
	float top = tan(3.14159 * double(fov) / 2 / 180);
	float bot = -tan(3.14159 * double(fov) / 2 / 180);

	//Vector3 origin = Vector3(this->m_camera.position[0], this->m_camera.position[1], this->m_camera.position[2]);
	Vector3* origin = new Vector3(0, 0, this->m_camera.position[2]);

	for (int x = 0; x < xres; ++x) {
		for (int y = 0; y < yres; ++y) {
			// TODO - Antialiasing by rays
			Vector3 color = Vector3(0, 0, 0);
			for (int sample = 0; sample < SAMPLES_PER_PIXEL; sample++) {
				auto offset = sample_square(); // a vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
				// Center of pixel + offset
				float cx = float(2 * x + 1 + offset.base[0]) / (2 * xres);
				float cy = float(2 * y + 1 + offset.base[1]) / (2 * yres);
				// Ray Direction
				Vector3 ray = Vector3(
					(1.0 - cx) * left + cx * right,
					cy * bot + (1.0 - cy) * top,
					1
				).Normalize();

				int* triIndex = new int();
				*triIndex = -1;
				Vector3* intersectPos = new Vector3(0, 0, 0);
				RayCast(origin, ray, triIndex, intersectPos);

				if (*triIndex != -1) {
					Vector3 color = ComputeShading(*triIndex, intersectPos, ray, 2);
					GzPut(x, y, ctoi(color.base[0]), ctoi(color.base[1]), ctoi(color.base[2]), 1, 1);
				}

			delete intersectPos;
			delete triIndex;
		}
	}
	delete origin;
}


void GzRender::RayCast(Vector3* origin, Vector3 direction, int* triangleIndex, Vector3* position, int ignoreIndex) {
	float dist = MAXINT;
	for (int i = 0; i < numTriangles; ++i)
	{
		if (i == ignoreIndex) continue;
		Triangle current = triangleList[i];
		Vector3 normal = (current.GetPosition(1).Subtract(current.GetPosition(0)).Crossproduct(current.GetPosition(2).Subtract(current.GetPosition(0)))).Normalize();
		Vector3 p = Vector3(0, 0, 0);

		float currentMag = intersection(origin, direction, current.GetPosition(0), normal, &p);
		Vector3 triangleCoords[] = {
			current.GetPosition(0),
			current.GetPosition(1),
			current.GetPosition(2)
		};
		bool inTriangle = positionInTriangle(triangleCoords, p);

		if (inTriangle && currentMag != -1 && currentMag < dist && currentMag > 0.001) {
			for (int j = 0; j < 3; ++j) {
				position->base[j] = p.base[j];
			}
			*triangleIndex = i;
			dist = currentMag;
		}
	}
}

Vector3 GzRender::ComputeShading(int triIndex, Vector3* intersection, Vector3 EyeRay) {
	Triangle tri = this->triangleList[triIndex];
	Vector3 coordData[] = { tri.GetPosition(0), tri.GetPosition(1) , tri.GetPosition(2) };
	Vector3 normData[] = { tri.GetNorms(0), tri.GetNorms(1) , tri.GetNorms(2) };
	
	Vector3 E = EyeRay.Mult(-1);
	Vector3 N = interpolateVector3(coordData, *intersection, normData).Normalize();

	GzColor baseColor = { 0,0,0 };
	if (tri.useTexture) {
		Vector3 uvData[] = { tri.GetUV(0), tri.GetUV(1) , tri.GetUV(2) };
		Vector3 intersectionUV = interpolateVector3(coordData, *intersection, uvData);
		this->tex_fun(intersectionUV.base[0], intersectionUV.base[1], baseColor);
	}
	else {
		Vector3 kd = tri.GetKd();
		baseColor[0] = kd.base[0];
		baseColor[1] = kd.base[1];
		baseColor[2] = kd.base[2];
	}
	/*else {
		Vector3 kd = tri.GetKd();
		baseColor[0] = kd.base[0];
		baseColor[1] = kd.base[1];
		baseColor[2] = kd.base[2];
	}*/


	Vector3 illumination(0, 0, 0);
	Vector3 ka = tri.GetKa();
	for (int i = 0; i < 3; ++i) {
		illumination.base[i] = ka.base[i] * this->ambientlight.color[i];
	}

	// TODO - Reflection
	// Recalculate reflective vector
	Vector3 reflectionColor = { 0,0,0 };
	if (depth > 1) {
		ray = ray.Mult(-1);
		float dot_RN = N.DotProduct(ray);
		if (dot_RN < 0) {
			N = N.Mult(-1);
			dot_RN = N.DotProduct(ray);
		}
		Vector3 reflection = (N.Mult(2 * dot_RN)).Subtract(ray).Normalize();
		int* intersectIndex = new int();
		*intersectIndex = -1;
		Vector3* intersect2 = new Vector3(0, 0, 0);
		RayCast(*intersection, reflection, intersectIndex, intersect2);
		if (*intersectIndex != -1) {
			//return Vector3(0, 0, 0);
			reflectionColor = ComputeShading(*intersectIndex, intersect2, reflection, depth - 1);
		}
		delete intersectIndex;
		delete intersect2;
	}

	for (int j = 0; j < this->numlights; ++j) {
		Vector3 L = Vector3(this->lights[j].direction[0], this->lights[j].direction[1], this->lights[j].direction[2]);
		L = L.Normalize();

		float dot_NL = N.DotProduct(L);
		float dot_NE = N.DotProduct(E);
		if (dot_NL >= 0 && dot_NE >= 0) {}
		else if (dot_NL < 0 && dot_NE < 0) {
			N = N.Mult(-1);
			dot_NL = N.DotProduct(L);
		}
		else {
			continue;
		}

		Vector3 R = (N.Mult(2 * dot_NL)).Subtract(L).Normalize();

		float dot_RE = R.DotProduct(E);
		if (dot_RE < 0) dot_RE = 0;
		else if (dot_RE > 1) dot_RE = 1;
		if (dot_NL < 0) dot_NL = 0;
		else if (dot_NL > 1) dot_NL = 1;

		// HARD SHADOW
		int* intersectIndex = new int();
		*intersectIndex = -1;
		Vector3* intersect2 = new Vector3(0, 0, 0);
		RayCast(intersection, L, intersectIndex, intersect2, triIndex);
		if (*intersectIndex == -1) {
			for (int i = 0; i < 3; ++i) {
				illumination.base[i] += baseColor[i] * this->lights[j].color[i] * pow(dot_RE, this->spec);
				illumination.base[i] += baseColor[i] * this->lights[j].color[i] * dot_NL;
			}
		}
		delete intersectIndex;
		delete intersect2;
	}

	// SOFT SHADOW
	{
		//**
		float lightIntensity = 0;
		for (int x = 0; x < aLight.samplePerSide; ++x) {
			for (int y = 0; y < aLight.samplePerSide; ++y) {
				for (int z = 0; z < aLight.samplePerSide; ++z) {
					float offsetX = aLight.sideLength * (x / aLight.samplePerSide) - (aLight.sideLength / 2);
					float offsetY = aLight.sideLength * (y / aLight.samplePerSide) - (aLight.sideLength / 2);
					float offsetZ = aLight.sideLength * (z / aLight.samplePerSide) - (aLight.sideLength / 2);
					Vector3 lightPosition = Vector3(aLight.position.base[0] + offsetX, aLight.position.base[1] + offsetY, aLight.position.base[2] + offsetZ);
					Vector3 L = lightPosition.Subtract(*intersection).Normalize();
					int* intersectIndex = new int();
					*intersectIndex = -1;
					Vector3* intersect2 = new Vector3(0, 0, 0);
					RayCast(intersection, L, intersectIndex, intersect2, triIndex);

					if (*intersectIndex == -1) {
						lightIntensity++;
					}
					delete intersectIndex;
					delete intersect2;
				}
			}
		}
		//*/
		Vector3 L = aLight.position.Subtract(*intersection).Normalize();
		bool calc = true;
		float dot_NL = N.DotProduct(L);
		float dot_NE = N.DotProduct(E);
		if (dot_NL >= 0 && dot_NE >= 0) {}
		else if (dot_NL < 0 && dot_NE < 0) {
			N = N.Mult(-1);
			dot_NL = N.DotProduct(L);
		}
		else calc = false;

		//int* intersectIndex = new int();
		//*intersectIndex = -1;
		//Vector3* intersect2 = new Vector3(0, 0, 0);
		//RayCast(intersection, L, intersectIndex, intersect2, triIndex);

		//if (calc && *intersectIndex == -1) {
		if (calc) {
			Vector3 R = (N.Mult(2 * dot_NL)).Subtract(L).Normalize();

			float dot_RE = R.DotProduct(E);
			if (dot_RE < 0) dot_RE = 0;
			else if (dot_RE > 1) dot_RE = 1;
			if (dot_NL < 0) dot_NL = 0;
			else if (dot_NL > 1) dot_NL = 1;

			float intensity = lightIntensity / (pow(aLight.samplePerSide, 3));
			//float intensity = 1.0;
			for (int j = 0; j < 3; ++j) {
				//illumination.base[j] += intensity * baseColor[j] * aLight.color.base[j] * pow(dot_RE, this->spec);
				illumination.base[j] += intensity * baseColor[j] * aLight.color.base[j] * dot_NL;
			}
		}

		//delete intersectIndex;
		//delete intersect2;
	}

	Vector3 color(0, 0, 0);
	for (int i = 0; i < 3; ++i) {
		color.base[i] = illumination.base[i] + 0.3 * reflectionColor.base[i];
	}
	return color;
}