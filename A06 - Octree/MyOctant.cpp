#include "MyOctant.h"
using namespace Simplex;

//instantiate the static variables
uint Simplex::MyOctant::octantCount = 0;
uint Simplex::MyOctant::idealEntCount;
uint Simplex::MyOctant::maxLevel;

void Simplex::MyOctant::Init(void)
{

	//Get an instance of the entity manager and mesh manager
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	
	//Set this octant's ID
	uID = octantCount;
	//this is a new octant, so we should increase the number of octants we have at the moment
	octantCount++;
}

void Simplex::MyOctant::Release(void)
{
	//Delete all the branches
	if (this == pRoot)
	{
		KillBranches();
	}

	//there's no branches anymore, reset octant count.
	octantCount = 0;
}

void Simplex::MyOctant::ConstructList(void)
{
	//gives IDs to the entities in a given leaf
	if (IsLeaf())
	{
		AssignIDtoEntity();
	}

	else
	{
		//go through all the children to assign their entities
		for (int i = 0; i < childCount; i++)
		{
			pChild[i]->ConstructList();
		}
	}

}


Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntCount)
{

	Init();

	//get a count of all the entities in the manager
	entityCount = m_pEntityMngr->GetEntityCount();

	//make this octant a root
	pRoot = this;

	//set the values given by the fields
	maxLevel = a_nMaxLevel;
	idealEntCount = a_nIdealEntCount;

	//get center value
	v3Center = m_pEntityMngr->GetRigidBody()->GetCenterGlobal();

	//currently, the only vector3 we have is the center, so set min and max to that
	min = v3Center;
	max = v3Center;

	//figure out how big to make the root octant and put the entities in a list
	for (int i = 0; i < entityCount; i++)
	{
		//put this entity in the list
		entityList.push_back(i);

		//set values to compare for the new minimum and maximum vectors
		//Calling "moomin" and "mooMax" because moo rhymes with "new" and to prevent accidentally using "new" keyword.  Also because moomin is a great show.
		vector3 mooMin = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal();
		vector3 mooMax = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal();

		//find the new minimum and maximum positions among all the items in the list
		//New min
		if (mooMin.x < min.x)
		{
			min.x = mooMin.x;
		}
		if (mooMin.y < min.y)
		{
			min.y = mooMin.y;
		}
		if (mooMin.z < min.z)
		{
			min.z = mooMin.z;
		}
		//New Max
		if (mooMax.x > max.x)
		{
			max.x = mooMax.x;
		}
		if (mooMax.y > max.y)
		{
			max.y = mooMax.y;
		}
		if (mooMax.z > max.z)
		{
			max.z = mooMax.z;
		}
	}
	//center in global space might not be the center of the objects
	//we want the center of the objects
	v3Center = (min + max) / 2.0f;

	//find object's size
	//needs to be an int, so use whichever axis has the greatest distance between max and min
	//if y diff is larger than f diff:
	if ((max.x - min.x) < (max.y - min.y))
	{
		//y is at least second largest, is it the largest?
		if ((max.y - min.y) < (max.z - min.z))
		{
			//z's difference is largest, use that
			fSize = max.z - min.z;
		}	
		else
		{
			//y's difference is largest, use that
			fSize = max.y - min.y;
		}
	}
		
	else
	{
		//x's difference is largest, use that
		fSize = max.x - min.x;
	}
	//make a tree with subdivision
	ConstructTree(a_nMaxLevel);

	//make the list of leafs that contain objects
	ConstructList();
}



Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	//this is going to be a new octant, init.
	Init();

	//set the center to the given value
	v3Center = a_v3Center;
	//set the size to the given value
	fSize = a_fSize;
	float radius = fSize / 2.0f;

	//since we know the center, and we know the size, we can find the min and max by using the radius, which is half the size
	min = v3Center - radius;
	max = v3Center + radius;
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
	//this is going to be a new octant, init.
	Init();

	//set info of this octant to the info of the given octant
	entityCount = other.entityCount;
	maxLevel = other.maxLevel;
	idealEntCount = other.idealEntCount;
	uLevel = other.uLevel;
	fSize = other.fSize;
	v3Center = other.v3Center;
	min = other.min;
	max = other.max;
	pParent = other.pParent;
	octantCount = other.octantCount;
	childCount = other.childCount;
	m_pEntityMngr = other.m_pEntityMngr;
	m_pMeshMngr = other.m_pMeshMngr;
	pRoot = other.pRoot;
	uID = other.uID;
	
	//set children to the other children
	for (int i = 0; i < childCount; i++)
	{
		pChild[i] = new MyOctant(*other.pChild[i]);
	}

	//set this entity list to the other one
	entityList = other.entityList;

	//if this is the root, set the child list.
	if (this ==pRoot)
	{
		childList = other.childList;
	}
}

