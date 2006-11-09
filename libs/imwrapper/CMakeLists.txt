ow_create_project(owimwrapper)

# TODO: test
#subdirs(
	#src/gaim
	#src/multiim
	#src/tests
#)

ow_use_public_libraries(
	owsettings
)

ow_use_private_libraries(
	tinyxml
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/Account.cpp
	src/EnumIMProtocol.cpp
	src/EnumPresenceState.cpp
	src/IMAccount.cpp
	src/IMAccountHandler.cpp
	src/IMAccountHandlerFileStorage.cpp
	src/IMAccountHandlerXMLSerializer.cpp
	src/IMAccountParameters.cpp
	src/IMAccountParametersXMLSerializer.cpp
	src/IMAccountXMLSerializer.cpp
	src/IMChatSession.cpp
	src/IMContact.cpp
	src/IMContactSet.cpp
	src/IMContactXMLSerializer.cpp
	src/IMWrapperFactory.cpp
)

ow_create_static_library()
