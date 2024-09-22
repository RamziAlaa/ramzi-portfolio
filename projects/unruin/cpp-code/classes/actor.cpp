#include "classes/actor.h"

using namespace godot;
using namespace unruin;

void Actor::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_entity_stats"), &Actor::get_entity_stats);
	ClassDB::bind_method(D_METHOD("set_entity_stats", "entity_stats"), &Actor::set_entity_stats);
	ClassDB::add_property("Actor", PropertyInfo(Variant::OBJECT, "Entity Stats"), "set_entity_stats", "get_entity_stats");
}