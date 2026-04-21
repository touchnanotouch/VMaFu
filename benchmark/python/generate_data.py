#!/usr/bin/env python3
"""
Generate test data for benchmarks (matrices and vectors in CSV format).

Usage:
    python generate_data.py              # Default: 500x500 matrices
    python generate_data.py --n 1000     # 1000x1000 matrices
    python generate_data.py -n 100       # 100x100 matrices
"""

import argparse
import numpy as np
import os


def generate_matrix(filepath: str, rows: int, cols: int, seed: int = 42):
    np.random.seed(seed)
    matrix = np.random.rand(rows, cols)
    np.savetxt(filepath, matrix, delimiter=',', fmt='%.6f')


def generate_vector(filepath: str, size: int, seed: int = 42):
    np.random.seed(seed + 1)
    vector = np.random.rand(1, size)
    np.savetxt(filepath, vector, delimiter=',', fmt='%.6f')


def main():
    parser = argparse.ArgumentParser(
        description='Generate test matrices and vectors for benchmarks'
    )

    parser.add_argument(
        '-n', '--size',
        type=int,
        default=500,
        help='Matrix size N for NxN matrix (default: 500)'
    )
    parser.add_argument(
        '--seed',
        type=int,
        default=42,
        help='Random seed for reproducibility (default: 42)'
    )
    parser.add_argument(
        '-o', '--output-dir',
        type=str,
        default='data',
        help='Output directory for data files (default: data)'
    )
    
    args = parser.parse_args()

    os.makedirs(args.output_dir, exist_ok=True)

    n = args.size
    
    generate_matrix(f'{args.output_dir}/matrix1.csv', n, n, seed=args.seed)
    generate_matrix(f'{args.output_dir}/matrix2.csv', n, n, seed=args.seed + 100)
    generate_vector(f'{args.output_dir}/vector1.csv', n, seed=args.seed + 200)
    
    print(f"\nTest data generated in '{args.output_dir}/' directory:\n  - matrix1.csv: {n}x{n}\n  - matrix2.csv: {n}x{n}\n  - vector1.csv: {n} elements\n")


if __name__ == '__main__':
    main()
