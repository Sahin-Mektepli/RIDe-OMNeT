# Bunu tamamen AI yazdı - esm


import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from scipy.optimize import curve_fit


# --- 1. Güvenli Sigmoid Fonksiyonu ---
# np.exp() fonksiyonu çok büyük sayılarda 'inf' hatası verir.
# Bunu önlemek için giren veriyi -500 ile 500 arasında kısıtlıyoruz (clip).
# e^500 zaten devasa bir sayıdır, e^1000 ile aynı sonucu verir (sonsuz/çok büyük).
def sigmoid_safe(x, L, k, x0):
    # x tek bir sayı da olabilir, bir numpy dizisi de.
    # Argument: -k * (x - x0)
    argument = -k * (x - x0)
    
    # Taşmayı önlemek için limiti 700 ile sınırla (e^710 overflow yapar)
    # np.clip kullanımı sayesinde argument ne kadar büyük olursa olsun 700'de kalır.
    argument = np.clip(argument, -700, 700)
    
    return L / (1 + np.exp(argument))

# CSV dosyasını oku
try:
    df = pd.read_csv('results/trust_vs_ability.csv')
except FileNotFoundError:
    print("CSV dosyası bulunamadı!")
    exit()

# --- 2. Veri Temizliği ---
# Olası sonsuzluk veya NaN değerlerini temizle
df.replace([np.inf, -np.inf], np.nan, inplace=True)
df.dropna(inplace=True)

# Grafik Ayarları
plt.figure(figsize=(10, 6))
sns.set_style("whitegrid")

# Noktaları çizdir
sns.scatterplot(data=df, x='Ability', y='AvgTrustScore', hue='Type', style='Type', s=80, alpha=0.7)

colors = {'Government': 'blue', 'Private': 'orange'}
types = df['Type'].unique()

for t in types:
    subset = df[df['Type'] == t].sort_values('Ability')
    
    if len(subset) > 4: # Eğri uydurmak için en az 4-5 nokta olması sağlıklı
        x_data = subset['Ability'].values
        y_data = subset['AvgTrustScore'].values
        
        try:
            # --- 3. Başlangıç Tahminleri ve Sınırlar (Bounds) ---
            # curve_fit'in işini kolaylaştırmak için mantıklı sınırlar koyuyoruz.
            # L (Max Trust): 0.8 ile 1.1 arasında olmalı (Trust max 1 zaten)
            # k (Eğim): 0 ile 20 arasında olmalı (Negatif eğim istemiyoruz, aşırı dik de istemiyoruz)
            # x0 (Orta nokta): Verinin min ve max ability değerleri arasında olmalı
            
            p0 = [1.0, 0.5, np.mean(x_data)] # Başlangıç tahmini
            
            # Alt Sınırlar (Lower) ve Üst Sınırlar (Upper)
            # L, k, x0
            lower_bounds = [0.8, 0.0, np.min(x_data)] 
            upper_bounds = [1.1, 10.0, np.max(x_data)]
            
            popt, pcov = curve_fit(sigmoid_safe, x_data, y_data, p0=p0, 
                                   bounds=(lower_bounds, upper_bounds), 
                                   method='trf', maxfev=5000)
            
            # Eğriyi çiz
            x_range = np.linspace(x_data.min(), x_data.max(), 200)
            y_fit = sigmoid_safe(x_range, *popt)
            
            plt.plot(x_range, y_fit, color=colors.get(t, 'black'), linewidth=2.5, label=f'{t} Fit')
            print(f"{t} Parametreleri -> Max: {popt[0]:.2f}, Eğim: {popt[1]:.2f}, Dönüm: {popt[2]:.2f}")
            
        except Exception as e:
            print(f"{t} için eğri uydurulamadı: {e}")
            # Eğri uyduramazsa düz çizgi çekmeyi dener (fallback) veya boş geçer
    else:
        print(f"{t} için yeterli veri noktası yok.")

plt.title('Ability vs. Average Trust Score (Sigmoid Fit)')
plt.xlabel('True Ability (Potency * Consistency)')
plt.ylabel('Network-Wide Average Trust Score')
plt.ylim(-0.1, 1.1)
plt.legend()
plt.show()
