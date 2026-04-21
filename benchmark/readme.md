# MPI Matrix Multiplication Benchmark

Benchmark для сравнения производительности умножения матриц на разных количествах процессов (1-9) и с разными стратегиями распределения данных.

## Quick Start

```bash
# 1. Generate test data (default: 500x500 matrices)
make generate-data MATRIX_SIZE=1000

# 2. Run benchmarks
make benchmark-all

# 3. Generate plots and reports
make plot
make excel
```

## Commands

| Command | Description |
|---------|-------------|
| `make generate-data MATRIX_SIZE=1000` | Generate test matrices |
| `make benchmark-all` | Run benchmarks 1-9 processes |
| `make benchmark-run NUM_PROCS=4` | Run specific process count |
| `make plot` | Generate HTML visualization |
| `make excel` | Excel report |
| `make compare CONFIG1="A:file1.txt" CONFIG2="B:file2.txt"` | Compare configs |

## Output Files

| File | Description |
|------|-------------|
| `results.txt` | Raw benchmark data |
| `plots/*.html` | Interactive visualizations |
| `benchmark_report.xlsx` | Excel summary |

## Visualization

| Plot | Purpose |
|------|---------|
| `01_compute_time.html` | Compute time vs processes (with ideal line) |
| `02_speedup.html` | Speedup comparison |
| `03_efficiency.html` | Parallel efficiency |
| `04_overhead.html` | Communication vs computation breakdown |
| `05_distribution.html` | ROWS vs COLS vs 2D strategies |

## Formulas

Для анализа производительности в скриптах визуализации используются следующие метрики на основе `compute_time` (время чистых вычислений без учета I/O):

### Speedup (Ускорение)
Показывает, во сколько раз параллельная версия быстрее последовательной.
$$Speedup(N) = \frac{T_{compute}(1)}{T_{compute}(N)}$$
* **Идеал**: $Speedup = N$ (линейное ускорение).
* В коде используется среднее значение `compute_time` для всех запусков на $N$ процессах.

### Parallel Efficiency (Эффективность)
Показывает долю времени, в течение которой процессы выполняли полезную работу.
$$Efficiency(N) = \frac{Speedup(N)}{N} \times 100\%$$
* **Идеал**: $100\%$.
* Отражает, насколько эффективно масштабируется алгоритм при добавлении новых узлов.

### Overhead Analysis (Анализ накладных расходов)
* **Communication Time (Overhead)**:
  $$T_{overhead} = T_{load} + T_{distribute}$$
* **Useful Work %**:
  $$Ratio = \frac{T_{compute}}{T_{load} + T_{distribute} + T_{compute}} \times 100\%$$

## Expected Results (1000 x 1000 matrices)

| Processes | Speedup | Efficiency |
|-----------|---------|------------|
| 1 | 1x | 100% |
| 4 | ~3.5x | ~88% |
| 6 | ~5x | ~83% |
| 9 | ~4.5x | ~50% |

**Note**: После 6 процессов ускорение замедляется (закон Амдала).

## Requirements

```bash
pip install pandas plotly openpyxl
```

## For Lab (9 PC Network)

```bash
# Baseline (single PC)
make benchmark-all
cp results.txt results_single.txt

# Network test (9 PCs)
mpiexec -n 9 -hosts PC1,...,PC9 ./build_mpi/benchmark.exe > results_network.txt

# Compare
make compare CONFIG1="Single:results_single.txt" CONFIG2="Network:results_network.txt"
```
