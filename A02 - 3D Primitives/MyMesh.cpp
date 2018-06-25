#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	float bottomHeight = -a_fHeight / 2;
	std::vector <vector3> conePoints;
	vector3 top(0.0f, a_fHeight / 2, 0.0f);
	vector3 bottom(0.0f, bottomHeight, 0.0f);

	bool startPoint = true;
	float theta = 0;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta = ((i* (2 * PI)) / a_nSubdivisions);
		conePoints.push_back(vector3((cos(theta)*a_fRadius), bottomHeight, (sin(theta)*a_fRadius)));

		//after the first point, make triangles with preexisting points as points are set
		if (startPoint == true) {
			startPoint = false;
		}

		else if (startPoint == false)
		{
			
			AddTri(conePoints[i], conePoints[i - 1], top);
			AddTri(conePoints[i - 1], conePoints[i], bottom);
		}
		else 
		{
			std::cout << "CONE ERROR";
		}
	}

	//reconnect to the start
	AddTri(conePoints[0], conePoints[a_nSubdivisions - 1], top);
	AddTri(conePoints[a_nSubdivisions - 1], conePoints[0], bottom);


	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	float topHeight = a_fHeight / 2;
	float bottomHeight = -a_fHeight / 2;
	std::vector <vector3> cylTopPoints;
	std::vector <vector3> cylBottomPoints;

	//top center
	vector3 topCent(0.0f, topHeight, 0.0f);
	vector3 bottomCent(0.0f, bottomHeight, 0.0f);

	bool startPoint = true;
	float theta = 0;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta = ((i* (2 * PI)) / a_nSubdivisions);
		cylTopPoints.push_back(vector3((cos(theta)*a_fRadius), topHeight, (sin(theta)*a_fRadius)));
		cylBottomPoints.push_back(vector3((cos(theta)*a_fRadius), bottomHeight, (sin(theta)*a_fRadius)));

		//after the first point, make triangles with existing points.
		if (startPoint == true)
		{
			startPoint = false;
		}
		else if (startPoint == false)
		{
			//make the top and bottom triangles
			AddTri(cylTopPoints[i], cylTopPoints[i - 1], topCent);
			AddTri(cylBottomPoints[i - 1], cylBottomPoints[i], bottomCent);

			//make the sides
			AddQuad(cylBottomPoints[i - 1], cylTopPoints[i - 1], cylBottomPoints[i], cylTopPoints[i]);

		}
		else
		{
			std::cout << "CYLINDER ERROR";
		}
	}

	//Fill in the gaps
	AddTri(cylTopPoints[0], cylTopPoints[a_nSubdivisions - 1], topCent);
	AddTri(cylBottomPoints[a_nSubdivisions - 1], cylBottomPoints[0], bottomCent);
	AddQuad(cylBottomPoints[a_nSubdivisions - 1], cylTopPoints[a_nSubdivisions - 1], cylBottomPoints[0], cylTopPoints[0]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code


	float topHeight = a_fHeight / 2;
	float botHeight = -a_fHeight / 2;

	//put points in 4 rings
	std::vector <vector3> tubeInnerTopPoints;
	std::vector <vector3> tubeOuterTopPoints;
	std::vector <vector3> tubeInnerBotPoints;
	std::vector <vector3> tubeOuterBotPoints;

	bool startPoint = true;
	float theta = 0;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta = ((i* (2 * PI)) / a_nSubdivisions);
		//make rings for the inner points
		tubeInnerTopPoints.push_back(vector3((cos(theta)*a_fInnerRadius), topHeight, (sin(theta)*a_fInnerRadius)));
		tubeInnerBotPoints.push_back(vector3((cos(theta)*a_fInnerRadius), botHeight, (sin(theta)*a_fInnerRadius)));

		//make rings for the outer points
		tubeOuterTopPoints.push_back(vector3((cos(theta)*a_fOuterRadius), topHeight, (sin(theta)*a_fOuterRadius)));
		tubeOuterBotPoints.push_back(vector3((cos(theta)*a_fOuterRadius), botHeight, (sin(theta)*a_fOuterRadius)));

		//after first points, make triangles with previous points
		if (startPoint == true)
		{
			startPoint = false;
		}
		else if (startPoint == false)
		{
			//draw top ring
			AddQuad(tubeOuterTopPoints[i], tubeOuterTopPoints[i - 1], tubeInnerTopPoints[i], tubeInnerTopPoints[i - 1]);

			//draw bottom ring
			AddQuad(tubeOuterBotPoints[i-1], tubeOuterBotPoints[i], tubeInnerBotPoints[i-1], tubeInnerBotPoints[i]);

			//draw outside
			AddQuad(tubeOuterBotPoints[i], tubeOuterBotPoints[i - 1], tubeOuterTopPoints[i], tubeOuterTopPoints[i - 1]);

			//draw inside
			AddQuad(tubeInnerBotPoints[i - 1], tubeInnerBotPoints[i], tubeInnerTopPoints[i - 1], tubeInnerTopPoints[i]);

		}
		else
		{
			std::cout << "TUBE ERROR";
		}

		
	}
	//fill in the blanks.
	//Top ring
	AddQuad(tubeInnerTopPoints[a_nSubdivisions - 1], tubeInnerTopPoints[0], tubeOuterTopPoints[a_nSubdivisions - 1], tubeOuterTopPoints[0]);

	//bottom ring
	AddQuad(tubeInnerBotPoints[0], tubeInnerBotPoints[a_nSubdivisions - 1], tubeOuterBotPoints[0], tubeOuterBotPoints[a_nSubdivisions - 1]);

	//outer face
	AddQuad(tubeOuterBotPoints[0], tubeOuterBotPoints[a_nSubdivisions - 1], tubeOuterTopPoints[0], tubeOuterTopPoints[a_nSubdivisions - 1]);

	//inner face
	AddQuad(tubeInnerBotPoints[a_nSubdivisions - 1], tubeInnerBotPoints[0], tubeInnerTopPoints[a_nSubdivisions - 1], tubeInnerTopPoints[0]);



	// -------------------------------
	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code

	//Hmmmm, let's try making a ring of circles?  That feels like it should be pretty easy, but how should I get them all in the same direction.....
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	//making sphere using longitude (sphereLon) and latitude to make the main body of the sphere in slices vertical orange slices.  Then, makes the top and bottom with triangles.

	std::vector <std::vector<vector3>> sphereLon(a_nSubdivisions * a_nSubdivisions);

	vector3 topCent(0.0f, a_fRadius, 0.0f);
	vector3 botCent(0.0f, -a_fRadius, 0.0f);

	bool startPoint = true;
	float theta = 0.0f;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		theta = ((i* (2 * PI)) / a_nSubdivisions);
		sphereLon.push_back(std::vector<vector3>());

		for (int j = 0; j < a_nSubdivisions; j++)
		{
			//will probably have to edit this one
			float heightTheta = ((j* (2 * PI)) / a_nSubdivisions);
			//float currentHeight = ((-(a_fRadius)) *(sin(heightTheta)) + a_fRadius);
			//float currentHeight = (sin(heightTheta)*a_fRadius);
			
			//Need to make sure the value of this loops around after passing the middle of the circle, thus making it curved.  Need to fine tune this to make it more even
			float currentHeight = ((a_nSubdivisions + 1.0f) * ((a_fRadius * 2.0f) / (a_nSubdivisions + 2.0f)) - (j * ((a_fRadius * 2.0f)) / (a_nSubdivisions + 2.0f)));
			
			

			float currentRadius = (glm::sqrt((currentHeight * ((a_fRadius * 2) - currentHeight))));

			sphereLon[i].push_back(vector3((cos(theta)*currentRadius), (currentHeight - a_fRadius), (sin(theta) * currentRadius)));
		}

		//start drawing stuff after the first point
		if (startPoint == true)
		{
			startPoint = false;
		}
		else if (startPoint == false)
		{
			//draw squares on sides
			bool innerStartPoint = true;
			for (int j = 0; j < a_nSubdivisions; j++)
			{
				if (innerStartPoint == true)
				{
					innerStartPoint = false;
				}
				else if (innerStartPoint == false && startPoint == false)
				{
					//i is basically x value, j is y value
					AddQuad(sphereLon[i - 1][j - 1], sphereLon[i][j - 1], sphereLon[i - 1][j], sphereLon[i][j]);
				}
				else
				{
					std::cout << "INNER SPHERE ERROR";
				}

				//make some triangles on the top
				AddTri(sphereLon[i][0], sphereLon[i - 1][0], topCent);
				//Make triangles on the bottom
				AddTri(sphereLon[i - 1][a_nSubdivisions - 1], sphereLon[i][a_nSubdivisions - 1], botCent);
			}
		}
	}

	//CONNECT STUFF HERE once sphere's stable.
	//Top Triangle
	AddTri(sphereLon[0][0], sphereLon[a_nSubdivisions - 1][0], topCent);
	//Bottom Triangle
	AddTri(sphereLon[a_nSubdivisions - 1][a_nSubdivisions - 1], sphereLon[0][a_nSubdivisions - 1], botCent);

	//faces for the remaining vertical "slice"
	for (int j = 0; j < a_nSubdivisions; j++) {
		if (j != 0) {
			AddQuad(sphereLon[0][j], sphereLon[a_nSubdivisions - 1][j], sphereLon[0][j - 1], sphereLon[a_nSubdivisions - 1][j - 1]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}