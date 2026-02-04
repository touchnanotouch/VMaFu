.PHONY: help build build-mpi build-nompi run run-mpi run-nompi clean clean-mpi clean-nompi info check-mpi

# Configuration

BUILD_DIR_MPI = build_mpi
BUILD_DIR_NOMPI = build_nompi
NUM_PROCS = 4

DEMO_EXEC = demo.exe
MPIEXEC = "C:/Program Files/Microsoft MPI/Bin/mpiexec.exe"

BUILD_TYPE ?= mpi

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
	@echo "  MPI executor: $(MPIEXEC)"
	@echo "  Processes: $(NUM_PROCS)"

# Build targets

build:
	@$(MAKE) build-$(BUILD_TYPE)

build-mpi: $(BUILD_DIR_MPI)
	@echo "Building MPI version..."
	@cd $(BUILD_DIR_MPI) && cmake -DUSE_MPI=ON .. && cmake --build .
	@echo "Build completed: $(BUILD_DIR_MPI)/$(DEMO_EXEC)"

build-nompi: $(BUILD_DIR_NOMPI)
	@echo "Building non-MPI version..."
	@cd $(BUILD_DIR_NOMPI) && cmake -DUSE_MPI=OFF .. && cmake --build .
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
	@$(MPIEXEC) -n $(NUM_PROCS) "./$(BUILD_DIR_MPI)/$(DEMO_EXEC)"
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
