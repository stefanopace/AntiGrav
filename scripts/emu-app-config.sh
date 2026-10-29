#!/usr/bin/env bash
set -euo pipefail

# pebble opens the configuration page with webbrowser.open_new(), which asks
# Firefox for a new window (-new-window). Pointing BROWSER at this script makes
# that call land here: open a tab in the window already running instead.
if [[ "${1:-}" == file:* || "${1:-}" == http:* || "${1:-}" == https:* ]]; then
  nohup firefox --new-tab "$1" >/dev/null 2>&1 &
  exit 0
fi

# Relaunch pebble emu-app-config after every save.
# Ctrl+C or a connection error stops the loop.
# Extra arguments are passed through, for example:
#   ./scripts/emu-app-config.sh --emulator flint

SCRIPT_PATH="$(cd "$(dirname "$0")" && pwd)/$(basename "$0")"
cd "$(cd "$(dirname "$0")/.." && pwd)"
export BROWSER="$SCRIPT_PATH"

while true; do
  pebble emu-app-config "$@" || exit $?
done
