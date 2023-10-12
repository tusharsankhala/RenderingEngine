#include "application/window.h"

#include "foundation/log.h"
#include "foundation/numerics.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>

static SDL_Window* window = nullptr;

namespace Engine
{
	static f32 sdl_get_monitor_refresh()
	{
		SDL_DisplayMode current;
		int should_be_zero = SDL_GetCurrentDisplayMode( 0, &current );
		RASSERT( !should_be_zero );
		return 1.0f / current.refresh_rate;
	}

	void Window::init(void* configuration_)
	{
		rprint( "WindowService init\n" );

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			rprint( "SDL init Error: %s\n", SDL_GetError() );
			return;
		}

		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode( 0, &current );

		WindowConfiguration& configuration = *( WindowConfiguration* )configuration_;

		SDL_WindowFlags window_flags = ( SDL_WindowFlags )( SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );

		window = SDL_CreateWindow( configuration.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, configuration.width, configuration.height, window_flags );

		rprint( "Window created sucessfully\n" );

		int window_width, window_height;
		SDL_Vulkan_GetDrawableSize( window, &window_width, &window_height );

		width = ( u32 )window_width;
		height = ( u32 )window_height;

		// Assign this so it can be accessed from outside.
		platform_handle = window;

		// Callbacks
		os_messages_callbacks.init( configuration.allocator, 4 );
		os_messages_callbacks_data.init( configuration.allocator, 4);

		display_refresh = sdl_get_monitor_refresh();
	}

	void Window::shutdown()
	{
		os_messages_callbacks_data.shutdown();
		os_messages_callbacks.shutdown();

		SDL_DestroyWindow( window );
		SDL_Quit();

		rprint( "WindowService shutdown\n" );	
	}

	void Window::handle_os_messages()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			switch (event.type)
			{
				case SDL_QUIT:
				{
					requested_exit = true;
					goto propagate_event;
					break;
				}

				// Handle subevent
				case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						case SDL_WINDOWEVENT_RESIZED:
						{
							u32 new_width = ( u32 )( event.window.data1 );
							u32 new_height = ( u32 )( event.window.data2 );

							// Update only if needed.
							if ( new_width != width || new_height != height )
							{
								resized = true;
								width = new_width;
								height = new_height;

								rprint( "Resizing to %u, %u\n", width, height );
							}
						}
						break;
					}
					default:
					{
						display_refresh = sdl_get_monitor_refresh();
						break;
					}
				}
				goto propagate_event;
				break;
			}

		// Mavarick:
		propagate_event:
			// Callbacks
			for ( u32 i = 0; i < os_messages_callbacks.size; ++i ) {
				OsMessagesCallback callback = os_messages_callbacks[ i ];
				callback( &event, os_messages_callbacks_data[ i ] );
			}
		}	
	}

	void Window::set_fullscreeninit(bool value)
	{
		if( value )
		{
			SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN_DESKTOP );
		}
		else
		{
			SDL_SetWindowFullscreen(window, 0);
		}
	}

	void Window::register_os_messages_callback(OsMessagesCallback callback, void* user_data)
	{
		os_messages_callbacks.push( callback );
		os_messages_callbacks_data.push( user_data );
	}

	void Window::unregister_os_messages_callback(OsMessagesCallback callback)
	{
		RASSERT( os_messages_callbacks.size < 8, "This array is too big for a liner search,. Consider using something different:" );

	propagate_event:
		// Callbacks
		for (u32 i = 0; i < os_messages_callbacks.size; ++i)
		{
			if (os_messages_callbacks[i] == callback)
			{
				os_messages_callbacks.delete_swap(i);
				os_messages_callbacks_data.delete_swap(i);
			}
		}
	}

	void Window::center_mouse(bool dragging)
	{
		if( dragging )
		{
			SDL_WarpMouseInWindow( window, Engine::roundu32(width / 2.f), Engine::roundu32(height / 2.f) );
			SDL_SetWindowGrab( window, SDL_TRUE );
			SDL_SetRelativeMouseMode( SDL_TRUE );
		}
		else
		{
			SDL_SetWindowGrab( window, SDL_TRUE );
			SDL_SetRelativeMouseMode( SDL_TRUE );
		}
	}

} // namespace Engine