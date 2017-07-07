#ifndef VE_SHADER_DX_H
#define VE_SHADER_DX_H


// ------------- Forward Declarations -------------

class VEBasicCamera;
class VELight;


// ------------------- Classes --------------------

// An abstract class for implementing vertex & pixel shaders. A specific implementation of this class needs to
// define the buffers sent through to the shader. It creates a bit of overhead when you need to add in a new
// shader, but the voxel engine shouldn't need too many shaders so it's cheaper than developing a more generic
// system. Derived classes must set the vertex and pixel shader strings in their constructor
class VEShader
{
    public :

        // ---------- Public Functions ----------

        // Construction
        VEShader( int aVertexCBSize, int aPixelCBSize );

        // Deconstruction
        virtual ~VEShader();

        // Loads the vertex & pixel shaders, initialises the associated buffers
        virtual bool        Initialise();

		// Cleans up the memory used by the shader
		virtual void		Uninitialise();

		// Fills the vertex shader constant buffer
		virtual bool		PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* aLight );

		// Fills the pixel shader constant buffer
		virtual bool		PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight );

		// Sets the shader resources (textures) for the pixel shader
		void				SetShaderResources( ID3D11ShaderResourceView** someShaderResources, int aShaderResourceCount );

		// Unbinds the shader resources (textures) from the pixel shader
		void				ClearResources();

		// Sets the sampler states used by the pixel shader
		void				SetSamplerStates( ID3D11SamplerState** someShaderSamplers, int aSamplerCount );

		// Unbinds the samplers from the pixel shader
		void				ClearSamplers();

		// Sends the geometry in the input assembler through to the shader
		void				Draw( int aVertexCount );

		// Sends the indexed geometry in the input assembler through to the shader
		void				DrawIndexed( int anIndexCount );

		// Sends the instanced geometry in the input assembler through to the shader
		void				DrawInstanced( int anIndexCount, int anInstanceCount );

        
        // ------------- Accessors --------------

        ID3D11VertexShader* GetVertexShader()   { return myVertexShader; }

		ID3D11Buffer*		GetVertexBuffer()	{ return myVertexConstantBuffer; }

        ID3D11PixelShader*  GetPixelShader()    { return myPixelShader; }

		ID3D11Buffer*		GetPixelBuffer()	{ return myPixelConstantBuffer; }


    protected :

        // --------- Required Functions ---------

        // Creates the data layout used to send vertex information to the vertex shader
        virtual bool		CreateDataLayout( ID3D10Blob* aVertexShader )   = 0;


        // --------- Protected Functions --------

        // Logs an error in the shader log file
        void				LogError( ID3D10Blob* anErrorMessage, const std::string& aShaderFile );


        // --------- Protected Variables --------

        std::wstring				myVertexShaderFile;
        std::string					myVertexShaderProfile;
        std::string					myVertexShaderFunction;
        ID3D11InputLayout*			myVertexShaderInputLayout;
        ID3D11Buffer*				myVertexConstantBuffer;
        ID3D11VertexShader*			myVertexShader;

        std::wstring				myPixelShaderFile;
        std::string					myPixelShaderProfile;
        std::string					myPixelShaderFunction;
        ID3D11Buffer*				myPixelConstantBuffer;
        ID3D11PixelShader*			myPixelShader;

		ID3D11ShaderResourceView**	myShaderResources;
		int							myResourceCount;

		ID3D11SamplerState**		myShaderSamplers;
		int							mySamplerCount;

		int							myVertexCBSize;
		int							myPixelCBSize;


    private :

        // ---------- Private Functions ----------

        // Loads and builds the vertex shader
        bool            LoadVertexShader();

        // Loads and builds the pixel shader
        bool            LoadPixelShader();

        // Loads and compiles shader file from disk, storing the result in the supplied buffer
        ID3D10Blob*     LoadShader( const std::wstring& aFileName, const std::string& aMainFunction, const std::string& aShaderProfile );

		// Creates the vertex shader constant buffer ( matrices)
		bool			CreateVertexConstantBuffer();

		// Creates the pixel shader buffer (e.g. lighting parameters)
		bool			CreatePixelConstantBuffer();
};


#endif // !VE_SHADER_DX_H