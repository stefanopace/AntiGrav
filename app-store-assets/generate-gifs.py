# Builds preview-fade.gif. Run by hand.
# Holds each still for 1 second, then a 120 ms fade. Frames are full and opaque.
import io
from pathlib import Path

from PIL import Image, GifImagePlugin

ROOT = Path(__file__).resolve().parent
NAMES = ["01-default.png", "02-slots.png", "03-racer.png", "04-samurai.png"]
HOLD_MS = 1000
FADE_STEPS = 6
FADE_FRAME_MS = 20


def blend(a, b, t):
    return tuple(round(a[i] + (b[i] - a[i]) * t) for i in range(3))


def build(images):
    pixels = [list(im.getdata()) for im in images]
    sequence = []
    durations = []
    for i, pix in enumerate(pixels):
        nxt = pixels[(i + 1) % len(pixels)]
        sequence.append(pix)
        durations.append(HOLD_MS)
        for k in range(1, FADE_STEPS + 1):
            t = k / (FADE_STEPS + 1)
            sequence.append([blend(a, b, t) for a, b in zip(pix, nxt)])
            durations.append(FADE_FRAME_MS)

    colors = []
    seen = set()
    for pix in sequence:
        for color in pix:
            if color not in seen:
                seen.add(color)
                colors.append(color)
    if len(colors) > 256:
        raise SystemExit(f"too many colors: {len(colors)}")

    table = []
    for color in colors:
        table.extend(color)
    table.extend([0, 0, 0] * (256 - len(colors)))
    index = {color: i for i, color in enumerate(colors)}
    size = images[0].size
    frames = []
    for pix in sequence:
        frame = Image.new("P", size)
        frame.putpalette(table)
        frame.putdata([index[color] for color in pix])
        frames.append(frame)
    return frames, durations


def save_full_frames(frames, durations, path):
    header = {"duration": durations[0], "loop": 0, "disposal": 1}
    buf = io.BytesIO()
    for chunk in GifImagePlugin._get_global_header(frames[0], header):
        buf.write(chunk)
    for frame, duration in zip(frames, durations):
        GifImagePlugin._write_frame_data(
            buf,
            frame,
            (0, 0),
            {"duration": duration, "disposal": 1},
        )
    buf.write(b";")
    path.write_bytes(buf.getvalue())


def main(platforms=None):
    chosen = ["emery"] if platforms is None else platforms
    for platform in chosen:
        folder = ROOT / platform
        images = [Image.open(folder / name).convert("RGB") for name in NAMES]
        frames, durations = build(images)
        save_full_frames(frames, durations, folder / "preview-fade.gif")
        print(f"{platform}: {len(frames)} frames, {len({c for im in images for c in im.getdata()})} still colors")

if __name__ == "__main__":
    main()
