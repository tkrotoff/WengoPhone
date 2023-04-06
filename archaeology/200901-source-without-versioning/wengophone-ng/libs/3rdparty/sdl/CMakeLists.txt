if (NOT SDL_FOUND)
	if (WIN32)
		set(ENV{SDLDIR}
			C:/SDL-1.2.11
		)
	endif (WIN32)

	find_package(SDL REQUIRED)
endif (NOT SDL_FOUND)

ow_create_project(sdl)

ow_use_public_libraries(
	SDL
)
