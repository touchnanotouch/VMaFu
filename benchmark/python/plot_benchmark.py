import os
from datetime import datetime

import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots


def parse_benchmark_results(filepath: str) -> pd.DataFrame:
    data = []

    with open(filepath, "r") as f:
        lines = f.readlines()

    for line in lines[2:]:
        line = line.strip()
        if not line or line.startswith("-"):
            continue

        parts = line.split("|")
        # Ожидаем 7 колонок согласно новому формату[cite: 2]
        if len(parts) == 7:
            try:
                dist = parts[1]
                # Определяем тип операции по названию распределения
                if "x" in dist:
                    operation = "Matrix x Matrix"
                elif dist in ["ROWS", "COLS", "2D"]:
                    operation = "Matrix x Vector"
                else:
                    operation = "Unknown"

                data.append({
                    "processes": int(parts[0]),
                    "operation": operation,
                    "distribution": dist,
                    "load_time": float(parts[2]),
                    "distribute_time": float(parts[3]),
                    "compute_time": float(parts[4]),
                    "total_time": float(parts[5]),
                    "verified": parts[6] == "OK",
                })
            except (ValueError, IndexError):
                continue

    return pd.DataFrame(data)


def create_compute_time_plot(df: pd.DataFrame, output_dir: str):
    mm_df = df[df["operation"] == "Matrix x Matrix"].copy()
    mv_df = df[df["operation"] == "Matrix x Vector"].copy()

    compute_avg_mm = mm_df.groupby("processes")["compute_time"].mean().reset_index()
    compute_std_mm = mm_df.groupby("processes")["compute_time"].std().reset_index()

    compute_avg_mv = mv_df.groupby("processes")["compute_time"].mean().reset_index()
    compute_std_mv = mv_df.groupby("processes")["compute_time"].std().reset_index()

    fig = go.Figure()

    # Matrix x Matrix
    fig.add_trace(
        go.Scatter(
            x=compute_avg_mm["processes"],
            y=compute_avg_mm["compute_time"],
            mode="lines+markers",
            name="Matrix x Matrix",
            marker=dict(color="#1f77b4", size=10),
            line=dict(width=3, color="#1f77b4"),
            error_y=dict(type="data", array=compute_std_mm["compute_time"], visible=True),
        )
    )

    # Matrix x Vector
    fig.add_trace(
        go.Scatter(
            x=compute_avg_mv["processes"],
            y=compute_avg_mv["compute_time"],
            mode="lines+markers",
            name="Matrix x Vector",
            marker=dict(color="#ff7f0e", size=10),
            line=dict(width=3, color="#ff7f0e"),
            error_y=dict(type="data", array=compute_std_mv["compute_time"], visible=True),
        )
    )

    # Ideal scaling для Matrix x Matrix
    baseline_mm = compute_avg_mm.loc[compute_avg_mm["processes"] == 1, "compute_time"].values[0]
    ideal_mm = [baseline_mm / n for n in compute_avg_mm["processes"]]

    fig.add_trace(
        go.Scatter(
            x=compute_avg_mm["processes"],
            y=ideal_mm,
            mode="lines",
            name="Ideal MM (T₁ / N)",
            line=dict(color="#2ca02c", width=2, dash="dash"),
        )
    )

    # Ideal scaling для Matrix x Vector
    baseline_mv = compute_avg_mv.loc[compute_avg_mv["processes"] == 1, "compute_time"].values[0]
    ideal_mv = [baseline_mv / n for n in compute_avg_mv["processes"]]

    fig.add_trace(
        go.Scatter(
            x=compute_avg_mv["processes"],
            y=ideal_mv,
            mode="lines",
            name="Ideal MV (T₁ / N)",
            line=dict(color="#d62728", width=2, dash="dot"),
        )
    )

    fig.update_layout(
        title="Compute Time vs Number of Processes",
        xaxis_title="Processes",
        yaxis_title="Time (s)",
        height=500,
        hovermode="x unified",
        legend=dict(orientation="h", yanchor="bottom", y=1.02),
        plot_bgcolor="white",
    )

    fig.update_xaxes(gridcolor="lightgray", zerolinecolor="gray")
    fig.update_yaxes(gridcolor="lightgray", zerolinecolor="gray")

    fig.write_html(os.path.join(output_dir, "01_compute_time.html"))


