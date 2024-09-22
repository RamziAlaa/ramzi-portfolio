#include "player_movement.h"
#include "player.h"
#include "settings.h"
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/capsule_shape3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/shape3d.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace unruin;

PlayerMovement::PlayerMovement() :
		crouch_acceleration(ground_acceleration * 0.5f)
{
	current_state = std::unique_ptr<MovementState>(new NormalState(this));
}

PlayerMovement::~PlayerMovement()
{
	if (current_state != nullptr)
	{
		current_state->exit();
	}
}

void PlayerMovement::initialize(Player *p_player)
{
	player = p_player;
	camera_rig_default_position = player->camera_rig->get_position();
	current_state->enter();
}

void PlayerMovement::handle_controls(const Ref<InputEvent> &event)
{
	current_state->get_input(event);
}

void PlayerMovement::update(double delta)
{
	Vector2 direction_2d = Input::get_singleton()->get_vector("move_left", "move_right", "move_forward", "move_backward");
	wish_move = !direction_2d.is_zero_approx();
	local_move_direction = Vector3(direction_2d.x, 0, direction_2d.y);
	global_move_direction = player->get_transform().basis.xform(local_move_direction);
	velocity = player->get_velocity();

	current_state->process_movement(delta);
	// Switch state
	if (queued_state != nullptr)
	{
		current_state->exit();
		previous_state_type = current_state->get_type();
		next_state_type = MovementType::None;

		current_state = std::unique_ptr<MovementState>(queued_state);
		queued_state = nullptr;

		current_state->enter();
	}
	player->set_velocity(velocity);
}

template <typename T>
void PlayerMovement::queue_movement_state()
{
	queued_state = new T(this);
	next_state_type = queued_state->get_type();
}

void PlayerMovement::accelerate(Vector3 &velocity, float stop_speed, float acceleration)
{
	Vector3 move_to = global_move_direction * stop_speed;
	move_to.y = velocity.y;
	velocity = velocity.move_toward(move_to, acceleration);
	//UtilityFunctions::print(velocity, " | ", velocity.length());
}

PlayerMovement::MovementState::MovementState(PlayerMovement *p_player_movement)
{
	pm = p_player_movement;
}

void PlayerMovement::MovementState::apply_gravity(double delta)
{
	if (!pm->player->is_on_floor())
	{
		pm->velocity.y -= pm->gravity * delta;
	}
}

void PlayerMovement::NormalState::get_input(const Ref<InputEvent> &event)
{
	if (event->is_class("InputEventKey"))
	{
		const InputEventKey *key_event = pm->player->cast_to<InputEventKey>(event.ptr());
		// Wishes
		wish_crouch = wish_crouch ? !key_event->is_action_released("crouch") : key_event->is_action_pressed("crouch");
	}
}

void PlayerMovement::NormalState::process_movement(double delta)
{
	apply_gravity(delta);

	// Crouching
	if (wish_crouch)
	{
		pm->queue_movement_state<CrouchingState>();
		return;
	}

	if (pm->player->is_on_floor())
	{
		if (Input::get_singleton()->is_action_pressed("roll"))
		{
			pm->queue_movement_state<RollingState>();
			return;
		}

		// Grounded movement
		pm->accelerate(pm->velocity, pm->stop_speed, pm->ground_acceleration);
	}
	else
	{
		if (!pm->global_move_direction.is_zero_approx())
		{
			pm->accelerate(pm->velocity, pm->stop_speed, pm->air_acceleration);
		}
	}
}

void PlayerMovement::CrouchingState::enter()
{
	SceneTree *tree = pm->player->get_tree();

	if (pm->player->is_on_floor())
	{
		tween = tree->create_tween()->set_trans(Tween::TRANS_QUAD)->set_parallel();
		tween->tween_property(pm->player->capsule_shape.ptr(), "height", pm->player->max_height * 0.5f, 0.2f);
		tween->tween_property(pm->player->camera_rig, "position", Vector3(0, 0, 0), 0.2f);
	}
	else
	{
		pm->player->set_global_position(pm->player->camera_rig->get_global_position());
		pm->player->camera_rig->set_position(Vector3(0, 0, 0));
		pm->player->capsule_shape->set_height(pm->player->max_height * 0.5f);
	}
}

void PlayerMovement::CrouchingState::exit()
{
	if (is_skipping_exit_tween)
	{
		return;
	}

	SceneTree *tree = pm->player->get_tree();
	Ref<Tween> exit_tween = tree->create_tween()->set_trans(Tween::TRANS_QUAD)->set_parallel();
	exit_tween->tween_property(pm->player->collider, "position", Vector3(0, 0, 0), 0.2f);
	exit_tween->tween_property(pm->player->capsule_shape.ptr(), "height", pm->player->max_height, 0.2f);
	exit_tween->tween_property(pm->player->camera_rig, "position", pm->camera_rig_default_position, 0.2f);

	tween.~Ref();
}

void PlayerMovement::CrouchingState::get_input(const Ref<InputEvent> &event)
{
	if (event->is_class("InputEventKey"))
	{
		const InputEventKey *key_event = pm->player->cast_to<InputEventKey>(event.ptr());
		// Wishes
		wish_crouch = wish_crouch ? !key_event->is_action_released("crouch") : key_event->is_action_pressed("crouch");
	}
}

