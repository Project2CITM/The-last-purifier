#include "TestScene.h"
#include "PlayerController.h"

TestScene::TestScene(Application* app):Scene(app,"testScene")
{
}

TestScene::~TestScene()
{
}

bool TestScene::Start()
{
    PlayerController* playerController = new PlayerController("test", "test", _app);
    gameObjects.add(playerController);


    roomManager.Start();

    Scene::Start();

    return true;
}

bool TestScene::PreUpdate()
{
    if (_app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
        roomManager.CleanUp();
        roomManager.Start();
    }
    
    Scene::PreUpdate();
    return true;
}

bool TestScene::Update()
{
    //if (_app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
    //    roomManager.mapMovement.x -= 10;

    //if (_app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
    //    roomManager.mapMovement.x += 10;

    //if (_app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
    //    roomManager.mapMovement.y -= 10;

    //if (_app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
    //    roomManager.mapMovement.y += 10;

    roomManager.Update();
    Scene::Update();
    return true;
}

bool TestScene::PostUpdate()
{
    // Test draw circle
     //_app->renderer->AddCircleRenderQueue(iPoint{ 50, 50 }, 40, SDL_Color{ 50,0,0,255 }, 2, 50);

    // Test draw line  
    //_app->renderer->AddLineRenderQueue(iPoint{ 50,50 }, iPoint{ 100,50 }, true, SDL_Color{ 50,0,0,255 }, 2, 50);

    // Tesst draw Rect
    //_app->renderer->AddRectRenderQueue(SDL_Rect{ 0,0,50,50 }, SDL_Color{0,0,255,255},2,50);

    roomManager.PostUpdate();
    //_app->physics->ShapesRender();
   
    Scene::PostUpdate();
    return true;
}

bool TestScene::CleanUp()
{
    roomManager.CleanUp();
    Scene::CleanUp();

    return false;
}
