#include "application/input.h"

#include "foundation/assert.h"
#include <imgui/imgui.h>

#include <cmath>

#define INPUT_BACKEND_SDL

#if defined (INPUT_BACKEND_SDL)
#include <SDL.h>
#endif // INPUT_BACKEND_SDL

namespace Engine
{
	struct InputBackend
	{
		void								init( Gamepad* gamepads, u32 num_gamepads);
		void								shutdown();

		void								get_mouse_state( InputVector2& position, u8* buttons, u32 num_buttons );

		void								on_event( void* event_, u8* keys, u32 num_keys, Gamepad* gamepads, u32 num_gamepads, bool& has_focus );

	}; // struct InputBackend

#if defined (INPUT_BACKEND_SDL)

	static bool init_gamepad( int32_t index, Gamepad& gamepad )
	{
		SDL_GameController* pad = SDL_GameControllerOpen( index );

		// Set memory to 0.
		memset( &gamepad, 0, sizeof( Gamepad ) );

		if( pad )
		{
			rprint( "Opened Joystick 0\n" );
			rprint( "Name: %s\n", SDL_GameControllerNameForIndex( index ) );

			SDL_Joystick* joy = SDL_GameControllerGetJoystick( pad );

			gamepad.index	= index;
			gamepad.name	= SDL_JoystickNameForIndex( index );
			gamepad.handle	= pad;
			gamepad.id		= SDL_JoystickInstanceID( joy );

			return true;
		}
		else
		{
			rprint( "Couldn't Open Joystick %u\n", index  );
			gamepad.index = u32_max;

			return false;
		}
	}

	// InputBackendSDL //////////////////////////////////////////////////////////////
	void InputBackend::init(Gamepad* gamepads, u32 num_gamepads)
	{
		if( SDL_WasInit( SDL_INIT_GAMECONTROLLER ) != 1 )
			SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER );

		SDL_GameControllerEventState( SDL_ENABLE );

		for( u32 i=0; i < num_gamepads; ++i )
		{
			gamepads[ i ].index = u32_max;
			gamepads[ i ].id	= u32_max;
		}

		const i32 num_joystics = SDL_NumJoysticks();
		if( num_joystics > 0)
		{
			rprint( "Detected Joystick!" );

			for ( u32 i = 0; i < num_joystics; ++i )
			{
				if ( SDL_IsGameController( i ) )
				{
					init_gamepad( i, gamepads[ i ] );
				}
			}
		}
	}

	void InputBackend::shutdown()
	{
		SDL_GameControllerEventState( SDL_DISABLE );
	}

	static u32 to_sdl_mouse_button( MouseButtons button )
	{
		switch( button )
		{
		case MOUSE_BUTTONS_LEFT:
			return SDL_BUTTON_LEFT;
		case MOUSE_BUTTONS_MIDDLE:
			return SDL_BUTTON_MIDDLE;
		case MOUSE_BUTTONS_RIGHT:
			return SDL_BUTTON_RIGHT;
		}

		return u32_max;
	}

	void InputBackend::get_mouse_state(InputVector2& position, u8* buttons, u32 num_buttons)
	{
		i32 mouse_x, mouse_y;
		u32 mouse_buttons = SDL_GetMouseState( &mouse_x, &mouse_y );

		for( u32 i =0 ; i < num_buttons; ++i )
		{
			u32 sdl_button = to_sdl_mouse_button( ( MouseButtons )i );
			buttons[ i ] = mouse_buttons & SDL_BUTTON( sdl_button );
		}

		position.x = ( f32 )mouse_x;
		position.y = ( f32 )mouse_y;
	}

	void InputBackend::on_event(void* event_, u8* keys, u32 num_keys, Gamepad* gamepads, u32 num_gamepads, bool& has_focus)
	{
		SDL_Event& event = *( SDL_Event* )event_; 
		switch( event.type )
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				i32 key = event.key.keysym.scancode;
				if( key >= 0 && key < (i32)num_keys )
				{
					keys[ key ] = ( event.type == SDL_KEYDOWN);
				}

				break;
			}
		}
	}

