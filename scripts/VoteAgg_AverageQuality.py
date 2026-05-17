import re
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt

RESULTS_DIR = Path(r"C:/Users/ipekm/Downloads/omnetpp-6.0.3-windows-x86_64/omnetpp-6.0.3/RIDe-OMNeT/src/results/VoteAgg/OpportunisticAverageQuality")
#Bunu çalıştırdığında ortalama servis kalitesi sonuçlarını verecek

#Buraya diğer metodları eklemeyi unutmayalım!!
METHOD_NAMES = {
    "0": "Additive",
    "1": "Multiplicative",
    "2": "Borda",
    "additive": "Additive",
    "multiplicative": "Multiplicative",
    "borda": "Borda",
}


def extract_time_from_scalar_name(name: str):
    match = re.search(r"At_(\d+)", name)
    if match:
        return int(match.group(1))
    return None


def infer_method_from_path(path: Path):
    for part in path.parts:
        if part.startswith("method="):
            raw_method = part.split("=", 1)[1].replace('"', "")
            return METHOD_NAMES.get(raw_method, raw_method)

    return "Unknown"


def parse_sca_file(sca_path: Path):
    rows = []

    with sca_path.open("r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            line = line.strip()

            if not line.startswith("scalar "):
                continue

            parts = line.split()
            if len(parts) < 4:
                continue

            scalar_name = parts[2]

            if not scalar_name.startswith("AverageReceivedQualityAt_"):
                continue

            time = extract_time_from_scalar_name(scalar_name)
            if time is None:
                continue

            # Keep only 30-second intervals
            if time % 30 != 0:
                continue

            try:
                avg_quality = float(parts[3])
            except ValueError:
                continue

            rows.append(
                {
                    "time": time,
                    "average_received_quality": avg_quality,
                    "source_file": str(sca_path),
                }
            )

    return rows


def load_all_results():
    all_rows = []

    sca_files = list(RESULTS_DIR.rglob("*.sca"))

    if not sca_files:
        raise FileNotFoundError(f"No .sca files found under {RESULTS_DIR.resolve()}")

    for sca_file in sca_files:
        method = infer_method_from_path(sca_file)
        rows = parse_sca_file(sca_file)

        rep = None
        for part in sca_file.parts:
            if part.startswith("rep="):
                rep = part.split("=", 1)[1]

        for row in rows:
            row["method"] = method
            row["rep"] = rep
            all_rows.append(row)

    if not all_rows:
        raise RuntimeError(
            "No AverageReceivedQualityAt_ scalars found. "
            "Make sure you added recordScalar() in VoteAgg.cpp and reran the simulations."
        )

    return pd.DataFrame(all_rows)


def plot_average_quality(df: pd.DataFrame):
    summary = (
        df.groupby(["method", "time"], as_index=False)
        .agg(
            mean_quality=("average_received_quality", "mean"),
            std_quality=("average_received_quality", "std"),
            runs=("average_received_quality", "count"),
        )
        .sort_values(["method", "time"])
    )

    # (0,0)'dan başlamasını istersek
    start_rows = []
    #for method in summary["method"].unique():
        # start_rows.append(
        #     {
        #         "method": method,
        #         "time": 0,
        #         "mean_quality": 0.0,
        #         "std_quality": 0.0,
        #         "runs": 0,
        #     }
        # )

    summary = pd.concat([pd.DataFrame(start_rows), summary], ignore_index=True)
    summary = summary.sort_values(["method", "time"])

    plt.figure(figsize=(10, 6))

    for method, group in summary.groupby("method"):
        plt.plot(
            group["time"],
            group["mean_quality"],
            marker="o",
            label=method,
        )

    plt.axhline(0, linestyle="--", linewidth=1)
    plt.xlabel("Simulation time (s)")
    plt.ylabel("Average received service quality")
    plt.title("Average Received Service Quality Over Time by Aggregation Method")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    output_path = RESULTS_DIR / "average_received_quality_by_method.png"
    plt.savefig(output_path, dpi=300)
    plt.show()

    print(f"Saved plot to: {output_path}")
    return summary


if __name__ == "__main__":
    df = load_all_results()

    print("Loaded rows:")
    print(df.head())

    print("\nMethods found:")
    print(df["method"].value_counts())

    summary = plot_average_quality(df)

    csv_path = RESULTS_DIR / "average_received_quality_summary.csv"
    summary.to_csv(csv_path, index=False)
    print(f"Saved summary CSV to: {csv_path}")