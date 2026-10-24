
import os
import glob
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

BASE = os.getcwd()
OUT = "/mnt/c/Users/amish/Downloads/QtColFair_Graphs"
os.makedirs(OUT, exist_ok=True)

# Explicitly separate the experiment families.
EXPERIMENTS = {
    "Adaptive": ["qt_adaptive_005"],
    "Modified QtColFair": [
        "qt_loss_0001",
        "qt_loss_0005",
        "qt_loss_001",
        "qt_loss_005",
        "qt_loss_01",
    ],
    "Original QtColFair": ["qt_original_005"],
}

rows = []

for label, folders in EXPERIMENTS.items():
    for folder in folders:
        pattern = os.path.join(
            BASE, folder, "**", "*-flow0-cwnd.data"
        )

        for path in glob.glob(pattern, recursive=True):
            try:
                df = pd.read_csv(path, sep=r"\s+", header=None)

                if df.shape[1] < 11 or df.empty:
                    continue

                filename = os.path.basename(path)
                marker = "minRTT-"

                if marker not in filename:
                    continue

                loss = float(
                    filename.split(marker, 1)[1].split("-", 1)[0]
                )

                rows.append({
                    "experiment": label,
                    "folder": folder,
                    "loss_rate": loss,
                    "loss_percent": loss * 100,
                    "goodput_Mbps": df[6].mean(),
                    "throughput_Mbps": df[4].mean(),
                    "rtt_ms": df[3].mean() * 1000,
                    "cwnd_KB": df[1].mean() / 1000,
                    "final_retransmission_counter": df[9].iloc[-1],
                    "trace": path,
                })

            except (ValueError, IndexError, OSError) as e:
                print("Skipped:", path, e)

if not rows:
    raise SystemExit("No usable traces found.")

data = pd.DataFrame(rows)
csv_path = os.path.join(OUT, "loss_rate_trace_metrics.csv")
data.to_csv(csv_path, index=False)

metrics = [
    ("goodput_Mbps", "Mean Goodput vs Packet Error Rate",
     "Mean goodput (Mbps)", "21_goodput_vs_loss_rate.png"),
    ("throughput_Mbps", "Mean Throughput vs Packet Error Rate",
     "Mean throughput (Mbps)", "22_throughput_vs_loss_rate.png"),
    ("rtt_ms", "Mean RTT vs Packet Error Rate",
     "Mean RTT (ms)", "23_rtt_vs_loss_rate.png"),
    ("cwnd_KB", "Mean Congestion Window vs Packet Error Rate",
     "Mean congestion window (KB)", "24_cwnd_vs_loss_rate.png"),
    ("final_retransmission_counter", "Final Retransmission Counter vs Packet Error Rate",
     "Final retransmission counter", "25_retransmissions_vs_loss_rate.png"),
]

for metric, title, ylabel, output in metrics:
    plt.figure(figsize=(9, 5.5))

    for label, group in data.groupby("experiment"):
        grouped = group.groupby("loss_percent")[metric].mean().sort_index()
        plt.plot(
            grouped.index, grouped.values,
            marker="o", label=label
        )

    plt.xlabel("Configured packet error rate (%)")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(OUT, output), dpi=200)
    plt.close()
    print("Saved:", output)

print("\nExperiment counts:")
print(data.groupby("experiment")["trace"].count())
print("\nLoss rates (%):")
print(data.groupby("experiment")["loss_percent"].apply(
    lambda x: sorted(x.unique())
))
print("\nSaved CSV:", csv_path)
print("All graphs saved in:", OUT)
