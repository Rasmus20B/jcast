
add_library(jcast_util)
target_sources(jcast_util
  PUBLIC
  FILE_SET
  CXX_MODULES FILES
  util/util.cc
  util/types.cc
  )

add_library(jcast_xml)
target_sources(jcast_xml
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

target_link_libraries(jcast_xml jcast_util)
target_link_libraries(${PROJECT_NAME} jcast_xml)
