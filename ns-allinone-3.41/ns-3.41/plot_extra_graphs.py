
import os
import glob
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

OUT = "/mnt/c/Users/amish/Downloads/QtColFair_Graphs"
os.makedirs(OUT, exist_ok=True)

# Folder, label, and trace pattern for each experiment.
experiments = [
    ("qt_baseline", "Baseline (0% loss)",
     "TcpQtColFair_1.2minRTT-0.000000"),
    ("qt_baseline", "Baseline alpha=1.5 (0% loss)",
     "TcpQtColFair_1.5minRTT-0.000000"),
    ("qt_adaptive_005", "Adaptive (0.5% loss)",
     "TcpQtColFair_1.2minRTT-0.005000"),
]

def load_trace(folder, prefix):
    pattern = os.path.join(
        folder, "**", prefix + "*-flow0-cwnd.data"
    )
    files = glob.glob(pattern, recursive=True)
    if not files:
        print("Trace not found:", pattern)
        return None, None

    # Use the first matching file for this configuration.
    path = sorted(files)[0]
    try:
        data = pd.read_csv(path, sep=r"\s+", header=None)
        if data.shape[1] < 11:
            print("Unexpected columns:", path, data.shape)
            return None, None
        print("Loaded:", path, "| rows:", len(data))
        return data, path
    except Exception as e:
        print("Could not read:", path, e)
        return None, None

loaded = []
for folder, label, prefix in experiments:
    df, path = load_trace(folder, prefix)
    if df is not None:
        loaded.append((label, df, path))

if not loaded:
    raise SystemExit("No traces loaded. Check the experiment folders.")

# Column indices from the observed trace format:
# 0 time(s), 1 cwnd(bytes), 2 in-flight(bytes), 3 RTT(s)
# 4 throughput(Mbps), 5 average throughput(Mbps)
# 6 goodput(Mbps), 7 average goodput(Mbps)
# 8 transmitted packets, 9 retransmitted packets, 10 cost

def save_plot(filename, title, ylabel, column, scale=1.0):
    plt.figure(figsize=(10, 5.5))
    for label, df, _ in loaded:
        plt.plot(df[0], df[column] * scale, label=label, linewidth=1.2)
    plt.xlabel("Time (seconds)")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, filename), dpi=200)
    plt.close()
    print("Saved:", filename)

# 08: Congestion window
save_plot("08_cwnd_vs_time.png",
          "TCP Congestion Window vs Time",
          "Congestion window (KB)", 1, 1 / 1000)

# 09: In-flight data
save_plot("09_inflight_vs_time.png",
          "In-flight Data vs Time",
          "In-flight data (KB)", 2, 1 / 1000)

# 10: RTT
save_plot("10_rtt_vs_time.png",
          "Round-Trip Time vs Time",
          "RTT (milliseconds)", 3, 1000)

# 11: Instantaneous throughput
save_plot("11_throughput_vs_time.png",
          "Instantaneous Throughput vs Time",
          "Throughput (Mbps)", 4)

# 12: Average throughput
save_plot("12_average_throughput_vs_time.png",
          "Average Throughput vs Time",
          "Average throughput (Mbps)", 5)

# 13: Instantaneous goodput
save_plot("13_goodput_vs_time.png",
          "Instantaneous Goodput vs Time",
          "Goodput (Mbps)", 6)

# 14: Average goodput
save_plot("14_average_goodput_vs_time.png",
          "Average Goodput vs Time",
          "Average goodput (Mbps)", 7)

# 15: Retransmitted packets (cumulative counter)
save_plot("15_retransmissions_vs_time.png",
          "Retransmitted Packets vs Time",
          "Retransmitted packet count", 9)

# 16: Cost
save_plot("16_cost_vs_time.png",
          "Cost Metric vs Time",
          "Cost", 10)

# Distribution comparisons
for filename, title, ylabel, column, scale in [
    ("17_cwnd_distribution.png", "Congestion Window Distribution",
     "Congestion window (KB)", 1, 1 / 1000),
    ("18_rtt_distribution.png", "RTT Distribution",
     "RTT (milliseconds)", 3, 1000),
    ("19_goodput_distribution.png", "Goodput Distribution",
     "Goodput (Mbps)", 6, 1),
    ("20_inflight_distribution.png", "In-flight Data Distribution",
     "In-flight data (KB)", 2, 1 / 1000),
]:
    plt.figure(figsize=(9, 5.5))
    values, labels = [], []
    for label, df, _ in loaded:
        v = (df[column] * scale).replace([np.inf, -np.inf], np.nan).dropna()
        if len(v):
            values.append(v.to_numpy())
            labels.append(label)
    plt.boxplot(values, tick_labels=labels, showfliers=False)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.xticks(rotation=12, ha="right")
    plt.grid(True, axis="y", alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, filename), dpi=200)
    plt.close()
    print("Saved:", filename)

# Export basic summary statistics from the loaded traces.
summary = []
for label, df, path in loaded:
    summary.append({
        "experiment": label,
        "trace_file": path,
        "samples": len(df),
        "mean_cwnd_KB": df[1].mean() / 1000,
        "mean_inflight_KB": df[2].mean() / 1000,
        "mean_RTT_ms": df[3].mean() * 1000,
        "mean_throughput_Mbps": df[4].mean(),
        "mean_goodput_Mbps": df[6].mean(),
        "final_retransmission_counter": df[9].iloc[-1],
    })

pd.DataFrame(summary).to_csv(
    os.path.join(OUT, "extra_graphs_summary.csv"), index=False
)
print("\nAll available extra graphs saved to:", OUT)
