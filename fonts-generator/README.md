# Font generator

Builds the watchface TTF fonts from `fonts_mold.png` and copies them to `resources/fonts`.

Docker is required. The first run builds an image from [stefanopace/bitmap2ttf](https://github.com/stefanopace/bitmap2ttf), which provides FontForge and `imagestottf`. Later runs reuse that image.

## Usage

From the repository root:

```sh
./fonts-generator/build-fonts.sh
```

The script stops if `font_bitmaps` or `font_ttf` already exists. Remove them before running it again. `--skip-unpack` is the exception: it requires `font_bitmaps` and refuses to run without it.

`font_ttf` is always deleted at the end, after the fonts have been copied to `resources/fonts`. A full run also deletes `font_bitmaps`. `--skip-unpack` leaves that directory in place.

The usual way to change a glyph is to edit `fonts_mold.png` and run the script again. If you would rather not touch the sheet, unpack it, edit the PNGs, and build from those files instead:

```sh
./fonts-generator/build-fonts.sh --unpack-only
# edit the PNGs in fonts-generator/font_bitmaps
./fonts-generator/build-fonts.sh --skip-unpack
```

`--unpack-only` writes one PNG per code point under `font_bitmaps` and does not build the TTF files. `--skip-unpack` reads that directory and copies the fonts into `resources/fonts`. The PNGs stay on disk, so they can be edited and built again.

## Inputs

`fonts_mold.png` is the sheet of glyphs. `fonts.json` lists each font name and the character order used to slice that sheet. Fonts are read top to bottom, in the same order as the `fonts` array. Glyphs are read left to right, in the same order as each font's `order` string. That string includes the leading space.

## Drawing `fonts_mold.png`

Use pure black on pure white. A pixel counts as black at 5 or below and as white at 250 or above.

Each font is one horizontal band. From top to bottom:

1. A row of one-pixel black ticks. Put a tick on the left edge of every glyph, and one more after the last glyph. The first tick is in column 0.
2. A solid black line across the band.
3. Two blank rows.
4. The glyphs, at the height the font should have.
5. Two blank rows.
6. A solid black line across the band.

Column 0 stays white from the first blank row through the last blank row. That white bar is the height ruler: its length is the glyph height plus the four blank rows. Do not draw glyph ink in column 0, and do not draw on a tick column. The glyph sits strictly between two ticks.

Two extra tick rows may sit above the required one. They are not read. If you draw them, keep them on the same columns as the row just above the top black line.

Leave a blank gap before the next font. The next band starts at its own tick row.
