"""
Real-time chart for 5 MCP3008 channels + LED + Button state — via ESP32-S3 native USB.
Format: "A0:512,R0:0.500,E0:0.498,A1:...,A4:...,LED:1,BTN:0"
"""

import serial
import re
from collections import deque
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as patches

PORT = "/dev/tty.usbmodem11201"   # đổi sang /dev/cu.usbmodem11201 nếu treo lúc mở cổng
BAUD = 115200
WINDOW = 300
N_CH = 5

PATTERN = re.compile(
    r"A0:(\d+),R0:([\d.]+),E0:([\d.]+),"
    r"A1:(\d+),R1:([\d.]+),E1:([\d.]+),"
    r"A2:(\d+),R2:([\d.]+),E2:([\d.]+),"
    r"A3:(\d+),R3:([\d.]+),E3:([\d.]+),"
    r"A4:(\d+),R4:([\d.]+),E4:([\d.]+),"
    r"LED:(\d),BTN:(\d)"
)

COLORS = ["#0E6B52", "#2F4E8F", "#B0362A", "#B07A2A", "#6A4FA0"]

raw_bufs = [deque(maxlen=WINDOW) for _ in range(N_CH)]
ema_bufs = [deque(maxlen=WINDOW) for _ in range(N_CH)]
led_state = [0]
btn_state = [0]

ser = serial.Serial(PORT, BAUD, timeout=0.05)
ser.reset_input_buffer()

fig, ax = plt.subplots(figsize=(11, 5.5))

raw_lines, ema_lines = [], []
for i in range(N_CH):
    rl, = ax.plot([], [], lw=0.7, color=COLORS[i], alpha=0.3, label=f"CH{i} Raw")
    el, = ax.plot([], [], lw=1.8, color=COLORS[i], alpha=1.0, label=f"CH{i} EMA")
    raw_lines.append(rl)
    ema_lines.append(el)

ax.set_xlim(0, WINDOW)
ax.set_ylim(-0.05, 1.05)
ax.set_xlabel("Sample index (window of last 300 samples, ~6 s)")
ax.set_ylabel("Normalized value (0.0–1.0)")
ax.set_title("MCP3008 5-Channel — Raw vs EMA, Real-Time")
ax.grid(alpha=0.3)
ax.legend(loc="lower right", fontsize=7.5, ncol=5)

stats_text = ax.text(
    0.015, 0.97, "",
    transform=ax.transAxes,
    verticalalignment="top",
    fontsize=9,
    family="monospace",
    bbox=dict(boxstyle="round", facecolor="white", alpha=0.78, edgecolor="#8A948F"),
)

# Hai ô vuông trạng thái, đặt cạnh nhau góc trên phải — dùng transAxes để vị trí
# luôn cố định bất kể trục dữ liệu đang hiển thị khoảng nào
led_box = patches.Rectangle(
    (0.865, 0.93), 0.045, 0.045,
    transform=ax.transAxes,
    facecolor="#444444", edgecolor="black", linewidth=1.2,
)
btn_box = patches.Rectangle(
    (0.925, 0.93), 0.045, 0.045,
    transform=ax.transAxes,
    facecolor="#444444", edgecolor="black", linewidth=1.2,
)
ax.add_patch(led_box)
ax.add_patch(btn_box)
ax.text(0.860, 0.905, "LED", transform=ax.transAxes, fontsize=7.5, family="monospace", ha="right")
ax.text(0.920, 0.905, "BTN", transform=ax.transAxes, fontsize=7.5, family="monospace", ha="right")

def read_latest_lines():
    lines = []
    while ser.in_waiting:
        raw_line_str = ser.readline().decode("utf-8", errors="ignore").strip()
        if raw_line_str:
            lines.append(raw_line_str)
    return lines

def update(frame):
    for line_str in read_latest_lines():
        m = PATTERN.search(line_str)
        if not m:
            continue
        for i in range(N_CH):
            raw_bufs[i].append(float(m.group(i * 3 + 2)))
            ema_bufs[i].append(float(m.group(i * 3 + 3)))
        led_state[0] = int(m.group(16))
        btn_state[0] = int(m.group(17))

    if len(ema_bufs[0]) > 1:
        x_vals = range(len(ema_bufs[0]))
        stats_lines = []
        for i in range(N_CH):
            raw_lines[i].set_data(x_vals, raw_bufs[i])
            ema_lines[i].set_data(x_vals, ema_bufs[i])
            stats_lines.append(
                f"CH{i}  raw={raw_bufs[i][-1]:.3f}  ema={ema_bufs[i][-1]:.3f}"
            )
        stats_text.set_text("\n".join(stats_lines))

        led_box.set_facecolor("#2ECC71" if led_state[0] else "#444444")
        btn_box.set_facecolor("#E67E22" if btn_state[0] else "#444444")

    return (*raw_lines, *ema_lines, stats_text, led_box, btn_box)

ani = animation.FuncAnimation(
    fig, update, interval=20, blit=True, cache_frame_data=False
)

plt.tight_layout()
plt.show()

ser.close()