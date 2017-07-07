#ifndef VE_VOXEL_SHADER_H
#define VE_VOXEL_SHADER_H


// --------------------- Includes ---------------------

#include "VEShader.h"


// --------------------- Classes ----------------------

// The default shader for voxels
class VEVoxelShader : public VEShader
{
    public :

        // ---------- Public Functions ----------

        // Construction
        VEVoxelShader();

        // Deconstruction
        ~VEVoxelShader();


		// -------- Overridden Functions --------

		// Sets up the lighting buffer for the pixel shader
		virtual bool PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight ) override;


    protected :     

		// -------- Protected Structures --------

		// Lighting buffer passed in to the voxel pixel shader
		struct LightingBuffer
		{
			DirectX::XMFLOAT4	myAmbientColour;
			DirectX::XMFLOAT4	myDiffuseColour;
			DirectX::XMFLOAT3	myDiffuseDirection;
			float				myPadding;
		};


        // --------- Required Functions ---------

        // Creates the data layout used to send information to the vertex shader
        virtual bool CreateDataLayout( ID3D10Blob* aVertexShader ) override;
};


#endif // !VE_VOXEL_SHADER_H