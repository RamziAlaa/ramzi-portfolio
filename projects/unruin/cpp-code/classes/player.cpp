#include "player.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/object_id.hpp"
#include "godot_cpp/variant/node_path.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "player_movement.h"
#include "settings.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
using namespace unruin;

void Player::look(Vector2 mouseDelta)
{
	rotate_y(Math::deg_to_rad(-mouseDelta.x));

	Vector3 camera_rotation_degrees = camera->get_rotation_degrees();
	camera_rotation_degrees.x = Math::clamp<float>(
			camera_rotation_degrees.x + -mouseDelta.y,
			-90.0f,
			90.0f);
	camera->set_rotation_degrees(camera_rotation_degrees);
}

void Player::_ready()
{
	if (Engine::get_singleton()->is_editor_hint())
	{
		return;
	}
	if (player_movement.is_null())
	{
		UtilityFunctions::print("PlayerMovement not defined in inspector. Instantiating...");
		player_movement.instantiate();
	}
	camera = get_node<Camera3D>(camera_node_path);
	camera_rig = get_node<Node3D>(camera_rig_node_path);
	collider = get_node<CollisionShape3D>(collider_node_path);
	capsule_shape = collider->get_shape();
	max_height = capsule_shape->get_height();

	player_movement->initialize(this);

	// TODO: Move this to a more reasonable class.
	Input::get_singleton()->set_mouse_mode(Input::MouseMode::MOUSE_MODE_CAPTURED);
}

void Player::_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint())
	{
		return;
	}
	if (Input::get_singleton()->is_action_pressed("attack"))
	{
		attack();
	}
	// UtilityFunctions::print("Memory (bytes): ", OS::get_singleton()->get_static_memory_usage());
}

void Player::_physics_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint())
	{
		return;
	}
	player_movement->update(delta);
	move_and_slide();
}

void Player::_input(const Ref<InputEvent> &event)
{
	if (event->is_class("InputEventMouseMotion"))
	{
		const InputEventMouseMotion *mouse_event = cast_to<InputEventMouseMotion>(event.ptr());
		look(mouse_event->get_relative() * Settings::mouse_sensitivity);
	}
	player_movement->handle_controls(event);
}

void Player::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_CHILD_ORDER_CHANGED:
		{
			// Check if any weapons have been removed.
			Pistol *pistol = Object::cast_to<Pistol>(ObjectDB::get_instance(pistol_instance_id));
			if (pistol == nullptr || pistol->get_parent() != this)
			{
			    // Weapon was removed. Take action.
			    UtilityFunctions::print("Weapon with id ", pistol_instance_id, " was removed.");
				pistol_instance_id = 0;
			}
		}
	}
}

void Player::attack()
{
	Pistol *pistol = Object::cast_to<Pistol>(ObjectDB::get_instance(pistol_instance_id));
	if (pistol == nullptr)
	{
		UtilityFunctions::printerr("Attempted to attack without weapon.");
		return;
	}
	pistol->attack(camera->get_global_position(),
			camera->get_global_basis().xform(Vector3(0, 0, -1)));
}

float Player::get_max_height()
{
	return max_height;
}

void Player::add_weapon(uint64_t p_weapon_instance_id)
{
	pistol_instance_id = p_weapon_instance_id;
	Pistol *pistol = Object::cast_to<Pistol>(ObjectDB::get_instance(pistol_instance_id));
	if (pistol == nullptr)
	{
		UtilityFunctions::printerr("Player::add_weapon recieved invalid instance id to weapon child.");
		return;
	}
	UtilityFunctions::print("Weapon added! ", pistol->get_name());
}