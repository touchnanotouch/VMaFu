#!/usr/bin/env python3
"""
Analysis of scalability based on three datasets (500, 1000, 2000).
Shows how well results match parallel computing theory.

Usage:
    python compare_three.py [--output-dir OUTPUT_DIR]
"""

import os
import argparse
import pandas as pd
import numpy as np
import plotly.graph_objects as go
from plotly.subplots import make_subplots


def parse_results(filepath: str, size: int) -> pd.DataFrame:
    """Parse results from results{size}.txt"""
    data = []
    with open(filepath, 'r') as f:
        lines = f.readlines()

    for line in lines[2:]:
        line = line.strip()
        if not line or line.startswith('-'):
            continue
        parts = line.split('|')
        if len(parts) >= 7:
            try:
                dist = parts[1]
                operation = "Matrix x Matrix" if 'x' in dist else "Matrix x Vector"
                data.append({
                    'size': size,
                    'processes': int(parts[0]),
                    'operation': operation,
                    'distribution': dist,
                    'load_time': float(parts[2]),
                    'distribute_time': float(parts[3]),
                    'compute_time': float(parts[4]),
                    'total_time': float(parts[5]),
                })
            except:
                continue
    return pd.DataFrame(data)


def load_all_results():
    """Load all three files"""
    sizes = [500, 1000, 2000]
    all_data = []
    for size in sizes:
        filename = f'benchmark/results{size}.txt'
        if os.path.exists(filename):
            df = parse_results(filename, size)
            all_data.append(df)
            print(f"Loaded {filename}: {len(df)} records")
        else:
            print(f"Warning: {filename} not found")
    return pd.concat(all_data, ignore_index=True)


def compute_theoretical_complexity(df: pd.DataFrame) -> pd.DataFrame:
    """Add theoretical time for realism check"""
    # Take baseline time for 500x500
    baseline_mm = df[(df['size'] == 500) & (df['operation'] == 'Matrix x Matrix')]['compute_time'].mean()
    baseline_mv = df[(df['size'] == 500) & (df['operation'] == 'Matrix x Vector')]['compute_time'].mean()

    def theoretical_time(row):
        if row['operation'] == 'Matrix x Matrix':
            # O(N^3) — cubic scaling
            return baseline_mm * (row['size'] / 500) ** 3
        else:
            # O(N^2) — quadratic scaling
            return baseline_mv * (row['size'] / 500) ** 2

    df['theoretical_compute'] = df.apply(theoretical_time, axis=1)
    return df


def plot_scalability_comparison(df: pd.DataFrame, output_dir: str):
    """Plot: how compute_time changes with problem size"""
    mm_df = df[df['operation'] == 'Matrix x Matrix'].copy()
    mv_df = df[df['operation'] == 'Matrix x Vector'].copy()

    fig = make_subplots(rows=1, cols=2, subplot_titles=('Matrix x Matrix', 'Matrix x Vector'))

    default_colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
    default_symbols = ["circle", "square", "triangle-up", "diamond", "cross", "x"]
    colors = {500: default_colors[0], 1000: default_colors[1], 2000: default_colors[2]}
    symbols = {500: default_symbols[0], 1000: default_symbols[1], 2000: default_symbols[2]}

    for size in [500, 1000, 2000]:
        size_data = mm_df[mm_df['size'] == size]
        avg_by_proc = size_data.groupby('processes')['compute_time'].mean().reset_index()

        fig.add_trace(
            go.Scatter(
                x=avg_by_proc['processes'],
                y=avg_by_proc['compute_time'],
                mode='lines+markers',
                name=f'{size}x{size}',
                marker=dict(color=colors[size], symbol=symbols[size], size=10),
                line=dict(color=colors[size], width=2),
            ),
            row=1, col=1
        )

        # Add theoretical line for 2000 (if available)
        if size == 2000:
            theo = mm_df[mm_df['size'] == 2000].iloc[0]['theoretical_compute'] if not mm_df.empty else None
            if theo:
                fig.add_hline(y=theo, line_dash='dash', line_color='gray',
                              annotation_text=f'Theoretical 2000: {theo:.1f}s', row=1, col=1)

    for size in [500, 1000, 2000]:
        size_data = mv_df[mv_df['size'] == size]
        avg_by_proc = size_data.groupby('processes')['compute_time'].mean().reset_index()

        fig.add_trace(
            go.Scatter(
                x=avg_by_proc['processes'],
                y=avg_by_proc['compute_time'],
                mode='lines+markers',
                name=f'{size}x{size}',
                marker=dict(color=colors[size], symbol=symbols[size], size=10),
                line=dict(color=colors[size], width=2),
                showlegend=False,
            ),
            row=1, col=2
        )

    fig.update_layout(
        title='Compute Time vs Matrix Size (log scale)',
        height=500,
        yaxis_type='log',
        xaxis_title='Processes',
        plot_bgcolor='white',
        showlegend=True,
        legend=dict(
            bgcolor="white",
            bordercolor="black",
            borderwidth=1,
        ),
    )
    fig.update_yaxes(title_text='Time (s)', row=1, col=1, gridcolor='lightgray', zerolinecolor='gray')
    fig.update_yaxes(title_text='Time (s)', row=1, col=2, gridcolor='lightgray', zerolinecolor='gray')
    fig.update_xaxes(gridcolor='lightgray', zerolinecolor='gray')

    fig.write_html(os.path.join(output_dir, 'size_scalability.html'))
    print(f"  -> size_scalability.html")


