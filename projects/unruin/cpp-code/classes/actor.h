#ifndef UNRUIN_ACTOR_H_
#define UNRUIN_ACTOR_H_

#include "classes/entity_stats.h"
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/ref.hpp>

using namespace godot;

namespace unruin
{

class Actor : public CharacterBody3D
{
	GDCLASS(Actor, CharacterBody3D)

public:
	Actor() = default;

private:
	Ref<EntityStats> entity_stats;

protected:
	static void _bind_methods();
	void set_entity_stats(EntityStats *entity_stats) { this->entity_stats = Ref<EntityStats>(entity_stats); };

public:
	Ref<EntityStats> get_entity_stats() { return entity_stats; }
};

} //namespace unruin

#endif // UNRUIN_ACTOR_H_