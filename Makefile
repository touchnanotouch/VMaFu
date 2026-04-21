.PHONY: help build build-mpi build-nompi run run-mpi run-nompi clean clean-mpi clean-nompi info check-mpi benchmark benchmark-all plot generate-data report excel compare

# Configuration

BUILD_DIR_MPI = build_mpi
BUILD_DIR_NOMPI = build_nompi

NUM_PROCS ?= 8
DATA_SIZE ?= 500

DEMO_EXEC = demo.exe
BENCHMARK_EXEC = benchmark.exe
CMAKE_EXEC = "C:/msys64/mingw64/bin/cmake.exe"
MPI_EXEC = "C:/Program Files/Microsoft MPI/Bin/mpiexec.exe"

BUILD_TYPE ?= nompi

BENCHMARK_DIR = benchmark
BENCHMARK_OUTPUT = $(BENCHMARK_DIR)/results.txt

help:
	@echo "Makefile Commands:"
	@echo "  make help         - Show this help"
	@echo "  make info         - Show current configuration"
	@echo "  make check-mpi    - Check if MPI is available"
	@echo ""
	@echo "Build commands:"
	@echo "  make build        - Build with default type (mpi)"
	@echo "  make build-mpi    - Build with MPI support"
	@echo "  make build-nompi  - Build without MPI support"
	@echo ""
	@echo "Run commands:"
	@echo "  make run          - Run with default type"
	@echo "  make run-mpi      - Run MPI version"
	@echo "  make run-nompi    - Run non-MPI version"
	@echo ""
	@echo "Clean commands:"
	@echo "  make clean        - Clean all builds"
	@echo "  make clean-mpi    - Clean MPI build only"
	@echo "  make clean-nompi  - Clean non-MPI build only"

info:
	@echo "Build Configuration:"
	@echo "  Build type: $(BUILD_TYPE)"
	@echo "  MPI build dir: $(BUILD_DIR_MPI)"
	@echo "  Non-MPI build dir: $(BUILD_DIR_NOMPI)"
	@echo "  Executable: $(DEMO_EXEC)"
	@echo "MPI Configuration:"
	@echo "  MPI executor: $(MPI_EXEC)"
	@echo "  Processes: $(NUM_PROCS)"

# Build targets

build:
	@$(MAKE) build-$(BUILD_TYPE)

build-mpi: $(BUILD_DIR_MPI)
	@echo "Building MPI version..."
	@cd $(BUILD_DIR_MPI) && $(CMAKE_EXEC) -DVMAFU_USE_MPI=ON .. && $(CMAKE_EXEC) --build .
	@echo "Build completed: $(BUILD_DIR_MPI)/$(DEMO_EXEC)"

build-nompi: $(BUILD_DIR_NOMPI)
	@echo "Building non-MPI version..."
	@cd $(BUILD_DIR_NOMPI) && $(CMAKE_EXEC) -DVMAFU_USE_MPI=OFF .. && $(CMAKE_EXEC) --build .
	@echo "Build completed: $(BUILD_DIR_NOMPI)/$(DEMO_EXEC)"

# Create build directories

$(BUILD_DIR_MPI):
	@echo "Creating MPI build directory: $@"
	@mkdir -p $@

$(BUILD_DIR_NOMPI):
	@echo "Creating non-MPI build directory: $@"
	@mkdir -p $@

# Run targets

run:
	@$(MAKE) run-$(BUILD_TYPE)

run-mpi: build-mpi
	@echo "Running MPI version with $(NUM_PROCS) processes..."
	@echo "========================================"
	@$(MPI_EXEC) -n $(NUM_PROCS) "./$(BUILD_DIR_MPI)/$(DEMO_EXEC)"
	@echo "========================================"
	@echo "Execution completed"

run-nompi: build-nompi
	@echo "Running non-MPI version..."
	@echo "========================================"
	@./$(BUILD_DIR_NOMPI)/$(DEMO_EXEC)
	@echo "========================================"
	@echo "Execution completed"

# Clean targets

clean: clean-mpi clean-nompi
	@echo "All builds cleaned"

clean-mpi:
	@if [ -d "$(BUILD_DIR_MPI)" ]; then \
		echo "Cleaning MPI build..."; \
		rm -rf "$(BUILD_DIR_MPI)"; \
		echo "MPI build cleaned"; \
	else \
		echo "No MPI build directory found"; \
	fi

clean-nompi:
	@if [ -d "$(BUILD_DIR_NOMPI)" ]; then \
		echo "Cleaning non-MPI build..."; \
		rm -rf "$(BUILD_DIR_NOMPI)"; \
		echo "Non-MPI build cleaned"; \
	else \
		echo "No non-MPI build directory found"; \
	fi

# Benchmark targets

benchmark: $(BUILD_DIR_MPI)
	@echo "Building benchmark..."
	@cd $(BUILD_DIR_MPI) && $(CMAKE_EXEC) -DVMAFU_USE_MPI=ON .. && $(CMAKE_EXEC) --build . --target benchmark
	@echo "Benchmark built: $(BUILD_DIR_MPI)/$(BENCHMARK_EXEC)"

benchmark-run: benchmark
	@echo "Running benchmark with $(NUM_PROCS) processes..."
	@$(MPI_EXEC) -n $(NUM_PROCS) "./$(BUILD_DIR_MPI)/$(BENCHMARK_EXEC)" > $(BENCHMARK_OUTPUT)
	@echo "Results saved to $(BENCHMARK_OUTPUT)"

benchmark-all: benchmark
	@echo "Running benchmark for 1-9 processes..."
	@echo "processes|distribution|load_time|distribute_time|compute_time|total_time|verified" > $(BENCHMARK_OUTPUT)
	@echo "---------|------------|---------|---------------|------------|----------|--------" >> $(BENCHMARK_OUTPUT)
	@for i in 1 2 3 4 5 6 7 8 9; do \
		echo "Running with $$i processes..."; \
		$(MPI_EXEC) -n $$i "./$(BUILD_DIR_MPI)/$(BENCHMARK_EXEC)" | tail -n +3 >> $(BENCHMARK_OUTPUT); \
	done
	@echo "All results saved to $(BENCHMARK_OUTPUT)"

plot:
	@echo "Generating plots..."
	@cd $(BENCHMARK_DIR) && python python/plot_benchmark.py

generate-data:
	@echo "Generating test data (matrix size: $(DATA_SIZE)x$(DATA_SIZE))..."
	@python $(BENCHMARK_DIR)/python/generate_data.py --size $(DATA_SIZE)

report: plot
	@echo "Generating Excel report..."
	@cd $(BENCHMARK_DIR) && python python/export_to_excel.py

excel:
	@echo "Generating Excel report..."
	@cd $(BENCHMARK_DIR) && python python/export_to_excel.py

compare:
	@echo "Usage: make compare CONFIG1='Name1:path1.txt' CONFIG2='Name2:path2.txt'"
	@cd $(BENCHMARK_DIR) && python python/compare_configs.py --config1 "$(CONFIG1)" --config2 "$(CONFIG2)"