def plot_speedup_by_size(df: pd.DataFrame, output_dir: str):
    """Speedup plot for different problem sizes"""
    mm_df = df[df['operation'] == 'Matrix x Matrix'].copy()

    fig = go.Figure()

    default_colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
    colors = {500: default_colors[0], 1000: default_colors[1], 2000: default_colors[2]}

    for size in [500, 1000, 2000]:
        size_data = mm_df[mm_df['size'] == size]
        baseline = size_data[size_data['processes'] == 1]['compute_time'].mean()
        avg_by_proc = size_data.groupby('processes')['compute_time'].mean().reset_index()
        avg_by_proc['speedup'] = baseline / avg_by_proc['compute_time']

        fig.add_trace(
            go.Scatter(
                x=avg_by_proc['processes'],
                y=avg_by_proc['speedup'],
                mode='lines+markers',
                name=f'{size}x{size}',
                marker=dict(color=colors[size], size=10),
                line=dict(color=colors[size], width=2),
            )
        )

    # Ideal speedup
    fig.add_trace(
        go.Scatter(
            x=list(range(1, 10)),
            y=list(range(1, 10)),
            mode='lines',
            name='Ideal (linear)',
            line=dict(color='red', width=2, dash='dash'),
        )
    )

    fig.update_layout(
        title='Speedup: Larger Problems Scale Better',
        xaxis_title='Processes',
        yaxis_title='Speedup (x)',
        height=500,
        plot_bgcolor='white',
        showlegend=True,
        legend=dict(
            bgcolor="white",
            bordercolor="black",
            borderwidth=1,
        ),
        annotations=[
            dict(
                x=5, y=8,
                text='▲ Larger problems -> better speedup<br>▼ Small problems limited by overhead',
                showarrow=True,
                arrowhead=2,
                ax=0, ay=-40
            )
        ]
    )
    fig.update_xaxes(gridcolor='lightgray', zerolinecolor='gray')
    fig.update_yaxes(gridcolor='lightgray', zerolinecolor='gray')

    fig.write_html(os.path.join(output_dir, 'speedup_by_size.html'))
    print(f"  -> speedup_by_size.html")


