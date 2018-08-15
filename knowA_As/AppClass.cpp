#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(7.0f, 20.0f, 40.0f), //Position
		vector3(7.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light

	//X and Z coordinate values for grid
	sPX = -10;
	sPZ = -10;

	//Smallest grid values (make them equal to starting grid values)
	sPXMin = sPX;
	sPZMin = sPZ;

	//seed for randomization
	srand((unsigned)time(0));

	//number of blocks to use for the ground+obstacles
	int blockNumber = columnLength * rowLength;

	//TODO: put all of this in a method
#pragma region Make Grid
	for (int i = 0; i < blockNumber; i++)
	{
		//give the cube an id based on its effective grid placement.
		m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(sPX) +"_"+ std::to_string(sPZ));

		//set the a_sUniqueID of this cube to a variable for use elsewhere if necessary.  Currently not used, but VERY useful for debug.
		cubeLabel = "Cube_" + std::to_string(sPX) + "_" + std::to_string(sPZ);

		//whether or not this tile will be an obstacle is determined by giving it a value with which to determine if it should be raised or lowered.
		//Obstacles CAN be traversed to prevent lockouts due to the random generation, but are slower to do so and the algorithm should avoid it as much as possible.
		float isOb = rand();

		//make one tenth of the blocks randomly raised.  These ones are obstacles
		//DON'T raise the block that steve or the zombie start on, or the block immediately afterwards
		if (isOb > (RAND_MAX *0.90) && ((i != 0) && (i != 1) && (i!= blockNumber-1) && (i !=blockNumber - 2) &&(i != blockNumber))) {
			matrix4 m4Position = glm::translate(vector3(sPX * 2.1, 0.0f, sPZ * 2.1));
			m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
		}
		//The rest of them are lowered beneath the principle actors, and are not obstacles (and thus act as the "ground")
		else
		{
			matrix4 m4Position = glm::translate(vector3(sPX * 2.1, -2.2f, sPZ * 2.1));
			m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
		}

		//don't want  sPX or sPZ to increase after the final block is placed, as that's used to place the Zombie.
		if (i < (blockNumber - 1))
		{
			//Increase relative x coordinate for next block by 1
			sPX = sPX + 1;
			
			//If the row is of the desired length, increase the number of rows.
			if (sPX > rowLength + sPXMin - 1) 
			{
				//reset x coordinate to starting one to start a new row
				sPX = sPXMin;

				//increase relative y coordinate for next block by 1 to start a new row
				sPZ = sPZ + 1;
			}
		}
		//increase the max value based on the blocks made so far.
		sPXMax = sPX;
		sPZMax = sPZ;
	}
#pragma endregion

