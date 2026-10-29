# Captures the store screenshots. Run by hand.
import datetime, json, subprocess, time
from pathlib import Path
from PIL import Image

RELEASE_DIR = Path(__file__).resolve().parent
ROOT = RELEASE_DIR.parent
OUT_ROOT = RELEASE_DIR
UUID = "7b758a23-0125-46e8-95ac-3b5ea1c27fd6"
KEYS = json.loads((ROOT / "build/js/message_keys.json").read_text())
WHITE, BLACK, MINT, AMBER, BLUE, GRAY = 0xFFFFFF, 0x000000, 0x00FFAA, 0xFFAA00, 0x00AAFF, 0xAAAAAA
PLATFORMS = [
    ("basalt", False),
    ("emery", False),
    ("diorite", True),
    ("flint", True),
    ("gabbro", False),
]

# Gabbro has two bands, not DateFormat. Closest reading of each shot.
GABBRO_DATE = {
    "DayMonthName": ("Day", "MonthName"),
    "DayMonthNum": ("DayName", "DayMonth"),
    "MonthDayNum": ("DayName", "MonthDay"),
    "DayNameMonthDay": ("DayName", "MonthName"),
    "YearMonthDay": ("DayMonth", "MonthName"),
}

def run(cmd, cwd=ROOT):
    print("+", " ".join(cmd), flush=True)
    result = subprocess.run(cmd, text=True, capture_output=True, cwd=cwd)
    if result.returncode != 0:
        print(result.stdout)
        print(result.stderr)
        raise SystemExit(result.returncode)

def send(emu, ints=None, strings=None):
    cmd = ["pebble", "send-app-message", "--emulator", emu, "--app-uuid", UUID]
    if ints:
        cmd += ["--int"] + [f"{KEYS[k]}={v}" for k, v in ints.items()]
    if strings:
        cmd += ["--string"] + [f"{KEYS[k]}={v}" for k, v in strings.items()]
    run(cmd)

def configure(emu, font, date_format, separator, slots, **flags):
    advanced = flags.get("advanced", False)
    send(emu, {
        "AdvancedColors": 1 if advanced else 0,
        "BarColors": 1 if flags.get("bar_colors") else 0,
        "IconColors": 0,
        "InvertColors": 1 if flags.get("invert") else 0,
        "ColorPrimary": BLACK,
        "ColorSecondary": WHITE,
        "ColorAccent": flags.get("accent", MINT),
        "ColorBackground": BLACK,
        "ColorHours": flags.get("hours", WHITE),
        "ColorMinutes": flags.get("minutes", WHITE),
        "ColorSeconds": WHITE,
        "ColorIcons": WHITE,
        "ColorDate": flags.get("date_color", WHITE),
        "ColorBar": flags.get("bar", MINT),
        "ColorBarWatchBattery": MINT,
        "ColorBarPhoneBattery": BLUE,
        "ColorBarSteps": MINT,
        "ColorBarSeconds": AMBER,
        "HoursLeadingZero": 1,
        "MinutesLeadingZero": 1,
        "SecondsLeadingZero": 1,
        "DayLeadingZero": 1,
        "MonthLeadingZero": 1,
        "AmPmIconEnabled": 1,
        "GoalIconEnabled": 1,
        "QuietTimeIconEnabled": 1,
        "BtIconEnabled": 1,
        "WatchBatteryMirrored": 1 if flags.get("watch_mirrored") else 0,
        "PhoneBatteryMirrored": 0,
        "StepGoalMirrored": 0,
        "SecondsBarMirrored": 1 if flags.get("seconds_mirrored") else 0,
        "StepGoal": 10000,
        "LowBatteryThreshold": 20,
    }, {
        "Font": font,
        "Language": "EN",
        "DateFormat": date_format,
        "DateSeparator": separator,
        "ShowSecondsOnTapCount": flags.get("seconds", "0"),
        "BatteryStyle": "5Bars",
        "PhoneBatteryStyle": "Continuous",
        "StepsStyle": "Continuous",
        "SecondsBarStyle": "5Bars",
        "SlotLeft": slots[0],
        "SlotCenter": slots[1],
        "SlotRight": slots[2],
        "ComboBarTop": "WatchBattery",
        "ComboBarBottom": "PhoneBattery",
        "IconsAlignment": "Automatic",
        "IconsPlaceholder": flags.get("icon_ph", "Thick"),
        "SecondsAlignment": "Automatic",
        "SecondsPlaceholder": flags.get("sec_ph", "Thick"),
        "GabbroDateAbove": GABBRO_DATE.get(date_format, ("Day", "MonthName"))[0],
        "GabbroDateBelow": GABBRO_DATE.get(date_format, ("Day", "MonthName"))[1],
    })
    time.sleep(2.3)
    if "phone" in flags:
        send(emu, {"PhoneBattery": flags["phone"], "PhoneBatteryCharging": 0})
        time.sleep(0.5)

