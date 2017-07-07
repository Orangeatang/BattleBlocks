#ifndef BATTLE_BLOCKS_H
#define BATTLE_BLOCKS_H


// ---------- Forward Declarations ----------

class VEFreeCamera;
class SystemInputProcessor;
class LocalPlayer;


// ---------------- Classes -----------------

// Initialises the voxel engine, creates the game window and handles the main update loop for the game
class BattleBlocksGame
{
	public :

		// ---------- Public Functions ----------

		//~BattleBlocksGame();

		// Returns/creates the static game instance
		static BattleBlocksGame*    GetInstance();

		// Cleans up the static instance of the game
		static void                 Cleanup();

		// Creates the game window and initialises the voxel engine
		bool                        Initialise( int aScreenWidth = 1024, int aScreenHeight = 768 );

		// Closes and unregisters the game window
		void                        Uninitialise();

		// Runs the main update loop for the game
		void                        Run();

		// Deals with messages from Windows
		LRESULT                     ProcessWindowsMessage( HWND aWnd, UINT aMessage, WPARAM aWParam, LPARAM aLParam );


	private :

		// ---------- Private Functions ---------
		
		// Construction, access game class using GetInstance
		BattleBlocksGame();
		
		// Registers the BattleBlocks game window
		bool	RegisterGameWindow();

		// Creates the window used for rendering
		bool	CreateGameWindow();

		// Starts/restarts high performance timer
		void	StartFrameTimer();

		// Returns the number of milliseconds that have elapsed since the last time
		// this function was called
		float	GetFrameTime();


		// ---------- Private Variables ---------

		LPCTSTR                     myGameName;
		HINSTANCE                   myGameInstance;
		HWND                        myWindowHandle;

		bool                        myFullScreen;
		bool                        myVsync;
		bool                        myShowCursor;
		int                         myScreenWidth;
		int                         myScreenHeight;

		float						myFrameTime;
		__int64						myOldFrameTime;
		double						myTicksPerSecond;

		SystemInputProcessor*		myInputProcessor;

		LocalPlayer*				myLocalPlayer;

		static BattleBlocksGame*    myInstance;
};


#endif // !BATTLE_BLOCKS_H