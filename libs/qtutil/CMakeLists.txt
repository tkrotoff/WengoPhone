ow_create_static_library(owqtutil)

ow_use_public_libraries(
	owthread
	qt4
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
	include/qtutil/EventFilter.h
	include/qtutil/KeyEventFilter.h
	include/qtutil/MouseEventFilter.h
	include/qtutil/PaintEventFilter.h
	include/qtutil/EnabledChangeEventFilter.h
	include/qtutil/PostEvent.h
	include/qtutil/WidgetFactory.h
	include/qtutil/Object.h
	include/qtutil/QObjectThreadSafe.h
	include/qtutil/ResizeEventFilter.h
	include/qtutil/NonResizeableWidget.h
	include/qtutil/StringListConvert.h
	include/qtutil/Widget.h
	include/qtutil/WengoStyleLabel.h
	include/qtutil/ToolTipLineEdit.h
	include/qtutil/ClickableLabel.h
	include/qtutil/WidgetBackgroundImage.h
	include/qtutil/Wizard.h
	include/qtutil/QtThreadEvent.h
	include/qtutil/ThreadEventFilter.h
	include/qtutil/PixmapMerging.h
	include/qtutil/SafeConnect.h
	include/qtutil/LanguageChangeEventFilter.h
	include/qtutil/CloseEventFilter.h
	include/qtutil/AnimatedButton.h
	include/qtutil/UpQComboBox.h
)

qt4_wrap_ui(${PROJECT_NAME}_SRCS
	src/Wizard.ui
)

ow_add_sources(
	src/EventFilter.cpp
	src/KeyEventFilter.cpp
	src/MouseEventFilter.cpp
	src/PaintEventFilter.cpp
	src/EnabledChangeEventFilter.cpp
	src/WidgetFactory.cpp
	src/Object.cpp
	src/QObjectThreadSafe.cpp
	src/ResizeEventFilter.cpp
	src/NonResizeableWidget.cpp
	src/StringListConvert.cpp
	src/Widget.cpp
	src/WengoStyleLabel.cpp
	src/ToolTipLineEdit.cpp
	src/ClickableLabel.cpp
	src/WidgetBackgroundImage.cpp
	src/Wizard.cpp
	src/ThreadEventFilter.cpp
	src/PixmapMerging.cpp
	src/SafeConnect.cpp
	src/LanguageChangeEventFilter.cpp
	src/CloseEventFilter.cpp
	src/AnimatedButton.cpp
	src/UpQComboBox.cpp
)

ow_create_binary()