#pragma region Moving Object And Goal Placement
	//make steve, our goal
	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	//make note of what cube steve should be on, so-as to give him his proper position
	steveCube = "Cube_" + std::to_string(sPXMin) + "_" + std::to_string(sPZMin);
	
	//DEBUG
	//std::cout << "Value check between label given to last cube ID, last cube ID, and ID input for Steve's starting cube.";
	//std::cout << cubeLabel << std::endl;
	//std::cout << steveCube << std::endl;

	//error handling
	if (m_pEntityMngr->GetRigidBody(steveCube) == nullptr)
	{
		stevePosition = vector3(0.0f, 0.0f, 0.0f);
		std::cout << "ERROR: required cube for Steve is out of bounds." << std::endl;
	}
	else
	{
		//set steve's position to his cube's
		stevePosition = m_pEntityMngr->GetRigidBody(steveCube)->GetCenterGlobal();
	}
	//set steve's height to the normal height, so that he is above his cube.
	stevePosition.y = 0.0f;
	//translate steve to his proper position.
	steveMatrix = glm::translate(stevePosition);
	m_pEntityMngr->SetModelMatrix(steveMatrix * glm::scale(vector3(2.0f)));
	m_pEntityMngr->UsePhysicsSolver();


	//make a zombie.  He's our starting point and we want him to get to the goal.
	m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "Zomb");

	//label the zombie's rightful cube as the final cube.  We will want to find the path from the zombie's cube to steve's cube.
	zombCube = "Cube_" + std::to_string(sPXMax) + "_" + std::to_string(sPZMax);
	
	//DEBUG
	//std::cout << "Value check between label given to last cube ID, last cube ID, and ID input for Zombie's starting cube.";
	//std::cout << cubeLabel << std::endl;
	//std::cout << zombCube << std::endl;

	//Error handling
	if (m_pEntityMngr->GetRigidBody(zombCube) == nullptr)
	{
		//zombie's cube is invalid somehow.  Give him a position so that stuff doesn't break.  We will give him a rightful cube later.
		zombPosition = vector3(0.0f, 0.0f, 0.0f);
		std::cout << "ERROR: required cube is out of bounds." << std::endl;
	}
	else
	{
		//Put zombie's position to that of his cube.
		zombPosition = m_pEntityMngr->GetRigidBody(zombCube)->GetCenterGlobal();
	}

	//Make sure the zombie's on TOP of the cube.
	zombPosition.y = 0.0f;
	//translate zombie to his proper position
	zombMatrix = glm::translate(zombPosition);
	m_pEntityMngr->SetModelMatrix(zombMatrix * glm::scale(vector3(2.0f)));
	m_pEntityMngr->UsePhysicsSolver();

	//Generate the starting path.  The cube the zombie's on is the start, and the cube Steve's on is the end.
	GeneratePath(zombCube, steveCube);
#pragma endregion
}

