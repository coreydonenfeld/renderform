include_directories(${CMAKE_SOURCE_DIR}/include/renderform)

set(HEADER_FILES
	${CMAKE_SOURCE_DIR}/include/renderform/renderform.hpp
	${CMAKE_SOURCE_DIR}/include/renderform/formula.hpp
	${CMAKE_SOURCE_DIR}/include/renderform/formtree.hpp
	modules/cleaner.hpp
	modules/partitioner.hpp
	modules/recognizer.hpp
	modules/character.hpp
	classifiers/symbols.hpp
	classifiers/knn.hpp
)
set(SOURCE_FILES
	renderform.cpp
	formula.cpp
	cli.cpp
	formtree.cpp
	modules/cleaner.cpp
	modules/partitioner.cpp
	modules/recognizer.cpp
	modules/character.cpp
	classifiers/symbols.cpp
	classifiers/knn.cpp
)

add_executable(renderform ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(renderform PRIVATE ${OpenCV_LIBS} ${TESSERACT_LIBRARIES} ${LEPTONICA_LIBRARIES})
