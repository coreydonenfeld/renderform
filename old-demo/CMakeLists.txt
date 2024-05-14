set(HEADER_FILES

)
set(SOURCE_FILES
	main.cpp
)

add_executable(math_textual ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(math_textual PRIVATE ${OpenCV_LIBS} ${TESSERACT_LIBRARIES} ${LEPTONICA_LIBRARIES})
