#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(1.0f, C_BLACK);
	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);


	//translate all the squares to their proper places
	//Rows start at the top left.  Separated and labled so that I can easily find what's off about any misplaced squares.
	//ROW 1
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-4.0f, 8.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(2.0f, 8.0f, 3.0f))); 

	//ROW 2
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-3.0f, 7.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(1.0f, 7.0f, 3.0f))); 

	//ROW 3
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-4.0f, 6.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-3.0f, 6.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-2.0f, 6.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-1.0f, 6.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(0.0f, 6.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(1.0f, 6.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(2.0f, 6.0f, 3.0f)));

	//ROW 4
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-5.0f, 5.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-4.0f, 5.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-2.0f, 5.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-1.0f, 5.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-0.0f, 5.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(2.0f, 5.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(3.0f, 5.0f, 3.0f)));

	//ROW 5
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-6.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-5.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-4.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-3.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-2.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-1.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(0.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(1.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(2.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(3.0f, 4.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(4.0f, 4.0f, 3.0f)));

	//ROW 6
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-6.0f, 3.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-4.0f, 3.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-3.0f, 3.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-2.0f, 3.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-1.0f, 3.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(0.0f, 3.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(1.0f, 3.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(2.0f, 3.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(4.0f, 3.0f, 3.0f)));

	//ROW 7
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-6.0f, 2.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-4.0f, 2.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(2.0f, 2.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(4.0f, 2.0f, 3.0f)));

	//ROW 8
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-3.0f, 1.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(-2.0f, 1.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(0.0f, 1.0f, 3.0f)));
	translationList.push_back(glm::translate(IDENTITY_M4, vector3(1.0f, 1.0f, 3.0f)));
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(0.6f,0.6f,0.6f));
	static float value = 0.0f;

	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 0.0f, 0.0f));
	value += 0.02f;

	//matrix4 m4Model = m4Translate * m4Scale;
	//matrix4 m4Model = m4Scale * m4Translate;

	//for each "pixel", translate to its proper position, then translate it to the right, then draw.
	for (int i = 0; i < translationList.size(); i++) {
		matrix4 m4Model = (m4Scale * translationList[i] * m4Translate);

		m_pMesh->Render(m4Projection, m4View, m4Model);
	}


	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}