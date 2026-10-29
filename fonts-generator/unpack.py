#!/usr/bin/env python3
import json
import os
from PIL import Image

# Path of fonts.json
fonts_json_path = os.path.join(os.path.dirname(__file__), 'fonts.json')
# Path of the fonts_mold.png image
fonts_mold_path = os.path.join(os.path.dirname(__file__), 'fonts_mold.png')
# Path of the font_bitmaps directory
font_bitmaps_path = os.path.join(os.path.dirname(__file__), 'font_bitmaps')

# Open fonts_mold.png
img = Image.open(fonts_mold_path)
img_width, img_height = img.size

# Check whether a pixel is white
def is_white_pixel(pixel):
    """Return whether a pixel is white, across grayscale, RGB, and RGBA."""
    if isinstance(pixel, int):
        # Grayscale image
        return pixel >= 250  # Threshold for white
    elif isinstance(pixel, tuple):
        # RGB or RGBA image
        if len(pixel) == 3:
            # RGB
            return all(c >= 250 for c in pixel[:3])
        elif len(pixel) == 4:
            # RGBA
            return all(c >= 250 for c in pixel[:3])
    return False

# Check whether a pixel is black
def is_black_pixel(pixel):
    """Return whether a pixel is black, across grayscale, RGB, and RGBA."""
    if isinstance(pixel, int):
        # Grayscale image
        return pixel <= 5  # Threshold for black
    elif isinstance(pixel, tuple):
        # RGB or RGBA image
        if len(pixel) == 3:
            # RGB
            return all(c <= 5 for c in pixel[:3])
        elif len(pixel) == 4:
            # RGBA
            return all(c <= 5 for c in pixel[:3])
    return False

# Cursor for walking the image pixels
class ImageCursor:
    def __init__(self, image):
        self.image = image
        self.max_x, self.max_y = image.size

        # Find the first white pixel in the first column (x=0)
        first_white_y = None
        for y in range(self.max_y):
            pixel = image.getpixel((0, y))
            if is_white_pixel(pixel):
                first_white_y = y
                break

        # Place the cursor at x=0, two pixels above the first white pixel
        if first_white_y is not None:
            initial_y = max(0, first_white_y - 2)
            self.x = 0
            self.y = initial_y

            # Font height is the run of white pixels starting two pixels
            # below the initial position (that is, at first_white_y)
            font_height = 0
            start_y = first_white_y  # 2 pixels below initial_y
            for y in range(start_y, self.max_y):
                pixel = image.getpixel((0, y))
                if is_white_pixel(pixel):
                    font_height += 1
                else:
                    break

            # Drop the 4 pixels used as empty spacing
            self.font_height = max(0, font_height - 4)
            print(f"First white pixel found at row: {first_white_y}")
            print(f"Initial cursor position set to: (0, {initial_y})")
            print(f"Font height: {font_height} pixels")
        else:
            print("Warning: no white pixel found in the first column!")
            self.x = 0
            self.y = 0
            self.font_height = 0

    def move_right(self, steps=1):
        """Move the cursor right."""
        self.x = min(self.x + steps, self.max_x - 1)

    def move_left(self, steps=1):
        """Move the cursor left."""
        self.x = max(self.x - steps, 0)

    def move_down(self, steps=1):
        """Move the cursor down."""
        self.y = min(self.y + steps, self.max_y - 1)

    def move_up(self, steps=1):
        """Move the cursor up."""
        self.y = max(self.y - steps, 0)

    def set_position(self, x, y):
        """Set the cursor position."""
        self.x = max(0, min(x, self.max_x - 1))
        self.y = max(0, min(y, self.max_y - 1))

    def get_position(self):
        """Return the current position (x, y)."""
        return (self.x, self.y)

    def get_pixel(self):
        """Return the pixel at the current position."""
        return self.image.getpixel((self.x, self.y))

    def get_font_height(self):
        """Return the current font height."""
        return self.font_height

    def get_glyph_width(self):
        """Return the width of the current glyph.

        The cursor must already sit on the glyph's first black pixel.
        The width is the distance to the second black pixel.
        """
        current_x = self.x
        current_y = self.y

        # Make sure we are on a black pixel
        current_pixel = self.image.getpixel((current_x, current_y))
        if not is_black_pixel(current_pixel):
            # If not, search for the first black pixel
            for x in range(current_x, self.max_x):
                pixel = self.image.getpixel((x, current_y))
                if is_black_pixel(pixel):
                    current_x = x
                    self.x = x  # Update the cursor position
                    break
            else:
                # No black pixel found
                return 0

        # Find the second black pixel (end of the glyph)
        second_black_x = None
        for x in range(current_x + 1, self.max_x):
            pixel = self.image.getpixel((x, current_y))
            if is_black_pixel(pixel):
                second_black_x = x
                break

        if second_black_x is None:
            # No second black pixel, so the width cannot be computed
            return 0

        # Width is the distance between the two black pixels
        return second_black_x - current_x

    def move_to_next_glyph(self):
        """Move the cursor to the next glyph of the same font.

        The black pixel that ends one glyph is the one that starts the next.
        """
        start_x = self.x
        current_y = self.y

        # Find the second black pixel (end of the current glyph = start of the next)
        second_black_x = None
        for x in range(start_x + 1, self.max_x):
            pixel = self.image.getpixel((x, current_y))
            if is_black_pixel(pixel):
                second_black_x = x
                break

        if second_black_x is not None:
            # The second black pixel is the start of the next glyph
            self.x = second_black_x
            return True
        else:
            # No second black pixel: this is the end
            return False

    def move_to_next_font(self):
        """Move the cursor to the next font.

        Return to column x=0, find the next white pixel below, and place
        the cursor two pixels above it.
        """
        # Return to column x=0
        current_y = self.y

        # Approximate end of the current font.
        # The font starts at current_y + 2 (first white pixel)
        # and is font_height + 4 tall, including the spacing pixels.
        font_start_y = current_y + 2
        font_end_y = font_start_y + self.font_height + 4

        # Search the first column for the next white pixel, starting after this font
        next_white_y = None
        for y in range(font_end_y + 1, self.max_y):
            pixel = self.image.getpixel((0, y))
            if is_white_pixel(pixel):
                next_white_y = y
                break

        if next_white_y is not None:
            # Place the cursor two pixels above the white pixel
            new_y = max(0, next_white_y - 2)
            self.x = 0
            self.y = new_y

            # Recompute the font height
            font_height = 0
            start_y = next_white_y
            for y in range(start_y, self.max_y):
                pixel = self.image.getpixel((0, y))
                if is_white_pixel(pixel):
                    font_height += 1
                else:
                    break

            # Drop the 4 pixels used as empty spacing
            self.font_height = max(0, font_height - 4)
            return True
        else:
            # No white pixel found: there are no more fonts
            return False