def plot_efficiency_degradation(df: pd.DataFrame, output_dir: str):
    """Shows efficiency drop (oversubscription)"""
    mm_df = df[df['operation'] == 'Matrix x Matrix'].copy()

    fig = go.Figure()

    default_colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
    colors = {500: default_colors[0], 1000: default_colors[1], 2000: default_colors[2]}

    for size in [500, 1000, 2000]:
        size_data = mm_df[mm_df['size'] == size]
        baseline = size_data[size_data['processes'] == 1]['compute_time'].mean()
        avg_by_proc = size_data.groupby('processes')['compute_time'].mean().reset_index()
        avg_by_proc['efficiency'] = (baseline / avg_by_proc['compute_time']) / avg_by_proc['processes'] * 100

        fig.add_trace(
            go.Scatter(
                x=avg_by_proc['processes'],
                y=avg_by_proc['efficiency'],
                mode='lines+markers',
                name=f'{size}x{size}',
                marker=dict(color=colors[size], size=10),
                line=dict(color=colors[size], width=2),
            )
        )

    # 6 cores / 12 threads marker
    fig.add_vline(x=6, line_dash='dash', line_color='gray', annotation_text='6 cores', annotation_position='top')
    fig.add_vline(x=12, line_dash='dot', line_color='gray', annotation_text='12 threads', annotation_position='top')

    fig.add_hline(y=100, line_dash='dash', line_color='green', annotation_text='100% ideal')

    fig.update_layout(
        title='Parallel Efficiency: Oversubscription after 6 processes',
        xaxis_title='Processes',
        yaxis_title='Efficiency (%)',
        height=500,
        plot_bgcolor='white',
        showlegend=True,
        legend=dict(
            bgcolor="white",
            bordercolor="black",
            borderwidth=1,
        ),
    )
    fig.update_xaxes(gridcolor='lightgray', zerolinecolor='gray')
    fig.update_yaxes(gridcolor='lightgray', zerolinecolor='gray', range=[0, 110])

    fig.write_html(os.path.join(output_dir, 'efficiency_degradation.html'))
    print(f"  -> efficiency_degradation.html")


def plot_communication_overhead(df: pd.DataFrame, output_dir: str):
    """How overhead grows with number of processes"""
    mm_df = df[df['operation'] == 'Matrix x Matrix'].copy()

    fig = make_subplots(rows=1, cols=2, subplot_titles=('Load Time', 'Distribute Time'))

    default_colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b"]
    colors = {500: default_colors[0], 1000: default_colors[1], 2000: default_colors[2]}

    for size in [500, 1000, 2000]:
        size_data = mm_df[mm_df['size'] == size]
        avg_load = size_data.groupby('processes')['load_time'].mean().reset_index()
        avg_dist = size_data.groupby('processes')['distribute_time'].mean().reset_index()

        fig.add_trace(
            go.Scatter(
                x=avg_load['processes'],
                y=avg_load['load_time'],
                mode='lines+markers',
                name=f'{size}x{size}',
                marker=dict(color=colors[size], size=8),
                line=dict(color=colors[size], width=2),
            ),
            row=1, col=1
        )

        fig.add_trace(
            go.Scatter(
                x=avg_dist['processes'],
                y=avg_dist['distribute_time'],
                mode='lines+markers',
                name=f'{size}x{size}',
                marker=dict(color=colors[size], size=8),
                line=dict(color=colors[size], width=2),
                showlegend=False,
            ),
            row=1, col=2
        )

    fig.update_layout(
        title='Communication Overhead vs Problem Size',
        height=500,
        plot_bgcolor='white',
        showlegend=True,
        legend=dict(
            bgcolor="white",
            bordercolor="black",
            borderwidth=1,
        ),
    )
    fig.update_yaxes(title_text='Time (s)', row=1, col=1, gridcolor='lightgray', zerolinecolor='gray')
    fig.update_yaxes(title_text='Time (s)', row=1, col=2, gridcolor='lightgray', zerolinecolor='gray')
    fig.update_xaxes(gridcolor='lightgray', zerolinecolor='gray')

    fig.write_html(os.path.join(output_dir, 'communication_overhead.html'))
    print(f"  -> communication_overhead.html")


