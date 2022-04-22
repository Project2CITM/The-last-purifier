#include "HUDInGame.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "GUIButton.h"
#include "GUISlider.h"
#include "GUICheckbox.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleAudio.h"
#include "ModuleScene.h"
#include "SceneGame.h"
#include "PlayerCombat.h"
#include "PlayerController.h"
#include "ModuleEvents.h"
#include "SpellInfo.h"

RenderObject Controls1;
RenderObject Controls2;
RenderObject PauseBG;
RenderObject SettingsBG;

RenderObject iconSouls;

HUDInGame::HUDInGame() :Scene("HUDInGame")
{
	// Init event sysem
	this->listenTo = GameEvent::PLAYER_HIT;

	app->events->AddListener(this);
}

HUDInGame::~HUDInGame()
{
}

bool HUDInGame::InitScene()
{

	Scene::InitScene();

	return true;
}

bool HUDInGame::Start()
{
	PauseBG.InitAsTexture(app->textures->Load("Assets/Sprites/UI/PauseBG.png"), { app->renderer->camera->x + 10, app->renderer->camera->y -10 }, { 0,0,0,0 }, 0.35f, 4, 0, 0 , SDL_FLIP_NONE, 0);
	SettingsBG.InitAsTexture(app->textures->Load("Assets/Sprites/UI/FondoSettings.png"), { app->renderer->camera->x, app->renderer->camera->y }, { 0,0,0,0 }, 0.5f, 4, 0, 0, SDL_FLIP_NONE, 0);
	Controls1.InitAsTexture(app->textures->Load("Assets/Sprites/UI/Controls1_2.png"), { app->renderer->camera->x, app->renderer->camera->y }, { 0,0,0,0 }, 0.5f, 4, 1, 0, SDL_FLIP_NONE, 0);
	Controls2.InitAsTexture(app->textures->Load("Assets/Sprites/UI/Controls2_2.png"), { app->renderer->camera->x, app->renderer->camera->y }, { 0,0,0,0 }, 0.5f, 4, 1, 0, SDL_FLIP_NONE, 0);

	iconSouls.InitAsTexture(app->textures->Load("Assets/Sprites/UI/icons.png"), { app->renderer->camera->x + 15, app->renderer->camera->y + 21 }, { 62,31,31,31 }, 0.5f, 4, 0, 0, SDL_FLIP_NONE, 0);

	Hover = app->audio->LoadFx("Assets/Audio/SFX/UI/sfx_uiHover.wav");
	Press = app->audio->LoadFx("Assets/Audio/SFX/UI/sfx_uiSelect.wav");

	text = new Text({app->renderer->camera->x + 30, app->renderer->camera->y + 25 }, std::to_string(score));

	playerHp.bg = playerHp.delayHp = playerHp.currentHp = { app->renderer->camera->x + 15, app->renderer->camera->y + 10, 200, 10 };
	miniMap = { app->renderer->camera->x + 535, app->renderer->camera->y + 5, 100, 100 };

	resumeBUT = { app->renderer->camera->x + 155, app->renderer->camera->y + 70};//640 pixeles with pantalla
	settingsBUT = { app->renderer->camera->x + 155, app->renderer->camera->y + 160};
	controlsBUT = { app->renderer->camera->x + 155, app->renderer->camera->y +250};
	saveBUT = { app->renderer->camera->x + 355, app->renderer->camera->y + 70 };
	loadBUT = { app->renderer->camera->x + 355, app->renderer->camera->y + 130 };
	giveUpBUT = { app->renderer->camera->x + 355, app->renderer->camera->y +190};
	quitBUT = { app->renderer->camera->x + 355, app->renderer->camera->y +250};

	ResumeBUT = new GUIButton(resumeBUT, 117, 47, MenuButton::INGAMEPUASE, "Assets/Sprites/UI/Resume.png");
	SettingsBUT = new GUIButton(settingsBUT, 117, 47, MenuButton::INGAMEPUASE, "Assets/Sprites/UI/Settings.png");
	ControlsBUT = new GUIButton(controlsBUT, 117, 47, MenuButton::INGAMEPUASE, "Assets/Sprites/UI/Controls.png");
	SaveBUT = new GUIButton(saveBUT, 117, 47, MenuButton::INGAMEPUASE, "Assets/Sprites/UI/Save.png");
	LoadBUT = new GUIButton(loadBUT, 117, 47, MenuButton::INGAMEPUASE, "Assets/Sprites/UI/Load.png");
	GiveUpBUT = new GUIButton(giveUpBUT, 117, 47, MenuButton::INGAMEPUASE, "Assets/Sprites/UI/giveUp.png");
	QuitBUT = new GUIButton(quitBUT, 117, 47, MenuButton::INGAMEPUASE, "Assets/Sprites/UI/Quit.png");

	CloseControlsBUT = new GUIButton({ app->renderer->camera->x + 297, app->renderer->camera->y + 315 }, 46, 46, MenuButton::CONTROLSPAUSE, "Assets/Sprites/UI/Back.png");
	CloseSettingsBUT = new GUIButton({ app->renderer->camera->x + 297, app->renderer->camera->y + 315 }, 46, 46, MenuButton::SETTINGSPAUSE, "Assets/Sprites/UI/Back.png");

	MusicBUT = new GUIButton({ app->renderer->camera->x + 200, app->renderer->camera->y + 125 }, 27, 46, MenuButton::SETTINGSPAUSE, "Assets/Sprites/UI/fireSlider.png");
	MusicSlider = new GUISlider({ app->renderer->camera->x + 200, app->renderer->camera->y + 125 }, 300, 14, MenuButton::SETTINGSPAUSE, "Assets/Sprites/UI/Slider1.png");
	MusicSlider->CreateGUIBtn(MusicBUT);
	MusicSlider->SetValue(app->audio->musicVol/255);

	fxBUT = new GUIButton({ app->renderer->camera->x + 200, app->renderer->camera->y + 200 }, 27, 46, MenuButton::SETTINGSPAUSE, "Assets/Sprites/UI/fireSlider.png");
	fxSlider = new GUISlider({ app->renderer->camera->x + 200, app->renderer->camera->y + 200 }, 300, 14, MenuButton::SETTINGSPAUSE, "Assets/Sprites/UI/Slider1.png");
	fxSlider->CreateGUIBtn(fxBUT);
	fxSlider->SetValue(app->audio->fxVol / 255);

	FullScreenCHK = new GUICheckbox({ app->renderer->camera->x + 350, app->renderer->camera->y + 215 }, 60, 60, MenuButton::SETTINGSPAUSE, "Assets/Sprites/UI/CheckBox.png");

	InitializeSpellSlotsPositions();
	currentSpellText = new Text({ 280, 300 }, "");

	Scene::Start();

	return true;
}