#pragma region AStar methods
void Application::GeneratePath(String a_StartPoint, String a_EndPoint)
{

	//This section essentially makes the "map" we'll be using from the cubes that have been placed: 
	//the location and "status" (y value) of every traversible point, the starting and current point for the entity traversing the path, and the path's destination
#pragma region cube group position setup
	//If there's already a collection of cubes, destroy it.
	/*
	//Worry about getting this to work once I need to reroute the search tree and thus actually delete and remake this
	if (cubeGroup)
	{
		for (int i = 0; i < columnLength - 1; i++)
		{
			for (int j = 0; j < rowLength - 1; j++)
			{
				SafeDelete(cubeGroup[i][j]);
			}
		}
	}
	*/
	
	//TODO: 
	/*
	Delete any lists, arrays, or vectors previously made.  DEFINITELY delete the current path.  
	Possibly set Zombie and steve's currentcube to their current cube instead of the one at [0][0] for if rerouting in the middle of movement
	*/

	//The only info I need for the pathfinding is the center of each cube.  The X and Z values are its location, (the y value is whether it's an obstacle or not)

	//where in the array the start and end points are
	//start as negative  to give error and set to zero if they're not set.
	int startI = -1;
	int startJ = -1;
	int endI = -1;
	int endJ = -1;

	//for debug
	int iIterator = (columnLength + sPZMin);
	//make a collection of all the cubes, the only information I need for them at the moment is their center vector3s.  
	for (int i = sPZMin; i < iIterator; i++)
	{
		//for debug
		int jIterator = (rowLength + sPXMin);
		for (int j = sPXMin; j < jIterator; j++)
		{
			//get the label needed for the correct cube
			String mooCubeLabel = "Cube_" + std::to_string(j) + "_" + std::to_string(i);
			std::cout << "Added cube " << mooCubeLabel << std::endl;
			
			//get the positional data from the cube with the given label
			vector3 cubeInfo = (m_pEntityMngr->GetRigidBody(mooCubeLabel)->GetCenterGlobal());

			//DEBUG output the x and Z coordinates of the cube
			std::cout << cubeInfo.x << " " << cubeInfo.z << std::endl;

			//i and j DO NOT start at 0 here.  make sure you subtract the appropriate number to make them hit 0.
			cubeGroup[i-sPZMin][j-sPXMin] = &cubeInfo;

			//DEBUG make sure this cube outputs a proper vector 3
			//vector3 testOut = *cubeGroup[i - sPZMin][j - sPXMin];

			//if Zombie has the cube being assigned to the traversal grid as their current cube, make note of where in the grid it is.
			if (cubeInfo == m_pEntityMngr->GetRigidBody(a_StartPoint)->GetCenterGlobal())
			{
				//Remember!  Starting values for I and J aren't zero, so subtract the amount added to them to make their starting value.
				startI = i - sPZMin;
				startJ = j - sPXMin;
				std::cout << "ZOMBIE SET" << std::endl;
			}
			//if Steve has the cube that was just assigned to the traversal grid as his current cube, note where in the array HE is
			else if (cubeInfo == m_pEntityMngr->GetRigidBody(a_EndPoint)->GetCenterGlobal())
				{
					endI = i - sPZMin;
					endJ = j - sPXMin;
					std::cout << "STEVE SET" << std::endl;
				}

			//DEBUG
			//std::cout << testOut.x << " " << testOut.z << std::endl;
		}
	}
	//const int modifiedCol = columnLength - 1;
	//const int modifiedRow = rowLength - 1;


	//error handling
	if (startI < 0 || startJ < 0)
	{
		std::cout << "ZOMBIE IS NOT SET" << std::endl;
		startI = 0; 
		startJ = 0;
		vector3 errorCube = *cubeGroup[startI][startJ];
		vector3 errorZom = m_pEntityMngr->GetRigidBody(a_StartPoint)->GetCenterGlobal();
		std::cout << a_StartPoint << " Zombie's X should be" << errorCube.x << " But it is actually " << errorZom.x << std::endl;
	}
	if(endI < 0 || endJ < 0)
	{
		std::cout << "STEVE IS NOT SET" << std::endl;
		endI = 0;
		endJ = 0;
		vector3 errorCube = *cubeGroup[endI][endJ];
		vector3 errorSteve = m_pEntityMngr->GetRigidBody(a_EndPoint)->GetCenterGlobal();
		std::cout << a_EndPoint<< " Steve's X should be" << errorCube.x << " But it is actually " << errorSteve.x << std::endl;
	}


	

#pragma endregion
	
	//Get vector3s for the current cubes.
	vector3 currentZombCube = *cubeGroup[startI][startJ];
	vector3 currentSteveCube = *cubeGroup[endI][endJ];
	//USAGE NOTE: for *cubeGroup[i][j] on the grid, I is Y (well, technically Z) and J is X.  
	//Should reverse this so that it's [x][y] when I get the time.  I tried a little bit ago, but it somewhat inexplicably seemed to cause errors in display despite going through this whole method with no problems


	//ASTAR GOES HERE
	/*
		Traverse through cubeGroup from the starting point.  The grid can be traversed by incrementing and decrementing the I and J values.
		make a set of routes towards the goal.  The route that currently has the greatest distance for the lowest cost is prioritized.
		moving into raised blocks has a higher cost
		Each route must keep track of points that have already been traversed.  
		If a cheaper way of getting to the current priority space is found, it should be prioritized.
		New points in each route are only valid if they're closer to destination than the current point in those routes (only judging with X and Z), and not outside the bounds of each part of *cubeGroup[][] (so not if they're less than 0, or greater than columnLength - 1 or rowLength - 1)
		

		Upon reaching the destination with the various routes, for the highest prioritized route, put all of the destination points into a list to go through in FollowPath
		FollowPath will be called to traverse the zombie through the list of points in the route 
	*/

}

void Application::FollowPath(void)
{
	/*
	Either lerp through the finalized path (slowing speed if current and/or destination square are raised) setting the zombie's current cube location (but not his current position) to each cube as it is passed
	OR
	Apply force towards next point on path.  On reaching it, set as current point in the path (and set the zombie's current cube to the one with that point), and keep doing this to get from point to point
	
	Light up a panel on each part of the route being traversed, to show the selected route
	*/
}

#pragma endregion

void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Move the guy along the path
	FollowPath();

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