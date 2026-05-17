import re
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt
RESULTS_DIR = Path(r"C:/Users/ipekm/Downloads/omnetpp-6.0.3-windows-x86_64/omnetpp-6.0.3/RIDe-OMNeT/src/results/VoteAgg/Camouflage")
#Bunu çalıştırdığında Good Service Ratio sonuçlarını verecek

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
    """
    Example scalar names:
      BadServiceRatioGoodNodesAt_10
      BadServiceRatioAt_10
    """
    match = re.search(r"At_(\d+)", name)
    if match:
        return int(match.group(1))
    return None


def parse_sca_file(sca_path: Path):
    """
    Reads one OMNeT++ .sca file and extracts bad service ratio scalar values.
    Keeps only results at 30-second intervals.
    """
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

            if not (
                scalar_name.startswith("BadServiceRatioGoodNodesAt_")
                or scalar_name.startswith("BadServiceRatioAt_")
            ):
                continue

            time = extract_time_from_scalar_name(scalar_name)
            if time is None:
                continue

            # Keep only 30-second intervals: 30, 60, 90, ...
            if time % 30 != 0:
                continue

            try:
                bad_ratio = float(parts[3])
            except ValueError:
                continue

            good_ratio = 1.0 - bad_ratio

            rows.append(
                {
                    "time": time,
                    "bad_service_ratio": bad_ratio,
                    "good_service_ratio": good_ratio,
                    "source_file": str(sca_path),
                }
            )

    return rows

def infer_method_from_path(path: Path):
    """
    Expected folder examples:
      results/VoteAggMethodSweep/method=0/rep=3/...
      results/VoteAggMethodSweep/method=2/rep=7/...
    """
    for part in path.parts:
        if part.startswith("method="):
            raw_method = part.split("=", 1)[1].replace('"', "")
            return METHOD_NAMES.get(raw_method, raw_method)

    return "Unknown"


def load_all_results():
    all_rows = []

    sca_files = list(RESULTS_DIR.rglob("*.sca"))

    if not sca_files:
        raise FileNotFoundError(f"No .sca files found under {RESULTS_DIR.resolve()}")

    for sca_file in sca_files:
        method = infer_method_from_path(sca_file)
        rows = parse_sca_file(sca_file)

        for row in rows:
            row["method"] = method

            # Optional: infer repetition from folder name
            rep = None
            for part in sca_file.parts:
                if part.startswith("rep="):
                    rep = part.split("=", 1)[1]
            row["rep"] = rep

            all_rows.append(row)

    if not all_rows:
        raise RuntimeError(
            "No bad service ratio scalars found. "
            "Check scalar names in .sca files."
        )

    return pd.DataFrame(all_rows)


def plot_good_service_ratio(df: pd.DataFrame):
    """
    Averages repetitions for each method and time.
    """
    summary = (
        df.groupby(["method", "time"], as_index=False)
        .agg(
            mean_good_service_ratio=("good_service_ratio", "mean"),
            std_good_service_ratio=("good_service_ratio", "std"),
            runs=("good_service_ratio", "count"),
        )
        .sort_values(["method", "time"])
    )

    plt.figure(figsize=(10, 6))

    for method, group in summary.groupby("method"):
        plt.plot(
            group["time"],
            group["mean_good_service_ratio"],
            marker="o",
            label=method,
        )

    plt.xlabel("Simulation time (s)")
    plt.ylabel("Good service ratio")
    plt.title("Good Service Ratio Over Time by Aggregation Method - Camouflage Attack")
    #plt.ylim(0.94, 1.05)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    output_path = RESULTS_DIR / "good_service_ratio_by_method.png"
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

    summary = plot_good_service_ratio(df)

    csv_path = RESULTS_DIR / "good_service_ratio_summary.csv"
    summary.to_csv(csv_path, index=False)
    print(f"Saved summary CSV to: {csv_path}")