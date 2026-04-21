#!/usr/bin/env python3
"""
Compare benchmark results from different configurations.
Example: Single PC vs Local Network (9 PCs)

Usage:
    python compare_configs.py
        --config1 "Single PC:benchmark/results_single.txt"
        --config2 "Network:benchmark/results_network.txt"
"""

import os
import argparse
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots


def parse_results_file(filepath: str) -> pd.DataFrame:
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
                operation = "Matrix x Matrix" if "x" in dist else "Matrix x Vector"

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
            except:
                continue

    return pd.DataFrame(data)


def calculate_summary(df: pd.DataFrame) -> pd.DataFrame:
    mm_df = df[df["operation"] == "Matrix x Matrix"].copy()

    summary = []
    baseline = mm_df[mm_df["processes"] == 1]["compute_time"].mean()

    for n_procs in sorted(mm_df["processes"].unique()):
        proc_data = mm_df[mm_df["processes"] == n_procs]

        avg_compute = proc_data["compute_time"].mean()
        avg_total = proc_data["total_time"].mean()
        avg_distribute = proc_data["distribute_time"].mean()

        summary.append(
            {
                "processes": n_procs,
                "compute_time": avg_compute,
                "total_time": avg_total,
                "distribute_time": avg_distribute,
                "speedup": baseline / avg_compute if avg_compute > 0 else 0,
                "efficiency": (
                    (baseline / avg_compute) / n_procs * 100 if avg_compute > 0 else 0
                ),
            }
        )

    return pd.DataFrame(summary)


def create_comparison_plot(configs: dict, output_file: str):
    fig = make_subplots(
        rows=2,
        cols=2,
    )

    # Цвета в HEX формате для надёжного отображения
    default_colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
    default_symbols = ["circle", "square", "triangle-up", "diamond", "cross", "x"]

    for idx, (config_name, data) in enumerate(configs.items()):
        summary = calculate_summary(data)
        color = default_colors[idx % len(default_colors)]
        symbol = default_symbols[idx % len(default_symbols)]

        fig.add_trace(
            go.Scatter(
                x=summary["processes"],
                y=summary["compute_time"],
                mode="lines+markers",
                name=config_name,
                marker=dict(color=color, symbol=symbol, size=8),
                line=dict(color=color, width=3),
            ),
            row=1,
            col=1,
        )

        fig.add_trace(
            go.Scatter(
                x=summary["processes"],
                y=summary["speedup"],
                mode="lines+markers",
                name=config_name,
                marker=dict(color=color, symbol=symbol, size=8),
                line=dict(color=color, width=3),
                showlegend=False,
            ),
            row=1,
            col=2,
        )

        fig.add_trace(
            go.Scatter(
                x=summary["processes"],
                y=summary["total_time"],
                mode="lines+markers",
                name=config_name,
                marker=dict(color=color, symbol=symbol, size=8),
                line=dict(color=color, width=3),
                showlegend=False,
            ),
            row=2,
            col=1,
        )

        fig.add_trace(
            go.Scatter(
                x=summary["processes"],
                y=summary["efficiency"],
                mode="lines+markers",
                name=config_name,
                marker=dict(color=color, symbol=symbol, size=8),
                line=dict(color=color, width=3),
                showlegend=False,
            ),
            row=2,
            col=2,
        )

    fig.update_layout(
        title="Configuration Comparison",
        height=800,
        hovermode="x unified",
        plot_bgcolor="white",
        showlegend=True,
        legend=dict(
            bgcolor="white",
            bordercolor="black",
            borderwidth=1,
        ),
    )

    # Добавляем сетку для лучшей читаемости
    for row in [1, 2]:
        for col in [1, 2]:
            fig.update_xaxes(
                title_text="Processes",
                row=row,
                col=col,
                gridcolor="lightgray",
                zerolinecolor="gray",
            )
            fig.update_yaxes(
                gridcolor="lightgray",
                zerolinecolor="gray",
            )

    fig.update_yaxes(title_text="Time (s)", row=1, col=1)
    fig.update_yaxes(title_text="Speedup (x)", row=1, col=2)
    fig.update_yaxes(title_text="Time (s)", row=2, col=1)
    fig.update_yaxes(title_text="Efficiency (%)", row=2, col=2)

    fig.write_html(output_file)


def create_network_overhead_plot(configs: dict, output_file: str):
    default_colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]

    fig = go.Figure()

    for idx, (config_name, data) in enumerate(configs.items()):
        summary = calculate_summary(data)
        color = default_colors[idx % len(default_colors)]

        baseline_distribute = summary[summary["processes"] == 1][
            "distribute_time"
        ].values
        baseline_distribute = (
            baseline_distribute[0] if len(baseline_distribute) > 0 else 0
        )

        summary["network_overhead"] = summary["distribute_time"] - baseline_distribute
        summary["network_overhead_percent"] = (
            summary["network_overhead"] / summary["total_time"] * 100
        ).clip(lower=0)

        fig.add_trace(
            go.Scatter(
                x=summary["processes"],
                y=summary["network_overhead_percent"],
                mode="lines+markers",
                name=config_name,
                marker=dict(color=color, size=10),
                line=dict(color=color, width=3),
            )
        )

    fig.update_layout(
        title="Network Overhead: Distribution Time as % of Total",
        xaxis_title="Processes",
        yaxis_title="Network Overhead (%)",
        height=500,
        hovermode="x unified",
        plot_bgcolor="white",
        showlegend=True,
        legend=dict(
            bgcolor="white",
            bordercolor="black",
            borderwidth=1,
        ),
    )

    fig.update_xaxes(gridcolor="lightgray", zerolinecolor="gray")
    fig.update_yaxes(gridcolor="lightgray", zerolinecolor="gray")

    fig.write_html(output_file)


def main():
    parser = argparse.ArgumentParser(description="Compare benchmark configurations")
    parser.add_argument(
        "--config1",
        type=str,
        required=True,
        help='Config 1: "Name:path/to/results.txt"',
    )
    parser.add_argument(
        "--config2",
        type=str,
        required=True,
        help='Config 2: "Name:path/to/results.txt"',
    )
    parser.add_argument(
        "--output-dir",
        type=str,
        default="plots",
        help="Output directory for plots (default: plots)",
    )

    args = parser.parse_args()

    configs = {}
    for config_str in [args.config1, args.config2]:
        name, path = config_str.split(":")
        if not os.path.exists(path):
            print(f"Error: {path} not found!")
            continue
        configs[name] = parse_results_file(path)

    if len(configs) < 2:
        print("Need at least 2 valid configurations to compare")
        return

    os.makedirs(args.output_dir, exist_ok=True)

    create_comparison_plot(
        configs, os.path.join(args.output_dir, "config_comparison.html")
    )
    # create_network_overhead_plot(
    #     configs, os.path.join(args.output_dir, "network_overhead.html")
    # )

    print("\nComparison Summary:")
    for name, data in configs.items():
        summary = calculate_summary(data)
        print(
            f"  {name}:\n    Best speedup: {summary['speedup'].max():.2f}x\n    Best efficiency: {summary['efficiency'].max():.1f}%"
        )

    print(
        f"\nPlots saved to {args.output_dir}/\n  config_comparison.html  - Side-by-side comparison"
    )


if __name__ == "__main__":
    main()
