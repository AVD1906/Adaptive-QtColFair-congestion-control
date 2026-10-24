
import re
import csv
from pathlib import Path
from statistics import mean
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# All graph outputs go into this one folder
OUT = Path("/mnt/c/Users/amish/Downloads/QtColFair_Graphs/01_Main_Adaptive_Comparison")
OUT.mkdir(parents=True, exist_ok=True)

LOGS = {
    "Adaptive OFF": [
        "baseline_debug.log",
        "baseline_run2.log",
        "baseline_run3.log",
    ],
    "Adaptive ON": [
        "adaptive_debug.log",
        "adaptive_run2.log",
        "adaptive_run3.log",
    ],
}

SERIES_RE = re.compile(
    r"^\s*([\d.]+)\s+NodeId=(\d+)\s+"
    r"inFlightValue=([\d.]+)pkts\s+RttValue=([\d.]+)s"
)
GOODPUT_RE = re.compile(r"aveAggGoodput=([\d.]+)Mbps")
THROUGHPUT_RE = re.compile(r"aveAggThroughput=([\d.]+)Mbps")
FAIRNESS_RE = re.compile(r"fairness=([\d.]+)")


def read_log(filename):
    path = Path(filename)
    if not path.exists():
        raise FileNotFoundError(
            f"Missing {filename}. Run this script from your ns-3.41 folder."
        )

    text = path.read_text(errors="replace")

    def last_value(pattern):
        matches = pattern.findall(text)
        return float(matches[-1]) if matches else None

    samples = []
    for line in text.splitlines():
        match = SERIES_RE.search(line)
        if match and match.group(2) == "2":
            samples.append((
                float(match.group(1)),
                float(match.group(3)),
                float(match.group(4)) * 1000
            ))

    return {
        "file": filename,
        "goodput": last_value(GOODPUT_RE),
        "throughput": last_value(THROUGHPUT_RE),
        "fairness": last_value(FAIRNESS_RE),
        "samples": samples,
    }


def save_bar(off, on, ylabel, title, filename):
    fig, ax = plt.subplots(figsize=(8, 5))
    bars = ax.bar(["Adaptive OFF", "Adaptive ON"], [off, on])
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(axis="y", alpha=0.25)
    ax.bar_label(bars, fmt="%.3f", padding=4)
    fig.tight_layout()
    target = OUT / filename
    temp = OUT / (target.stem + "_temp.png")
    fig.savefig(temp, format="png", dpi=250)
    plt.close(fig)
    temp.replace(target)


def save_line(off, on, value_index, ylabel, title, filename,
              remove_zero=False):
    if not off or not on:
        print("Skipped:", filename, "- missing time-series samples")
        return

    if remove_zero:
        off = [p for p in off if p[value_index] > 0]
        on = [p for p in on if p[value_index] > 0]

    if not off or not on:
        print("Skipped:", filename, "- no valid samples")
        return

    start = max(off[0][0], on[0][0])
    off = [p for p in off if p[0] >= start]
    on = [p for p in on if p[0] >= start]

    fig, ax = plt.subplots(figsize=(10, 5.5))
    ax.plot([p[0] for p in off], [p[value_index] for p in off],
            label="Adaptive OFF", linewidth=1)
    ax.plot([p[0] for p in on], [p[value_index] for p in on],
            label="Adaptive ON", linewidth=1)
    ax.set_xlabel("Simulation time (seconds)")
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(True, alpha=0.25)
    ax.legend()
    fig.tight_layout()
    target = OUT / filename
    temp = OUT / (target.stem + "_temp.png")
    fig.savefig(temp, format="png", dpi=250)
    plt.close(fig)
    temp.replace(target)


def save_box(off_values, on_values, ylabel, title, filename):
    if not off_values or not on_values:
        print("Skipped:", filename, "- missing samples")
        return

    fig, ax = plt.subplots(figsize=(8, 5))
    ax.boxplot(
        [off_values, on_values],
        labels=["Adaptive OFF", "Adaptive ON"],
        showfliers=True
    )
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(axis="y", alpha=0.25)
    fig.tight_layout()
    target = OUT / filename
    temp = OUT / (target.stem + "_temp.png")
    fig.savefig(temp, format="png", dpi=250)
    plt.close(fig)
    temp.replace(target)


def main():
    results = {
        name: [read_log(f) for f in files]
        for name, files in LOGS.items()
    }

    for name, runs in results.items():
        for run in runs:
            print(
                f"{name}: {run['file']}, "
                f"{len(run['samples'])} time-series samples"
            )

    metrics = [
        ("Aggregate goodput (Mbps)", "goodput"),
        ("Aggregate throughput (Mbps)", "throughput"),
        ("Fairness index", "fairness"),
    ]

    summary = []

    for label, key in metrics:
        off_values = [
            r[key] for r in results["Adaptive OFF"]
            if r[key] is not None
        ]
        on_values = [
            r[key] for r in results["Adaptive ON"]
            if r[key] is not None
        ]

        if not off_values or not on_values:
            print("Metric missing:", label)
            continue

        off = mean(off_values)
        on = mean(on_values)
        change = on - off
        percent = change / off * 100 if off else None

        summary.append([label, off, on, change, percent])

        if key == "goodput":
            save_bar(
                off, on, "Goodput (Mbps)",
                "Aggregate Goodput Comparison",
                "01_goodput_comparison.png"
            )
        elif key == "throughput":
            save_bar(
                off, on, "Throughput (Mbps)",
                "Aggregate Throughput Comparison",
                "02_throughput_comparison.png"
            )
        elif key == "fairness":
            save_bar(
                off, on, "Jain fairness index",
                "Final Fairness Comparison",
                "03_fairness_comparison.png"
            )

    off_series = results["Adaptive OFF"][0]["samples"]
    on_series = results["Adaptive ON"][0]["samples"]

    # Time-series graphs use the first logged run for each configuration.
    save_line(
        off_series, on_series, 1,
        "Data in flight (packets)",
        "Data in Flight vs Simulation Time",
        "04_inflight_vs_time.png"
    )

    save_line(
        off_series, on_series, 2,
        "RTT (milliseconds)",
        "RTT vs Simulation Time",
        "05_rtt_vs_time.png",
        remove_zero=True
    )

    # Distribution plots use all available samples from the six logs.
    off_flight = [
        p[1] for r in results["Adaptive OFF"]
        for p in r["samples"]
    ]
    on_flight = [
        p[1] for r in results["Adaptive ON"]
        for p in r["samples"]
    ]

    off_rtt = [
        p[2] for r in results["Adaptive OFF"]
        for p in r["samples"] if p[2] > 0
    ]
    on_rtt = [
        p[2] for r in results["Adaptive ON"]
        for p in r["samples"] if p[2] > 0
    ]

    save_box(
        off_flight, on_flight,
        "Data in flight (packets)",
        "Data-in-Flight Distribution",
        "06_inflight_boxplot.png"
    )

    save_box(
        off_rtt, on_rtt,
        "RTT (milliseconds)",
        "RTT Distribution",
        "07_rtt_boxplot.png"
    )

    with (OUT / "summary_metrics.csv").open(
        "w", newline=""
    ) as file:
        writer = csv.writer(file)
        writer.writerow([
            "Metric", "Adaptive OFF mean", "Adaptive ON mean",
            "Absolute change", "Relative change (%)"
        ])
        writer.writerows(summary)

    print("\nFinished. Output folder:")
    print(OUT)
    print("\nGenerated graphs and summary_metrics.csv.")
    print(
        "Other paper-style plots need additional measurements or "
        "new simulations; this script does not invent those results."
    )


if __name__ == "__main__":
    main()
