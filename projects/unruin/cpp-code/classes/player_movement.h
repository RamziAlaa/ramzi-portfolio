#ifndef PLAYER_MOVEMENT_H_
#define PLAYER_MOVEMENT_H_

#include "core/smart_properties.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/capsule_shape3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/shape3d.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <memory>

using namespace godot;

namespace unruin
{

class Player;

/// @brief A state machine that processes the players movement.
class PlayerMovement : public RefCounted
{
	GDCLASS(PlayerMovement, RefCounted)
public:
	enum class MovementType
	{
		None,
		Dead,
		Normal,
		Crouching,
		Rolling,
		Sliding,
		Diving,
		Prone
	};

	class MovementState
	{
	public:
		MovementState(PlayerMovement *p_player_movement);

	protected:
		PlayerMovement *pm;

		void apply_gravity(double delta);

	public:
		virtual MovementType get_type() const = 0;
		virtual void enter(){};
		virtual void exit(){};
		virtual void get_input(const Ref<InputEvent> &event){};
		virtual void process_movement(double delta) = 0;
	};

	class DeadState : public MovementState
	{
	public:
		MovementType get_type() const { return MovementType::Dead; };
		void process_movement(double delta);
	};

	class NormalState : public MovementState
	{
	public:
		NormalState(PlayerMovement *p_player_movement) :
				MovementState(p_player_movement) {}

	private:
		bool wish_crouch = false;

	public:
		MovementType get_type() const { return MovementType::Normal; };
		void get_input(const Ref<InputEvent> &event);
		void process_movement(double delta);
	};

	// Functionally the same as NormalState, but other states
	// need to account for the halved height when transitioning.
	class CrouchingState : public MovementState
	{
	public:
		CrouchingState(PlayerMovement *p_player_movement) :
				MovementState(p_player_movement) {}

	private:
		bool wish_crouch = true;
		bool is_skipping_exit_tween = false;
		Ref<Tween> tween;

	public:
		MovementType get_type() const { return MovementType::Crouching; };
		void enter();
		void exit();
		void get_input(const Ref<InputEvent> &event);
		void process_movement(double delta);
	};

	class RollingState : public MovementState
	{
	private:
		float elapsed_time = 0.0f;
		Ref<Tween> displacement_tween;
		Ref<Tween> tilt_tween;
		float roll_animation_time;
		float start_roll_animation_time;
		float roll_control_acceleration;
		bool is_exiting_roll = false;

	public:
		RollingState(PlayerMovement *p_player_movement) :
				MovementState(p_player_movement) {}
		MovementType get_type() const { return MovementType::Rolling; };
		void enter();
		void exit();
		void process_movement(double delta);
	};

	class SlidingState : public MovementState
	{
	public:
		MovementType get_type() const { return MovementType::Sliding; };
		// TODO: Not Implemented
	};

	PlayerMovement();
	~PlayerMovement();

	// Modifiers
	const float crouch_stop_speed = 3.0f;
	const float stop_speed = 6.0f;
	const float gravity = 9.81f;
	const float ground_acceleration = 0.8f;
	const float air_acceleration = 0.2f;
	const float crouch_acceleration;
	const float max_roll_time = 0.5f;
	const float roll_speed = 8.0f;

protected:
	static void _bind_methods() {}

private:
	Player *player;
	Vector3 local_move_direction;
	Vector3 global_move_direction;
	Vector3 velocity;
	Vector3 camera_rig_default_position;
	std::unique_ptr<MovementState> current_state = nullptr;
	MovementState *queued_state = nullptr;
	MovementType previous_state_type;
	MovementType next_state_type;
	bool wish_move = false;

public:
	/// @brief Setup function that must be called during the player's _ready function.
	void initialize(Player *p_player);
	void handle_controls(const Ref<InputEvent> &event);
	void update(double delta);

private:
	/// @brief Sets the movement state to process by PlayerMovement. If you're
	///        gonna use this in a MovementState object, remember to return
	///        after calling this to avoid potential conflicts.
	/// @param p_movement_state The instance of the movement state to switch to.
	template <typename T>
	void queue_movement_state();
	void accelerate(Vector3 &velocity, float stop_speed, float acceleration);
};

} //namespace unruin

#endif