bool HUDInGame::PreUpdate()
{
	if (!app->isPause)
	{
		if (startPause)
		{
			app->audio->SetMusicVolume(app->audio->musicVol*2);
			app->audio->SetSFXVolume(app->audio->fxVol * 2);
			//currentPauseMenu = CurrentPauseMenu::Pause;
			ControllerPos = 0;
			startPause = false;
		}
	}

	if (player == nullptr)
	{
		SceneGame* scene = (SceneGame*)app->scene->scenes[app->scene->currentScene];
		player = scene->player->controller->combat;
		InitializeSlots();
	}

	Scene::PreUpdate();

	return true;
}

bool HUDInGame::Update()
{
	int leftYMain, leftYOptions, leftXOptions;
	leftYMain = app->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTY);
	leftYOptions = app->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTY);
	leftXOptions = app->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX);

	score = player->player->souls;
	text->SetText(std::to_string(score));

	if (app->isPause)
	{
		if (!startPause)
		{
			app->audio->SetMusicVolume(app->audio->musicVol/2);
			app->audio->SetSFXVolume(app->audio->fxVol/2);
			startPause = true;
		}

		if (currentPauseMenu == CurrentPauseMenu::Pause)
		{
			for (int i = 0; i < guisPause.count(); i++)
			{
				if (guisPause[i]) guisPause[i]->Update();
			}
		}

		if (currentPauseMenu == CurrentPauseMenu::Controls)
		{
			for (int i = 0; i < guisControls.count(); i++)
			{
				if (guisControls[i]) guisControls[i]->Update();
			}
		}

		if (currentPauseMenu == CurrentPauseMenu::Settings)
		{
			for (int i = 0; i < guisSettingsP.count(); i++)
			{
				if (guisSettingsP[i]) guisSettingsP[i]->Update();
			}
		}
	
		if (currentPauseMenu == CurrentPauseMenu::Pause)
		{
			if (app->input->usingGameController)
			{
				if ((leftYMain > 10000 || app->input->GetControllerButton(BUTTON_DOWN) == KEY_DOWN) && !AxisPress)
				{
					ControllerPos += 1;
					if (ControllerPos >= 7) ControllerPos = 0;
					AxisPress = true;
				}
				else if ((leftYMain < -10000 || app->input->GetControllerButton(BUTTON_UP) == KEY_DOWN) && !AxisPress)
				{
					ControllerPos -= 1;
					if (ControllerPos < 0) ControllerPos = 6;
					AxisPress = true;
				}
				else if (abs(leftYMain) < 1000)
				{
					AxisPress = false;
				}

				GUIButton* selectedButton = (GUIButton*)guisPause.At(ControllerPos)->data;
				selectedButton->HoverButton();
			}

			if (ResumeBUT->doAction || (ControllerPos == 0 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN) || app->input->GetControllerButton(BUTTON_B) == KEY_DOWN)
			{
				app->isPause = false;
				ResumeBUT->PressButton();
				ResumeBUT->doAction = false;
				return true;
			}

			if (ControlsBUT->doAction || (ControllerPos == 2 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN))
			{
				currentPauseMenu = CurrentPauseMenu::Controls;
				ResumeBUT->PressButton();
				ControlsBUT->doAction = false;
				return true;
			}

			if (SettingsBUT->doAction || (ControllerPos == 1 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN))
			{
				currentPauseMenu = CurrentPauseMenu::Settings;
				ResumeBUT->PressButton();
				SettingsBUT->doAction = false;
				return true;
			}

			if (SaveBUT->doAction || (ControllerPos == 3 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN))
			{
				app->events->TriggerEvent(GameEvent::SAVE_GAME);
				ResumeBUT->PressButton();
				SaveBUT->doAction = false;
				return true;
			}

			if (LoadBUT->doAction || (ControllerPos ==4 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN))
			{
				app->scene->ChangeCurrentSceneRequest(SCENES::HUB);
				ResumeBUT->PressButton();
				LoadBUT->doAction = false;
				return true;
			}

			if (GiveUpBUT->doAction || (ControllerPos == 5 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN))
			{
				app->scene->ChangeCurrentSceneRequest(HUB);//et porta al hall
				app->isPause = false;
				ResumeBUT->PressButton();
				GiveUpBUT->doAction = false;
				return true;
			}

			if (QuitBUT->doAction || (ControllerPos == 6 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN))
			{
				app->scene->ChangeCurrentSceneRequest(MAIN_MENU);
				app->isPause = false;
				ResumeBUT->PressButton();
				QuitBUT->doAction = false;
				return true;
			}
		}

		if (currentPauseMenu == CurrentPauseMenu::Settings)
		{
			if (app->input->usingGameController)
			{
				if ((leftYOptions > 10000 || app->input->GetControllerButton(BUTTON_DOWN) == KEY_DOWN) && !AxisPress)
				{
					ControllerPosOpY += 1;
					if (ControllerPosOpY > 3) ControllerPosOpY = 0;
					AxisPress = true;
				}
				else if ((leftYOptions < -10000 || app->input->GetControllerButton(BUTTON_UP) == KEY_DOWN) && !AxisPress)
				{
					ControllerPosOpY -= 1;
					if (ControllerPosOpY < 0) ControllerPosOpY = 3;
					AxisPress = true;
				}
				else if (abs(leftYOptions) < 1000)
				{
					AxisPress = false;
				}

				switch (ControllerPosOpY)
				{
				case 0:
					MusicBUT->HoverButton();
					break;
				case 1:
					fxBUT->HoverButton();
					break;
				case 2:
					if (!FullScreenCHK->isActive) FullScreenCHK->checkboxState = CheckboxState::FOCUS;
					break;
				case 3:
					CloseSettingsBUT->HoverButton();
					break;
				}
			}

			if (app->input->usingGameController)
			{
				// Music controller with Gamepad
				if (ControllerPosOpY == 0 && leftXOptions > 10000)
				{
					MusicSlider->SetValue(MusicSlider->GetValue() + 0.01f);
				}
				if (ControllerPosOpY == 0 && leftXOptions < -10000)
				{
					MusicSlider->SetValue(MusicSlider->GetValue() - 0.01f);
				}

				// SFX controller with Gamepad
				if (ControllerPosOpY == 1 && leftXOptions > 10000)
				{
					fxSlider->SetValue(fxSlider->GetValue() + 0.01f);
				}
				if (ControllerPosOpY == 1 && leftXOptions < -10000)
				{
					fxSlider->SetValue(fxSlider->GetValue() - 0.01f);
				}
			}

			if (CloseSettingsBUT->doAction || (ControllerPosOpY == 3 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN) || app->input->GetControllerButton(BUTTON_B) == KEY_DOWN)
			{
				currentPauseMenu = CurrentPauseMenu::Pause;
				ControllerPosOpY = 0;
				CloseSettingsBUT->doAction = false;
			}

			if ((FullScreenCHK->isActive || (ControllerPosOpY == 2 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN)) && !app->FullScreenDesktop)
			{
				app->window->ToggleFullScreen(true);
				FullScreenCHK->ChangeState(true);
			}
			else if ((!FullScreenCHK->isActive || (ControllerPosOpY == 2 && app->input->GetControllerButton(BUTTON_A) == KEY_DOWN)) && app->FullScreenDesktop)
			{
				app->window->ToggleFullScreen(false);
				FullScreenCHK->ChangeState(false);
			}

			app->audio->SetMusicVolume(MusicSlider->GetValue() * 255);
			app->audio->SetSFXVolume(fxSlider->GetValue() * 255);
		}

		if (currentPauseMenu == CurrentPauseMenu::Controls)
		{
			if (CloseControlsBUT->doAction || app->input->GetControllerButton(BUTTON_A) == KEY_DOWN || app->input->GetControllerButton(BUTTON_B) == KEY_DOWN)
			{
				currentPauseMenu = CurrentPauseMenu::Pause;
				CloseControlsBUT->doAction = false;
				return true;
			}
		}
	}

	UpdatePlayerHp();

	return true;
}

