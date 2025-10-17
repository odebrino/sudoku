import os
import glob
import pandas as pd
import matplotlib.pyplot as plt

DATA_DIRS = ["results", "data/results"]
OUT_DIR = "plots"

def find_files():
    for d in DATA_DIRS:
        if os.path.isdir(d):
            files = sorted(glob.glob(os.path.join(d, "results_k*.txt")))
            if files:
                return d, files
    return None, []

def read_results_file(path: str) -> "pd.DataFrame":
    # Robust: skip any intro lines until we find a header that starts with 'name,'
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()
    header_idx = None
    for i, line in enumerate(lines[:10]):  # search first 10 lines
        if line.strip().lower().startswith("name,"):
            header_idx = i
            break
    if header_idx is None:
        # Try naive parse and then coerce columns if needed
        df = pd.read_csv(path)
        if df.shape[1] == 3 and list(df.columns) == [0, 1, 2]:
            df.columns = ["name", "time_ms", "solved"]
        return df
    else:
        return pd.read_csv(path, skiprows=header_idx)

def load_all(files):
    frames = []
    for f in files:
        try:
            df = read_results_file(f)
            kpart = os.path.basename(f).split("_k")
            if len(kpart) > 1:
                k = int(kpart[1].split(".")[0])
            else:
                k = -1
            df["k"] = k
            frames.append(df[["name","time_ms","solved","k"]])
        except Exception as e:
            print(f"skip {f}: {e}")
    if not frames:
        return pd.DataFrame(columns=["name","time_ms","solved","k"])
    return pd.concat(frames, ignore_index=True)

def main():
    d, files = find_files()
    if not files:
        print("⚠️ Aucun résultat trouvé dans le dossier de données.")
        return
    os.makedirs(OUT_DIR, exist_ok=True)

    df = load_all(files)
    if df.empty:
        print("⚠️ Aucun résultat trouvé après chargement.")
        return

    # aggregate: mean time per k per solver
    agg = df.groupby(["k","name"])["time_ms"].mean().reset_index()

    # plot per solver
    for name, sub in agg.groupby("name"):
        sub = sub.sort_values("k")
        plt.figure()
        plt.plot(sub["k"], sub["time_ms"], marker="o")
        plt.xlabel("k (given cells)")
        plt.ylabel("mean time (ms)")
        plt.title(f"Solver: {name}")
        out = os.path.join(OUT_DIR, f"{name}_moyenne.png")
        plt.savefig(out, bbox_inches="tight")
        plt.close()
        print(f"saved {out}")

    # global classement (lowest mean over all k first)
    total = agg.groupby("name")["time_ms"].mean().sort_values()
    plt.figure()
    total.plot(kind="bar")
    plt.ylabel("mean time over k (ms)")
    plt.title("Classement global")
    out = os.path.join(OUT_DIR, "classement_global.png")
    plt.savefig(out, bbox_inches="tight")
    plt.close()
    print(f"saved {out}")

if __name__ == "__main__":
    main()
