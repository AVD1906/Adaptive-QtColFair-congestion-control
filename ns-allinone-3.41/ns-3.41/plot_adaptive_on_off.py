
import os
import glob
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

OUT = "/mnt/c/Users/amish/Downloads/QtColFair_Graphs/02_Two_Flow_Comparison"
os.makedirs(OUT, exist_ok=True)

CONFIGS = {
    "Adaptive ON": "compare_adaptive_on",
    "Adaptive OFF": "compare_adaptive_off",
}

def load_config(root):
    paths = sorted(glob.glob(
        os.path.join(root, "**", "*-flow[0-9]*-cwnd.data"),
        recursive=True
    ))
    results = []
    for path in paths:
        if "-Agg-cwnd.data" in path:
            continue
        df = pd.read_csv(path, sep=r"\s+", header=None)
        if df.shape[1] >= 11 and len(df):
            results.append((path, df))
    return results

data = {}
for label, root in CONFIGS.items():
    flows = load_config(root)
    if not flows:
        raise SystemExit(f"No flow traces found in {root}")
    data[label] = flows
    print(f"{label}: loaded {len(flows)} flow traces")

# Verify both configurations contain the same number of flows.
counts = [len(v) for v in data.values()]
if len(set(counts)) != 1:
    raise SystemExit(f"Flow counts differ: {counts}")

# Column mapping:
# 0 time(s), 1 cwnd(bytes), 2 in-flight(bytes), 3 RTT(s)
# 4 throughput(Mbps), 5 average throughput(Mbps)
# 6 goodput(Mbps), 7 average goodput(Mbps)
# 8 transmitted packets, 9 retransmitted packets, 10 cost

plots = [
    (1, 1/1000, "Congestion window (KB)", "Congestion Window"),
    (2, 1/1000, "In-flight data (KB)", "In-flight Data"),
    (3, 1000, "RTT (ms)", "Round-Trip Time"),
    (4, 1, "Throughput (Mbps)", "Instantaneous Throughput"),
    (5, 1, "Average throughput (Mbps)", "Average Throughput"),
    (6, 1, "Goodput (Mbps)", "Instantaneous Goodput"),
    (7, 1, "Average goodput (Mbps)", "Average Goodput"),
    (9, 1, "Retransmission counter", "Retransmissions"),
    (10, 1, "Cost", "Cost"),
]

summary = []

# Aggregate each flow's data across the two flows for time-series comparison.
for col, scale, ylabel, title in plots:
    plt.figure(figsize=(10, 5.5))
    for label, flows in data.items():
        all_series = []
        for path, df in flows:
            series = pd.Series(
                df[col].to_numpy() * scale,
                index=df[0].to_numpy()
            )
            series = series[~series.index.duplicated(keep="last")]
            all_series.append(series)

        combined = pd.concat(all_series, axis=1).sort_index()
        # Mean across flows at each available timestamp.
        mean_line = combined.mean(axis=1, skipna=True)
        plt.plot(mean_line.index, mean_line.values,
                 label=label, linewidth=1.2)

    plt.xlabel("Time (seconds)")
    plt.ylabel(ylabel)
    plt.title(f"{title}: Adaptive ON vs OFF")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    filename = f"29_adaptive_comparison_{title.lower().replace(' ', '_')}.png"
    plt.savefig(os.path.join(OUT, filename), dpi=200)
    plt.close()
    print("Saved:", filename)

    # Additional display-only smoothed plots; original plots and data remain unchanged.
    if title in ("Average Goodput", "Average Throughput", "Cost"):
        fig, ax = plt.subplots(figsize=(10, 5.5))

        for label, flows in data.items():
            all_series = []
            for path, df in flows:
                series = pd.Series(
                    df[col].to_numpy() * scale,
                    index=df[0].to_numpy()
                )
                series = series[~series.index.duplicated(keep="last")]
                all_series.append(series)

            combined = pd.concat(all_series, axis=1).sort_index()
            mean_line = combined.mean(axis=1, skipna=True)
            smooth_line = mean_line.rolling(
                window=21, center=True, min_periods=1
            ).median()

            ax.plot(
                smooth_line.index, smooth_line.values,
                label=label, linewidth=1.8
            )

        ax.set_xlabel("Time (seconds)")
        ax.set_ylabel(ylabel)
        ax.set_title(f"{title}: Adaptive ON vs OFF (smoothed display)")
        ax.grid(True, alpha=0.3)
        ax.legend()
        fig.tight_layout()

        smooth_filename = filename.replace(".png", "_smoothed.png")
        fig.savefig(os.path.join(OUT, smooth_filename), dpi=200)
        plt.close(fig)
        print("Saved:", smooth_filename)

    # Add a clearly labelled zoomed view for instantaneous rates.
    # Raw trace values are unchanged; this is a separate presentation.
    if title in ("Instantaneous Throughput", "Instantaneous Goodput"):
        fig, ax = plt.subplots(figsize=(10, 5.5))
        plotted_values = []
        for label, flows in data.items():
            all_series = []
            for path, df in flows:
                series = pd.Series(
                    df[col].to_numpy() * scale,
                    index=df[0].to_numpy()
                )
                series = series[~series.index.duplicated(keep="last")]
                all_series.append(series)
            combined = pd.concat(all_series, axis=1).sort_index()
            mean_line = combined.mean(axis=1, skipna=True)
            ax.plot(mean_line.index, mean_line.values,
                    label=label, linewidth=1.0)
            plotted_values.extend(mean_line.dropna().tolist())

        positive = [v for v in plotted_values if v >= 0]
        if positive:
            zoom_max = float(np.percentile(positive, 95)) * 1.15
            if zoom_max > 0:
                ax.set_ylim(0, zoom_max)

        ax.set_xlabel("Time (seconds)")
        ax.set_ylabel(ylabel)
        ax.set_title(f"{title}: Adaptive ON vs OFF (zoomed; 95th-percentile scale)")
        ax.grid(True, alpha=0.3)
        ax.legend()
        fig.tight_layout()
        zoom_filename = filename.replace(".png", "_zoomed.png")
        fig.savefig(os.path.join(OUT, zoom_filename), dpi=200)
        plt.close(fig)
        print("Saved:", zoom_filename)

