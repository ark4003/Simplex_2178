//Derived (with permission) from sample code given by Alberto Bobadilla, labigm@rit.edu
//date:2018 (alberto's 2017)

#ifndef __MYOCTANT_H_
#define __MYOCTANT_H_

#include "MyEntityManager.h"
namespace Simplex
{
//class
class MyOctant
{
	//maybe making the entity count into a variable will help fix some problems
	uint entityCount = 0;

	//stores number of octants instantiated
	static uint octantCount;

	//maximum level an octant can go
	static uint maxLevel;

	//idea number of entities
	static uint idealEntCount;

	//stores the current ID for the Octant
	uint uID = 0;

	//stores current level of the octant
	uint uLevel = 0;

	//current number of children for the octant (should only be either 2 or 8)
	uint childCount = 0;

	//size of the octant
	float fSize = 0.0f;

	//Mesh Manager singleton
	MeshManager* m_pMeshMngr = nullptr;
	//entity manager singleton
	MyEntityManager* m_pEntityMngr = nullptr;

	//center of the octant
	vector3 v3Center = vector3(0.0f);
	//minimum vector of Octant
	vector3 min = vector3(0.0f);
	//Max vector of octant
	vector3 max = vector3(0.0f);

	//stores the parent of the current octant
	//NOTE:specifying what is and isn't a pointer is probably the most useful specification here
	MyOctant* pParent = nullptr;
	//stores children of current octant.  We know that each octant will only have 0 or 8 children
	MyOctant* pChild[8];

	//list of entities in this octant (index in Entity Manager)
	std::vector<uint> entityList;

	//Root octant
	MyOctant* pRoot = nullptr;
	//list of nodes containing objects (only applied to root)
	std::vector<MyOctant*> childList;

public:
	//to copy to speed up writing these
	/*
	USAGE:
	ARGUMENTS
	OUTPUT:
	*/

	/*
	USAGE:Find collisions
	ARGUMENTS
	OUTPUT:void
	*/
	void TestCollisions();

	/*
	USAGE:Find collisions
	ARGUMENTS
	-testID ~ octant to test collisions in
	OUTPUT:void
	*/
	void TestCollisions(uint testID);

	/*
	USAGE:Constructor, creates an octant containing all MagnaEntities instances the Mesh Manager currently contains
	ARGUMENTS
	- uint MaxLevel = 2 ~ sets max level of subdivision
	- uint IdealEntCount = 5 ~ sets ideal level of objects per octant
	OUTPUT: class object
	*/
	//a_specifies it's an argument
	MyOctant(uint a_maxLevel = 2, uint a_idealEntCount = 5);

	/*
	USAGE: constructor
	ARGUMENTS
	- vector3 v3Center ~ Center of the octant in global space
	-float fSize ~size of each side of the octant volume
	OUTPUT: class object
	*/
	MyOctant(vector3 a_v3Center, float a_fSize);

	/*
	USAGE: Copy Constructor
	ARGUMENTS: the class object being copied
	OUTPUT:Class object instance
	*/
	MyOctant(MyOctant const & other);

	/*
	USAGE: Copy Assignment Operator
	ARGUMENTS: the class object being copied
	OUTPUT:
	*/
	MyOctant& operator=(MyOctant const& other);

	/*
	USAGE:Destructor
	ARGUMENTS
	OUTPUT:
	*/
	~MyOctant(void);

	/*
	USAGE:Change object contents for another object's
	ARGUMENTS
	- MyOctant& other ~ object to exchange content from
	OUTPUT:
	*/
	void Exchange(MyOctant& other);



	/*
	USAGE: gets octant's size
	ARGUMENTS
	OUTPUT: size of octant
	*/
	float GetSize(void);

	/*
	USAGE: gets center of octant in global space
	ARGUMENTS
	OUTPUT: center octant in global space
	*/
	vector3 GetCenterGlobal(void);

	/*
	USAGE: min corner of octant in global space
	ARGUMENTS
	OUTPUT: min in global space
	*/
	vector3 GetMinGlobal(void);

	/*
	USAGE: max corner of octant in global space
	ARGUMENTS
	OUTPUT: max in global space
	*/
	vector3 GetMaxGlobal(void);


	/*
	USAGE: sees if the octant contains the Entity specified by Index
	from Bounding Object Manager
	ARGUMENTS
	- int a_uRBIndex ~ index of entity in Entity Manager
	OUTPUT: Collision check
	*/
	bool IsInOctant(uint a_uRBIndex);

	/*
	USAGE: Displays MyOctant volume specified by index including objects underneath
	ARGUMENTS
	- uint nIndex, ~ MyOctant to be displayed
	- vector3 v3Color = REYELLOW ~ color of volume to display
	OUTPUT:
	*/
	void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);

	/*
	USAGE: Displays MyOctant volume in the color specified
	ARGUMENTS
	- vector3 v3Color = REYELLOW ~ color of volume to display
	OUTPUT:
	*/
	void Display(vector3 a_v3Color = C_YELLOW);

	/*
	USAGE:Displays occupied leafs in the octree
	ARGUMENTS
	-vector3 a_v3Color = REYELLOW ~color of volume to display
	OUTPUT:
	*/
	void DisplayLeafs(vector3 a_v3Color = C_YELLOW);

	/*
	USAGE:  Clears Entity List for each node
	ARGUMENTS
	OUTPUT:
	*/
	void ClearEntityList(void);

	/*
	USAGE: allocates 8 smaller octants in each of the child pointers
	ARGUMENTS
	OUTPUT:
	*/

	void Subdivide(void);

	/*
	USAGE:returns child specified in iindex
	ARGUMENTS 
	-uint a_nChild ~ index of child (0-7)
	OUTPUT:MyOctant object(child in index
	*/
	MyOctant* GetChild(uint a_nChild);

	/*
	USAGE: Returns parent of the octant
	ARGUMENTS
	OUTPUT:MyOctant object (parent)
	*/
	MyOctant* GetParent(void);

	/*
	USAGE:Checks the Myoctant to see if it doesn't contain any Children (it's a leaf)
	ARGUMENTS
	OUTPUT: It contains no children
	*/
	bool IsLeaf(void);

	/*
	USAGE:Asks MyOctant if it contains more than a specified number of Bounding Objects
	ARGUMENTS
	- uint a_nEntities ~number of Entities to Query
	OUTPUT: Contains at least this many Entities
	*/
	bool ContainsMoreThan(uint a_nEntities);

	/*
	USAGE:Deletes all children and the children of their children
	ARGUMENTS
	OUTPUT:
	*/
	void KillBranches(void);

	/*
	USAGE: Create tree using subdivisions, max number of objects and leels
	ARGUMENTS
	- uint a__nMaxLevel = 3 ~ Sets maximum level of the tree while constructing it
	OUTPUT:
	*/
	void ConstructTree(uint a_nMaxLevel = 3);

	/*
	USAGE: traverse tree up to leaves and set the objects in them to the index
	ARGUMENTS
	OUTPUT:
	*/
	void AssignIDtoEntity(void);

	/*
	USAGE: gets total number of octants in the world
	ARGUMENTS
	OUTPUT:
	*/
	uint GetOctantCount(void);

private:
	/*
	USAGE:Deallocate member fields
	ARGUMENTS
	OUTPUT:
	*/
	void Release(void);

	/*
	USAGE:Allocate Member Fields
	ARGUMENTS
	OUTPUT:
	*/
	void Init(void);

	/*
	USAGE:create list of all leafs that contain objects
	ARGUMENTS
	OUTPUT:
	*/
	void ConstructList(void);


};


}

#endif