def plot_accuracy_check(df: pd.DataFrame, output_dir: str):
    """Accuracy check: does real time match O(N^3) and O(N^2)?"""
    # Take data for 1 process (no parallel noise)
    single_proc = df[df['processes'] == 1].copy()

    fig = make_subplots(rows=1, cols=2, subplot_titles=('Matrix x Matrix', 'Matrix x Vector'))

    # MM: O(N^3)
    mm_data = single_proc[single_proc['operation'] == 'Matrix x Matrix'].copy()
    sizes = sorted(mm_data['size'].unique())
    actual_mm = [mm_data[mm_data['size'] == s]['compute_time'].mean() for s in sizes]
    # Normalize to 500
    actual_mm_norm = [actual_mm[0] * (s/500)**3 for s in sizes]

    fig.add_trace(
        go.Scatter(
            x=sizes,
            y=actual_mm,
            mode='lines+markers',
            name='Actual',
            marker=dict(color='blue', size=10),
            line=dict(color='blue', width=3),
        ),
        row=1, col=1
    )
    fig.add_trace(
        go.Scatter(
            x=sizes,
            y=actual_mm_norm,
            mode='lines',
            name='Theoretical O(N^3)',
            line=dict(color='red', width=2, dash='dash'),
        ),
        row=1, col=1
    )

    # MV: O(N^2)
    mv_data = single_proc[single_proc['operation'] == 'Matrix x Vector'].copy()
    actual_mv = [mv_data[mv_data['size'] == s]['compute_time'].mean() for s in sizes]
    actual_mv_norm = [actual_mv[0] * (s/500)**2 for s in sizes]

    fig.add_trace(
        go.Scatter(
            x=sizes,
            y=actual_mv,
            mode='lines+markers',
            name='Actual',
            marker=dict(color='orange', size=10),
            line=dict(color='orange', width=3),
            showlegend=False,
        ),
        row=1, col=2
    )
    fig.add_trace(
        go.Scatter(
            x=sizes,
            y=actual_mv_norm,
            mode='lines',
            name='Theoretical O(N^2)',
            line=dict(color='red', width=2, dash='dash'),
            showlegend=False,
        ),
        row=1, col=2
    )

    fig.update_layout(
        title='Accuracy Check: Does real time follow theoretical complexity?',
        height=500,
        plot_bgcolor='white',
        showlegend=True,
        legend=dict(
            bgcolor="white",
            bordercolor="black",
            borderwidth=1,
        ),
    )
    fig.update_xaxes(title_text='Matrix Size N', row=1, col=1, gridcolor='lightgray', zerolinecolor='gray')
    fig.update_xaxes(title_text='Matrix Size N', row=1, col=2, gridcolor='lightgray', zerolinecolor='gray')
    fig.update_yaxes(title_text='Compute Time (s)', row=1, col=1, type='log', gridcolor='lightgray', zerolinecolor='gray')
    fig.update_yaxes(title_text='Compute Time (s)', row=1, col=2, type='log', gridcolor='lightgray', zerolinecolor='gray')

    fig.write_html(os.path.join(output_dir, 'accuracy_check.html'))
    print(f"  -> accuracy_check.html")


def plot_distribution_ranking(df: pd.DataFrame, output_dir: str):
    """Which data distribution is better for different sizes"""
    mm_df = df[df['operation'] == 'Matrix x Matrix'].copy()

    # Group by size and distribution
    ranking = mm_df.groupby(['size', 'distribution'])['compute_time'].mean().reset_index()

    fig = go.Figure()

    distributions = ranking['distribution'].unique()
    default_colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22"]

    for i, dist in enumerate(distributions):
        dist_data = ranking[ranking['distribution'] == dist]
        fig.add_trace(
            go.Bar(
                x=dist_data['size'].astype(str),
                y=dist_data['compute_time'],
                name=dist,
                marker_color=default_colors[i % len(default_colors)],
            )
        )

    fig.update_layout(
        title='Distribution Strategy Comparison (lower is better)',
        xaxis_title='Matrix Size',
        yaxis_title='Compute Time (s)',
        height=500,
        barmode='group',
        plot_bgcolor='white',
        showlegend=True,
        legend=dict(
            bgcolor="white",
            bordercolor="black",
            borderwidth=1,
        ),
    )

    fig.write_html(os.path.join(output_dir, 'distribution_ranking.html'))
    print(f"  -> distribution_ranking.html")


