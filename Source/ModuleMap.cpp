﻿#include "ModuleMap.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"

ModuleMap::ModuleMap(bool start_enabled) : Module(start_enabled), mapLoaded(false)
{
    name = "map";
}

// Destructor
ModuleMap::~ModuleMap()
{
	//RELEASE(pathFinding);
}

//Ask for the value of a custom property
int Properties::GetProperty(const char* value, int defaultValue) const
{
	ListItem<Property*>* item = list.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return defaultValue;
}

// Called before render is available
bool ModuleMap::Init(pugi::xml_node& config)
{
	this->config = config;

    bool ret = true;

    folder = (config.child("hub_folder").child_value());

    return ret;
}

// Draw the map (all requried layers)
UpdateStatus ModuleMap::PostUpdate()
{
	if (mapLoaded == false || app->scene->currentScene != SCENES::HUB) return UpdateStatus::UPDATE_CONTINUE;

	//Prepare the loop to draw all tilesets + DrawTexture()
	ListItem<MapLayer*>* mapLayerItem = nullptr;
	mapLayerItem = mapData.layers.start;

	//Make sure we draw all the layers and not just the first one
	while (mapLayerItem != NULL) {

		if (mapLayerItem->data->properties.GetProperty("Draw") == 1)
		{
			Draw(mapLayerItem);
		}

		//Draws roof only when players isn't inside
		if (mapLayerItem->data->properties.GetProperty("Roof") == 1 && roof)
		{
			Draw(mapLayerItem);
		}

		//Draws darkness
		if (mapLayerItem->data->properties.GetProperty("Roof") == 2 && !roof)
		{
			Draw(mapLayerItem);
		}

		mapLayerItem = mapLayerItem->next;
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleMap::Draw(ListItem<MapLayer*>* mapLayerItem)
{
	for (int x = 0; x < mapLayerItem->data->width; x++)
	{
		for (int y = 0; y < mapLayerItem->data->height; y++)
		{
			int gid = mapLayerItem->data->Get(x, y);

			if (gid > 0) {

				TileSet* tileset = GetTilesetFromTileId(gid);

				SDL_Rect r = tileset->GetTileRect(gid);
				iPoint pos = MapToWorld(x, y);

				try
				{
					app->renderer->AddTextureRenderQueue(tileset->texture, iPoint(pos.x, pos.y), r, 1, 0, 0.5f);
				}
				catch (const std::exception& e)
				{
					LOG(e.what());
				}
			}
		}
	}
}

// L04: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint ModuleMap::MapToWorld(int x, int y) const
{
	iPoint ret;

	// L05: DONE 1: Add isometric map to world coordinates
	if (mapData.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * mapData.tileWidth;
		ret.y = y * mapData.tileHeight;
	}
	else if (mapData.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (mapData.tileWidth / 2);
		ret.y = (x + y) * (mapData.tileHeight / 2);
	}
	else
	{
		//LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

// L04: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint ModuleMap::MapToWorld(iPoint pos) const
{
	iPoint ret;

	// L05: DONE 1: Add isometric map to world coordinates
	if (mapData.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = pos.x * mapData.tileWidth;
		ret.y = pos.y * mapData.tileHeight;
	}
	else if (mapData.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (pos.x - pos.y) * (mapData.tileWidth / 2);
		ret.y = (pos.x + pos.y) * (mapData.tileHeight / 2);
	}
	else
	{
		//LOG("Unknown map type");
		ret.x = pos.x; ret.y = pos.y;
	}

	return ret;
}

// L05: DON 2: Add orthographic world to map coordinates
iPoint ModuleMap::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	// L05: DONE 3: Add the case for isometric maps to WorldToMap
	if (mapData.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / mapData.tileWidth;
		ret.y = y / mapData.tileHeight;
	}
	else if (mapData.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = mapData.tileWidth * 0.5f;
		float half_height = mapData.tileHeight * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2);
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		//LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}
	return ret;
}

// L05: DON 2: Add orthographic world to map coordinates
iPoint ModuleMap::WorldToMap(iPoint pos) const
{
	iPoint ret(0, 0);

	// L05: DONE 3: Add the case for isometric maps to WorldToMap
	if (mapData.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = pos.x / mapData.tileWidth;
		ret.y = pos.y / mapData.tileHeight;
	}
	else if (mapData.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = mapData.tileWidth * 0.5f;
		float half_height = mapData.tileHeight * 0.5f;
		ret.x = int((pos.x / half_width + pos.y / half_height) / 2);
		ret.y = int((pos.y / half_height - (pos.x / half_width)) / 2);
	}
	else
	{
		//LOG("Unknown map type");
		ret.x = pos.x; ret.y = pos.y;
	}

	return ret;
}

bool ModuleMap::InTileCenter(iPoint worldPos, int tileDimensions) const
{
	if (tileDimensions > 8) tileDimensions = 8;

	iPoint mapPos = WorldToMap(worldPos);

	iPoint tileWorldPos = MapToWorld(mapPos);

	tileWorldPos.x += 8;
	tileWorldPos.y += 8;

	if (worldPos.x > tileWorldPos.x - tileDimensions && worldPos.x < tileWorldPos.x + tileDimensions &&
		worldPos.y > tileWorldPos.y - tileDimensions && worldPos.y < tileWorldPos.y + tileDimensions) return true;
	
	return false;
}

void ModuleMap::LoadLayerMeta()
{
	ListItem<MapLayer*>* mapLayerItem;
	mapLayerItem = mapData.layers.start;

	//Make sure we draw all the layers and not just the first one
	while (mapLayerItem != NULL) 
	{
		if (mapLayerItem->data->properties.GetProperty("Logic") == 1)
		{
			for (int x = 0; x < mapLayerItem->data->width; x++)
			{
				for (int y = 0; y < mapLayerItem->data->height; y++)
				{
					// L04: DONE 9: Complete the draw function
					uint gid = mapLayerItem->data->Get(x, y);

					if (gid > 0) 
					{
						MapObject obj;

						if (gid > 3221225470)
						{
							// if the tile was rotated 180º
							gid -= 3221225472;
							obj.rotation = 180;
						}
						else if (gid > 2684354560)
						{
							//if the tile was rotated 90º
							gid -= 2684354560;
							obj.rotation = 90;
						}
						else if (gid > 5000)
						{ 
							//if the tile was rotated 270º
							gid -= 1610612736;
							obj.rotation = 270;
						}

						TileSet* tileset = GetTilesetFromTileId(gid);

						// ID
						obj.id = gid - tileset->firstgid;
	
						obj.position = MapToWorld(x, y);		

						mapObjects.add(obj);
					}
				}
			}
		}

		mapLayerItem = mapLayerItem->next;
	}
}

//Pick the right Tileset based on a tile id
TileSet* ModuleMap::GetTilesetFromTileId(int id) const
{
	ListItem<TileSet*>* item = mapData.tilesets.start;
	TileSet* set = item->data;

	while (item)
	{
		if (id < item->data->firstgid)
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	return set;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect rect = { 0 };

	// L04: DONE 7: Get relative Tile rectangle
	int relativeId = id - firstgid;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + spacing) * (relativeId % columns));
	rect.y = margin + ((rect.h + spacing) * (relativeId / columns));
	
	return rect;
}

// Called before quitting
bool ModuleMap::CleanUp()
{
    LOG("Unloading map");

    // L03: DONE 2: Make sure you clean up any memory allocated from tilesets/map
    // Remove all tilesets
	currentMap = "null";

	ListItem<TileSet*>* item;
	item = mapData.tilesets.start;

	while (item != nullptr)
	{
		RELEASE(item->data);
		item = item->next;
	}
	mapData.tilesets.clear();

	// L04: DONE 2: clean up all layer data
	// Remove all layers
	ListItem<MapLayer*>* item2;
	item2 = mapData.layers.start;

	while (item2 != nullptr)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	mapData.layers.clear();

	mapObjects.clear();

    return true;
}

bool ModuleMap::CleanUpScene()
{
	LOG("Unloading map");

	// L03: DONE 2: Make sure you clean up any memory allocated from tilesets/map
	// Remove all tilesets
	currentMap = "null";

	ListItem<TileSet*>* item;
	item = mapData.tilesets.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	mapData.tilesets.clear();

	// L04: DONE 2: clean up all layer data
	// Remove all layers
	ListItem<MapLayer*>* item2;
	item2 = mapData.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	mapData.layers.clear();

	mapObjects.clear();

	return true;
}

// Load new map
bool ModuleMap::Load(std::string filename)
{
	if (filename == currentMap) return true;
	
	currentMap = filename;

    bool ret = true;
    std::string tmp = folder + filename;

	pugi::xml_document mapFile; 
    pugi::xml_parse_result result = mapFile.load_file(tmp.c_str());

    if(result == NULL)
    {
      //  LOG("Could not load map xml file %s. pugi error: %s", filename, result.description());
        ret = false;
    }

	// Load general info
    if(ret == true)
    {
        // L03: DONE 3: Create and call a private function to load and fill all your map data
		ret = LoadMap(mapFile);
	}

    // L03: DONE 4: Create and call a private function to load a tileset
    // remember to support more any number of tilesets!
	if (ret == true)
	{
		ret = LoadTileSets(mapFile.child("map"));
	}

	// L04: DONE 4: Iterate all layers and load each of them
	// Load layer info
	if (ret == true)
	{
		ret = LoadAllLayers(mapFile.child("map"));
	}
    
    if(ret == true)
    {
		// Load mate data
		LoadLayerMeta();
    }

    mapLoaded = ret;

    return ret;
}

// Load map general properties
bool ModuleMap::LoadMap(pugi::xml_node mapFile)
{
	bool ret = true;
	pugi::xml_node map = mapFile.child("map");

	if (map == NULL)
	{
		//LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		// L03: DONE 3: Load map general properties
		mapData.height = map.attribute("height").as_int();
		mapData.width = map.attribute("width").as_int();
		mapData.tileHeight = map.attribute("tileheight").as_int();
		mapData.tileWidth = map.attribute("tilewidth").as_int();

		// L05: DONE 1: Add formula to go from isometric map to world coordinates
		mapData.type = MAPTYPE_UNKNOWN;
		if (strcmp(map.attribute("orientation").as_string(), "isometric") == 0)
		{
			mapData.type = MAPTYPE_ISOMETRIC;
		}
		if (strcmp(map.attribute("orientation").as_string(), "orthogonal") == 0)
		{
			mapData.type = MAPTYPE_ORTHOGONAL;
		}
	}

	return ret;
}

// L03: DONE 4: Implement the LoadTileSet function to load the tileset properties
bool ModuleMap::LoadTileSets(pugi::xml_node mapFile) {

	bool ret = true;

	pugi::xml_node tileset;
	for (tileset = mapFile.child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();
		if (ret == true) ret = LoadTilesetDetails(tileset, set);
		if (ret == true) ret = LoadTilesetImage(tileset, set);
		mapData.tilesets.add(set);
	}

	return ret;
}

// L03: DONE 4: Load Tileset attributes
bool ModuleMap::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	// L03: DONE 4: Load Tileset attributes
	set->name = tileset_node.attribute("name").as_string();
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tileWidth = tileset_node.attribute("tilewidth").as_int();
	set->tileHeight = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	set->tilecount = tileset_node.attribute("tilecount").as_int();
	set->columns = tileset_node.attribute("columns").as_int();

	return ret;
}

// L03: DONE 4: Load Tileset image
bool ModuleMap::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		//LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		// L03: DONE 4: Load Tileset image
		std::string imageS = image.attribute("source").as_string();
	
		//Removes ../../
		auto n = imageS.find("../../");
		if (n != std::string::npos)
		{
			imageS.erase(n, 6);
		}


		//std::string tmp = folder + imageS;
		set->texture = app->textures->Load(imageS);
	}

	return ret;
}

