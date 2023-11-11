
target_sources(
  ${CMAKE_PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/echo-utils.cpp
  )

  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utils)
