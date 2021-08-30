cmake_minimum_required(VERSION 3.10)

project(CPURayTracer)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tracer 
  "${PROJECT_SOURCE_DIR}/src/main.cpp"

  "${PROJECT_SOURCE_DIR}/src/AABB.cpp"
  "${PROJECT_SOURCE_DIR}/src/Camera.cpp"
  "${PROJECT_SOURCE_DIR}/src/Color.cpp"
  "${PROJECT_SOURCE_DIR}/src/MSColor.cpp"
  "${PROJECT_SOURCE_DIR}/src/Image.cpp"

  "${PROJECT_SOURCE_DIR}/src/textures/CheckeredTexture.cpp"
  "${PROJECT_SOURCE_DIR}/src/textures/ImageTexture.cpp"

  "${PROJECT_SOURCE_DIR}/src/traceables/BVHNode.cpp"
  "${PROJECT_SOURCE_DIR}/src/traceables/ListTraceable.cpp"
  "${PROJECT_SOURCE_DIR}/src/traceables/MovingSphereTraceable.cpp"
  "${PROJECT_SOURCE_DIR}/src/traceables/SphereTraceable.cpp"
  "${PROJECT_SOURCE_DIR}/src/traceables/StaticMeshTraceable.cpp"

  "${PROJECT_SOURCE_DIR}/src/render_targets/RenderTargetPNGImage.cpp"
  )

target_include_directories(tracer PUBLIC
  "${PROJECT_SOURCE_DIR}"
  "${PROJECT_SOURCE_DIR}/include"
  )

target_compile_features(tracer PUBLIC cxx_std_17)
target_compile_options(tracer PRIVATE
  # Fast Math
  $<$<CXX_COMPILER_ID:GNU>:-ffast-math>
  $<$<CXX_COMPILER_ID:MSVC>:/fp:fast>

  $<$<CXX_COMPILER_ID:GNU>:-msse2>
  $<$<CXX_COMPILER_ID:MSVC>:/arch:SSE2>

  )

find_package(OpenMP)
target_link_libraries(tracer PUBLIC
  $<$<BOOL:OpenMP_CXX_FOUND>:OpenMP::OpenMP_CXX>
  )