def create_speedup_plot(df: pd.DataFrame, output_dir: str):
    mm_df = df[df["operation"] == "Matrix x Matrix"].copy()
    mv_df = df[df["operation"] == "Matrix x Vector"].copy()

    compute_avg_mm = mm_df.groupby("processes")["compute_time"].mean().reset_index()
    compute_avg_mv = mv_df.groupby("processes")["compute_time"].mean().reset_index()

    baseline_mm = compute_avg_mm.loc[compute_avg_mm["processes"] == 1, "compute_time"].values[0]
    baseline_mv = compute_avg_mv.loc[compute_avg_mv["processes"] == 1, "compute_time"].values[0]

    compute_avg_mm["speedup"] = baseline_mm / compute_avg_mm["compute_time"]
    compute_avg_mv["speedup"] = baseline_mv / compute_avg_mv["compute_time"]

    fig = go.Figure()

    # Matrix x Matrix
    fig.add_trace(
        go.Bar(
            x=compute_avg_mm["processes"],
            y=compute_avg_mm["speedup"],
            name="Matrix x Matrix",
            marker_color="#1f77b4",
            opacity=0.7,
        )
    )

    # Matrix x Vector
    fig.add_trace(
        go.Bar(
            x=compute_avg_mv["processes"],
            y=compute_avg_mv["speedup"],
            name="Matrix x Vector",
            marker_color="#ff7f0e",
            opacity=0.7,
        )
    )

    # Ideal scaling
    fig.add_trace(
        go.Scatter(
            x=compute_avg_mm["processes"],
            y=list(range(1, 10)),
            name="Ideal (N)",
            mode="lines",
            line=dict(color="#2ca02c", width=3, dash="dash"),
        )
    )

    fig.update_layout(
        title="Speedup vs Number of Processes",
        xaxis_title="Processes",
        yaxis_title="Speedup (x)",
        height=500,
        yaxis=dict(range=[0, max(9.5, compute_avg_mm["speedup"].max() * 1.2, compute_avg_mv["speedup"].max() * 1.2)]),
        plot_bgcolor="white",
        barmode="group",
    )

    fig.update_xaxes(gridcolor="lightgray", zerolinecolor="gray")
    fig.update_yaxes(gridcolor="lightgray", zerolinecolor="gray")

    fig.write_html(os.path.join(output_dir, "02_speedup.html"))


def create_efficiency_plot(df: pd.DataFrame, output_dir: str):
    mm_df = df[df["operation"] == "Matrix x Matrix"].copy()
    mv_df = df[df["operation"] == "Matrix x Vector"].copy()

    compute_avg_mm = mm_df.groupby("processes")["compute_time"].mean().reset_index()
    compute_avg_mv = mv_df.groupby("processes")["compute_time"].mean().reset_index()

    baseline_mm = compute_avg_mm.loc[compute_avg_mm["processes"] == 1, "compute_time"].values[0]
    baseline_mv = compute_avg_mv.loc[compute_avg_mv["processes"] == 1, "compute_time"].values[0]

    compute_avg_mm["efficiency"] = (baseline_mm / compute_avg_mm["compute_time"]) / compute_avg_mm["processes"]
    compute_avg_mv["efficiency"] = (baseline_mv / compute_avg_mv["compute_time"]) / compute_avg_mv["processes"]

    fig = go.Figure()

    # Matrix x Matrix
    fig.add_trace(
        go.Bar(
            x=compute_avg_mm["processes"],
            y=compute_avg_mm["efficiency"] * 100,
            name="Matrix x Matrix",
            marker_color="#1f77b4",
            opacity=0.7,
        )
    )

    # Matrix x Vector
    fig.add_trace(
        go.Bar(
            x=compute_avg_mv["processes"],
            y=compute_avg_mv["efficiency"] * 100,
            name="Matrix x Vector",
            marker_color="#ff7f0e",
            opacity=0.7,
        )
    )

    fig.add_hline(
        y=100,
        line_dash="dash",
        line_color="#d62728",
        annotation_text="Ideal (100%)",
        annotation_position="top",
    )

    fig.add_hline(y=50, line_dash="dot", line_color="#ff7f0e", opacity=0.5)

    fig.update_layout(
        title="Parallel Efficiency",
        xaxis_title="Processes",
        yaxis_title="Efficiency (%)",
        height=500,
        yaxis=dict(range=[0, 110]),
        plot_bgcolor="white",
        barmode="group",
    )

    fig.update_xaxes(gridcolor="lightgray", zerolinecolor="gray")
    fig.update_yaxes(gridcolor="lightgray", zerolinecolor="gray")

    fig.write_html(os.path.join(output_dir, "03_efficiency.html"))


