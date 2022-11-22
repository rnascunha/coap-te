find_package(Doxygen)
if (Doxygen_FOUND)
  set(DOCS_DIR        ${CMAKE_SOURCE_DIR}/docs)
  set(DOXYGEN_INPUT   ${DOCS_DIR}/Doxyfile.in)
  set(DOCS_INPUT      ${CMAKE_SOURCE_DIR}/include)
  set(DOCS_OUTPUT     ${CMAKE_BINARY_DIR}/docs)
  set(DOXYGEN_OUTPUT  ${DOCS_OUTPUT}/Doxyfile)

  configure_file(${DOXYGEN_INPUT} ${DOXYGEN_OUTPUT} @ONLY)

  add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUTPUT}
        COMMENT "Generating API documentation of CoAP-te"
        VERBATIM)
else()
  message(WARNING "Doxygen NOT found! Documentation will not be built")
endif()