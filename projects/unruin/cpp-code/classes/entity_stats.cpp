#include "classes/entity_stats.h"

using namespace godot;
using namespace unruin;

EntityStats::EntityStats() :
		max_health(100), health(max_health), is_alive(true)
{
}

EntityStats::EntityStats(int max_health, int health, bool is_alive) :
		max_health(max_health), health(health), is_alive(is_alive)
{
}

void EntityStats::_bind_methods()
{
}

EntityStats::~EntityStats()
{
}