// L04: DONE 3: Implement a function that loads a single layer layer
bool ModuleMap::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	//Load the attributes
	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();

	//Call Load Propoerties
	LoadProperties(node, layer->properties);

	//Reserve the memory for the tile array
	layer->data = new uint[layer->width * layer->height];
	memset(layer->data, 0, layer->width * layer->height);

	//Iterate over all the tiles and assign the values
	pugi::xml_node tile;
	int i = 0;
	for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
	{
		layer->data[i] = tile.attribute("gid").as_uint();
		i++;
	}

	return ret;
}

// L04: DONE 4: Iterate all layers and load each of them
bool ModuleMap::LoadAllLayers(pugi::xml_node mapNode) {
	bool ret = true;
	for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
	{
		//Load the layer
		MapLayer* mapLayer = new MapLayer();
		ret = LoadLayer(layerNode, mapLayer);

		//add the layer to the map
		mapData.layers.add(mapLayer);
	}

	return ret;
}

//Load a group of properties from a node and fill a list with it
bool ModuleMap::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
	{
		Properties::Property *p = new Properties::Property();
		p->name = propertieNode.attribute("name").as_string();
		p->value = propertieNode.attribute("value").as_int();

		properties.list.add(p);
	}
	
	return ret;
}

// L12b: Create walkability map for pathfinding
bool ModuleMap::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	ListItem<MapLayer*>* item;
	item = mapData.layers.start;

	for (item = mapData.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.GetProperty("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width * layer->height];
		memset(map, 1, layer->width * layer->height);

		for (int y = 0; y < mapData.height; ++y)
		{
			for (int x = 0; x < mapData.width; ++x)
			{
				int i = (y * layer->width) + x;

				int tileId = layer->Get(x, y);
				TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
				}
			}
		}

		*buffer = map;
		width = mapData.width;
		height = mapData.height;
		ret = true;

		break;
	}

	return ret;
}