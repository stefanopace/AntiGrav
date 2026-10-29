#!/bin/bash

# Build TTF fonts from fonts_mold.png and copy them to resources/fonts.
# On the host, re-run this script inside the container of the fork
# https://github.com/stefanopace/bitmap2ttf (imagestottf and FontForge).

cd "$(cd "$(dirname "$0")" && pwd)"

BITMAP2TTF_REF="d82716b5435d710d37073a53d7df29733e86ce4f"
BITMAP2TTF_IMAGE="watchface-bitmap2ttf:${BITMAP2TTF_REF}"
FONT_BITMAPS_DIR="./font_bitmaps"
FONT_TTF_DIR="./font_ttf"
PEBBLE_FONTS_DIR="../resources/fonts"
UNPACK_ONLY=0
SKIP_UNPACK=0

for arg in "$@"; do
    case "$arg" in
        --unpack-only) UNPACK_ONLY=1 ;;
        --skip-unpack) SKIP_UNPACK=1 ;;
        *)
            echo "Error: unknown argument: $arg"
            exit 1
            ;;
    esac
done

if [ "$UNPACK_ONLY" -eq 1 ] && [ "$SKIP_UNPACK" -eq 1 ]; then
    echo "Error: --unpack-only and --skip-unpack cannot be used together"
    exit 1
fi

if [ "$SKIP_UNPACK" -eq 0 ] && [ -d "$FONT_BITMAPS_DIR" ]; then
    echo "Error: directory $FONT_BITMAPS_DIR already exists. Remove it before running this script again."
    exit 1
fi

if [ "$SKIP_UNPACK" -eq 1 ] && [ ! -d "$FONT_BITMAPS_DIR" ]; then
    echo "Error: directory $FONT_BITMAPS_DIR does not exist. Run with --unpack-only first."
    exit 1
fi

if [ "$UNPACK_ONLY" -eq 0 ] && [ -d "$FONT_TTF_DIR" ]; then
    echo "Error: directory $FONT_TTF_DIR already exists. Remove it before running this script again."
    exit 1
fi

if [ "$UNPACK_ONLY" -eq 0 ] && [ ! -d "$PEBBLE_FONTS_DIR" ]; then
    echo "Error: directory $PEBBLE_FONTS_DIR does not exist"
    exit 1
fi

if [ -z "${INSIDE_BITMAP2TTF:-}" ]; then
    if ! command -v docker >/dev/null 2>&1; then
        echo "Error: Docker is required to build the fonts"
        exit 1
    fi

    if ! docker image inspect "$BITMAP2TTF_IMAGE" >/dev/null 2>&1; then
        echo "Building image ${BITMAP2TTF_IMAGE}"
        docker build -t "$BITMAP2TTF_IMAGE" \
            "https://github.com/stefanopace/bitmap2ttf.git#${BITMAP2TTF_REF}"
    fi

    # FontForge writes the uid's full name into the copyright string.
    # The Ubuntu image already has a user with uid 1000: update that name,
    # or create a user with the host uid and name.
    host_uid="$(id -u)"
    host_gid="$(id -g)"
    host_name="$(getent passwd "$host_uid" | cut -d: -f5 | cut -d, -f1)"
    if [ -z "$host_name" ]; then
        host_name="$(id -un)"
    fi

    repo_root="$(cd .. && pwd)"
    exec docker run --rm \
        -e INSIDE_BITMAP2TTF=1 \
        -e HOME=/tmp \
        -e HOST_UID="$host_uid" \
        -e HOST_GID="$host_gid" \
        -e HOST_NAME="$host_name" \
        -v "${repo_root}:/work" \
        -w /work/fonts-generator \
        "$BITMAP2TTF_IMAGE" \
        bash -c 'if existing=$(getent passwd "$HOST_UID" | cut -d: -f1) && [ -n "$existing" ]; then usermod -c "$HOST_NAME" -d /tmp "$existing"; else getent group "$HOST_GID" >/dev/null || groupadd -g "$HOST_GID" hostuser; useradd -M -u "$HOST_UID" -g "$HOST_GID" -c "$HOST_NAME" -d /tmp hostuser; existing=hostuser; fi; quoted_args=; if [ "$#" -gt 0 ]; then quoted_args=$(printf " %q" "$@"); fi; exec su --preserve-environment -s /bin/bash "$existing" -c "PATH=/opt/venv/bin:/usr/bin:/bin bash ./build-fonts.sh${quoted_args}"' bash "$@"
fi

if [ "$SKIP_UNPACK" -eq 0 ]; then
    python3 ./unpack.py || exit 1
fi

if [ "$UNPACK_ONLY" -eq 1 ]; then
    echo "Done!"
    exit 0
fi

echo "Creating directory $FONT_TTF_DIR"
mkdir -p "$FONT_TTF_DIR"

# Iterate over every directory in font_bitmaps
for folder in "$FONT_BITMAPS_DIR"/*; do
    if [ -d "$folder" ]; then
        folder_name=$(basename "$folder")
        font_name="${folder_name}.ttf"

        echo "Building font: $folder_name"

        # Build the TTF font
        imagestottf "$folder" "$font_name"

        if [ $? -eq 0 ]; then
            # Move the font into font_ttf, then copy it into the Pebble project
            mv "$font_name" "$FONT_TTF_DIR/"
            cp "$FONT_TTF_DIR/$font_name" "$PEBBLE_FONTS_DIR/$font_name"

            echo "✓ Font $folder_name built and copied"
        else
            echo "✗ Failed to build font $folder_name"
        fi
    fi
done

rm -rf "$FONT_TTF_DIR"

if [ "$SKIP_UNPACK" -eq 0 ]; then
    rm -rf "$FONT_BITMAPS_DIR"
fi

echo "Done!"