def create_overhead_plot(df: pd.DataFrame, output_dir: str):
    mm_df = df[df["operation"] == "Matrix x Matrix"].copy()
    mv_df = df[df["operation"] == "Matrix x Vector"].copy()

    grouped_mm = (
        mm_df.groupby("processes")[
            ["load_time", "distribute_time", "compute_time"]
        ]
        .sum()
        .reset_index()
    )

    grouped_mv = (
        mv_df.groupby("processes")[
            ["load_time", "distribute_time", "compute_time"]
        ]
        .sum()
        .reset_index()
    )

    grouped_mm["overhead"] = (
        grouped_mm["load_time"] + grouped_mm["distribute_time"]
    )
    grouped_mm["useful_ratio"] = (
        grouped_mm["compute_time"]
        / grouped_mm[["load_time", "distribute_time", "compute_time"]].sum(
            axis=1
        )
        * 100
    )

    grouped_mv["overhead"] = (
        grouped_mv["load_time"] + grouped_mv["distribute_time"]
    )
    grouped_mv["useful_ratio"] = (
        grouped_mv["compute_time"]
        / grouped_mv[["load_time", "distribute_time", "compute_time"]].sum(
            axis=1
        )
        * 100
    )

    fig = make_subplots(
        rows=1,
        cols=2,
        vertical_spacing=0.1,
    )

    # Matrix x Matrix stacked bars
    fig.add_trace(
        go.Bar(
            x=grouped_mm["processes"],
            y=grouped_mm["load_time"],
            name="MM Load",
            marker_color="#1f77b4",
            offsetgroup="mm",
        ),
        row=1,
        col=1,
    )

    fig.add_trace(
        go.Bar(
            x=grouped_mm["processes"],
            y=grouped_mm["distribute_time"],
            name="MM Distribute",
            marker_color="#ff7f0e",
            offsetgroup="mm",
        ),
        row=1,
        col=1,
    )

    fig.add_trace(
        go.Bar(
            x=grouped_mm["processes"],
            y=grouped_mm["compute_time"],
            name="MM Compute",
            marker_color="#2ca02c",
            offsetgroup="mm",
        ),
        row=1,
        col=1,
    )

    # Matrix x Vector stacked bars
    fig.add_trace(
        go.Bar(
            x=grouped_mv["processes"],
            y=grouped_mv["load_time"],
            name="MV Load",
            marker_color="#9467bd",
            offsetgroup="mv",
        ),
        row=1,
        col=1,
    )

    fig.add_trace(
        go.Bar(
            x=grouped_mv["processes"],
            y=grouped_mv["distribute_time"],
            name="MV Distribute",
            marker_color="#8c564b",
            offsetgroup="mv",
        ),
        row=1,
        col=1,
    )

    fig.add_trace(
        go.Bar(
            x=grouped_mv["processes"],
            y=grouped_mv["compute_time"],
            name="MV Compute",
            marker_color="#e377c2",
            offsetgroup="mv",
        ),
        row=1,
        col=1,
    )

    # Useful ratio lines
    fig.add_trace(
        go.Scatter(
            x=grouped_mm["processes"],
            y=grouped_mm["useful_ratio"],
            mode="lines+markers",
            name="MM Useful %",
            marker=dict(color="#2ca02c", size=8),
            line=dict(width=3, color="#2ca02c"),
        ),
        row=1,
        col=2,
    )

    fig.add_trace(
        go.Scatter(
            x=grouped_mv["processes"],
            y=grouped_mv["useful_ratio"],
            mode="lines+markers",
            name="MV Useful %",
            marker=dict(color="#ff7f0e", size=8),
            line=dict(width=3, color="#ff7f0e"),
        ),
        row=1,
        col=2,
    )

    fig.update_layout(
        title="Overhead Analysis: Communication vs Computation",
        height=500,
        showlegend=True,
        legend=dict(orientation="h", yanchor="bottom", y=1.02),
        plot_bgcolor="white",
        barmode="stack",
    )

    fig.update_xaxes(title_text="Processes", row=1, col=1, gridcolor="lightgray", zerolinecolor="gray")
    fig.update_xaxes(title_text="Processes", row=1, col=2, gridcolor="lightgray", zerolinecolor="gray")
    fig.update_yaxes(title_text="Time (s)", row=1, col=1, gridcolor="lightgray", zerolinecolor="gray")
    fig.update_yaxes(title_text="Useful Work (%)", row=1, col=2, range=[0, 100], gridcolor="lightgray", zerolinecolor="gray")

    fig.write_html(os.path.join(output_dir, "04_overhead.html"))


