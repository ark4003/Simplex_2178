#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 5.0f, 25.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	m_pEntityMngr->UsePhysicsSolver();

	int sPX = -10;
	//int sPY = 0;
	int sPZ = -10;
	int weird = 0;

	srand((unsigned)time(0));

	
	for (int i = 0; i < 500; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(i));
		vector3 v3Position = vector3(glm::sphericalRand(12.0f));
		v3Position.y = 0.0f;
		//matrix4 m4Position = glm::translate(v3Position);

		//whether or not this tile will be an obstacle
		float isOb = rand();
		std::cout << isOb << std::endl;

		//make one tenth of the blocks randomly raised.  These ones are obstacles
		if (isOb > (RAND_MAX *0.90) && (i != 0)) {
			matrix4 m4Position = glm::translate(vector3(sPX * 2.1, 0.0f, sPZ * 2.1));
			m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
		}
		//The rest of them are lowered beneath the principle actors.
		else
		{
			matrix4 m4Position = glm::translate(vector3(sPX * 2.1, -2.2f, sPZ * 2.1));
			m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
		}
		//m_pEntityMngr->UsePhysicsSolver();
		//m_pEntityMngr->SetMass(2);

		//m_pEntityMngr->SetMass(i+1);
		sPX++;
		//sPY++;
		if (sPX > 15) {
			sPX = -10;
			sPZ++;
		}
		weird++;
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();

	//release GUI
	ShutdownGUI();
}