bool HUDInGame::PostUpdate()
{
	// Player Hp
	app->renderer->AddRectRenderQueue(playerHp.bg, playerHp.bgColor, false, 4, 3.5f, 0.0f);
	app->renderer->AddRectRenderQueue(playerHp.delayHp, playerHp.hpDelayColor, true, 4, 2.5f, 0.0f);
	app->renderer->AddRectRenderQueue(playerHp.currentHp, playerHp.hpColor, true, 4, 3.0f, 0.0f);

	app->renderer->AddRectRenderQueue(miniMap, { 155, 155, 155, 255 }, false, 3, 2.0f, 0.0f);

	// Draw an empty rectangle for every abailable spell slot space
	for (int i = 0; i < player->availableSpellSlots; i++)
	{
		// Get position from spellSlotsPosition[numberOfSlots][currentSlot] (Initialized at InitializeSpellSlotsPositions())
		app->renderer->AddRectRenderQueue(spellSlotsPositions[player->availableSpellSlots-1][i], { 155, 155, 155, 255 }, false, 3, 2.0f, 0.0f);
		// Get current Spell section from GetSpellSection()
		spellSlots[i].section = GetSpellSection(i, false);
		// Draw Spell icon
		app->renderer->AddRenderObjectRenderQueue(spellSlots[i]);
	}

	// Draw a spell for every available deck slots
	for (int i = 0; i < player->availableDeckSlots; i++)
	{
		// Get spell section from GetSpellSection()
		deckSlots[i].section = GetSpellSection(i, true);
		// Draw spell slot
		app->renderer->AddRenderObjectRenderQueue(deckSlots[i]);
	}

	// Draw red rectangle on spellSlotsPosition[numberOfSlots][player->selectedSpell] position
	app->renderer->AddRectRenderQueue(spellSlotsPositions[player->availableSpellSlots - 1][player->selectedSpell], { 255, 0, 0, 255 }, false, 3, 3.0f, 0.0f);
	
	// Get selected spell name and current uses.
	std::string selectedSpellText = GetSpellName(player->spellSlots[player->selectedSpell]->id) + " x" + std::to_string(player->spellSlots[player->selectedSpell]->uses);
	// If we dont have an empty spell slot, we set Text
	if (player->spellSlots[player->selectedSpell]->id != SpellID::NONE) currentSpellText->SetText(selectedSpellText);
	else currentSpellText->SetText(""); // Else, clear current text

	if (app->isPause)
	{
		if (currentPauseMenu == CurrentPauseMenu::Pause)
		{
			for (int i = 0; i < guisPause.count(); i++)
			{
				if (guisPause[i]) guisPause[i]->PostUpdate();
			}
		}

		if (currentPauseMenu == CurrentPauseMenu::Controls)
		{
			for (int i = 0; i < guisControls.count(); i++)
			{
				if (guisControls[i]) guisControls[i]->PostUpdate();
			}
		}

		if (currentPauseMenu == CurrentPauseMenu::Settings)
		{
			for (int i = 0; i < guisSettingsP.count(); i++)
			{
				if (guisSettingsP[i]) guisSettingsP[i]->PostUpdate();
			}
		}

		if (currentPauseMenu == CurrentPauseMenu::Pause)
		{
			app->renderer->AddRenderObjectRenderQueue(PauseBG);
		}

		if (currentPauseMenu == CurrentPauseMenu::Controls)
		{
			if(app->input->usingGameController)
				app->renderer->AddRenderObjectRenderQueue(Controls2);
			else
				app->renderer->AddRenderObjectRenderQueue(Controls1);
		}

		if (currentPauseMenu == CurrentPauseMenu::Settings)
		{
			app->renderer->AddRenderObjectRenderQueue(SettingsBG);
		}
	}

	app->renderer->AddRenderObjectRenderQueue(iconSouls);

	/*app->renderer->AddRenderObjectRenderQueue(iconSpells);
	app->renderer->AddRenderObjectRenderQueue(deckSpells1);
	app->renderer->AddRenderObjectRenderQueue(deckSpells2);*/

	Scene::PostUpdate();

	return true;
}

