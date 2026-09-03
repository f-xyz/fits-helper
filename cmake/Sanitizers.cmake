if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(fits-helper PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer -g>
  )
  target_link_options(fits-helper PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
  )
endif()