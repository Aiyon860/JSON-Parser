EXEC = build/json_parser.exe
JSON_FOLDER = tests/early_tests/step1
COLOR_ENABLED = false

CC=gcc	# Default compiler

all: $(EXEC)

$(EXEC): src/*.c include/*.h
	cmd /C "if not exist build mkdir build"
	$(CC) -Iinclude src/*.c -o $(EXEC)

run: $(EXEC)
	$(EXEC) $(JSON_FOLDER) $(if $(filter true,$(COLOR_ENABLED)),--color,)

clean:
	cmd /C "if exist build\\json_parser.exe del /Q build\\json_parser.exe"