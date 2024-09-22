#include "pistol.h"
#include "godot_cpp//classes/scene_tree.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/physics_body3d.hpp"
#include "godot_cpp/classes/physics_direct_space_state3d.hpp"
#include "godot_cpp/classes/physics_ray_query_parameters3d.hpp"
#include "godot_cpp/classes/physics_server2d.hpp"
#include "godot_cpp/classes/physics_server3d.hpp"
#include "godot_cpp/classes/ray_cast3d.hpp"
#include "godot_cpp/classes/world3d.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "player.h"

using namespace unruin;

void Pistol::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_PARENTED:
		{
			Player *player = Object::cast_to<Player>(get_parent());
			if (player)
			{
				player->add_weapon(get_instance_id());
				owner = player;
			}
		}
		case NOTIFICATION_UNPARENTED:
			update_configuration_warnings();
			break;
	}
}

PackedStringArray Pistol::_get_configuration_warnings() const
{
	PackedStringArray warnings;
	if (!get_parent()->is_class("Player"))
	{
		warnings.append(get_class() + " is a player specific weapon. Please parent under a Player node.");
	}
	return warnings;
}

void Pistol::attack(Vector3 origin, Vector3 direction, TypedArray<RID> owner_collision)
{
	// Get the raycast
	PhysicsDirectSpaceState3D *space_state;
	space_state = PhysicsServer3D::get_singleton()->space_get_direct_state(owner->get_world_3d()->get_space());

	Ref<PhysicsRayQueryParameters3D> ray_parameters;
	ray_parameters = PhysicsRayQueryParameters3D::create(origin, origin + (direction.normalized() * max_range));

	owner_collision.append(owner->get_rid()); // Don't shoot ourselves.
	ray_parameters->set_exclude((owner_collision));

	Dictionary result = space_state->intersect_ray(ray_parameters);

	if (!result.is_empty())
	{
		PhysicsBody3D *collider = Object::cast_to<PhysicsBody3D>(ObjectDB::get_instance(result["collider_id"]));
		if (collider != nullptr)
		{
			UtilityFunctions::print("Shot something! ", collider->get_name());
		}
	}
}