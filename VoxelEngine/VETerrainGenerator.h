#ifndef VE_TERRAIN_GENERATOR_H
#define VE_TERRAIN_GENERATOR_H


// ---------------- Forward Declarations ---------------

namespace noise
{
	namespace utils
	{
		class NoiseMap;
	}
}


// ---------------------- Classes ----------------------

// The terrain generator interacts with the chunk manager and a coherent noise generate to construct
// chunks based on height map data. The terrain generator can create an arbitrary number of chunks
// with a smooth transition
class VETerrainGenerator
{
	public :

		// -------- Public Functions --------

		// Construction
		VETerrainGenerator();

		// Initialises the noise generator, noise map and plane builder
		bool	Initialise();

		// Cleans up the memory used by the terrain generator
		void	Uninitialise();

		// Generates random terrain based on the number of required chunks
		void	GenerateTerrain( int aChunkWidth, int aChunkDepth, DirectX::XMFLOAT3 aPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) );


		// ----------- Accessors ------------

		// Whether the noise map is saved to a texture
		bool	GetGenerateNoiseTexture()					{ return myGenerateNoiseTexture; }
		void	SetGenerateNoiseTexture( bool aGenerate )	{ myGenerateNoiseTexture = aGenerate; }

		// The size of the noise map being sampled to generate the terrain data, the higher the value the
		// noisier the terrain
		double	GetStepSize()								{ return myNoiseStepSize; }
		void	SetStepSize( double aStepSize )				{ myNoiseStepSize = aStepSize; }


	private :

		// -------- Private Functions -------

		// Saves the noise map to a texture file
		void	SaveNoiseMapTexture( noise::utils::NoiseMap* aNoiseMap, int aCount );


		// -------- Private Variables -------

		bool		myGenerateNoiseTexture;
		std::string myNoiseTexture;

		double		myNoiseStepSize;
		int			myNoiseRange;
};


#endif // !VE_TERRAIN_GENERATOR_H