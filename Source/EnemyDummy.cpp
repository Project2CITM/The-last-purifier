#include "EnemyDummy.h"
#include "ModulePhysics.h"

EnemyDummy::EnemyDummy(iPoint pos) : Enemy()
{
	pBody = app->physics->CreateRectangle(pos, 10, 14, this,b2_kinematicBody);
	name = "Dummy";
}
