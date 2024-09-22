#ifndef UNRUIN_PISTOL_HPP_INCLUDED
#define UNRUIN_PISTOL_HPP_INCLUDED

#include "godot_cpp/classes/character_body3d.hpp"
#include "godot_cpp/classes/collision_object3d.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/typed_array.hpp"

using namespace godot;

namespace unruin
{
class Pistol : public Node
{
	GDCLASS(Pistol, Node)

	const int max_range = 150;
	CharacterBody3D *owner;

protected:
	static void _bind_methods() {}

public:
	void _notification(int p_what);
	PackedStringArray _get_configuration_warnings() const override;

	// Fires a hitscan bullet.
	// The owner's RID is already added in `excluded_collisions`.
	void attack(Vector3 origin, Vector3 direction, TypedArray<RID> excluded_collisions = TypedArray<RID>());
};
} //namespace unruin

#endif