void PlayerMovement::CrouchingState::process_movement(double delta)
{
	UtilityFunctions::print("Shape Height: ", pm->player->capsule_shape->get_height(), " | Collider Position: ", pm->player->collider->get_position(), " | Camera Position: ", pm->player->camera_rig->get_position(), " | Player Position", pm->player->get_position());

	apply_gravity(delta);

	// Crouching
	if (!wish_crouch)
	{
		pm->queue_movement_state<NormalState>();
		return;
	}

	if (pm->player->is_on_floor())
	{
		if (Input::get_singleton()->is_action_just_pressed("roll"))
		{
			is_skipping_exit_tween = true;
			pm->queue_movement_state<RollingState>();
			return;
		}

		pm->accelerate(pm->velocity, pm->crouch_stop_speed, pm->crouch_acceleration);
	}
	else
	{
		if (!pm->global_move_direction.is_zero_approx())
		{
			pm->accelerate(pm->velocity, pm->stop_speed, pm->air_acceleration);
		}
	}
}

// FIXME: Roll doesn't start with inital velocity as expected.
void PlayerMovement::RollingState::enter()
{
	roll_animation_time = pm->max_roll_time * 0.33f;
	start_roll_animation_time = Math::clamp<float>(roll_animation_time * 1.5f, 0.0f, pm->max_roll_time - roll_animation_time);
	roll_control_acceleration = pm->crouch_acceleration * 0.5f;
	Vector3 roll_direction;

	if (!pm->wish_move)
	{
		// Roll forward if no direction was inputted.
		roll_direction = pm->player->get_transform().basis.xform(Vector3(0, 0, -1));
	}
	else
	{
		roll_direction = pm->global_move_direction;
	}

	// Start the roll
	pm->velocity.x = roll_direction.x * pm->roll_speed;
	pm->velocity.z = roll_direction.z * pm->roll_speed;

	SceneTree *tree = pm->player->get_tree();
	if (pm->previous_state_type != MovementType::Crouching || pm->player->camera_rig->get_position() != Vector3(0, 0, 0))
	{
		displacement_tween = tree->create_tween()->set_trans(Tween::TRANS_QUAD)->set_parallel();
		//displacement_tween->tween_property(pm->player, "position", Vector3(0, 0, -pm->player->max_height * 0.5f), start_roll_animation_time)->as_relative();
		displacement_tween->tween_property(pm->player->capsule_shape.ptr(), "height", pm->player->max_height * 0.5f, start_roll_animation_time);
		displacement_tween->tween_property(pm->player->camera_rig, "position", Vector3(0, 0, 0), start_roll_animation_time);
	}
	// TODO: The camera should probably be animated for this.
	// // Tilt the camera
	// Vector3 tilt_rotation = pm->local_move_direction.is_zero_approx() ? Vector3(0, 0, -1) : pm->local_move_direction;
	// tilt_rotation = Vector3(tilt_rotation.z, 0, -tilt_rotation.x) * (Math_TAU / 72);
	// tilt_tween = tree->create_tween()->set_trans(Tween::TRANS_QUAD)->set_ease(Tween::EASE_IN_OUT);
	// tilt_tween->tween_property(pm->player->camera_rig, "rotation", tilt_rotation, start_roll_animation_time * 0.4f)->as_relative();
	// tilt_tween->set_ease(Tween::EASE_IN_OUT)->set_trans(Tween::TRANS_SINE);
	// tilt_tween->tween_property(pm->player->camera_rig, "rotation", -tilt_rotation, pm->max_roll_time)->as_relative();
}

void PlayerMovement::RollingState::exit()
{
	if (pm->next_state_type != MovementType::Crouching)
	{
		if (!displacement_tween.is_null() && displacement_tween->is_valid())
		{
			displacement_tween->kill();
			pm->player->camera_rig->set_position(pm->camera_rig_default_position);
			pm->player->capsule_shape->set_height(pm->player->max_height);
			pm->player->collider->set_position(Vector3(0, 0, 0));
		}
		if (!tilt_tween.is_null() && tilt_tween->is_valid())
		{
			tilt_tween->kill();
			Vector3 rotation = pm->player->camera_rig->get_rotation_degrees();
			rotation.z = 0;
			pm->player->camera_rig->set_rotation_degrees(rotation);
		}
	}

	displacement_tween.~Ref();
	tilt_tween.~Ref();
}

void PlayerMovement::RollingState::process_movement(double delta)
{
	elapsed_time += delta;
	if (is_exiting_roll)
	{
		pm->accelerate(pm->velocity, pm->stop_speed, pm->crouch_acceleration);
		if (displacement_tween.is_null() || !displacement_tween->is_valid())
		{
			pm->queue_movement_state<NormalState>();
		}
		return;
	}
	if (elapsed_time >= pm->max_roll_time)
	{
		if (Input::get_singleton()->is_action_pressed("crouch"))
		{
			pm->queue_movement_state<CrouchingState>();
			return;
		}

		// We're done here. Start getting up.
		is_exiting_roll = true;
		if (!displacement_tween.is_null() && displacement_tween->is_valid())
		{
			displacement_tween->kill();
		}
		displacement_tween = pm->player->get_tree()->create_tween()->set_trans(Tween::TRANS_SINE)->set_ease(Tween::EASE_OUT)->set_parallel();
		displacement_tween->tween_property(pm->player->collider, "position", Vector3(0, 0, 0), roll_animation_time);
		displacement_tween->tween_property(pm->player->capsule_shape.ptr(), "height", pm->player->max_height, roll_animation_time);
		displacement_tween->tween_property(pm->player->camera_rig, "position", pm->camera_rig_default_position, roll_animation_time);
		return;
	}

	// Give the player at least some control when rolling.
	if (pm->wish_move)
	{
		pm->accelerate(pm->velocity, pm->roll_speed, roll_control_acceleration);
	}

	apply_gravity(delta);
}

void PlayerMovement::DeadState::process_movement(double delta)
{
	// Dead people don't move
	pm->velocity = pm->velocity.move_toward(Vector3(0, pm->velocity.y, 0), pm->ground_acceleration);
}