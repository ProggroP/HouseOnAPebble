#!/usr/bin/env python3
# Erzeugt die Bildressourcen von House on a Pebble: Hausquerschnitt je Plattform und
# Sprite-Blatt der Figur. Alle Grafik ist hier im Skript gezeichnet, es gibt
# keine externen Vorlagen. Aufruf: python tools/gen_art.py
#
# Die Geometrie (Stockwerke, Treppen, Raumpositionen) muss zu den #defines in
# src/c/houseonapebble.c passen.

import os
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'resources', 'images')

# ---- Palette: nur Pebble-Farben (Kanaele 0/85/170/255), maximal 16 im Haus ---
SKY    = (85, 170, 255)
WHITE  = (255, 255, 255)
BLACK  = (0, 0, 0)
BEAM   = (0, 0, 85)
ROOF   = (0, 85, 85)
ATTIC  = (170, 170, 255)
MINT   = (170, 255, 170)
AQUA   = (170, 255, 255)
CREME  = (255, 255, 170)
ROSE   = (255, 170, 170)
GREY   = (170, 170, 170)
WOOD   = (170, 85, 0)
EARTH  = (85, 0, 0)
RED    = (255, 0, 0)
GREEN  = (0, 170, 0)
BLUE   = (0, 85, 170)

# ---- Geometrie je Plattform ---------------------------------------------------
PLATFORMS = {
    'emery':  dict(W=200, H=172, cx=100, apex=2,  half=100, F2=64, F1=110, F0=156,
                   left=6,  right=194, dx=66),
    'gabbro': dict(W=260, H=190, cx=130, apex=12, half=112, F2=74, F1=120, F0=166,
                   left=22, right=238, dx=74),
}
STAIR_HALF = 18    # Treppe laeuft von cx-18 nach cx+18 und zurueck
COL_HALF = 21      # halbe Breite des Treppenhauses
DOOR_H = 27        # Hoehe der offenen Durchgaenge


def rect(d, x, y, w, h, c):
    d.rectangle([x, y, x + w - 1, y + h - 1], fill=c)


# ---- Moebel: x ist die linke Kante, f die Oberkante des Bodens ----------------
def fridge(d, x, f):
    rect(d, x, f - 30, 13, 30, BEAM)
    rect(d, x + 1, f - 29, 11, 28, WHITE)
    rect(d, x + 1, f - 20, 11, 1, GREY)
    rect(d, x + 9, f - 26, 1, 4, BEAM)
    rect(d, x + 9, f - 17, 1, 6, BEAM)


def stove(d, x, f):
    # Herd mit Topf und Dunstabzug
    rect(d, x, f - 14, 16, 14, BEAM)
    rect(d, x + 1, f - 13, 14, 13, GREY)
    rect(d, x + 3, f - 9, 10, 7, BEAM)
    rect(d, x + 4, f - 8, 8, 5, RED)
    rect(d, x + 3, f - 12, 2, 1, BEAM)
    rect(d, x + 11, f - 12, 2, 1, BEAM)
    rect(d, x + 7, f - 20, 8, 6, BLUE)
    rect(d, x + 6, f - 20, 10, 1, BEAM)
    rect(d, x + 3, f - 38, 10, 5, GREY)
    rect(d, x + 6, f - 42, 4, 4, GREY)


def shelf_jars(d, x, f):
    rect(d, x, f - 30, 22, 2, WOOD)
    rect(d, x + 2, f - 36, 4, 6, RED)
    rect(d, x + 8, f - 37, 4, 7, GREEN)
    rect(d, x + 14, f - 35, 5, 5, BLUE)


def sofa(d, x, f):
    rect(d, x, f - 17, 30, 17, BEAM)
    rect(d, x + 1, f - 16, 28, 10, BLUE)
    rect(d, x + 1, f - 9, 28, 7, BLUE)
    rect(d, x + 3, f - 11, 11, 1, BEAM)
    rect(d, x + 16, f - 11, 11, 1, BEAM)
    rect(d, x + 2, f - 2, 2, 2, BEAM)
    rect(d, x + 26, f - 2, 2, 2, BEAM)
    rect(d, x + 21, f - 15, 6, 4, CREME)


