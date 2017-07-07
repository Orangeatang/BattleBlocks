

// ------------------ Includes ------------------

#include "Stdafx.h"
#include "VEVoxel.h"


// --------------- Class Functions --------------

// Construction
VEVoxel::VEVoxel( VoxelType aVoxelType ) :
    myType( aVoxelType ),
	myVisibility( VV_None )
{	
}


VEVoxel::VEVoxel() :
	myType( VT_Grass ),
	myVisibility( VV_None )
{
}