#ifndef UNRUIN_PLAYER_H_
#define UNRUIN_PLAYER_H_

#include "classes/actor.h"
#include "core/smart_properties.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/node_path.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "pistol.h"
#include "player_movement.h"
#include <cstdint>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/capsule_shape3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/shape3d.hpp>

using namespace godot;

namespace unruin
{

// TODO: Add melee, guns, and "enemies" for the Player to fight.
class Player : public Actor
{
	GDCLASS(Player, Actor)

public:
	Player() = default;

private:
	Ref<PlayerMovement> player_movement;
	NodePath camera_node_path;
	NodePath collider_node_path;
	NodePath camera_rig_node_path;
	Ref<CapsuleShape3D> capsule_shape;
	// FIXME: These pointers are unsafe and shouldn't exist as class members.
	//        Use NodePaths and create these pointers in a function's scope,
	//        then validate.
	//        (Fix by converting symbol references to method calls?)
	Camera3D *camera;
	Node3D *camera_rig;
	CollisionShape3D *collider;

	float max_height = 0.0f;
	uint64_t pistol_instance_id = 0;

	SP_BEGIN_PROPERTIES
	SP_PROPERTY(camera_node_path, PropertyInfo(Variant::NODE_PATH, "camera_node_path"))
	SP_PROPERTY(collider_node_path, PropertyInfo(Variant::NODE_PATH, "collider_node_path"))
	SP_PROPERTY(camera_rig_node_path, PropertyInfo(Variant::NODE_PATH, "camera_rig_node_path"))
	SP_END_PROPERTIES

	void look(Vector2 mouseDelta);

protected:
	static void _bind_methods() {}

public:
	void _ready() override;
	void _process(double delta) override;
	void _physics_process(double delta) override;
	void _input(const Ref<InputEvent> &event) override;
	void _notification(int p_what);

	float get_max_height();
	void attack();
	// Adds a weapon's NodePath to the Player's references;
	// The pointer for the weapon parameter is just for type safety.
	void add_weapon(uint64_t p_weapon_instance_id); // TODO: Replace parameter with generic weapon class.
	friend PlayerMovement;
};

} //namespace unruin

#endif // UNRUIN_PLAYER_H_