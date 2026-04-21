import os
from datetime import datetime

import pandas as pd


def parse_benchmark_results(filepath: str) -> pd.DataFrame:
    data = []

    with open(filepath, "r") as f:
        lines = f.readlines()

    for line in lines[2:]:
        line = line.strip()
        if not line or line.startswith("-"):
            continue

        parts = line.split("|")
        if len(parts) >= 7:
            try:
                dist = parts[1]
                if "x" in dist:
                    operation = "Matrix x Matrix"
                elif dist in ["ROWS", "COLS", "2D"]:
                    operation = "Matrix x Vector"
                else:
                    operation = "Unknown"

                data.append(
                    {
                        "processes": int(parts[0]),
                        "operation": operation,
                        "distribution": parts[1],
                        "load_time": float(parts[2]),
                        "distribute_time": float(parts[3]),
                        "compute_time": float(parts[4]),
                        "total_time": float(parts[5]),
                        "verified": parts[6] == "OK",
                    }
                )
            except (ValueError, IndexError):
                continue

    return pd.DataFrame(data)


def calculate_metrics(df: pd.DataFrame) -> pd.DataFrame:
    mm_df = df[df["operation"] == "Matrix x Matrix"].copy()

    metrics = []

    for n_procs in sorted(mm_df["processes"].unique()):
        proc_data = mm_df[mm_df["processes"] == n_procs]

        avg_load = proc_data["load_time"].mean()
        avg_distribute = proc_data["distribute_time"].mean()
        avg_compute = proc_data["compute_time"].mean()
        avg_total = proc_data["total_time"].mean()

        std_compute = proc_data["compute_time"].std()

        baseline_compute = mm_df[mm_df["processes"] == 1]["compute_time"].mean()
        speedup = baseline_compute / avg_compute
        efficiency = speedup / n_procs * 100

        communication_time = avg_load + avg_distribute
        useful_ratio = avg_compute / avg_total * 100
        overhead_ratio = communication_time / avg_total * 100

        metrics.append(
            {
                "processes": n_procs,
                "compute_time_avg": avg_compute,
                "compute_time_std": std_compute,
                "total_time_avg": avg_total,
                "load_time_avg": avg_load,
                "distribute_time_avg": avg_distribute,
                "communication_time": communication_time,
                "speedup": speedup,
                "efficiency": efficiency,
                "useful_work_percent": useful_ratio,
                "overhead_percent": overhead_ratio,
            }
        )

    return pd.DataFrame(metrics)


def create_distribution_summary(df: pd.DataFrame) -> pd.DataFrame:
    mm_df = df[df["operation"] == "Matrix x Matrix"].copy()

    summary = []

    for dist in mm_df["distribution"].unique():
        dist_data = mm_df[mm_df["distribution"] == dist]

        for n_procs in sorted(dist_data["processes"].unique()):
            proc_data = dist_data[dist_data["processes"] == n_procs]

            summary.append(
                {
                    "processes": n_procs,
                    "distribution": dist,
                    "compute_time_avg": proc_data["compute_time"].mean(),
                    "compute_time_min": proc_data["compute_time"].min(),
                    "compute_time_max": proc_data["compute_time"].max(),
                    "total_time_avg": proc_data["total_time"].mean(),
                    "speedup": mm_df[mm_df["processes"] == 1]["compute_time"].mean()
                    / proc_data["compute_time"].mean(),
                }
            )

    return pd.DataFrame(summary)


def export_to_excel(
    df: pd.DataFrame,
    metrics: pd.DataFrame,
    dist_summary: pd.DataFrame,
    output_file: str
):
    with pd.ExcelWriter(output_file, engine="openpyxl") as writer:
        df.to_excel(writer, sheet_name="Raw Data", index=False)
        metrics.to_excel(writer, sheet_name="Summary Metrics", index=False)
        dist_summary.to_excel(writer, sheet_name="Distribution Comparison", index=False)

        best_speedup_procs = int(metrics.loc[metrics['speedup'].idxmax(), 'processes'])
        best_efficiency_procs = int(metrics.loc[metrics['efficiency'].idxmax(), 'processes'])
        optimal_procs = int(metrics.loc[metrics['total_time_avg'].idxmin(), 'processes'])

        analysis_data = {
            "Metric": [
                "Data Size",
                "Best Speedup",
                "Best Efficiency",
                "Optimal Processes",
                "Avg Compute Time (1 proc)",
                "Avg Compute Time (best)",
                "Communication Overhead",
            ],
            "Value": [
                f"{df.shape[0]}",
                f"{metrics['speedup'].max():.2f}x (on {best_speedup_procs}p)",
                f"{metrics['efficiency'].max():.1f}% (on {best_efficiency_procs}p)",
                f"{optimal_procs} (lowest total time)",
                f"{metrics[metrics['processes'] == 1]['compute_time_avg'].values[0]:.3f} s",
                f"{metrics['compute_time_avg'].min():.3f} s",
                f"{metrics['overhead_percent'].mean():.1f}%",
            ],
        }
        pd.DataFrame(analysis_data).to_excel(writer, sheet_name="Analysis", index=False)

        workbook = writer.book
        for sheet_name in workbook.sheetnames:
            worksheet = workbook[sheet_name]
            for column in worksheet.columns:
                max_length = 0
                column_letter = column[0].column_letter
                for cell in column:
                    try:
                        if len(str(cell.value)) > max_length:
                            max_length = len(str(cell.value))
                    except Exception:
                        pass
                adjusted_width = min(max_length + 2, 50)
                worksheet.column_dimensions[column_letter].width = adjusted_width

    print(f"Excel report saved to: {output_file}")


def main():
    results_file = "results.txt"
    output_file = "benchmark_report.xlsx"

    if not os.path.exists(results_file):
        print(f"Error: {results_file} not found!\nRun 'make benchmark-all' first.")
        return

    df = parse_benchmark_results(results_file)

    if df.empty:
        print("Error: No valid data found!")
        return

    metrics = calculate_metrics(df)
    dist_summary = create_distribution_summary(df)
    export_to_excel(df, metrics, dist_summary, output_file)

    best_speedup_procs = int(metrics.loc[metrics['speedup'].idxmax(), 'processes'])
    best_efficiency_procs = int(metrics.loc[metrics['efficiency'].idxmax(), 'processes'])
    optimal_procs = int(metrics.loc[metrics['total_time_avg'].idxmin(), 'processes'])

    summary_lines = [
        f"Best speedup: {metrics['speedup'].max():.2f}x (on {best_speedup_procs} processes)",
        f"Best efficiency: {metrics['efficiency'].max():.1f}% (on {best_efficiency_procs} processes)",
        f"Optimal processes (lowest total time): {optimal_procs}",
    ]
    print(f"\nSummary:\n  " + "\n  ".join(summary_lines) + f"\n\nOpen {output_file} to view full report")


if __name__ == "__main__":
    main()
