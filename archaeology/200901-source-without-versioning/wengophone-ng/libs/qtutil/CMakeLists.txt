ow_create_static_library(owqtutil)

ow_use_public_libraries(
	qtcoreutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Because of UIC and MOC
ow_add_private_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/qtutil/NonResizeableWidget.h
	include/qtutil/WengoStyleLabel.h
	include/qtutil/ToolTipLineEdit.h
	include/qtutil/ClickableLabel.h
	include/qtutil/Wizard.h
	include/qtutil/AnimatedButton.h
	include/qtutil/UpQComboBox.h
	include/qtutil/LinkLabel.h
	include/qtutil/ScrollFrame.h
	include/qtutil/ImageSelector.h
	src/QtThumbnailManager.h
	src/QtThumbnailDirModel.h
)

qt4_wrap_ui(${PROJECT_NAME}_SRCS
	src/Wizard.ui
	src/ImageSelector.ui
)

qt4_add_resources(${PROJECT_NAME}_SRCS
	qtutil.qrc
)

ow_add_sources(
	src/WidgetFactory.cpp
	src/Object.cpp
	src/NonResizeableWidget.cpp
	src/Widget.cpp
	src/WengoStyleLabel.cpp
	src/ToolTipLineEdit.cpp
	src/ClickableLabel.cpp
	src/WidgetBackgroundImage.cpp
	src/Wizard.cpp
	src/PixmapMerging.cpp
	src/AnimatedButton.cpp
	src/DesktopService.cpp
	src/UpQComboBox.cpp
	src/LinkLabel.cpp
	src/ScrollFrame.cpp
	src/ImageSelector.cpp
	src/ThumbnailView.cpp
	src/QtThumbnailDirModel.cpp
	src/QtThumbnailManager.cpp
	src/WidgetRatioEnforcer.cpp
	src/WidgetUtils.cpp
	src/ExpandButton.cpp
	src/OsdButton.cpp
)

if (APPLE)
	ow_add_sources(
		src/MacDesktopService.cpp
	)
	qt4_wrap_cpp(${PROJECT_NAME}_SRCS
		include/qtutil/MacDesktopService.h
	)
	ow_add_private_include_dirs(
		/Developer/Headers/FlatCarbon/
	)
	ow_use_public_frameworks(
		Cocoa
		Carbon
	)
endif (APPLE)

if (WIN32)
	ow_add_sources(
		src/WindowsDesktopService.cpp
	)
	qt4_wrap_cpp(${PROJECT_NAME}_SRCS
		include/qtutil/WindowsDesktopService.h
	)
endif (WIN32)

ow_create_project_binary()

subdirs(
	tests
)
