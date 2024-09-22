#ifndef UNRUIN_ENTITY_STATS_H_
#define UNRUIN_ENTITY_STATS_H_

#include <godot_cpp/classes/resource.hpp>

using namespace godot;

namespace unruin
{

// All actors and some entities will hold this resource
// or some variation of it.
class EntityStats : public Resource
{
	GDCLASS(EntityStats, Resource)

private:
	int max_health;
	int health;
	bool is_alive;

protected:
	static void _bind_methods();

public:
	EntityStats();
	EntityStats(int max_health, int health, bool is_alive = true);
	~EntityStats();

	void set_max_health(int max_health);
	int get_max_health();
	void set_health(int health);
	int get_health();
	void set_is_alive(bool is_alive);
	bool get_is_alive();
};

} //namespace unruin

#endif //UNRUIN_ENTITY_STATS_H_