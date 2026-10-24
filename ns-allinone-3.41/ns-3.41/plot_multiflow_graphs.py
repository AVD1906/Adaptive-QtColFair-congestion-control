
import os
import glob
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

OUT = "/mnt/c/Users/amish/Downloads/QtColFair_Graphs"
os.makedirs(OUT, exist_ok=True)

ROOTS = {
    "2 flows": "multi_flow_2",
    "4 flows": "multi_flow_4",
}

def load_flows(root):
    paths = sorted(glob.glob(
        os.path.join(root, "**", "*-flow[0-9]*-cwnd.data"),
        recursive=True
    ))
    flows = []
    for path in paths:
        name = os.path.basename(path)
        if "-Agg-cwnd.data" in name:
            continue
        try:
            df = pd.read_csv(path, sep=r"\s+", header=None)
            if df.shape[1] >= 11 and not df.empty:
                flows.append((path, df))
        except Exception as e:
            print("Skipping", path, e)
    return flows

all_data = {}
for config, root in ROOTS.items():
    flows = load_flows(root)
    if not flows:
        print("No flow traces found in", root)
        continue
    all_data[config] = flows
    print(config, ":", len(flows), "flows loaded")

if not all_data:
    raise SystemExit("No valid flow traces found.")

# Plot 26: per-flow goodput over time, for each configuration.
for config, flows in all_data.items():
    plt.figure(figsize=(10, 5.5))
    for i, (path, df) in enumerate(flows):
        plt.plot(df[0], df[6], label=f"Flow {i}", linewidth=1)
    plt.xlabel("Time (seconds)")
    plt.ylabel("Instantaneous goodput (Mbps)")
    plt.title(f"Per-Flow Goodput vs Time — {config}")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    filename = "26_goodput_per_flow_" + config.replace(" ", "") + ".png"
    plt.savefig(os.path.join(OUT, filename), dpi=200)
    plt.close()
    print("Saved:", filename)

# Plot 27: congestion window per flow.
for config, flows in all_data.items():
    plt.figure(figsize=(10, 5.5))
    for i, (path, df) in enumerate(flows):
        plt.plot(df[0], df[1] / 1000, label=f"Flow {i}", linewidth=1)
    plt.xlabel("Time (seconds)")
    plt.ylabel("Congestion window (KB)")
    plt.title(f"Per-Flow Congestion Window — {config}")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    filename = "27_cwnd_per_flow_" + config.replace(" ", "") + ".png"
    plt.savefig(os.path.join(OUT, filename), dpi=200)
    plt.close()
    print("Saved:", filename)

# Align flow average-goodput samples by nearest available timestamp.
# Jain's index is computed over nonnegative average-goodput values.
# This is a trace-sample estimate, not packet-level ground truth.
for config, flows in all_data.items():
    series = []
    for i, (path, df) in enumerate(flows):
        s = pd.Series(
            df[7].clip(lower=0).to_numpy(),
            index=df[0].to_numpy()
        )
        s = s[~s.index.duplicated(keep="last")]
        series.append(s.rename(f"Flow {i}"))

    joined = pd.concat(series, axis=1).sort_index().ffill().fillna(0)
    values = joined.to_numpy()
    numerator = np.square(values.sum(axis=1))
    denominator = len(series) * np.square(values).sum(axis=1)
    fairness = np.divide(
        numerator, denominator,
        out=np.zeros_like(numerator, dtype=float),
        where=denominator > 0
    )

    plt.figure(figsize=(10, 5.5))
    plt.plot(joined.index, fairness, linewidth=1.2)
    plt.ylim(0, 1.05)
    plt.xlabel("Time (seconds)")
    plt.ylabel("Jain's fairness index")
    plt.title(f"Estimated Goodput Fairness vs Time — {config}")
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    filename = "28_jain_fairness_" + config.replace(" ", "") + ".png"
    plt.savefig(os.path.join(OUT, filename), dpi=200)
    plt.close()
    print("Saved:", filename)

# Summary: per-flow mean and average goodput.
rows = []
for config, flows in all_data.items():
    means = []
    for i, (path, df) in enumerate(flows):
        mean_goodput = float(df[6].mean())
        mean_avg_goodput = float(df[7].mean())
        means.append(mean_avg_goodput)
        rows.append({
            "configuration": config,
            "flow": i,
            "trace_file": path,
            "mean_instantaneous_goodput_Mbps": mean_goodput,
            "mean_average_goodput_Mbps": mean_avg_goodput,
        })

    total = sum(means)
    fairness = (total * total / (len(means) * sum(x*x for x in means))
                if means and sum(x*x for x in means) else 0)
    print(f"{config}: mean average-goodput Jain fairness = {fairness:.4f}")

pd.DataFrame(rows).to_csv(
    os.path.join(OUT, "multiflow_summary.csv"), index=False
)
print("Saved summary CSV.")
print("All multi-flow graphs saved to:", OUT)
