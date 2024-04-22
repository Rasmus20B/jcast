
add_library(util)
target_sources(util
  PUBLIC
  FILE_SET
  CXX_MODULES FILES
  util/util.cc
  util/types.cc
  )

add_library(xml)
target_sources(xml
  PUBLIC
  FILE_SET
  CXX_MODULES FILES
  xml/lexer.cc
  xml/parser.cc
  xml/xml.cc
  )

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
  PUBLIC
  FILE_SET
  CXX_MODULES FILES
  jcast.cc
  collada.cc
  asset.cc
  )

target_link_libraries(${PROJECT_NAME} util xml)
target_link_libraries(xml util)
