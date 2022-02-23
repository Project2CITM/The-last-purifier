#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

#include "Room.h"
#include "GameObject.h"
#include "Timer.h"


#define MAX_ROOMS_COLUMNS 19
#define MAX_ROOMS_ROWS 11

class RoomManager{
	
public:
	RoomManager(Application* app) {
		this->app = app;
	}

	void Start();
	void Update();
	void PostUpdate();
	void CleanUp();

	void GenerateMap(short RoomNumber);
	int CheckAdjacentSpace(Room* r);
	int CheckAdjacentSpace(iPoint p);
	void CreateDoors();
	Room* CreateRoom(iPoint mapPosition);

	void DrawRooms();
	void DrawDoors();

public:
	Application* app = nullptr;
	List<Room*> rooms;
	Room* roomPositions[MAX_ROOMS_COLUMNS][MAX_ROOMS_ROWS];
	SDL_Texture* tile_texture = nullptr;
	iPoint bossRoom = iPoint(-1, -1);
};

#endif //__ROOMMANAGER_H_