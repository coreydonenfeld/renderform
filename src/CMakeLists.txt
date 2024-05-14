include_directories(${CMAKE_SOURCE_DIR}/include/renderform)

set(HEADER_FILES
	${CMAKE_SOURCE_DIR}/include/renderform/renderform.hpp
	${CMAKE_SOURCE_DIR}/include/renderform/formula.hpp
	modules/cleaner.hpp
	modules/partitioner.hpp
	modules/recognizer.hpp
	modules/parser.hpp
	modules/evaluator.hpp
	modules/renderer.hpp
)
set(SOURCE_FILES
	renderform.cpp
	formula.cpp
	cli.cpp
	modules/cleaner.cpp
	modules/partitioner.cpp
	modules/recognizer.cpp
	modules/parser.cpp
	modules/evaluator.cpp
	modules/renderer.cpp
)

add_executable(renderform ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(renderform PRIVATE ${OpenCV_LIBS} ${TESSERACT_LIBRARIES} ${LEPTONICA_LIBRARIES})