def tv(d, x, f):
    rect(d, x, f - 10, 18, 10, WOOD)
    rect(d, x + 1, f - 8, 7, 7, EARTH)
    rect(d, x + 10, f - 8, 7, 7, EARTH)
    rect(d, x + 1, f - 24, 16, 14, BEAM)
    # Bildschirm aus: er leuchtet nur, wenn die Figur fernsieht (im C-Code)
    rect(d, x + 3, f - 22, 12, 10, BLACK)
    rect(d, x + 4, f - 21, 3, 1, GREY)


def picture(d, x, y):
    rect(d, x, y, 18, 12, WOOD)
    rect(d, x + 1, y + 1, 16, 10, SKY)
    rect(d, x + 1, y + 7, 16, 4, GREEN)
    rect(d, x + 11, y + 2, 3, 3, WHITE)


def bed(d, x, f):
    rect(d, x, f - 14, 3, 14, WOOD)
    rect(d, x + 35, f - 10, 3, 10, WOOD)
    rect(d, x + 2, f - 9, 34, 7, WOOD)
    rect(d, x + 3, f - 14, 32, 6, WHITE)
    rect(d, x + 3, f - 12, 9, 4, AQUA)
    rect(d, x + 13, f - 14, 22, 6, RED)
    rect(d, x + 13, f - 14, 22, 1, ROSE)


def nightstand(d, x, f):
    rect(d, x, f - 9, 10, 9, WOOD)
    rect(d, x + 1, f - 6, 8, 1, EARTH)
    rect(d, x + 4, f - 13, 2, 4, BEAM)
    rect(d, x + 1, f - 19, 8, 6, CREME)
    rect(d, x + 1, f - 13, 8, 1, BEAM)


