if (NOT SDLIMAGE_FOUND)
	if (WIN32)
		set(ENV{SDLIMAGEDIR}
			C:/SDL_image-1.2.5
		)
	endif (WIN32)

	find_package(SDL_image REQUIRED)
endif (NOT SDLIMAGE_FOUND)

ow_create_project(sdl_image)

ow_use_public_libraries(
	SDLIMAGE
	sdl
)
