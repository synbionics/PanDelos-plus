import matplotlib.pyplot as plt
import numpy as np

plt.style.use("ggplot")

def pie_plot(data: list, labels: list, colors: list, laber_msg : list, title: str, ofolder: str, filename: str = "pie", extension: str = "png"):
    ig, ax = plt.subplots(figsize=(6, 3), subplot_kw=dict(aspect="equal"))
    
    if len(colors) == 0:
        wedges, texts = ax.pie(
            data,
            wedgeprops=dict(width=0.5, edgecolor='black', linewidth=0.3),
            startangle=0,
        )
    else:
        wedges, texts = ax.pie(
            data,
            wedgeprops=dict(width=0.5, edgecolor='black', linewidth=0.3),
            startangle=0,
            colors=colors
        )
    
    total = sum(data)
    bbox_props = dict(boxstyle="square,pad=0.3", fc="w", ec="k", lw=0.72)
    kw = dict(arrowprops=dict(arrowstyle="-"),
              bbox=bbox_props, zorder=0, va="center")
    
    # per ogni fetta, calcolo della percentuale e creazione label
    for i, p in enumerate(wedges):
        # percentuale
        perc = data[i] / total * 100
        if perc == 0:
            continue
        
        # angolo medio per posizionare la label
        ang = (p.theta2 - p.theta1) / 2. + p.theta1
        y = np.sin(np.deg2rad(ang))
        x = np.cos(np.deg2rad(ang))
        horizontalalignment = {-1: "right", 1: "left"}[int(np.sign(x))]
        connectionstyle = f"angle,angleA=0,angleB={ang}"
        kw["arrowprops"].update({"connectionstyle": connectionstyle})
        
        
        
        new_label = f"{perc:.2f}% {laber_msg[i]} {labels[i]}"
        
        ax.annotate(new_label, xy=(x, y), xytext=(1.35 * np.sign(x), 1.4 * y),
                    horizontalalignment=horizontalalignment, **kw)
    
    ax.set_title(title)
    
    legend_labels = [f"{label}: {val} ({size/sum(data)*100:.1f}%)" 
                    for label, val, size in zip(labels, data, data)]
    
    plt.legend(legend_labels, loc="best", bbox_to_anchor=(1, 0.5))
    plt.tight_layout()
    plt.savefig(f"{ofolder}{filename}.{extension}")
    
    
def hist_plot(x: list, y: list, color: str, xlabel: str, ylabel: str, title: str, ofolder: str, filename: str = "hist", extension: str = "png", xticks: list = None, yticks: list = None):
    
    plt.bar(x, y, width=0.8, color = color, linewidth=0.3, edgecolor="black")
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    
    if xticks is not None:
        plt.xticks(xticks)
    
    if yticks is not None:
        plt.yticks(yticks)
    
    plt.title(title)
    plt.tight_layout()
    plt.savefig(f"{ofolder}{filename}.{extension}")