bool HUDInGame::CleanUp()
{
	app->events->RemoveListener(this);

	Scene::CleanUp();

	return true;
}

void HUDInGame::GameEventTriggered()
{
	// Call when player Hit

	float percent = (float)player->player->hpPlayer / (float)player->player->hpMax;

	float hp = (playerHp.bg.w * percent);

	playerHp.currentHp.w = (int)hp;
}

void HUDInGame::SetPlayerCombat(PlayerCombat* playerC)
{
	player = playerC;
}

void HUDInGame::UpdatePlayerHp()
{
	do
	{
		if (playerHp.delayHp.w < playerHp.currentHp.w) break;

		playerHp.startDelay--;

		if (playerHp.startDelay > 0) break;

		if (playerHp.countDelay <= 0)
		{
			playerHp.delayHp.w--;

			playerHp.countDelay = playerHp.maxCountDelay;
		}
		else
		{
			playerHp.countDelay -= playerHp.delaySpeed;

			if (playerHp.delayHp.w > playerHp.currentHp.w) break;

			playerHp.startDelay = playerHp.MaxStartDelay;
		}

	} while (false);
}

void HUDInGame::InitializeSlots()
{
	// Create a render Object per available spell slot
	for (int i = 0; i < player->availableSpellSlots; i++)
	{
		RenderObject rO;
		iPoint pos = { spellSlotsPositions[player->availableSpellSlots - 1][i].x, spellSlotsPositions[player->availableSpellSlots - 1][i].y };
		rO.InitAsTexture(app->textures->Load("Assets/Sprites/UI/icons.png"), pos, { 0,0,0,0 }, 1, 3, 0, 0, SDL_FLIP_NONE, 0);
		spellSlots.add(rO);
	}

	// Create a render object per available deck slot
	for (int i = 0; i < player->availableDeckSlots; i++)
	{
		RenderObject rO;
		// Get position from last spell slot position
		iPoint spellPos = { spellSlotsPositions[player->availableSpellSlots - 1].end->data.x, spellSlotsPositions[player->availableSpellSlots - 1].end->data.y };
		// Add an offset to that position based on the current iteration
		iPoint position = spellPos + iPoint(40, 0);
		position.x += 20 * i;
		position.y = 340;
		rO.InitAsTexture(app->textures->Load("Assets/Sprites/UI/icons.png"), position, { 0,0,0,0 }, 0.5f, 3, 0, 0, SDL_FLIP_NONE, 0);
		deckSlots.add(rO);
	}
}