def shoot(emu, path, h, m, s, battery, charging=False):
    cmd = ["pebble", "emu-battery", "--emulator", emu, "--percent", str(battery)]
    if charging:
        cmd.append("--charging")
    run(cmd)
    ts = str(int(datetime.datetime(2026, 10, 16, h, m, s).timestamp()))
    run(["pebble", "emu-set-time", "--emulator", emu, ts])
    time.sleep(1.2)
    run(["pebble", "screenshot", "--emulator", emu, "--no-open", str(path)])
    im = Image.open(path).convert("RGB")
    corner = im.getpixel((0, 0))
    print(f"  {path.name} {im.size} corner={corner}", flush=True)

def capture(emu, bw):
    out = OUT_ROOT / emu
    out.mkdir(parents=True, exist_ok=True)
    run(["pebble", "install", "--emulator", emu])
    time.sleep(1.0)
    run(["pebble", "emu-set-timeline-quick-view", "--emulator", emu, "off"])
    run(["pebble", "emu-bt-connection", "--emulator", emu, "--connected", "yes"])
    run(["pebble", "emu-time-format", "--emulator", emu, "--format", "24h"])

    configure(emu, "Replicant", "DayMonthName", "Slash", ("Icons", "Battery", "Seconds"),
              seconds="1")
    shoot(emu, out / "01-default.png", 10, 28, 36, 70)

    configure(emu, "Replicant", "DayMonthNum", "Slash", ("SecondsBar", "ComboA", "Icons"),
              seconds_mirrored=True, phone=45)
    shoot(emu, out / "02-slots.png", 7, 45, 18, 80, charging=True)

    style = dict(advanced=True, hours=WHITE, minutes=BLUE, date_color=GRAY, bar=AMBER, accent=AMBER)
    if bw:
        style["invert"] = True
    configure(emu, "Racer", "DayNameMonthDay", "Slash", ("Icons", "Battery", "Seconds"), **style)
    shoot(emu, out / "03-racer.png", 13, 46, 8, 60)

    configure(emu, "Samurai", "YearMonthDay", "Hyphen", ("SecondsBar", "Battery", "PhoneBattery"),
              advanced=True, bar_colors=True, watch_mirrored=True, phone=45)
    shoot(emu, out / "04-samurai.png", 21, 53, 44, 80)

def main(platforms=None, out_root=None):
    global OUT_ROOT
    if out_root is not None:
        OUT_ROOT = Path(out_root)
    chosen = PLATFORMS if platforms is None else platforms
    for emu, bw in chosen:
        print(f"\n==== {emu} bw={bw} ====", flush=True)
        capture(emu, bw)
    if any(emu == "basalt" for emu, _bw in chosen):
        configure("basalt", "Replicant", "DayMonthName", "Slash", ("Icons", "Battery", "Seconds"), seconds="1")
        run(["pebble", "emu-battery", "--emulator", "basalt", "--percent", "80"])
        run(["pebble", "emu-set-time", "--emulator", "basalt", str(int(datetime.datetime.now().timestamp()))])
    print("DONE")

if __name__ == "__main__":
    main()
