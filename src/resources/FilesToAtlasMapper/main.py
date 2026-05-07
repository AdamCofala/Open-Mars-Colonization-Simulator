from PIL import Image
import os

TILE_DIR = "Tiles"
OUTPUT = "atlas.png"

PADDING = 1
MAX_ROW_WIDTH = 1024

files = sorted([
    f for f in os.listdir(TILE_DIR)
    if f.endswith(".png")
])

images = []

for file in files:

    path = os.path.join(TILE_DIR, file)

    img = Image.open(path)

    images.append({
        "name": file,
        "img": img,
        "w": img.width,
        "h": img.height
    })

# -----------------------------
# PACKING
# -----------------------------

x = 0
y = 0

row_height = 0

atlas_w = MAX_ROW_WIDTH
atlas_h = 0

rects = {}

for data in images:

    w = data["w"]
    h = data["h"]

    # next row
    if x + w > MAX_ROW_WIDTH:
        x = 0
        y += row_height + PADDING
        row_height = 0

    rects[data["name"]] = {
        "x": x,
        "y": y,
        "w": w,
        "h": h
    }

    x += w + PADDING

    row_height = max(row_height, h)

atlas_h = y + row_height

# -----------------------------
# BUILD ATLAS
# -----------------------------

atlas = Image.new("RGBA", (atlas_w, atlas_h))

for data in images:

    r = rects[data["name"]]

    atlas.paste(
        data["img"],
        (r["x"], r["y"])
    )

atlas.save(OUTPUT)

# -----------------------------
# PRINT RECTS
# -----------------------------

print("=== RECTS ===")

for name, r in rects.items():

    print(
        f'{name}: '
        f'{{{r["x"]}, {r["y"]}, '
        f'{r["w"]}, {r["h"]}}}'
    )