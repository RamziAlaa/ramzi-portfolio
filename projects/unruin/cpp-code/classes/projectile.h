#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "core/smart_properties.hpp"
#include "godot_cpp/classes/area3d.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/vector3.hpp"

using namespace godot;

namespace unruin
{
class Projectile : public Area3D
{
	GDCLASS(Projectile, Area3D)

protected:
	float speed = 1.0f;
	Vector3 direction = Vector3(0, 0, 1);

	SP_BEGIN_PROPERTIES
	SP_PROPERTY(speed, PropertyInfo(Variant::FLOAT, "speed"))
	SP_PROPERTY(direction, PropertyInfo(Variant::VECTOR3, "direction"))
	SP_END_PROPERTIES

protected:
	static void _bind_methods() {}

public:
	void _ready() override;
	void _physics_process(double delta) override;
};
} //namespace unruin

#endif