#endif // INPUT_BACKEND_SDL

	Device device_from_part( DevicePart part )
	{
		switch( part )
		{
			case DEVICE_PART_MOUSE:
			{
				return DEVICE_MOUSE;
			}

			case DEVICE_PART_GAMEPAD_AXIS:
			case DEVICE_PART_GAMEPAD_BUTTONS:
			{
				return DEVICE_GAMEPAD;
			}

			case DEVICE_PART_KEYBOARD:
			default:
			{
				return DEVICE_KEYBOARD;
			}
		}
	}

	cstring* gamepad_axis_names()
	{
		static cstring names[] = { "left_x", "left_y", "right_x","right_y", "trigger_left","trigger_right", "gamepad_axis_count" };
		return names;
	}

	cstring* gamepad_button_names()
	{
		static cstring names[] = { "a", "b", "x", "y", "back", "guide", "start", "left_stick", "right_stick", "left_shoulder", "right_shoulder", "dpad_up", "dpad_down", "dpad_left", "dpad_right", "gamepad_button_count", };
		return names;
	}

	cstring* mouse_button_names()
	{
		static cstring names[] = { "left", "right", "middle", "mouse_button_count", };
		return names;
	}

	// InputService //////////////////////////////////////////////////////////////////
	static InputBackend						s_input_backend;
	static InputService						s_input_service;

	void InputService::init(Allocator* allocator)
	{
		rprint( "Input Service init\n" );

		string_buffer.init( 1000, allocator );
		action_maps.init( allocator, 16 );
		actions.init( allocator, 64 );
		bindings.init( allocator, 256 );

		// Init gamepads handles.
		for( size_t i =0; i < k_max_gamepads; ++i )
		{
			gamepads[ i ].handle = nullptr; 
		}

		memset( keys, 0, KEY_COUNT );
		memset( previous_keys, 0, KEY_COUNT);
		memset( mouse_button, 0, MOUSE_BUTTONS_COUNT );
		memset( previous_mouse_button, 0, MOUSE_BUTTONS_COUNT);

		s_input_backend.init( gamepads, k_max_gamepads );
	}

	void InputService::shutdown()
	{
		s_input_backend.shutdown();
		action_maps.shutdown();
		actions.shutdown();
		bindings.shutdown();

		string_buffer.shutdown();

		rprint( "InputService shutdown\n" );
	}

	static constexpr f32 k_mouse_drag_min_distance = 4.f;

	bool InputService::is_key_down(Keys key)
	{
		return keys[ key ] && has_focus;
	}

	bool InputService::is_key_just_pressed( Keys key, bool repeat )
	{
		return !keys[key] && previous_keys[ key ] && has_focus;
	}

	bool InputService::is_key_just_released( Keys key )
	{
		return !keys[key] && previous_keys[key] && has_focus;
	}

	bool InputService::is_mouse_down(MouseButtons button )
	{
		return mouse_button[ button ];
	}

	bool InputService::is_mouse_clicked( MouseButtons button )
	{
		return mouse_button[ button ] && !previous_mouse_button[ button ];
	}

	bool InputService::is_mouse_released( MouseButtons button )
	{
		return !mouse_button[ button ];
	}

	bool InputService::is_mouse_dragging( MouseButtons button )
	{
		if ( !mouse_button[ button ] )
		{
			return false;
		}

		return mouse_drag_distance[ button ] > k_mouse_drag_min_distance;
	}

	void InputService::on_event(void* input_event)
	{
		s_input_backend.on_event( input_event, keys, KEY_COUNT, gamepads, k_max_gamepads, has_focus );
	}

	bool InputService::is_triggered(InputHandle action) const
	{
		RASSERT( action < actions.size );
		return actions[ action ].triggered();
	}

	f32 InputService::is_read_value_1d(InputHandle action) const
	{
		RASSERT( action < actions.size );
		return actions[ action ].read_value_1d();
	}

	InputVector2 InputService::is_read_value_2d(InputHandle action) const
	{
		RASSERT( action < actions.size );
		return actions[action].read_value_2d();
	}

	InputHandle	InputService::create_action_map(const InputActionMapCreation& creation)
	{
		InputActionMap new_action_map;
		new_action_map.active = creation.active;
		new_action_map.name = creation.name;

		action_maps.push( new_action_map );

		return action_maps.size - 1;
	}

	InputHandle	InputService::create_action( const InputActionCreation& creation )
	{
		InputAction action;
		action.action_map = creation.action_map;
		action.name = creation.name;

		actions.push( action );

		return actions.size - 1;
	}

	InputHandle InputService::find_action_map(cstring name) const
	{
		// TODO: Move to hash map ?
		for( u32 i=0; i < action_maps.size; ++i )
		{
			if( strcmp( name, action_maps[i].name) == 0 )
			{
				return i;
			}
		}

		return InputHandle( u32_max );
	}

	InputHandle	InputService::find_action(cstring name) const
	{
		// TODO: Move to hash map ?
		for ( u32 i = 0; i < actions.size; ++i )
		{
			if ( strcmp(name, actions[ i ].name) == 0 )
			{
				return i;
			}
		}

		return InputHandle( u32_max );
	}

	void InputService::add_button( InputHandle action, DevicePart device_part, uint16_t button, bool repeat )
	{
		const InputAction& binding_action = actions[ action ];

		InputBinding binding;
		binding.set( BINDING_TYPE_BUTTON, device_from_part( device_part ), device_part, button, 0, 0, repeat ).set_handles( binding_action.action_map, action );

		bindings.push( binding );
	}

	void InputService::add_axis_1d( InputHandle action, DevicePart device_part, uint16_t axis, float min_deadzone, float max_deadzone)
	{
		const InputAction& binding_action = actions[action];

		InputBinding binding;
		binding.set( BINDING_TYPE_AXIS_1D, device_from_part( device_part ), device_part, axis, 0, 0, 0).set_deadzones( min_deadzone, max_deadzone ).set_handles( binding_action.action_map, action );

		bindings.push( binding );
	}

	void InputService::add_axis_2d( InputHandle action, DevicePart device_part, uint16_t x_axis, uint16_t y_axis, float min_deadzone, float max_deadzone)
	{
		const InputAction& binding_action = actions[ action ];

		InputBinding binding, binding_x, binding_y;
		binding.set( BINDING_TYPE_AXIS_2D, device_from_part(device_part), device_part, u16_max, 1, 0, 0).set_deadzones( min_deadzone, max_deadzone ).set_handles( binding_action.action_map, action );
		binding.set( BINDING_TYPE_AXIS_2D, device_from_part(device_part), device_part, u16_max, 0, 1, 0).set_deadzones(min_deadzone, max_deadzone).set_handles(binding_action.action_map, action );
		binding.set( BINDING_TYPE_AXIS_2D, device_from_part(device_part), device_part, u16_max, 0, 1, 0).set_deadzones(min_deadzone, max_deadzone).set_handles(binding_action.action_map, action );

		bindings.push( binding );
		bindings.push( binding_x );
		bindings.push( binding_y );
	}

	void InputService::add_vector_1d( InputHandle action, DevicePart device_part_pos, uint16_t button_pos, DevicePart device_neg, uint16_t button_neg, bool repeat )
	{
		const InputAction& binding_action = actions[ action ];

		InputBinding binding, binding_positive, binding_negative;
		binding.set( BINDING_TYPE_AXIS_1D, device_from_part( device_part_pos), device_part_pos, u16_max, 1, 0, repeat).set_handles( binding_action.action_map, action );
		binding_positive.set( BINDING_TYPE_AXIS_1D, device_from_part(device_part_pos), device_part_pos, u16_max, 0, 1, repeat).set_handles(binding_action.action_map, action );
		binding_negative.set( BINDING_TYPE_AXIS_1D, device_from_part(device_part_pos), device_part_pos, u16_max, 0, 1, repeat).set_handles(binding_action.action_map, action );

		bindings.push( binding );
		bindings.push( binding_positive );
		bindings.push( binding_negative );
	}

	void InputService::add_vector_2d( InputHandle action, DevicePart device_part_up, uint16_t button_up, DevicePart device_part_down, uint16_t button_down, DevicePart device_part_left, uint16_t button_left, DevicePart device_part_right, uint16_t button_right, bool repeat )
	{
		const InputAction& binding_action = actions[ action ];

		InputBinding binding, binding_up, binding_down, binding_left, binding_right;
		binding.set( BINDING_TYPE_VECTOR_2D, device_from_part( device_part_up ), device_part_up, u16_max, 1, 0, repeat ).set_handles( binding_action.action_map, action );
		binding_up.set( BINDING_TYPE_VECTOR_2D, device_from_part( device_part_up ), device_part_up, button_up, 0, 1, repeat).set_handles( binding_action.action_map, action );
		binding_down.set( BINDING_TYPE_VECTOR_2D, device_from_part( device_part_down ), device_part_down, button_down, 0, 1, repeat).set_handles( binding_action.action_map, action);
		binding_left.set( BINDING_TYPE_VECTOR_2D, device_from_part( device_part_left ), device_part_left, button_left, 0, 1, repeat).set_handles( binding_action.action_map, action);
		binding_right.set( BINDING_TYPE_VECTOR_2D, device_from_part( device_part_right ), device_part_right, button_right, 0, 1, repeat).set_handles( binding_action.action_map, action);

		bindings.push( binding );
		bindings.push( binding_up );
		bindings.push( binding_down );
		bindings.push( binding_left );
		bindings.push( binding_right );
	}

	void InputService::new_frame()
	{
		// Cache previous frame keys.
		// Resetting previous frame breaks key pressing = there can be more frames between key presses even if continously pressed.
		for( u32 i = 0; i < KEY_COUNT; ++i )
		{
			previous_keys[ i ] = keys[ i ];
		}

		for ( u32 i = 0; i < MOUSE_BUTTONS_COUNT; ++i )
		{
			previous_mouse_button[ i ] = mouse_button[i];
		}

		for ( u32 i = 0; i < k_max_gamepads; ++i )
		{
			if (gamepads[i].is_attached())
			{
				for (u32 k = 0; i < GAMEPAD_BUTTON_COUNT; ++k)
				{
					gamepads[ i ].previous_buttons[ k ] = gamepads[ i ].buttons[ k ];
				}
			}
		}
	}

	void InputService::update(f32 delta)
	{
		// Update Mouse ////////////////////////////////////////
		previous_mouse_position = mouse_position;

		// Update current mouse state.
		s_input_backend.get_mouse_state(mouse_position, mouse_button, MOUSE_BUTTONS_COUNT);

		for (u32 i = 0; i < MOUSE_BUTTONS_COUNT; ++i)
		{
			// Just Clicked. Save position
			if (is_mouse_clicked((MouseButtons)i))
			{
				mouse_clicked_position[i] = mouse_position;
			}
			else if (is_mouse_down((MouseButtons)i))
			{
				f32 delta_x = mouse_position.x - mouse_clicked_position[i].x;
				f32 delta_y = mouse_position.y - mouse_clicked_position[i].y;
				mouse_drag_distance[i] = sqrtf((delta_x * delta_x) + (delta_y * delta_y));
			}
		}

		// New Update.

		// Update all action maps.
		// Update all action.
		// Scan each action for the map.
		for (u32 j = 0; j < actions.size; ++j)
		{
			InputAction& input_action = actions[j];
			input_action.value = { 0, 0 };
		}

		// Read all input values for each binding.
		// First get all the buttons or composite parts. Composite input will be calculated after.
		for (u32 k = 0; k < bindings.size; ++k)
		{
			InputBinding& input_binding = bindings[k];
			// Skip composite bindings. There value will be calculated after.
			if (input_binding.is_composite)
				continue;

			input_binding.value = false;

			switch (input_binding.device)
			{
				case DEVICE_KEYBOARD:
				{
					bool key_value = input_binding.repeat ? is_key_down((Keys)input_binding.button) : is_key_just_pressed((Keys)input_binding.button, false);
					input_binding.value = key_value ? 1.0 : 0.0f;
					break;
				}

				case DEVICE_GAMEPAD:
				{
					Gamepad& gamepad = gamepads[0];
					if (gamepad.handle == nullptr)
					{
						break;
					}

					const float min_deadzone = input_binding.min_deadzone;
					const float max_deadzone = input_binding.max_deadzone;

					switch (input_binding.device_part)
					{
						case DEVICE_PART_GAMEPAD_AXIS:
						{
							input_binding.value = gamepad.axis[input_binding.button];
							input_binding.value = fabs(input_binding.value) < min_deadzone ? 0.0f : input_binding.value;
							input_binding.value = fabs(input_binding.value) > max_deadzone ? (input_binding.value < 0 ? -1.0 : 1.0f) : input_binding.value;

							break;
						}

						case DEVICE_PART_GAMEPAD_BUTTONS:
						{
							input_binding.value = input_binding.repeat ? gamepad.is_button_down( ( GamepadButtons )input_binding.value ) : gamepad.is_button_just_pressed( ( GamepadButtons )input_binding.button );

							break;
						}
					}
				}
			}
		}

		for (u32 k = 0; k < bindings.size; ++k)
		{
			InputBinding& input_binding = bindings[k];

			if (input_binding.is_part_of_composite)
				continue;

			InputAction& input_action = actions[input_binding.action_index];

			switch (input_binding.type)
			{
			case BINDING_TYPE_BUTTON:
			{
				input_action.value.x = fmax(input_action.value.x, input_binding.value ? 1.0f : 0.0f);
				break;
			}

			case BINDING_TYPE_AXIS_1D:
			{
				input_action.value.x = input_binding.value != 0.f ? input_binding.value : input_action.value.x;
				break;
			}

			case BINDING_TYPE_AXIS_2D:
			{
				// Retrieve following 2 bindings.
				InputBinding& input_binding_x = bindings[++k];
				InputBinding& input_binding_y = bindings[++k];

				input_action.value.x = input_binding_x.value != 0.f ? input_binding_x.value : input_action.value.x;
				input_action.value.x = input_binding_x.value != 0.f ? input_binding_x.value : input_action.value.x;

				break;
			}

			case BINDING_TYPE_VECTOR_1D:
			{
				// Retrieve following 2 bindings.
				InputBinding& input_binding_pos = bindings[++k];
				InputBinding& input_binding_neg = bindings[++k];

				input_action.value.x = input_binding_pos.value ? input_binding_pos.value : input_binding_neg.value ? -input_binding_neg.value : input_action.value.x;

				break;
			}

			case BINDING_TYPE_VECTOR_2D:
			{
				// Retrieve following 2 bindings.
				InputBinding& input_binding_up = bindings[++k];
				InputBinding& input_binding_down = bindings[++k];
				InputBinding& input_binding_left = bindings[++k];
				InputBinding& input_binding_right = bindings[++k];
				
				input_action.value.x = input_binding_right.value ? 1.0f : input_binding_left.value ? -1.0f : input_action.value.x;
				input_action.value.y = input_binding_right.value ? 1.0f : input_binding_down.value ? -1.0f : input_action.value.y;

				break;
			}
			}
		}
	}

	// Input Action ////////////////////////////////////////////////////////////
	bool InputAction::triggered() const
	{
		return value.x != 0.0f;
	}

	f32 InputAction::read_value_1d() const
	{
		return value.x;
	}

	InputVector2 InputAction::read_value_2d() const
	{
		return value;
	}

	InputBinding& InputBinding::set( BindingType type_, Device device_, DevicePart device_part_, u16 button_, u8 is_composite_, u8 is_part_of_composite_, u8 repeat_ )
	{
		type = type_;
		device = device_;
		device_part = device_part_;
		button = button_;
		is_composite = is_composite_;
		is_part_of_composite = is_part_of_composite_;
		repeat = repeat_;

		return *this;
	}

	InputBinding& InputBinding::set_deadzones( f32 min, f32 max )
	{
		min_deadzone = min;
		max_deadzone = max;

		return *this;
	}

	InputBinding& InputBinding::set_handles( InputHandle action_map, InputHandle action )
	{
		// Don't expect this to have more than 256.
		RASSERT( action_map < 256 );
		RASSERT( action < 16636 );

		action_map_index = ( u8 )action_map;
		action_index = ( u16 )action;

		return *this;
	}

}	// namespace Engine.