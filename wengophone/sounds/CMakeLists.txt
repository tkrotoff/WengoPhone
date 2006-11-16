project(sounds)

ow_copy_dir_recursive(${CMAKE_CURRENT_SOURCE_DIR}/*.wav sounds)
ow_copy_dir_recursive(${CMAKE_CURRENT_SOURCE_DIR}/*.raw sounds)
ow_copy_dir_recursive(${CMAKE_CURRENT_SOURCE_DIR}/*.png sounds)
ow_copy_dir_recursive(${CMAKE_CURRENT_SOURCE_DIR}/*.xml sounds)
