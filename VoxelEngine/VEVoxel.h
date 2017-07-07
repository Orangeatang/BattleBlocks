#ifndef VE_VOXEL_H
#define VE_VOXEL_H


// ------------------ Includes ------------------

#include "VETypes.h"


// ------------------- Enums --------------------

// A bit mask indicating which sides of a voxel are visible
enum VoxelVisibility
{
	VV_None		= 0x00,
	VV_Front	= 0x01,
	VV_Back		= 0x02,
	VV_Left		= 0x04,
	VV_Right	= 0x08,
	VV_Top		= 0x10,
	VV_Bottom	= 0x20,

	VV_All		= 0x3F
};


// ------------------ Classes -------------------

// The base voxel (volumetric pixel) structure. This class needs to be kept as small as possible
// as each in-game voxel has an instance... which could build up to be quite a few!
class VEVoxel
{
    public :

        // -------- Public Functions --------

        // Construction
        VEVoxel( VoxelType aVoxelType );

		// Default construction
		VEVoxel();


        // ----------- Accessors ------------

        void        SetEnabled( bool anIsEnabled )		{ myEnabled = anIsEnabled; }
        bool        GetEnabled()						{ return myEnabled; }

		void		SetVisibility( DWORD aVisibility )	{ myVisibility = aVisibility; }
		DWORD		GetVisibility()						{ return myVisibility; }

		VoxelType	GetType()							{ return myType; }    
		void		SetType( VoxelType aType )			{ myType = aType; }

    private :

        // ------- Private Variables -------

        VoxelType	myType;
        bool        myEnabled;

		DWORD		myVisibility;
};


#endif // !VE_VOXEL_H