def print_summary_statistics(df: pd.DataFrame):
    """Output key statistics to console"""
    print("\n" + "="*70)
    print("SUMMARY STATISTICS")
    print("="*70)

    mm_df = df[df['operation'] == 'Matrix x Matrix']

    for size in [500, 1000, 2000]:
        size_data = mm_df[mm_df['size'] == size]
        baseline = size_data[size_data['processes'] == 1]['compute_time'].mean()
        best = size_data.groupby('processes')['compute_time'].mean().min()
        best_proc = size_data.groupby('processes')['compute_time'].mean().idxmin()
        speedup_at_6 = baseline / size_data[size_data['processes'] == 6]['compute_time'].mean()

        print(f"\nSize {size}x{size}:")
        print(f"   Baseline (1 proc):  {baseline:.3f}s")
        print(f"   Best time:          {best:.3f}s (on {best_proc} processes)")
        print(f"   Speedup at 6 procs: {speedup_at_6:.2f}x")
        print(f"   Efficiency at 6:    {speedup_at_6/6*100:.1f}%")

    # Amdahl's Law check
    print("\nAmdahl's Law Check:")
    for size in [500, 1000, 2000]:
        size_data = mm_df[mm_df['size'] == size]
        t1 = size_data[size_data['processes'] == 1]['compute_time'].mean()
        t9 = size_data[size_data['processes'] == 9]['compute_time'].mean() if 9 in size_data['processes'].values else None

        if t9:
            sequential_fraction = (t9 / t1 - 1/9) / (1 - 1/9) if t9/t1 > 1/9 else 0
            print(f"   {size}x{size}: Estimated sequential fraction = {sequential_fraction:.1%}")

    # Oversubscription check
    print("\nOversubscription Check (6 cores / 12 threads):")
    for size in [500, 1000, 2000]:
        size_data = mm_df[mm_df['size'] == size]
        t6 = size_data[size_data['processes'] == 6]['compute_time'].mean()
        t9 = size_data[size_data['processes'] == 9]['compute_time'].mean() if 9 in size_data['processes'].values else None
        if t9:
            slowdown = (t9 - t6) / t6 * 100
            if slowdown > 5:
                print(f"   {size}x{size}: Performance drop at 9 processes: {slowdown:.1f}% slower than at 6")


def main():
    parser = argparse.ArgumentParser(
        description='Analyze scalability across three benchmark datasets (500, 1000, 2000)'
    )
    parser.add_argument(
        '--output-dir',
        type=str,
        default='benchmark/plots/',
        help='Output directory for plots (default: scalability_analysis)'
    )
    args = parser.parse_args()

    print("Loading benchmark results...")
    df = load_all_results()

    if df.empty:
        print("No data loaded. Please ensure results500.txt, results1000.txt, results2000.txt exist.")
        return

    # Add theoretical time
    df = compute_theoretical_complexity(df)

    # Create output directory
    os.makedirs(args.output_dir, exist_ok=True)

    print(f"\nGenerating plots in '{args.output_dir}/'...")

    # plot_scalability_comparison(df, args.output_dir)
    plot_speedup_by_size(df, args.output_dir)
    plot_efficiency_degradation(df, args.output_dir)
    # plot_communication_overhead(df, args.output_dir)
    # plot_accuracy_check(df, args.output_dir)
    # plot_distribution_ranking(df, args.output_dir)

    print_summary_statistics(df)

    print(f"\nAll plots saved to {args.output_dir}/")
    print("\nKey insights from these plots:")
    print("   1. size_scalability.html      -> Shows if compute_time grows as O(N^3) / O(N^2)")
    print("   2. speedup_by_size.html       -> Larger problems should scale better")
    print("   3. efficiency_degradation.html -> Shows oversubscription at >6 processes")
    print("   4. accuracy_check.html        -> Validates your measurements against theory")
    print("   5. distribution_ranking.html  -> Which data distribution is optimal")


if __name__ == '__main__':
    main()