def window(d, x, y, w=20, h=16):
    rect(d, x, y, w, h, WOOD)
    rect(d, x + 1, y + 1, w - 2, h - 2, SKY)
    rect(d, x + w // 2, y + 1, 1, h - 2, WOOD)
    rect(d, x + 1, y + h // 2, w - 2, 1, WOOD)
    rect(d, x + 1, y + 1, 3, h - 2, ROSE)
    rect(d, x + w - 4, y + 1, 3, h - 2, ROSE)


def tub(d, x, f):
    rect(d, x, f - 13, 30, 2, BEAM)
    rect(d, x + 1, f - 12, 28, 10, WHITE)
    rect(d, x + 1, f - 3, 28, 1, GREY)
    rect(d, x + 3, f - 2, 3, 2, BEAM)
    rect(d, x + 24, f - 2, 3, 2, BEAM)
    # Duschstange und Brause
    rect(d, x + 26, f - 34, 1, 21, GREY)
    rect(d, x + 20, f - 34, 7, 1, GREY)
    rect(d, x + 19, f - 33, 4, 2, BEAM)


def sink(d, x, f):
    rect(d, x + 3, f - 12, 4, 12, WHITE)
    rect(d, x, f - 15, 10, 4, WHITE)
    rect(d, x, f - 11, 10, 1, GREY)
    rect(d, x + 4, f - 18, 1, 3, BEAM)
    rect(d, x + 1, f - 32, 8, 11, GREY)
    rect(d, x + 2, f - 31, 6, 9, SKY)


def toilet(d, x, f):
    rect(d, x, f - 19, 5, 11, WHITE)
    rect(d, x, f - 19, 5, 1, GREY)
    rect(d, x + 2, f - 9, 8, 3, WHITE)
    rect(d, x + 2, f - 6, 4, 6, WHITE)
    rect(d, x + 2, f - 9, 8, 1, GREY)


def telescope(d, x, f):
    # Dreibein
    d.line([x + 11, f - 20, x + 5, f - 1], fill=BEAM)
    d.line([x + 11, f - 20, x + 17, f - 1], fill=BEAM)
    rect(d, x + 11, f - 20, 1, 19, BEAM)
    # Rohr: Okular rechts unten, Linse links oben im Dachfenster
    d.line([x + 21, f - 16, x + 3, f - 28], fill=BLUE, width=4)
    d.line([x + 1, f - 26, x + 5, f - 31], fill=WHITE, width=2)
    rect(d, x + 21, f - 16, 3, 2, BEAM)


def skylight(d, cx, roof_y):
    # Dachfenster in der linken Schraege, das Teleskop ragt hinein
    near, far = cx - 56, cx - 74
    pts = [(near, roof_y(near) + 2), (far, roof_y(far) + 2),
           (far, roof_y(far) + 13), (near, roof_y(near) + 13)]
    d.polygon(pts, fill=SKY, outline=WOOD)


def round_window(d, cx, cy):
    d.ellipse([cx - 8, cy - 8, cx + 8, cy + 8], fill=WOOD)
    d.ellipse([cx - 6, cy - 6, cx + 6, cy + 6], fill=BEAM)
    rect(d, cx - 3, cy - 3, 1, 1, WHITE)
    rect(d, cx + 2, cy - 1, 1, 1, WHITE)
    rect(d, cx - 1, cy + 3, 1, 1, WHITE)


def workbench(d, x, f):
    rect(d, x, f - 13, 30, 3, WOOD)
    rect(d, x + 1, f - 10, 3, 10, EARTH)
    rect(d, x + 26, f - 10, 3, 10, EARTH)
    rect(d, x + 1, f - 5, 28, 2, EARTH)
    # Raketenmodell auf der Werkbank
    rect(d, x + 4, f - 22, 5, 9, WHITE)
    rect(d, x + 5, f - 24, 3, 2, RED)
    rect(d, x + 6, f - 25, 1, 1, RED)
    rect(d, x + 2, f - 16, 2, 3, RED)
    rect(d, x + 9, f - 16, 2, 3, RED)
    rect(d, x + 5, f - 20, 3, 2, BLUE)
    # Werkzeugkiste
    rect(d, x + 15, f - 16, 10, 3, GREEN)


def stairs(d, cx, bottom, top, rising_right):
    # Stufen zwischen zwei Boeden, massiv bis zur Bodenlinie gefuellt
    steps = 6
    w = 2 * STAIR_HALF // steps
    rise = (bottom - top) / steps
    for i in range(steps):
        step_top = int(round(bottom - (i + 1) * rise))
        x = cx - STAIR_HALF + i * w if rising_right else cx + STAIR_HALF - (i + 1) * w
        rect(d, x, step_top, w, bottom - step_top, WOOD)
        rect(d, x, step_top, w, 1, CREME)


# ---- Haus ---------------------------------------------------------------------
def house(name, W, H, cx, apex, half, F2, F1, F0, left, right, dx):
    img = Image.new('RGB', (W, H), SKY)
    d = ImageDraw.Draw(img)

    # Sonne, Wolke, Mond und Sterne zeichnet der C-Code je nach Tageszeit.

    # Boden im Querschnitt: Wiese und Erde
    rect(d, 0, F0 + 4, W, 4, GREEN)
    rect(d, 0, F0 + 8, W, H - F0 - 8, EARTH)

    slope = (F2 - 8 - apex) / half

    def roof_y(x):
        return apex + abs(x - cx) * slope

    # Schornstein hinter dem Dach
    chim = cx + 48
    rect(d, chim, int(roof_y(chim)) - 14, 12, 20, EARTH)
    rect(d, chim - 1, int(roof_y(chim)) - 16, 14, 3, BEAM)

    # Dach
    d.polygon([(cx, apex), (cx + half, F2 - 8), (cx + half, F2 - 3),
               (cx - half, F2 - 3), (cx - half, F2 - 8)], fill=ROOF)
    d.line([(cx, apex), (cx + half, F2 - 8)], fill=BEAM)
    d.line([(cx, apex), (cx - half, F2 - 8)], fill=BEAM)

    # Hauskoerper mit Aussenwand
    rect(d, left, F2 - 10, right - left, F0 + 5 - (F2 - 10), BEAM)

    # Dachboden: Innenflaeche unter den Schraegen
    il, ir = left + 3, right - 3
    d.polygon([(cx, apex + 9), (ir, roof_y(ir) + 9), (ir, F2), (il, F2),
               (il, roof_y(il) + 9)], fill=BEAM)
    d.polygon([(cx, apex + 11), (ir - 1, roof_y(ir) + 11), (ir - 1, F2), (il + 1, F2),
               (il + 1, roof_y(il) + 11)], fill=ATTIC)

    # Raeume im Erd- und Obergeschoss
    col_l, col_r = cx - COL_HALF, cx + COL_HALF
    rect(d, il, F2 + 4, col_l - il, F1 - F2 - 4, MINT)       # Schlafzimmer
    rect(d, col_r, F2 + 4, ir - col_r, F1 - F2 - 4, AQUA)    # Bad
    rect(d, col_l, F2 + 4, col_r - col_l, F1 - F2 - 4, GREY)
    rect(d, il, F1 + 4, col_l - il, F0 - F1 - 4, CREME)      # Kueche
    rect(d, col_r, F1 + 4, ir - col_r, F0 - F1 - 4, ROSE)    # Wohnzimmer
    rect(d, col_l, F1 + 4, col_r - col_l, F0 - F1 - 4, GREY)

    # Bad: Fliesenraster bis Hueft-Hoehe
    for x in range(col_r + 3, ir, 6):
        rect(d, x, F1 - 16, 1, 16, WHITE)
    rect(d, col_r + 2, F1 - 16, ir - col_r - 2, 1, WHITE)

    # Fussleisten
    for f in (F2, F1, F0):
        rect(d, il, f - 2, ir - il, 2, WOOD)

    # Zwischenwaende mit offenem Durchgang
    for f, ceil in ((F1, F2 + 4), (F0, F1 + 4)):
        for x in (col_l - 2, col_r):
            rect(d, x, ceil, 2, f - DOOR_H - ceil, BEAM)

    # Boeden (ueber die volle Breite), danach die Treppen
    for f in (F2, F1, F0):
        rect(d, il, f, ir - il, 4, BEAM)
    stairs(d, cx, F0, F1, True)
    stairs(d, cx, F1, F2, False)
    # Treppenloecher in den Boeden
    rect(d, cx + 4, F1, STAIR_HALF - 4, 4, GREY)
    rect(d, cx - STAIR_HALF, F2, STAIR_HALF - 4, 4, GREY)

    # Moebel. Linke Raeume ab il, rechte Raeume bis ir
    L = il + 2
    # Kueche: Kuehlschrank, Herd, Regal
    fridge(d, L, F0)
    stove(d, L + 16, F0)
    shelf_jars(d, L + 36, F0)
    # Wohnzimmer: Fernseher, Sofa, Bild
    tv(d, col_r + 3, F0)
    sofa(d, ir - 32, F0)
    picture(d, ir - 26, F0 - 37)
    # Schlafzimmer: Bett, Nachttisch, Fenster
    bed(d, L, F1)
    nightstand(d, L + 40, F1)
    window(d, L + 12, F1 - 38)
    # Bad: Waschbecken, Toilette, Wanne
    sink(d, col_r + 3, F1)
    toilet(d, col_r + 15, F1)
    tub(d, ir - 31, F1)
    # Treppenhaus: kleines Fenster
    rect(d, cx + 6, F1 - 37, 10, 10, WOOD)
    rect(d, cx + 7, F1 - 36, 8, 8, SKY)
    # Dachboden: Teleskop unter dem Rundfenster, Werkbank
    round_window(d, cx - 10, F2 - 23)
    skylight(d, cx, roof_y)
    telescope(d, cx - 66, F2)
    workbench(d, cx + 38, F2)
    # Lampe am Dachfirst
    rect(d, cx, apex + 11, 1, 8, BEAM)
    rect(d, cx - 3, apex + 19, 7, 3, RED)

    colors = img.getcolors(256)
    assert colors is not None and len(colors) <= 16, f'{name}: {len(colors)} Farben'
    path = os.path.join(OUT, f'house_{name}.png')
    pal = img.convert('P', palette=Image.ADAPTIVE, colors=16)
    assert {c for _, c in pal.convert('RGB').getcolors(256)} == {c for _, c in colors}, 'Palette verfaelscht'
    pal.save(path, optimize=True)
    print(f'{path}: {W}x{H}, {len(colors)} Farben')


# ---- Figur --------------------------------------------------------------------
# Eigene Figur: Bommelmuetze, blauer Pulli, von rechts gesehen. Der schwarze Umriss kommt automatisch dazu, damit die Figur auf
# allen Wandfarben lesbar bleibt.
SPRITE_COLORS = {
    'W': WHITE, 'R': RED, 'S': (255, 170, 85), 'K': BLACK, 'T': BLUE,
    'N': BEAM, 'B': EARTH, 'G': GREY,
}
TOP = [
    '.....WW.....',
    '....RRRR....',
    '...RRRRRR...',
    '...RRRRRR...',
    '...WWWWWW...',
    '...SSSKSS...',
    '...SSSSSSS..',
    '....SSSS....',
]
BODY_SWING = [
    '...TTTTTT...',
    '..TTTTTTTT..',
    '.TTTTTTTTTT.',
    'TT.TTTTTT.TT',
    'S..TTTTTT..S',
    '....TTTT....',
]
BODY_PASS = [
    '...TTTTTT...',
    '...TTTTTT...',
    '...TTTTTT...',
    '...TTTTTT...',
    '...TSSSTT...',
    '....TTTT....',
]
LEGS_STAND = [
    '....NNNN....',
    '....NNNN....',
    '....N..N....',
    '....N..N....',
    '....N..N....',
    '....B..BB...',
]
LEGS_APART = [
    '....NNNN....',
    '...NNNNNN...',
    '..NN....NN..',
    '.NN......NN.',
    '.N........NN',
    'BB........BB',
]
LEGS_PASS = [
    '....NNNN....',
    '....NNNN....',
    '.....NN.....',
    '.....NN.....',
    '.....NN.....',
    '.....BBB....',
]
TOP_FWD = ['.' + r[:-1] for r in TOP]   # Kopf vorgebeugt
BLANK = '.' * 12
LEGS_SIT = [
    '...NNNNNNNN.',
    '...NNNNNNNN.',
    '.........NN.',
    '.........NN.',
    '.........NN.',
    '.........NN.',
    '.........NN.',
    '.........NN.',
]
LEGS_DANGLE = LEGS_SIT[:7] + ['.........NBB']
LEGS_DOWN = [
    '....NNNN....',
    '....NNNN....',
    '....N..N....',
    '....N..N....',
    '....N..N....',
    '....B..BB...',
]
TORSO_LOW = ['...TTTTTT...', '..TTTTTT....', '..TTTTTT....', '...TTTTT....', '...TTTTT....', '....TTTT....']


def arm(row, text):
    """Ersetzt eine Zeile des Rumpfs, um den Arm zu bewegen."""
    body = list(TORSO_LOW)
    body[row] = text
    return body


# Reihenfolge = Frame-Nummern im C-Code (FRAME_*). Alle Figuren sehen nach
# rechts; 'flip' spiegelt Posen, die nach links schauen.
FRAMES = [
    # 0, 1: Gehen nach rechts
    (TOP + BODY_SWING + LEGS_APART, False),
    (TOP + BODY_PASS + LEGS_PASS, False),
    # 2, 3: Gehen nach links
    (TOP + BODY_SWING + LEGS_APART, True),
    (TOP + BODY_PASS + LEGS_PASS, True),
    # 4, 5: Kochen, Blick zum Herd
    (TOP + arm(1, '..TTTTTTTTSS') + LEGS_DOWN, True),
    (TOP + arm(2, '..TTTTTTTTSS') + LEGS_DOWN, True),
    # 6, 7: Auf dem Sofa, zweite Pose mit Hand am Mund (Popcorn)
    (TOP + ['...TTTTT....', '...TTTTT....', '...TTTTTS...', '...TTTTTS...'] + LEGS_SIT, True),
    (TOP[:7] + ['....SSSSS...'] + ['...TTTTTS...', '...TTTTTS...', '...TTTTT....', '...TTTTT....'] + LEGS_SIT, True),
    # 8, 9: Auf der Bettkante lesen, zweite Pose blaettert um
    (TOP + ['...TTTTT....', '...TTTTTSRRR', '...TTTTTSRWW', '...TTTTT....'] + LEGS_DANGLE, False),
    (TOP + ['...TTTTT....', '...TTTTTSRWW', '...TTTTTSRRR', '...TTTTT....'] + LEGS_DANGLE, False),
    # 10, 11: In der Wanne (die Muetze bleibt auf), zweite Pose schrubbt
    ([BLANK] * 10 + TOP + ['..SSSSSSSS..', '..SSSSSSSS..'], False),
    ([BLANK] * 10 + TOP[:5]
     + ['...SSSKSS.S.', '...SSSSSSSS.', '....SSSS..S.', '..SSSSSSSSS.', '..SSSSSSSS..'], False),
    # 12: Schlafen, Kopf auf dem Kissen, Muetze zum Kopfende
    ([BLANK] * 15 + ['.......S....', '.WRRRSSSS...', 'WWRRRSSKS...', '.WRRRSSSS...', '...RRSSSS...'], False),
    # 13, 14: Am Teleskop, Hand am Okular
    (TOP_FWD + arm(0, '...TTTTTTTSS') + LEGS_DOWN, True),
    (TOP_FWD + arm(1, '..TTTTTTTTSS') + LEGS_DOWN, True),
    # 15, 16: An der Werkbank mit Schraubenschluessel
    (TOP + arm(0, '...TTTTTTTSG') + LEGS_DOWN, False),
    (TOP + arm(2, '..TTTTTTTTSG') + LEGS_DOWN, False),
]
FRAME_W, FRAME_H = 14, 22   # inklusive Umriss


def sprite_frame(rows):
    im = Image.new('RGBA', (FRAME_W, FRAME_H), (0, 0, 0, 0))
    px = im.load()
    for y, row in enumerate(rows):
        for x, ch in enumerate(row):
            if ch != '.':
                px[x + 1, y + 1] = SPRITE_COLORS[ch] + (255,)
    # Umriss um alle deckenden Pixel
    base = im.copy().load()
    for y in range(FRAME_H):
        for x in range(FRAME_W):
            if base[x, y][3]:
                continue
            for nx, ny in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)):
                if 0 <= nx < FRAME_W and 0 <= ny < FRAME_H and base[nx, ny][3]:
                    px[x, y] = BLACK + (255,)
                    break
    return im


def sprites():
    sheet = Image.new('RGBA', (FRAME_W * len(FRAMES), FRAME_H), (0, 0, 0, 0))
    for i, (rows, flip) in enumerate(FRAMES):
        assert len(rows) == FRAME_H - 2 and all(len(r) == FRAME_W - 2 for r in rows), f'Frame {i}'
        fr = sprite_frame(rows)
        sheet.paste(fr.transpose(Image.FLIP_LEFT_RIGHT) if flip else fr, (i * FRAME_W, 0))
    path = os.path.join(OUT, 'tenant.png')
    sheet.save(path, optimize=True)
    print(f'{path}: {sheet.size[0]}x{sheet.size[1]}')


if __name__ == '__main__':
    os.makedirs(OUT, exist_ok=True)
    for name, geo in PLATFORMS.items():
        house(name, **geo)
    sprites()
