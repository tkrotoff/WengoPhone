ow_create_shared_library(owutil)

ow_use_public_libraries(
	owcutil
	qtcore
	boost
)

if (APPLE)
	ow_use_private_frameworks(
		CoreFoundation
	)
endif (APPLE)

ow_add_sources(
	src/Base64.cpp
	src/CountryList.cpp
	src/Date.cpp
	src/File.cpp
	src/Identifiable.cpp
	src/Interface.cpp
	src/IStateDPContext.cpp
	src/IStateDPState.cpp
	src/Logger.cpp
	src/OWPicture.cpp
	src/Path.cpp
	src/String.cpp
	src/StringList.cpp
	src/Time.cpp
	src/Uuid.cpp
	src/NonCopyable.cpp
	src/Cloneable.cpp
	src/SafeConnect.cpp
	src/PostEvent.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	Logger.h
)

ow_create_project_binary()

subdirs(
	tests
)