# Create the cursor for the image
cursor = ImageCursor(img)

# Read the JSON file
with open(fonts_json_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

print(f"Image loaded: {fonts_mold_path}")
print(f"Image size: {img_width}x{img_height} pixels")
print(f"Initial cursor position: {cursor.get_position()}")
print(f"Current font height: {cursor.get_font_height()} pixels")
print()

# Create the font_bitmaps directory if it does not exist
os.makedirs(font_bitmaps_path, exist_ok=True)

# Print each font name and walk all of its glyphs
for font in data['fonts']:
    name = font['name']
    order = font['order']

    # Decimal code points of the characters in order
    codepoints = [ord(char) for char in order]

    # Current font height
    font_height = cursor.get_font_height()

    # Subdirectory for this font: font_name + "_" + height
    font_folder_name = f"{name}_{font_height}"
    font_folder_path = os.path.join(font_bitmaps_path, font_folder_name)
    os.makedirs(font_folder_path, exist_ok=True)

    print(f"Name: {name}")
    print(f"Code points: {codepoints}")
    print(f"Font height: {font_height} pixels")
    print(f"Directory created: {font_folder_path}")
    print()

    # Move the cursor to the first black pixel of this font's first glyph.
    # The cursor is already on the right row; only the horizontal position is missing.
    current_x, current_y = cursor.get_position()
    first_black_x = None

    # Search for the first black pixel from the current position (it may already be there)
    for x in range(current_x, cursor.max_x):
        pixel = img.getpixel((x, current_y))
        if is_black_pixel(pixel):
            first_black_x = x
            cursor.set_position(x, current_y)
            break

    if first_black_x is None:
        print(f"  Warning: no black pixel found for the first glyph at row {current_y}!")
        continue

    # Walk every glyph of this font
    for i, codepoint in enumerate(codepoints):
        # Width of the current glyph (the cursor is already on the first black pixel)
        width = cursor.get_glyph_width()
        height = cursor.get_font_height()

        if width == 0:
            print(f"  Warning: width 0 for glyph {i+1} (code point {codepoint})!")
            break

        # Current cursor position
        glyph_x, glyph_y = cursor.get_position()

        # Crop the glyph out of the full image.
        # The cursor points at the left tick, so the crop starts on the next pixel
        # and stops before the right tick.
        glyph_region = img.crop((glyph_x + 1, glyph_y + 4, glyph_x + width, glyph_y + height + 4))
        # File name: codepoint.png
        glyph_filename = f"{codepoint}.png"
        glyph_filepath = os.path.join(font_folder_path, glyph_filename)

        # Save the glyph image
        glyph_region.save(glyph_filepath)

        print(f"  Glyph {i+1}/{len(codepoints)} (code point {codepoint}): {width}x{height} pixels -> {glyph_filename}")

        if codepoint >= 48 and codepoint <= 57 and height == 15:
            # Numeric glyph without the spacing.
            # The cursor points at the first black pixel, so the crop starts there.
            glyph_region = img.crop((glyph_x + 1, glyph_y + 4, glyph_x + width, glyph_y + height + 4))

            # File name: codepoint.png
            glyph_filename = f"{codepoint - 10}.png"
            glyph_filepath = os.path.join(font_folder_path, glyph_filename)

            # Save the glyph image
            glyph_region.save(glyph_filepath)


        # Move to the next glyph (except after the last one)
        if i < len(codepoints) - 1:
            if not cursor.move_to_next_glyph():
                print(f"Error: cannot find glyph {i+2}/{len(codepoints)} (code point {codepoints[i+1]})!")
                print(f"  Processed only {i+1} of {len(codepoints)} expected glyphs.")
                print(f"  Cursor position: {cursor.get_position()}")
                exit(1)

    # Move the cursor to the next font (except after the last one)
    if font != data['fonts'][-1]:
        if not cursor.move_to_next_font():
            print(f"Warning: cannot move to the next font!")
        else:
            print(f"Moved to the next font. New position: {cursor.get_position()}, height: {cursor.get_font_height()} pixels")

    print()