# Per-flow goodput plot: useful for checking how the two flows share capacity.
for flow_index in range(counts[0]):
    plt.figure(figsize=(10, 5.5))
    for label, flows in data.items():
        path, df = flows[flow_index]
        plt.plot(df[0], df[7], label=label, linewidth=1.1)

    plt.xlabel("Time (seconds)")
    plt.ylabel("Average goodput (Mbps)")
    plt.title(f"Flow {flow_index}: Adaptive ON vs OFF")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    filename = f"30_flow{flow_index}_goodput_on_off.png"
    plt.savefig(os.path.join(OUT, filename), dpi=200)
    plt.close()
    print("Saved:", filename)

# Jain's fairness computed from the per-flow average-goodput traces.
fairness_series = {}
for label, flows in data.items():
    series_list = []
    for path, df in flows:
        s = pd.Series(df[7].clip(lower=0).to_numpy(),
                      index=df[0].to_numpy())
        s = s[~s.index.duplicated(keep="last")]
        series_list.append(s)

    aligned = pd.concat(series_list, axis=1).sort_index().ffill().fillna(0)
    vals = aligned.to_numpy()
    denom = len(series_list) * np.square(vals).sum(axis=1)
    numer = np.square(vals.sum(axis=1))
    fairness = np.divide(
        numer, denom,
        out=np.zeros_like(numer, dtype=float),
        where=denom > 0
    )
    fairness_series[label] = (aligned.index, fairness)

plt.figure(figsize=(10, 5.5))
for label, (times, fairness) in fairness_series.items():
    plt.plot(times, fairness, label=label, linewidth=1.2)
plt.xlabel("Time (seconds)")
plt.ylabel("Jain's fairness index")
plt.ylim(0, 1.05)
plt.title("Fairness: Adaptive ON vs OFF")
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(OUT, "31_adaptive_on_off_fairness.png"), dpi=200)
plt.close()
print("Saved: 31_adaptive_on_off_fairness.png")

# CSV with per-flow trace averages.
for label, flows in data.items():
    for i, (path, df) in enumerate(flows):
        summary.append({
            "configuration": label,
            "flow": i,
            "trace": path,
            "mean_throughput_Mbps": df[4].mean(),
            "mean_average_throughput_Mbps": df[5].mean(),
            "mean_goodput_Mbps": df[6].mean(),
            "mean_average_goodput_Mbps": df[7].mean(),
            "mean_rtt_ms": df[3].mean() * 1000,
            "mean_cwnd_KB": df[1].mean() / 1000,
            "mean_inflight_KB": df[2].mean() / 1000,
            "final_retransmission_counter": df[9].iloc[-1],
        })

pd.DataFrame(summary).to_csv(
    os.path.join(OUT, "adaptive_on_off_summary.csv"), index=False
)
print("Saved: adaptive_on_off_summary.csv")
print("All comparison outputs saved to:", OUT)
