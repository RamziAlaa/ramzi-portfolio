#include "projectile.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/variant/transform3d.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace unruin;

void Projectile::_ready()
{
	look_at(get_global_position() + direction);
}

void Projectile::_physics_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint())
	{
		return;
	}
	set_global_position(get_global_position() + (direction.normalized() * (speed * delta)));
}