void HUDInGame::InitializeSpellSlotsPositions()
{
	// This is an array of Lists, where we keep the desired position of every spell slot instead of creating an algorythm
	// To understand this, keep in mind:
	// spellSlotsPositions[numberOfSlots][slot]
	// Example: spellSlotsPositions[2Slots][slot1] = There are 2 available slots, and we are refering to the first one
	
	spellSlotsPositions[0].add({ 304, 320, 30, 40 }); // 1 Available: Refering to slot 1

	spellSlotsPositions[1].add({280, 320, 30, 40 });// 2 Available: Refering to slot 1
	spellSlotsPositions[1].add({ 320, 320 , 30, 40 });// 2 Available: Refering to slot 2

	spellSlotsPositions[2].add({ 320, 320 , 30, 40 });// 3 Available: Refering to slot 1
	spellSlotsPositions[2].add({ 320, 320, 30, 40 });// 3 Available: Refering to slot 2
	spellSlotsPositions[2].add({ 320, 320 , 30, 40 });// 3 Available: Refering to slot 3

	spellSlotsPositions[3].add({ 320, 320 , 30, 40 });// 4 Available: Refering to slot 1
	spellSlotsPositions[3].add({ 320, 320 , 30, 40 });// 4 Available: Refering to slot 2
	spellSlotsPositions[3].add({ 320, 320 , 30, 40 });// 4 Available: Refering to slot 3
	spellSlotsPositions[3].add({ 320, 320 , 30, 40 });// 4 Available: Refering to slot 4
}