MyOctant & Simplex::MyOctant::operator=(MyOctant const& other)
{
	//if the other one's the same, just return this.
	if (&other == this)
	{
		return *this;
	}

	//reinitialize the values
	Release();
	Init();

	entityCount = other.entityCount;
	maxLevel = other.maxLevel;
	idealEntCount = other.idealEntCount;
	uLevel = other.uLevel;
	fSize = other.fSize;
	v3Center = other.v3Center;
	min = other.min;
	max = other.max;
	pParent = other.pParent;
	octantCount = other.octantCount;
	childCount = other.childCount;
	pRoot = other.pRoot;


	//set children to the other children
	for (int i = 0; i < childCount; i++)
	{
		pChild[i] = new MyOctant(*other.pChild[i]);
	}

	//set this entity list to the other one
	entityList = other.entityList;

	if (this == pRoot)
	{
		childList = other.childList;
	}
	
	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

//TODO: make it so that you can test collisions in JUST the selected octant
void Simplex::MyOctant::TestCollisions()
{
	//If this isn't a leaf, look for the leaves and test the collision in them
	if (!IsLeaf())
	{
		//see if the children are leaves
		for (int i = 0; i < childCount; i++)
		{
			childList[i]->TestCollisions();
		}
	}
	//this is not a leaf, test the entities inside for collision
	else
	{
		for (uint i = 0; i < entityList.size(); i++)
		{
			m_pEntityMngr->GetEntity(entityList[i])->ClearCollisionList();
		}

		for (int i = 0; (i < entityList.size()); i++)
		{
			for (int j = i + 1; (j < entityList.size()); j++)
			{
				m_pEntityMngr->GetEntity(entityList[i])->IsColliding(m_pEntityMngr->GetEntity(entityList[j]));
			}
		}
	}
}

void Simplex::MyOctant::TestCollisions(uint testID)
{
	//If this isn't a leaf, look for the leaves and test the collision in them
	if (!IsLeaf())
	{
		//see if the children are leaves
		for (int i = 0; i < childCount; i++)
		{
			childList[i]->TestCollisions(testID);
		}
	}
	//this is not a leaf, test the entities inside for collision
	else
	{
		//if(/*octant is the test id*/)
		//{
			for (uint i = 0; i < entityList.size(); i++)
			{
				m_pEntityMngr->GetEntity(entityList[i])->ClearCollisionList();
			}

			for (int i = 0; (i < entityList.size()); i++)
			{
				for (int j = i + 1; (j < entityList.size()); j++)
				{
					m_pEntityMngr->GetEntity(entityList[i])->IsColliding(m_pEntityMngr->GetEntity(entityList[j]));
				}
			}
		}
	//}
}


void Simplex::MyOctant::Exchange(MyOctant & other)
{
	//exchange the values;
	std::swap(entityCount, other.entityCount);
	std::swap(maxLevel , other.maxLevel);
	std::swap(idealEntCount , other.idealEntCount);
	std::swap(uLevel , other.uLevel);
	std::swap(fSize , other.fSize);
	std::swap(v3Center , other.v3Center);
	std::swap(min , other.min);
	std::swap(max , other.max);
	std::swap(pParent , other.pParent);
	std::swap(octantCount , other.octantCount);
	std::swap(childCount , other.childCount);
	std::swap(m_pEntityMngr , other.m_pEntityMngr);
	std::swap(m_pMeshMngr , other.m_pMeshMngr);
	std::swap(pRoot , other.pRoot);
	std::swap(uID , other.uID);
	std::swap(entityList , other.entityList);
	std::swap(pChild , other.pChild);
	std::swap(childList, other.childList);

}


float Simplex::MyOctant::GetSize(void)
{
	return fSize;
}


vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return v3Center;
}


vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return min;
}


vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return max;
}

//Used to check to see if an entity is colliding with the octant it's in (meaning it's part of the octant)
bool Simplex::MyOctant::IsInOctant(uint a_uRBIndex)
{
	
	//get the rigid body of the index
	MyRigidBody* m_rigidBody = m_pEntityMngr->GetRigidBody(a_uRBIndex);

	//get the min and max values
	//once again, mooMin is a temporary minimum value (and a great television show and book series) and maxMin is a temporary max value
	vector3 mooMin = m_rigidBody->GetMinGlobal();
	vector3 mooMax = m_rigidBody->GetMaxGlobal();

	//collision check
	if (mooMin.x > max.x)
	{
		return false;
	}
	if (mooMin.y > max.y)
	{
		return false;
	}
	if (mooMin.z > max.z)
	{
		return false;
	}
	//New Max
	if (mooMax.x < min.x)
	{
		return false;
	}
	if (mooMax.y < min.y)
	{
		return false;
	}
	if (mooMax.z < min.z)
	{
		return false;
	}

	return true;
	

}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{

	//display octant if it shares the given index
	if (a_nIndex >= octantCount)
	{
		DisplayLeafs();
	}
	else
	{
		//display the child nodes instead
		//don't go out of bounds
		if (a_nIndex >= (childList.size()))
		{
			a_nIndex = 0;
		}
		childList[a_nIndex]->Display(a_v3Color);
	}
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	//renders a cube to show the leaves;
	m_pMeshMngr -> AddWireCubeToRenderList(glm::translate(IDENTITY_M4, v3Center) * glm::scale(IDENTITY_M4, max - min), a_v3Color);
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	//we only want to display leaves, so check to see if it's a leaf, and if so, display it.
	if (IsLeaf())
	{
		Display(a_v3Color);
	}

	//if this isn't a leaf, let's look until we find and display the ones that show up.
	else
	{
		for (int i = 0; i < childCount; i++)
		{
			pChild[i] -> DisplayLeafs(a_v3Color);
		}
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	//loop through this octant's children and call this (thus looping through their children) and then clear them out. 
	for (int i = 0; i < childCount; i++)
	{
		pChild[i]->ClearEntityList();
	}
	entityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	//create new children
	//we need half the size of the current one for the size of the new one (although an eighth of the volume, the "size" only measures height, width, or depth, and the length of that will only be halved
	//once again, the "moo" prefix means it's a temporary value used to find a new one
	float mooSize = fSize / 2.0f;

	//for the new center position, we need to find halfway into the center of the new child, which will be half of half of the distance from the current center to the new edge across all axes
	//although a bit slower, code might be more and easier to debug if I make six variables for close, far, top, bottom, left, right
	
	//assigning them all at the same time appears to give an error
	//float upper, furthest, right = mooSize / 2.0f; 
	float upper = mooSize / 2.0f;
	float furthest = mooSize / 2.0f;
	float right = mooSize / 2.0f;

	//float lower, closest, left = -(mooSize / 2.0f);
	float lower = -(mooSize / 2.0f);
	float closest = -(mooSize / 2.0f);
	float left = -(mooSize / 2.0f);
	
	//furthest upper right
	pChild[0] = new MyOctant( (v3Center + vector3(right, upper, furthest)), mooSize);
	//furthest upper left
	pChild[1] = new MyOctant((v3Center + vector3(left, upper, furthest)), mooSize);
	//closest upper left
	pChild[2] = new MyOctant((v3Center + vector3(left, upper, closest)), mooSize);
	//closest upper right
	pChild[3] = new MyOctant((v3Center + vector3(right, upper, closest)), mooSize);

	//furthest lower right
	pChild[4] = new MyOctant((v3Center + vector3(right, lower, furthest)), mooSize);
	//furthest lower left
	pChild[5] = new MyOctant((v3Center + vector3(left, lower, furthest)), mooSize);
	//closest lower left
	pChild[6] = new MyOctant((v3Center + vector3(left, lower, closest)), mooSize);
	//closest lower right
	pChild[7] = new MyOctant((v3Center + vector3(right, lower, closest)), mooSize);

	//this octant has children now, mark that down
	childCount = 8;

}

MyOctant* Simplex::MyOctant::GetChild(uint a_nChild)
{
	//if there's no children, there's nothing to return, so return a null pointer
	if (childCount == 0)
	{
		return nullptr;
	}

	//if there IS a child, return the index of the child
	return pChild[a_nChild];
}



MyOctant* Simplex::MyOctant::GetParent(void)
{
	return pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	
	//Testing to see if you can perform if checks this way for the heck of it.  It turns out you absolutely can.
	//an if statement checks to see if the comparison check is true, so simply returning the comparison check is simpler
	return (childCount == 0);
	
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	//Testing to see if you can perform if checks this way.  It turns out that this works just fine.
	return (entityCount >a_nEntities);
	
}

void Simplex::MyOctant::KillBranches(void)
{
	//can't delete branches from a leaf because it has none, exit the method
	if (IsLeaf())
	{
		//DEBUG
		if (childCount > 0) {
			std::cout << "IS a leaf, but has " << childCount << " children" << std::endl;
		}

		return;
	}
	//DEBUG
	else if (childCount < 0) {
		std::cout << "ISN'T a leaf, but has " << childCount << " children"<< std::endl;
	}

	//infanticide.
	for (int i = 0; i < childCount; i++)
	{
		pChild[i]->KillBranches();
		SafeDelete(pChild[i]);
	}

}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//if this has less entities than the ideal count, or its level is bigger than the max level, this will have children, so push this to the root and return
	if (!ContainsMoreThan(idealEntCount) || uLevel >= maxLevel)
	{
		pRoot->childList.push_back(this);
		return;
	}

	//if this octant has children, it's already been subdivided and there's no point subdividing it more
	if (childCount == 8)
	{
		return;
	}
	//DEBUG
	else if (childCount > 0)
	{
		std::cout << "IMPROPER CHILD COUNT: " << childCount << " children" << std::endl;
	}

	//Alright, this doesn't have children, but it should have some, so let's give it some
	Subdivide();

	//add the new children to the entity list
	for (int i = 0; i < childCount; i++){
		pChild[i]->pRoot = pRoot;
		pChild[i]->pParent = this;
		pChild[i]->uLevel = uLevel + 1;

		//If an entity is in a child's space, put it into the child's entityList.
		for (int j = 0; (j < entityCount); j++)
		{
			if (pChild[i]->IsInOctant(entityList[j]))
			{
				pChild[i]->entityList.push_back(entityList[j]);
			}
		}

		//entity count for the child will likely be different from the parent, make sure to reflect that.  Not doing this can lead to runtime exceptions, as I found the hard way
		pChild[i]->entityCount = pChild[i]->entityList.size();

		//make a tree inside the new child...... okay honestly that sounds kind of odd
		pChild[i]->ConstructTree(a_nMaxLevel);
	}
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	
	//give the entities an ID
	for (int i = 0; i < entityCount; i++)
	{
		m_pEntityMngr->AddDimension(entityList[i], uID);
	}

}


uint Simplex::MyOctant::GetOctantCount(void)
{
	return octantCount;
}

