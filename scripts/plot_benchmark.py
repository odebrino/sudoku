#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re
import pandas as pd
import matplotlib.pyplot as plt

# ============================
# Style général
# ============================
plt.rcParams.update({
    "figure.dpi": 110,
    "savefig.dpi": 300,
    "font.size": 12,
    "axes.titlesize": 16,
    "axes.labelsize": 13,
    "legend.fontsize": 11,
    "xtick.labelsize": 11,
    "ytick.labelsize": 11,
    "axes.grid": True,
    "grid.linestyle": "--",
    "grid.alpha": 0.45,
    "axes.spines.top": False,
    "axes.spines.right": False,
})

# ============================
# Chemins et regex
# ============================
DOSSIER_DONNEES = "/home/odebrino/ENSTA/Language C/sudoku/data"
DOSSIER_SORTIE = "/home/odebrino/ENSTA/Language C/sudoku/plots"
os.makedirs(DOSSIER_SORTIE, exist_ok=True)

motif = re.compile(
    r"Solver=(\w+)\nMin=(.+)\nMax=(.+)\nMedian=(.+)\nMean=(.+)"
)

# ============================
# Chargement des données
# ============================
lignes = []
for fichier in os.listdir(DOSSIER_DONNEES):
    if fichier.startswith("results_k") and fichier.endswith(".txt"):
        k_match = re.findall(r"k(\d+)", fichier)
        if not k_match:
            continue
        k = int(k_match[0])
        with open(os.path.join(DOSSIER_DONNEES, fichier), "r") as f:
            contenu = f.read()
        for solver, minv, maxv, median, mean in motif.findall(contenu):
            try:
                lignes.append({
                    "k": k,
                    "Solveur": solver,
                    "Min": float(minv),
                    "Max": float(maxv),
                    "Médiane": float(median),
                    "Moyenne": float(mean),
                })
            except ValueError:
                pass

if not lignes:
    raise SystemExit("⚠️ Aucun résultat trouvé dans le dossier de données.")

df = pd.DataFrame(lignes).sort_values(["Solveur", "k"]).reset_index(drop=True)
solveurs = list(df["Solveur"].unique())

# ============================
# Fonctions utilitaires
# ============================
def nom_fichier(nom: str) -> str:
    return nom.lower().replace(" ", "_")

def sauver(nom_base: str):
    plt.tight_layout()
    plt.savefig(f"{nom_base}.png", bbox_inches="tight")
    print(f"✅ Enregistré : {nom_base}.png")

# ============================
# 1) Graphique détaillé par solveur (Moyenne, Médiane, Min–Max)
# ============================
for solveur in solveurs:
    sous = df[df["Solveur"] == solveur].sort_values("k")
    x = sous["k"].to_numpy()

    plt.figure(figsize=(9, 6))
    plt.fill_between(x, sous["Min"], sous["Max"], alpha=0.2, label="Min–Max")
    plt.plot(x, sous["Moyenne"], marker="o", linewidth=2.2, label="Moyenne")
    plt.plot(x, sous["Médiane"], marker="s", linestyle="--", linewidth=2.0, alpha=0.8, label="Médiane")

    plt.title(f"Performance – {solveur}")
    plt.xlabel("k (difficulté)")
    plt.ylabel("Temps (s)")
    plt.legend(frameon=True)
    sauver(os.path.join(DOSSIER_SORTIE, f"{nom_fichier(solveur)}_detail"))
    plt.close()

# ============================
# 2) Graphique simple par solveur (Moyenne uniquement)
# ============================
for solveur in solveurs:
    sous = df[df["Solveur"] == solveur].sort_values("k")
    x = sous["k"].to_numpy()

    plt.figure(figsize=(9, 6))
    plt.plot(x, sous["Moyenne"], marker="o", linewidth=2.5, label="Moyenne", color="C0")

    plt.title(f"Performance (Moyenne seule) – {solveur}")
    plt.xlabel("k (difficulté)")
    plt.ylabel("Temps (s)")
    plt.legend(frameon=True)
    sauver(os.path.join(DOSSIER_SORTIE, f"{nom_fichier(solveur)}_moyenne"))
    plt.close()

# ============================
# 3) Classement global par k
# ============================
classement = (
    df.groupby(["k", "Solveur"])["Moyenne"]
      .mean()
      .reset_index()
      .sort_values(["k", "Moyenne"])
)

# correction ici ✅
classement["Rang"] = classement.groupby("k")["Moyenne"].rank(method="min")

plt.figure(figsize=(9, 6))
for solveur in solveurs:
    sous = classement[classement["Solveur"] == solveur]
    plt.plot(sous["k"], sous["Rang"], marker="o", label=solveur)

plt.gca().invert_yaxis()  # le meilleur rang = 1 en haut
plt.title("Classement global par difficulté (k)")
plt.xlabel("k (difficulté)")
plt.ylabel("Rang (1 = meilleur)")
plt.legend(frameon=True)
sauver(os.path.join(DOSSIER_SORTIE, "classement_global"))
plt.close()

print("🎉 Tous les graphiques ont été générés avec succès.")
