
// ---------------------- Includes ----------------------

#include "Stdafx.h"
#include "VETerrainGenerator.h"

#include "VoxelEngine.h"
#include "VEChunkManager.h"
#include "VEChunk.h"

#include <noise/noise.h>
#include "noiseutils.h"


// ---------------------- Classes -----------------------

// Construction
VETerrainGenerator::VETerrainGenerator() :
	myNoiseTexture( "NoiseTexture_" ),
	myGenerateNoiseTexture( false ),
	myNoiseStepSize( 0.8 ),
	myNoiseRange( 50 )
{
}


// Initialises the noise generator, noise map and plane builder
bool VETerrainGenerator::Initialise()
{
	return true;
}


// Cleans up the memory used by the terrain generator
void VETerrainGenerator::Uninitialise()
{	
}


// Generates random terrain based on the number of required chunks
void VETerrainGenerator::GenerateTerrain( int aChunkWidth, int aChunkDepth, DirectX::XMFLOAT3 aPosition )
{
	VEChunkManager* chunkManager = VoxelEngine::GetInstance()->GetChunkManager();
	assert( chunkManager != NULL );
	
	// Create the grid
	if( !chunkManager->CreateGrid( aChunkWidth, aChunkDepth, 64, aPosition ) )
	{
		assert( false );
		return;
	}

	// Standard (divide results by 2 for a flat terrain)
	module::Perlin				noiseGenerator;
	utils::NoiseMap				heightMap;
	utils::NoiseMapBuilderPlane planeBuilder;

	planeBuilder.SetSourceModule( noiseGenerator );
	planeBuilder.SetDestNoiseMap( heightMap );
	planeBuilder.SetDestSize( chunkManager->GetChunkDimensions(), chunkManager->GetChunkDimensions() );

	// Generate the terrain
	srand( (unsigned int)time(NULL) );
	double initialOffset				= (double)(rand() % myNoiseRange + 1);
	double currentZ						= initialOffset;
	for( int z = 0; z < aChunkDepth; z++ )
	{
		double currentX = initialOffset;
		for( int x = 0; x < aChunkWidth; x++ )
		{
			// Build the height map data
			planeBuilder.SetBounds( currentX, currentX + myNoiseStepSize, currentZ, currentZ + myNoiseStepSize );
			planeBuilder.Build();

			// Apply the height map to the chunk
			VEChunk* currentChunk = chunkManager->GetChunk( x, z );
			assert( currentChunk != NULL );

			currentChunk->ApplyHeightMap( &heightMap );
			//currentChunk->ApplyStyle( CS_Pyramid );

			currentX += myNoiseStepSize;
		}

		currentZ += myNoiseStepSize;
	}
}


// Saves the noise map to a texture file
void VETerrainGenerator::SaveNoiseMapTexture( utils::NoiseMap* aNoiseMap, int aCount )
{
	// Render the image
	utils::RendererImage renderer;
	utils::Image image;

	char count[2];
	_itoa_s( aCount, count, 10 );
	std::string path = myNoiseTexture + count + ".bmp";

	renderer.SetSourceNoiseMap( *aNoiseMap );
	renderer.SetDestImage( image );
	renderer.Render();

	// Write the image to a file
	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename( path.c_str() );
	writer.WriteDestFile();
}