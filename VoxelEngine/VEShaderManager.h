#ifndef VE_SHADER_MANAGER_H
#define VE_SHADER_MANAGER_H


// ----------- Forward Declarations ---------

class VEShader;


// ------------------ Enums -----------------

// An enumeration that defines the available shaders. Since this is a voxel engine, having a completely flexible 
// shader system isn't that important, so I'm using a pre-defined set of shaders. It adds to the work when creating
// a new shader, but probably cuts down on workload in the long run
enum VEShaderType
{
	VST_ClearGBuffer,
	VST_RenderGBuffer,
	VST_SSAO,
	VST_DirectionalLight,
	VST_PointLight,
	VST_SpotLight,
	VST_ShadowMap,
	VST_FinalBlend,

	VST_Voxel,

	VST_Max
};


// ----------------- Classes ------------------

// Handles loading/unloading of all shaders used in the game
class VEShaderManager
{
	public :

		// ----- Public Functions -----

		// Construction
		VEShaderManager();

		// Cleans up the memory used by all of the shaders
		void		Uninitialise();
		
		// Loads a shader from disk
		bool		LoadShader( VEShaderType aShaderType );

		// Returns a shader from the map
		VEShader*	GetShader( VEShaderType aShaderType );


	private :

		// ----- Private Variables -----

		std::map<VEShaderType, VEShader*> myShaders;
};


#endif // !VE_SHADER_MANAGER_H