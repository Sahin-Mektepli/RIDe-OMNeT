#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# 1. Load Data
# We assume the CSV has no header, or you added one manually.
# Here we define names explicitly.
df = pd.read_csv('ability_trust.csv', names=['Ability', 'TrustScore'])

# 2. Setup the "Academic" Aesthetic
sns.set_theme(style="whitegrid")
plt.figure(figsize=(10, 6))

# 3. Plot
sns.scatterplot(
    data=df,
    x='Ability',
    y='TrustScore',
    color='b',
    s=100, # Marker size
    alpha=0.7 # Transparency for overlapping points
)

# 4. Labels and Title
plt.title('Correlation between Node Ability and Final Trust Score', fontsize=16)
plt.xlabel('Node Ability ($A_i$)', fontsize=14)
plt.ylabel('Trust Score ($T_i$)', fontsize=14)

# 5. Save or Show
plt.savefig('trust_vs_ability.png', dpi=300)
print("Graph saved as trust_vs_ability.png")
