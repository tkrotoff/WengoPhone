ow_create_shared_library(coipmanagersdk)

ow_add_sources(
	fake_main.c
)

ow_create_project_binary()

add_dependencies(${PROJECT_NAME}
	coipmanager_threaded
	PyCoIpManager
	webcam
	owsettings
	owsystem
	owmemorydump
	owqtutil
	qtcoreutil
	qtevent
	purpleinitplugin
	phapiinitplugin
	purpleconnectmanager
	phapiconnectmanager
	purplechatsessionmanager
	phapichatsessionmanager
	phapicallsessionmanager
	phapifilesessionmanager
	phapiaccountpresencemanager
	phapicontactpresencemanager
	purpleaccountpresencemanager
	purplecontactpresencemanager
	purplecontactsyncmanager
	phamrplugin
	phspeexplugin
	sfp-plugin
)

ow_declare_install_script(-DCOIPMANAGER_BUNDLE_NAME=${COIPMANAGER_BUNDLE_NAME} -DCOIPMANAGER_VERSION=${COIPMANAGER_VERSION})
ow_declare_package_script(-DCOIPMANAGER_BUNDLE_NAME=${COIPMANAGER_BUNDLE_NAME} -DCOIPMANAGER_VERSION=${COIPMANAGER_VERSION})

# Generate CoIpManager SDK documentation
find_package(Doxygen)
if (DOXYGEN_EXECUTABLE)
	add_custom_target(${PROJECT_NAME}-javadoc
		COMMAND
			${DOXYGEN_EXECUTABLE}
		WORKING_DIRECTORY
			"${CMAKE_SOURCE_DIR}"
		COMMENT
			"Generates CoIpManager SDK Javadoc using Doxygen"
	)

	add_dependencies(${PROJECT_NAME}-install
		${PROJECT_NAME}-javadoc
	)

	add_dependencies(${PROJECT_NAME}-package
		${PROJECT_NAME}-javadoc
	)
endif (DOXYGEN_EXECUTABLE)
