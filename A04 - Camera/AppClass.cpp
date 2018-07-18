#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Andrew Knowland - ark4003@rit.edu";

	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 10.0f, ZERO_V3, AXIS_Y);

	//init the camera
	m_pCamera = new MyCamera();
	m_pCamera->SetPositionTargetAndUp(
			cameraPosition, //Where my eyes are
			cameraLook, //where what I'm looking at is
			cameraUp);					//what is up

	//Get the singleton
	m_pMyMeshMngr = MyMeshManager::GetInstance();
	m_pMyMeshMngr->SetCamera(m_pCamera);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//set the current forward vector for anything that needs it before movement changes
	frontVector = cameraLook - cameraPosition;

	//TO DO: move to method(s) in camera class.




	//change where the camera's looking
	//this will be cameralook's position relative to cameraposition.  right now it's in cameralook's relative position, but to the origin instead.
	vector3 newPosition = frontVector;
	//rotate the new position around the origin
	newPosition = newPosition * glm::angleAxis(viewChange.y, vector3(0.0f, 1.0f, 0.0f));
	newPosition = newPosition * glm::angleAxis(viewChange.x, vector3(1.0f, 0.0f, 0.0f));
	//move the new cameralook over to the correct relative position from the camera's current position.
	cameraLook = cameraPosition + newPosition;


	//DEPRECATED
	/*
	//change how player will move based on how much the camera's been rotated
	positionChange = positionChange * glm::angleAxis(rotationTracker.y, vector3(0.0f, 1.0f, 0.0f));
	//make sure camera will still be moving in the same direction as player in terms of keypresses
	lookChange = lookChange * glm::angleAxis(rotationTracker.y, vector3(0.0f, 1.0f, 0.0f));
	*/

	//reset positionChange to 0;
	positionChange = vector3(0.0f, 0.0f, 0.0f);
	lookChange = vector3(0.0f, 0.0f, 0.0f);

	//currently working on
	//m_pCamera->changePosition(frontVector, frontMove, rearMove, leftMove, rightMove, cameraPosition);


	//find current forward vectors based on difference between position and target
	frontVector = cameraLook - cameraPosition;
	//we don't want to be able to move in the y axis.
	frontVector.y = 0.0f;
	//normalize vector and decrease its speed so we're not flying all over the place.
	frontVector = glm::normalize(frontVector) / 2;
	//find other vectors based on the front vector.
	rightVector = frontVector *glm::angleAxis(glm::radians(90.0f), vector3(0.0f, 1.0f, 0.0f));
	rightVector = glm::normalize(rightVector) / 2;
	rearVector = (frontVector) * -1.0f;
	leftVector = (rightVector) * -1.0f;

	//add to positionChange based on which keys are being held down.
	if(frontMove == true)
	{
		positionChange = positionChange + frontVector;
		lookChange = lookChange + frontVector;
	}
	if (rearMove == true)
	{
		positionChange = positionChange + rearVector;
		lookChange = lookChange + rearVector;
	}
	if (leftMove == true)
	{
		positionChange = positionChange + leftVector;
		lookChange = lookChange + leftVector;
	}
	if (rightMove == true)
	{
		positionChange = positionChange + rightVector;
		lookChange = lookChange + rightVector;
	}


	//moves camera to proper position
	cameraPosition = cameraPosition + (vector3(positionChange.x, 0.0f, positionChange.z));
	cameraLook = cameraLook + lookChange;


	//REALLY shouldn't be an issue but just in case
	if (cameraLook == cameraUp)
	{
		//should never reach here in the first place but if it does, this will prevent a crash.
		cameraLook.y = cameraLook.y - 0.000001f;
		//adjusts camera's current position, target, and up
		m_pCamera->SetPositionTargetAndUp(
			cameraPosition,
			cameraLook,
			cameraUp);
		//prevents the slight change from theoretically stacking.
		cameraLook.y = cameraLook.y + 0.000001f;
	}
	else
	{
		//adjusts camera's current position, target, and up
		m_pCamera->SetPositionTargetAndUp(
			cameraPosition,
			cameraLook,
			cameraUp);
	}
	//Add objects to the Manager
	for (int j = -50; j < 50; j += 2)
	{
		for (int i = -50; i < 50; i += 2)
		{
			m_pMyMeshMngr->AddConeToRenderList(glm::translate(vector3(i, 0.0f, j)));
		}
	}
}
void Application::Display(void)
{
	//Clear the screen
	ClearScreen();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//Render the list of MyMeshManager
	m_pMyMeshMngr->Render();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the MyMeshManager list
	m_pMyMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the singleton
	MyMeshManager::ReleaseInstance();

	//release the camera
	SafeDelete(m_pCamera);

	//release GUI
	ShutdownGUI();
}
