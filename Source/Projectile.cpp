#include "Projectile.h"
#include "ModulePhysics.h"
#include "DamageArea.h"
#include "ParticleAttackSage.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "Particle.h"
#include "ParticleHitSage.h"
#include "PaticleHitFotieros.h"

Projectile::Projectile(std::string name, iPoint position, fPoint duration, int damage,int rotation, int width, int height, bool fire, bool stun, bool isEnemy) : GameObject(name, name)
{
	this->damage = damage;
	this->isEnemy = isEnemy;
	this->stun = stun;
	this->rotation = rotation;

	damageArea = new DamageArea(position, width, height, this->damage);
	pBody = app->physics->CreateRectangle(position, width, height, this);
	b2Filter filter;
	filter.categoryBits = app->physics->PROJECTILE_LAYER;

	if (isEnemy) filter.maskBits = app->physics->EVERY_LAYER & ~app->physics->ENEMY_LAYER & ~app->physics->PROJECTILE_LAYER & ~app->physics->TRIGGER_LAYER;
	else filter.maskBits = app->physics->EVERY_LAYER & ~app->physics->PLAYER_LAYER & ~app->physics->PROJECTILE_LAYER & ~app->physics->TRIGGER_LAYER;

	pBody->body->GetFixtureList()[0].SetFilterData(filter);
	damageArea->pBody->body->GetFixtureList()[0].SetFilterData(filter);

	if (fire) FireProjectile(duration);

	if (rotation == 180)
	{
		renderObjects[0].flip = SDL_FLIP_HORIZONTAL;
	}
	else
	{
		renderObjects[0].flip = SDL_FLIP_NONE;
	}

	float projectileScale = (float)(width + height) / 8.0f;

	renderObjects[0].InitAsTexture(app->textures->Load("Sprites/Player/Sage/basicAttackSageDuring.png"), position, { 0,0,0,0 }, projectileScale,1,1.0f,this->rotation);

	
	// Particle Effect

	for (int i = 0; i < 8; i++)
	{
		this->anim.PushBack({ 32 * i,0,32,32 });
	}
	this->anim.loop = true;
	this->anim.hasIdle = false;
	this->anim.duration = 0.032f;

	spriteOffset[0] = { (int)(-20 * projectileScale),(int)(-15 * projectileScale) };
	
}

/// <summary>
/// Foteiros Projectile!!
/// </summary>
/// <param name="position"></param>
/// <param name="damage"></param>
/// <param name="rotation"></param>
/// <param name="speed"></param>
Projectile::Projectile(iPoint position, int damage, int rotation, fPoint duration) : GameObject("Foteiros", "Foteiros")
{
	this->damage = damage;
	this->rotation = rotation;
	this->isFoteiros = true;

	damageArea = new DamageArea(position, 6, 6, this->damage);
	pBody = app->physics->CreateRectangle(position, 6, 6, this);
	b2Filter filter;
	filter.categoryBits = app->physics->PROJECTILE_LAYER;

	if (isEnemy) filter.maskBits = app->physics->EVERY_LAYER & ~app->physics->ENEMY_LAYER & ~app->physics->PROJECTILE_LAYER & ~app->physics->TRIGGER_LAYER;
	else filter.maskBits = app->physics->EVERY_LAYER & ~app->physics->PLAYER_LAYER & ~app->physics->PROJECTILE_LAYER & ~app->physics->TRIGGER_LAYER;

	pBody->body->GetFixtureList()[0].SetFilterData(filter);
	damageArea->pBody->body->GetFixtureList()[0].SetFilterData(filter);

	FireProjectile(duration);

	renderObjects[0].InitAsTexture(app->textures->Load("Sprites/Player/Sage/foteiros46x32.png"), position, { 0,0,0,0 }, 1.0f, 1, 1.0f, this->rotation);

	if (rotation == 0)
	{
		renderObjects[0].flip = SDL_FLIP_HORIZONTAL;
	}
	else if (rotation == 180)
	{
		renderObjects[0].flip = SDL_FLIP_NONE;
		renderObjects[0].rotation = 0;
	}
	else if (rotation == 270)
	{
		renderObjects[0].rotation = 90;
	}
	else 
	{
		renderObjects[0].rotation = 270;
	}

	// Particle Effect

	for (int i = 0; i < 4; i++)
	{
		this->anim.PushBack({ 46 * i,0,46,32 });
	}
	this->anim.loop = true;
	this->anim.hasIdle = false;
	this->anim.duration = 0.064f;

	spriteOffset[1] = { -40,-20 };
}

void Projectile::FireProjectile(fPoint duration)
{
	b2Vec2 s = { duration.x, duration.y };
	damageArea->pBody->body->SetLinearVelocity(s);
	pBody->body->SetLinearVelocity(s);
}

void Projectile::OnCollisionEnter(PhysBody* col)
{
	pendingToDelete = true;
	damageArea->pendingToDelete = true;

	// Particle effect

	if (!isFoteiros)new ParticleHitSage({ position.x - 20,position.y - 15 }, 1.5f);
	else new PaticleHitFotieros({ position.x - 20,position.y - 15 }, 0.1f);

}

void Projectile::PostUpdate()
{
	anim.Update();

	renderObjects[0].section = anim.GetCurrentFrame();

	renderObjects[0].destRect.x = isFoteiros ? GetDrawPosition().x + spriteOffset[1].x : GetDrawPosition().x + spriteOffset[0].x;
	renderObjects[0].destRect.y = isFoteiros ? GetDrawPosition().y + spriteOffset[1].y : GetDrawPosition().y + spriteOffset[0].y;

	
	app->renderer->AddRenderObjectRenderQueue(renderObjects[0]);

}
