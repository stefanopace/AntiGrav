#!/usr/bin/env python3
# One emery screenshot per weekday name. Run by hand:
#   python3 scripts/screenshot-weekdays.py
import datetime
import json
import subprocess
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "screenshots" / "weekdays"
EMU = "emery"
# Sunday 25 Oct 2026 through Saturday 31 Oct 2026. Noon stays clear of the DST change.
WEEK = [datetime.datetime(2026, 10, 25 + i, 12, 0, 0) for i in range(7)]
# Sunday-first, same order as src/c/ui/date/date_text.c. The trailing dot is the '*' glyph.
NAMES = {
    "IT": ["Do", "Lu", "Ma", "Me", "Gi", "Ve", "Sa"],
    "EN": ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"],
    "ES": ["Do", "Lu", "Ma", "Mi", "Ju", "Vi", "Sa"],
    "FR": ["Di", "Lu", "Ma", "Me", "Je", "Ve", "Sa"],
    "PT": ["Do", "Se", "Te", "Qa", "Qi", "Sx", "Sa"],
    "ID": ["Mi", "Sn", "Sl", "Ra", "Ka", "Ju", "Sa"],
    "DE": ["So*", "Mo*", "Di*", "Mi*", "Do*", "Fr*", "Sa*"],
    "NL": ["Zo*", "Ma*", "Di*", "Wo*", "Do*", "Vr*", "Za*"],
    "SV": ["Sö", "Må", "Ti", "On", "To", "Fr", "Lö"],
    "NB": ["Sø", "Ma", "Ti", "On", "To", "Fr", "Lø"],
    "TL": ["Li", "Lu", "Ma", "Mi", "Hu", "Bi", "Sa"],
    "DA": ["Sø", "Ma", "Ti", "On", "To", "Fr", "Lø"],
}


def run(cmd):
    print("+", " ".join(cmd), flush=True)
    result = subprocess.run(cmd, text=True, capture_output=True, cwd=ROOT)
    if result.returncode != 0:
        print(result.stdout)
        print(result.stderr)
        raise SystemExit(result.returncode)


def capture_at(when, path):
    stamp = str(int(when.timestamp()))
    # Set twice: a single call sometimes loses the race with the running emulator.
    run(["pebble", "emu-set-time", "--emulator", EMU, stamp])
    time.sleep(0.4)
    run(["pebble", "emu-set-time", "--emulator", EMU, stamp])
    time.sleep(1.6)
    run(["pebble", "screenshot", "--emulator", EMU, "--no-open", str(path)])


def main():
    package = json.loads((ROOT / "package.json").read_text())
    uuid = package["pebble"]["uuid"]
    keys = json.loads((ROOT / "build/js/message_keys.json").read_text())
    OUT.mkdir(parents=True, exist_ok=True)

    run(["pebble", "install", "--emulator", EMU])
    time.sleep(1.0)
    run(["pebble", "emu-set-timeline-quick-view", "--emulator", EMU, "off"])
    run(["pebble", "emu-time-format", "--emulator", EMU, "--format", "24h"])

    for lang, names in NAMES.items():
        run([
            "pebble", "send-app-message", "--emulator", EMU, "--app-uuid", uuid,
            "--int",
            f"{keys['AdvancedColors']}=0",
            f"{keys['ColorPrimary']}=0",
            f"{keys['ColorSecondary']}=16777215",
            "--string",
            f"{keys['Language']}={lang}",
            f"{keys['DateFormat']}=DayNameDay",
            f"{keys['Font']}=Replicant",
        ])
        time.sleep(2.3)
        for index, (when, name) in enumerate(zip(WEEK, names)):
            safe = name.replace("*", "")
            path = OUT / f"{lang}-{index}-{safe}.png"
            capture_at(when, path)
            print(f"  {path.name}  {name}", flush=True)

    print(f"DONE {OUT}")


if __name__ == "__main__":
    main()