SDL_Rect HUDInGame::GetSpellSection(int slot, bool isDeck)
{
	SDL_Rect sect;

	int spellSlot;

	if (isDeck) spellSlot = (int)player->deckSlots[slot]->id;
	else spellSlot = (int)player->spellSlots[slot]->id;

	switch (spellSlot)
	{
	case (int)SpellID::PURIFIED_SWORD:
		sect = { 31,0,31,31 };
		break;
	case (int)SpellID::SOUL_SHIELD:
		sect = { 31,31,31,31 };
		break;
	case (int)SpellID::PURIFICATION_SLASH:
		sect = { 0,31,31,31 };
		break;
	case (int)SpellID::EKRISKI:
		sect = { 61,0,31,31 };
		break;
	case (int)SpellID::FOTEIROS:
		sect = { 0,0,31,31 };
		break;
	default:
		sect = { 300,300,3,3 };
	}
	return sect;
}

std::string HUDInGame::GetSpellName(SpellID id)
{
	std::string text = "";
	switch (id)
	{
	case SpellID::PURIFIED_SWORD:
		text = "Purified Sword";
		break;
	case SpellID::SOUL_SHIELD:
		text = "Soul Shield";
		break;
	case SpellID::PURIFICATION_SLASH:
		text = "Purification Slash";
		break;
	case SpellID::EKRISKI:
		text = "Ekriski";
		break;
	case SpellID::FOTEIROS:
		text = "Foteiros";
		break;
	}
	return text;
}
