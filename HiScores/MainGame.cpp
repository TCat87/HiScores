#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <string>
#include <iterator>

using namespace std;
//array
constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

int xOffset = 50;
int inputIndex = 0;//used by MainGameEntry(),MainGameUpdate().
int entryCount = 0;//
int letterCountMax = 3;
int letterID = 0;
bool updateScores = true;//Score board update ON/OFF
bool disableEntry = false;//Dissables input
bool displayResults = false;//Switch for "name entry screen" / "score board screen"
std::vector<int>currentFrame = { 0, 0, 0 };//WIP
std::vector<int>currentID = { 0, 0, 0 };//WIP
std::vector<int> gPlayerEntry(4);
int NEWSCORE = 323;//Play::RandomRoll(2000);//New Player Score
std::vector<int> gFileData = {};//Player data from .txt
std::vector<int> DEFAULTDATA =
{
	33, 33, 33, 100,
	34, 34, 34, 200,
	35, 35, 35, 300,
	36, 36, 36, 400,
	37, 37, 37, 500,
	38, 38, 38, 600,
	39, 39, 39, 700,
	40, 40, 40, 800,
	41, 41, 41, 900,
	42, 42, 42, 1000
};
std::vector<std::vector<int>>gExTopPlayers;
std::vector<std::vector<int>>gTopPlayers;
std::vector<string>gNames2 = {};
std::vector<string>gNames = {};





void Draw();

enum GameObjectType
{
	TYPE_LETTER,
	TYPE_CURSOR,
};


void sort_file_data()
{
	int counter = 0;
	std::vector<int>tempPlayerData = {};

	for (int i = 0; i < gFileData.size();++i)	//old player data from file e.g. {33, 33, 33, 100, ........., 1000}
	{
		tempPlayerData.push_back(gFileData[i]);	//format data as groups of 4 entries to tempPlayerData e.g. {33, 33, 33, 100}, .........., {42, 42, 42, 1000}
		counter++;
		if (counter == 4)
		{
			gExTopPlayers.push_back(tempPlayerData); //add formated data togExTopPlayers e.g. {{33, 33, 33, 100}, .........., {42, 42, 42, 1000}}
			counter = 0;
			tempPlayerData.clear();
		};
	};
}


void merge_data()
{
	gExTopPlayers.push_back(gPlayerEntry);	//add new players data
}


void sort_by_score()
{
	std::vector<int>tempData(4);
	int slot = 0;

	for (int i = 0;i < gExTopPlayers.size();++i)
	{
		for (int j = 0;j < gExTopPlayers.size();++j)
		{
			if (tempData[3] < gExTopPlayers[j][3])
			{
				tempData = gExTopPlayers[j];
				slot = j;
			}
		}
		gTopPlayers.push_back(tempData);
		gExTopPlayers[slot][3] = 0;
		tempData = { 0, 0, 0, 0 };
		slot = 0;
	}
}


void remove_worst()
{
	gTopPlayers.pop_back();
}


void conversion()
{
	std::vector<char>letters = {};
	std::vector<int>V1(26);
	std::iota(V1.begin(), V1.end(), 33);
	std::vector<char>V2(26);
	std::iota(V2.begin(), V2.end(), 'A');
	int counter = 0;

	for (int i = 0;i < gTopPlayers.size();++i)
	{
		for (int j = 0;j < gTopPlayers[i].size();++j)
		{
			if (counter == 3)
			{
				counter = 0;
				std::string gNames2(letters.begin(), letters.end());
				gNames.push_back(gNames2);
				letters.clear();
				break;
			}
			for (int k = 0;k < V1.size();++k)
			{
				if (gTopPlayers[i][j] == V1[k])
				{
					letters.push_back(V2[k]);
					counter++;
				}
			}
		}
	}
}


void DrawGameStats()
{
	Play::DrawFontText("64px", "Encrypted: " + std::to_string(gPlayerEntry[0]) + " " + std::to_string(gPlayerEntry[1]) + " " + std::to_string(gPlayerEntry[2]), { DISPLAY_WIDTH - 200, 50 }, Play::CENTRE);
	Play::DrawFontText("64px", "Result screen: " + std::to_string(displayResults), { DISPLAY_WIDTH - 200, 115 }, Play::CENTRE);
	Play::DrawFontText("64px", "Current obj ID: " + std::to_string(currentID[letterID]), { DISPLAY_WIDTH - 200, 190 }, Play::CENTRE);
	Play::DrawFontText("64px", "Current obj Frame: " + std::to_string(currentFrame[letterID]), { DISPLAY_WIDTH - 200, 255 }, Play::CENTRE);
}

void DrawCursor()
{
	int id = Play::CreateGameObject
	(
		TYPE_CURSOR,
		{
			DISPLAY_WIDTH / 2 - 150 + xOffset * letterID, 270
		}, 20, "font132"
	);

	GameObject& CURSOR = Play::GetGameObject(id);
	CURSOR.frame = 63;
	Play::DrawObjectRotated(CURSOR);
}

bool SaveDefaultScores()
{
	std::ofstream ScoresFile;
	copy(DEFAULTDATA.begin(), DEFAULTDATA.end(), back_inserter(gFileData));
	ScoresFile.open("scores.txt");		//Create/Open file as "scores.txt"
	if (ScoresFile.is_open())           //Safety check if file got open, returns bool value
	{
		for (int i = 0; i < DEFAULTDATA.size();++i)
		{
			ScoresFile << DEFAULTDATA[i] << '\n';
		}
	}
	ScoresFile.close();                 //close file when done
	return 0;
}