def create_distribution_comparison(df: pd.DataFrame, output_dir: str):
    mm_df = df[df["operation"] == "Matrix x Matrix"].copy()
    mv_df = df[df["operation"] == "Matrix x Vector"].copy()

    fig = go.Figure()

    # Цвета для Matrix x Matrix стратегий
    mm_colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
    # Цвета для Matrix x Vector стратегий
    mv_colors = ["#1f77b4", "#ff7f0e", "#2ca02c"]

    # Matrix x Matrix стратегии
    for i, dist in enumerate(sorted(mm_df["distribution"].unique())):
        dist_data = mm_df[mm_df["distribution"] == dist]
        avg_time = dist_data.groupby("processes")["compute_time"].mean().reset_index()

        fig.add_trace(
            go.Scatter(
                x=avg_time["processes"],
                y=avg_time["compute_time"],
                mode="lines+markers",
                name=f"MM: {dist}",
                marker=dict(color=mm_colors[i % len(mm_colors)], size=8),
                line=dict(width=3, color=mm_colors[i % len(mm_colors)]),
            )
        )

    # Matrix x Vector стратегии
    for i, dist in enumerate(sorted(mv_df["distribution"].unique())):
        dist_data = mv_df[mv_df["distribution"] == dist]
        avg_time = dist_data.groupby("processes")["compute_time"].mean().reset_index()

        fig.add_trace(
            go.Scatter(
                x=avg_time["processes"],
                y=avg_time["compute_time"],
                mode="lines+markers",
                name=f"MV: {dist}",
                marker=dict(color=mv_colors[i % len(mv_colors)], size=10),
                line=dict(width=3, color=mv_colors[i % len(mv_colors)], dash="dot"),
            )
        )

    fig.update_layout(
        title="Distribution Strategy Comparison (MM=Matrix x Matrix, MV=Matrix x Vector)",
        xaxis_title="Processes",
        yaxis_title="Compute Time (s)",
        height=500,
        legend=dict(orientation="h", yanchor="bottom", y=1.02),
        plot_bgcolor="white",
    )

    fig.update_xaxes(gridcolor="lightgray", zerolinecolor="gray")
    fig.update_yaxes(gridcolor="lightgray", zerolinecolor="gray")

    fig.write_html(os.path.join(output_dir, "05_distribution.html"))


def main():
    # Paths are relative to benchmark/ directory (where script runs from)
    results_file = "results.txt"
    output_dir = "plots"

    os.makedirs(output_dir, exist_ok=True)

    if not os.path.exists(results_file):
        print(
            f"Error: {results_file} not found!\nRun 'make benchmark-all' first to generate benchmark data."
        )
        return

    df = parse_benchmark_results(results_file)

    if df.empty:
        print("Error: No valid data found in results file!")
        return

    print(
        f"Parsed {len(df)} benchmark records\nProcesses: {sorted(df['processes'].unique())}\nOperations: {df['operation'].unique().tolist()}"
    )

    create_compute_time_plot(df, output_dir)
    create_speedup_plot(df, output_dir)
    create_efficiency_plot(df, output_dir)
    create_overhead_plot(df, output_dir)
    create_distribution_comparison(df, output_dir)

    print(f"\nPlots saved to {output_dir}/")


if __name__ == "__main__":
    main()
