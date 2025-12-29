CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O3 -I include
VPATH = src tests

OBJECT_PATH = obj
BIN_PATH = bin
TABLE_PATH = tables

TARGET = solver

COORDS_TEST_TARGET = CoordsTests
ROTATION_TEST_TARGET = RotateTests
DB_RELATED_TEST_TARGET = DatabaseTests

OBJECTS = $(OBJECT_PATH)/Cube.o $(OBJECT_PATH)/Tables.o $(OBJECT_PATH)/Solver.o $(OBJECT_PATH)/Main.o $(OBJECT_PATH)/DBCube.o $(OBJECT_PATH)/DatabaseLogger.o

COORDS_TEST_OBJECT = $(OBJECT_PATH)/Cube.o $(OBJECT_PATH)/Tables.o $(OBJECT_PATH)/CoordsTests.o
ROTATION_TEST_OBJECT = $(OBJECT_PATH)/Cube.o $(OBJECT_PATH)/Tables.o $(OBJECT_PATH)/RotateTests.o
DB_RELATED_TEST_OBJECT = $(OBJECT_PATH)/Cube.o $(OBJECT_PATH)/Tables.o $(OBJECT_PATH)/Solver.o $(OBJECT_PATH)/DatabaseTests.o $(OBJECT_PATH)/DBCube.o $(OBJECT_PATH)/DatabaseLogger.o

GTEST_INC = /c/cpp_libraries/googletest/googletest/include
GTEST_LIB = /c/cpp_libraries/googletest/build/lib
GTEST_FLAGS = -I$(GTEST_INC) -L$(GTEST_LIB) -lgtest -lgtest_main -pthread

SQLITE_FLAGS = -lsqlite3

$(OBJECT_PATH):
	mkdir -p $(OBJECT_PATH)

$(BIN_PATH):
	mkdir -p $(BIN_PATH)

$(OBJECT_PATH)/%.o: %.cpp | $(OBJECT_PATH)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_PATH)/$(TARGET): $(OBJECTS) | $(BIN_PATH)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(SQLITE_FLAGS)

run: $(BIN_PATH)/$(TARGET)
	./$(BIN_PATH)/$(TARGET)

compile: $(BIN_PATH)/$(TARGET)

$(BIN_PATH)/$(COORDS_TEST_TARGET): $(COORDS_TEST_OBJECT) | $(BIN_PATH)
	$(CXX) $(CXXFLAGS) $(COORDS_TEST_OBJECT) -o $@ $(GTEST_FLAGS)

test_coords: $(BIN_PATH)/$(COORDS_TEST_TARGET) | $(BIN_PATH)
	./$(BIN_PATH)/$(COORDS_TEST_TARGET)


$(BIN_PATH)/$(ROTATION_TEST_TARGET): $(ROTATION_TEST_OBJECT) | $(BIN_PATH)
	$(CXX) $(CXXFLAGS) $(ROTATION_TEST_OBJECT) -o $@ $(GTEST_FLAGS)

test_rotations: $(BIN_PATH)/$(ROTATION_TEST_TARGET) | $(BIN_PATH)
	./$(BIN_PATH)/$(ROTATION_TEST_TARGET)


$(BIN_PATH)/$(DB_RELATED_TEST_TARGET): $(DB_RELATED_TEST_OBJECT) | $(BIN_PATH)
	$(CXX) $(CXXFLAGS) $(DB_RELATED_TEST_OBJECT) -o $@ $(GTEST_FLAGS) $(SQLITE_FLAGS)

test_database: $(BIN_PATH)/$(DB_RELATED_TEST_TARGET) | $(BIN_PATH)
	./$(BIN_PATH)/$(DB_RELATED_TEST_TARGET)


$(OBJECT_PATH)/%Tests.o: %Tests.cpp | $(OBJECT_PATH)
	$(CXX) $(CXXFLAGS) -DUNIT_TEST $(GTEST_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJECT_PATH)/*.o $(BIN_PATH)/$(TARGET) $(BIN_PATH)/$(COORDS_TEST_TARGET) $(BIN_PATH)/$(ROTATION_TEST_TARGET) $(BIN_PATH)/$(DB_RELATED_TEST_TARGET) $(TABLE_PATH)/*.bin

TestAll: $(BIN_PATH)/$(COORDS_TEST_TARGET) $(BIN_PATH)/$(ROTATION_TEST_TARGET) $(BIN_PATH)/$(DB_RELATED_TEST_TARGET)
	./$(BIN_PATH)/$(COORDS_TEST_TARGET)
	./$(BIN_PATH)/$(ROTATION_TEST_TARGET)
	./$(BIN_PATH)/$(DB_RELATED_TEST_TARGET)

.PHONY: run clean compile TestAll test_coords test_rotations test_solves