bool SaveScore()
{
	std::ofstream ScoresFile;
	ScoresFile.open("scores.txt");		//Create/Open file as "scores.txt"
	if (ScoresFile.is_open())           //Safety check if file got open, returns bool value
	{
		for (int i = 0; i < gTopPlayers.size();++i)
		{
			for (int j = 0; j < gTopPlayers[i].size();++j)
			{
				ScoresFile << gTopPlayers[i][j] << '\n';
			}
		}
	}
	ScoresFile.close();                 //close file when done
	return 0;
}


void LoadScores()
{
	std::fstream data_file;
	data_file.open("scores.txt", std::ios::in);

	if (!data_file)
	{
		SaveDefaultScores();
	}

	else if (data_file.peek() == std::ifstream::traits_type::eof())//check if file is empty
	{
		SaveDefaultScores();//JFF :).
	}

	else if (data_file.is_open())
	{
		std::string data;
		while (getline(data_file, data))
		{
			gFileData.push_back(std::stoi(data));
		}

		data_file.close();
	}
}


void showScores()
{
	if (updateScores == true)
	{
		LoadScores();
		sort_file_data();
		merge_data();
		sort_by_score();
		remove_worst();
		conversion();
		SaveScore();
	}
	if (updateScores == false)
	{

		LoadScores();
		sort_file_data();
		sort_by_score();
		conversion();
		//Data from file: gNames contains 10 char names("KKK",...,"TTT"), gTopPlayers contains rest of data({52,52,52,1000},...,{43,43,43,100}) Use case: HIGH SCORES button :).
	}
}


// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data//Backgrounds//background.png");

	for (letterID; letterID < 3; letterID++)
	{
		int id = Play::CreateGameObject
		(
			TYPE_LETTER,//TYPE_currentLETTER,
			{
				DISPLAY_WIDTH / 2 - 150 + xOffset * inputIndex, 250
			}, 20, "font132"
		);

		GameObject& LETTER = Play::GetGameObject(id);//GetGameObjectByType(TYPE_LETTER);//(TYPE_currentLETTER);
		LETTER.frame = 33;
		gPlayerEntry[letterID] = LETTER.frame;

		currentFrame[letterID] = LETTER.frame;//for DrawGameStats()
		currentID[letterID] = id;//for DrawGameStats()
		inputIndex++;
	}
	letterID = 0;
	inputIndex = 1;
}


// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{

	if (Play::KeyPressed(VK_UP) && !disableEntry)
	{
		GameObject& tempLETTER = Play::GetGameObject(inputIndex);
		if (tempLETTER.frame == 33)
		{
			tempLETTER.frame = 59;
		}
		tempLETTER.frame--;
		gPlayerEntry[letterID] = tempLETTER.frame;
		currentFrame[letterID] = tempLETTER.frame;
	}

	if (Play::KeyPressed(VK_DOWN) && !disableEntry)
	{
		GameObject& tempLETTER = Play::GetGameObject(inputIndex);
		if (tempLETTER.frame == 58)
		{
			tempLETTER.frame = 32;
		}
		tempLETTER.frame++;
		gPlayerEntry[letterID] = tempLETTER.frame;
		currentFrame[letterID] = tempLETTER.frame;//WIP
	}

	if (Play::KeyPressed(VK_RIGHT) && !disableEntry)
	{
		GameObject& tempLETTER = Play::GetGameObject(inputIndex);
		if (inputIndex == 3)
		{
			inputIndex = 1;
			letterID = 0;
		}
		else
		{
			inputIndex++;
			letterID++;
		}
	}

	if (Play::KeyPressed(VK_LEFT) && !disableEntry)
	{
		GameObject& tempLETTER = Play::GetGameObject(inputIndex);
		if (inputIndex == 1)
		{
			inputIndex = 3;
			letterID = 2;
		}
		else
		{
			inputIndex--;
			letterID--;
		}
	}

	if (Play::KeyPressed(VK_RETURN) && !disableEntry)
	{
		displayResults = true;
		gPlayerEntry[3] = NEWSCORE;
		showScores();
	};

	Draw();

	return Play::KeyDown(VK_ESCAPE);

}


// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

void Draw()
{
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();
	if (displayResults == false)
	{

		Play::DrawFontText("105px", "Enter your name:", { DISPLAY_WIDTH / 2, 150 }, Play::CENTRE);

		std::vector<int> vLETTERS = Play::CollectGameObjectIDsByType(TYPE_LETTER);
		for (int idPiece : vLETTERS)
		{
			GameObject& LETTER = Play::GetGameObject(idPiece);
			Play::DrawObjectRotated(LETTER);
		}
	};

	if (displayResults == true)
	{

		disableEntry = true;
		int LINEHEIGHT = 67;
		int lineHeight = 67;

		Play::DrawFontText("64px", "HIGH SCORES: ", { DISPLAY_WIDTH / 2, 35 }, Play::CENTRE);
		for (int i = 0; i < gTopPlayers.size(); ++i)
		{
			Play::DrawFontText("64px", gNames[i] + "............................................................" + std::to_string(gTopPlayers[i][3]), { DISPLAY_WIDTH / 2, (20 + lineHeight) }, Play::CENTRE);
			lineHeight = LINEHEIGHT + lineHeight;
		}
	}
	if (!disableEntry)
	{
		DrawCursor();
	}
	DrawGameStats();
	Play::PresentDrawingBuffer();

}
