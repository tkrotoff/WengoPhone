project(owcrashreport)

#Qt4 dependency
find_package(Qt4 REQUIRED)
include_directories(
	${QT_INCLUDE_DIR}
)
include(${QT_USE_FILE})

#Boost dependency
find_package(Boost REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})

#owthread dependency
include_directories(${owthread_INCLUDE_DIR})

#owsystem dependency
include_directories(${owsystem_INCLUDE_DIR})

#owqtutil dependency
include_directories(${owqtutil_INCLUDE_DIR})

#owcurl dependency
include_directories(${owcurl_INCLUDE_DIR})

#includes
include_directories(${owcrashreport_SOURCE_DIR})

#sources
set(owcrashreport_SRCS
	main.cpp
	FtpUpload.cpp
	QtCrashReport.cpp
)

qt4_automoc(${owcrashreport_SRCS})

include_directories(${owcrashreport_BINARY_DIR})
qt4_wrap_ui(owcrashreport_SRCS CrashReport.ui)

#owcrashreport
add_executable(owcrashreport ${owcrashreport_SRCS})

#owthread dependency
target_link_libraries(owcrashreport owthread)

#Qt4 dependency
target_link_libraries(owcrashreport ${QT_LIBRARIES})
