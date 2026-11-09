// House on a Pebble -- Watchface fuer Emery und Gabbro.
//
// Ein Haus im Querschnitt, in dem eine kleine Figur wohnt. Der Tagesablauf legt
// fest, welche Orte zu welcher Uhrzeit infrage kommen; im eingestellten Rhythmus
// (1, 5, 15 oder 60 Minuten) wechselt sie den Ort. Der Ort ergibt sich allein aus
// der Uhrzeit, es muss also kein Zustand gespeichert werden.
//
// Akku: Der Timer laeuft nur beim Gehen und ein paar Sekunden nach der Ankunft,
// dem Oeffnen des Watchface oder einem Schwung des Handgelenks. Sonst steht das
// Bild bis zum naechsten Minuten-Tick still. canvas_update ist der einzige
// Zeichen-Einstiegspunkt.
//
// Die Geometrie muss zu tools/gen_art.py passen.

#include <pebble.h>

// ---- Farben ----------------------------------------------------------------
#define COLOR_EARTH        GColorBulgarianRose  // wie die Erde im Hausbild
#define COLOR_TEXT         GColorWhite
#define COLOR_STEAM        GColorLightGray      // Weiss verschwindet auf der Kuechenwand
#define COLOR_BUBBLE       GColorWhite
#define COLOR_BUBBLE_EDGE  GColorPictonBlue
#define COLOR_SLEEP_Z      GColorOxfordBlue
#define COLOR_SPARK        GColorRajah
#define COLOR_TV_1         GColorPictonBlue
#define COLOR_TV_2         GColorMintGreen
#define COLOR_TV_3         GColorCeleste
#define COLOR_TV_SPOT      GColorWhite
#define COLOR_SLEEP_Z_NIGHT GColorWhite
#define COLOR_SUN          GColorYellow
#define COLOR_SUN_LOW      GColorOrange
#define COLOR_CLOUD        GColorWhite
#define COLOR_CLOUD_LOW    GColorMelon
#define COLOR_MOON         GColorPastelYellow
#define COLOR_STAR         GColorWhite
#define COLOR_SKY_NIGHT    GColorOxfordBlue
#define COLOR_SKY_LOW      GColorRajah

// Nachtfarben der Hauspalette: {Tagfarbe, gedimmte Farbe}. Die Tagfarben muessen
// zu gen_art.py passen; der Himmel ist die Farbe, die je Tageszeit wechselt.
#define COLOR_SKY_DAY_ARGB8 GColorPictonBlueARGB8
static const uint8_t NIGHT_COLORS[][2] = {
  { COLOR_SKY_DAY_ARGB8,        GColorOxfordBlueARGB8 },
  { GColorWhiteARGB8,           GColorLibertyARGB8 },
  { GColorOxfordBlueARGB8,      GColorBlackARGB8 },
  { GColorBabyBlueEyesARGB8,    GColorLibertyARGB8 },
  { GColorMintGreenARGB8,       GColorMidnightGreenARGB8 },
  { GColorCelesteARGB8,         GColorCobaltBlueARGB8 },
  { GColorPastelYellowARGB8,    GColorArmyGreenARGB8 },
  { GColorMelonARGB8,           GColorImperialPurpleARGB8 },
  { GColorLightGrayARGB8,       GColorDarkGrayARGB8 },
  { GColorWindsorTanARGB8,      GColorBulgarianRoseARGB8 },
  { GColorRedARGB8,             GColorDarkCandyAppleRedARGB8 },
  { GColorIslamicGreenARGB8,    GColorDarkGreenARGB8 },
  { GColorCobaltBlueARGB8,      GColorDukeBlueARGB8 },
};

// ---- Layout ----------------------------------------------------------------
#ifdef PBL_PLATFORM_EMERY
#define HOUSE_H            172
#define HOUSE_CX           100
#define HOUSE_APEX         2
#define HOUSE_HALF         100
// Sonne, Mond und Wolke liegen frei im Himmel: das Dach gehoert zum Bild und
// wird nicht neu gezeichnet, es darf also nichts davon ueberlappen.
#define SUN_POS            GPoint(174, 18)
#define CLOUD_POS          GPoint(10, 10)
static const GPoint STARS[] = {
  {12, 8}, {30, 22}, {52, 10}, {70, 5}, {8, 30}, {128, 6}, {140, 14}, {192, 30}, {188, 6},
};
#define FLOOR_0            156
#define FLOOR_1            110
#define FLOOR_2            64
#define INNER_L            9    // Innenkanten der Aussenwaende
#define INNER_R            191
#define TIME_RECT          GRect(6, 174, 134, 50)
#define TIME_ALIGN         GTextAlignmentLeft
#define DAY_RECT           GRect(136, 176, 60, 24)
#define DATE_RECT          GRect(136, 198, 60, 24)
#define INFO_ALIGN         GTextAlignmentRight
#else
#define HOUSE_H            190
#define HOUSE_CX           130
#define HOUSE_APEX         12
#define HOUSE_HALF         112
// Rund: naeher zur Mitte, sonst schneidet der Kreis den Himmel an.
#define SUN_POS            GPoint(202, 37)
#define CLOUD_POS          GPoint(46, 29)
static const GPoint STARS[] = {
  {60, 30}, {40, 48}, {85, 22}, {100, 10}, {160, 10}, {175, 20}, {222, 50}, {235, 58},
};
#define FLOOR_0            166
#define FLOOR_1            120
#define FLOOR_2            74
#define INNER_L            25
#define INNER_R            235
#define TIME_RECT          GRect(0, 184, 260, 50)
#define TIME_ALIGN         GTextAlignmentCenter
#define DATE_RECT          GRect(0, 228, 260, 24)
#define INFO_ALIGN         GTextAlignmentCenter
#endif

#define TIME_FONT_KEY      FONT_KEY_LECO_42_NUMBERS
#define INFO_FONT_KEY      FONT_KEY_GOTHIC_18_BOLD

#define STAIR_HALF         18   // Treppen laufen zwischen CX-18 und CX+18
#define COL_L              (HOUSE_CX - 21)  // Treppenhaus
#define COL_R              (HOUSE_CX + 21)
#define ROOF_INSET         11   // Innenflaeche des Dachbodens unter der Schraege
#define FLOOR_THICK        4
#define SUN_R              8
#define MOON_R             7
#define PALETTE_SIZE       16   // Das Hausbild hat maximal 16 Farben (4 Bit)

// Moebelpositionen wie in gen_art.py
#define BED_X              (INNER_L + 2)
#define STOVE_X            (INNER_L + 18)
#define SOFA_X             (INNER_R - 32)
#define TUB_X              (INNER_R - 31)
#define TV_X               (HOUSE_CX + 24)
#define BENCH_X            (HOUSE_CX + 38)

// ---- Figur -----------------------------------------------------------------
#define SPRITE_W           14
#define SPRITE_H           22
#define SPRITE_FOOT_X      7    // Fussmitte im Sprite
#define SPRITE_FOOT_Y      21   // unterste Umrisszeile steht auf dem Boden

// Nummern im Sprite-Blatt, Reihenfolge wie FRAMES in gen_art.py
#define FRAME_STRIDE       0
#define FRAME_PASS         1
#define FRAME_LEFT_OFFSET  2    // dieselben Bilder nach links
#define FRAME_COOK         4
#define FRAME_SIT          6
#define FRAME_READ         8
#define FRAME_BATH         10
#define FRAME_SLEEP        12
#define FRAME_LOOK         13
#define FRAME_WORK         15

// ---- Animation -------------------------------------------------------------
#define WALK_FRAME_MS      100
#define WALK_STEP_PX       4
#define WALK_FRAMES_PER_POSE 2
#define MAX_WAYPOINTS      6
#define ACT_FRAME_MS       400
#define ACT_FRAMES         10   // gut 4 Sekunden Taetigkeit
#define ANNOUNCE_MS        3000 // Vorlauf nach dem Brummen, um auf die Uhr zu schauen
#define VIBE_LEVEL_MAX     3
// Brummdauer je Stufe (Index = Stufe, 0 = aus). Stufe 1 war in 1.0.0 die einzige
// und am Handgelenk kaum zu spueren.
static const uint32_t VIBE_MS[] = { 0, 80, 250, 500 };

// ---- Orte ------------------------------------------------------------------
typedef enum {
  PLACE_KITCHEN, PLACE_SOFA, PLACE_READ, PLACE_BED, PLACE_TUB, PLACE_TELESCOPE, PLACE_BENCH,
} Place;

// x/floor: wohin die Figur geht (Fussmitte). frame/frames: Posenbilder (Schlafen
// hat nur eines). dx/dy: Versatz der Pose gegenueber dem Standplatz, etwa vom
// Wannenrand in die Wanne.
typedef struct {
  int16_t x;
  uint8_t floor;
  uint8_t frame;
  uint8_t frames;
  int8_t dx;
  int8_t dy;
} PlaceInfo;

static const PlaceInfo PLACES[] = {
  [PLACE_KITCHEN]   = { STOVE_X + 21,   0, FRAME_COOK,  2,   0,   0 },
  [PLACE_SOFA]      = { SOFA_X + 11,    0, FRAME_SIT,   2,   0,   0 },
  [PLACE_READ]      = { BED_X + 30,     1, FRAME_READ,  2,   0,  -6 },
  [PLACE_BED]       = { BED_X + 24,     1, FRAME_SLEEP, 1, -15, -11 },
  [PLACE_TUB]       = { TUB_X - 8,      1, FRAME_BATH,  2,  24, -12 },
  [PLACE_TELESCOPE] = { HOUSE_CX - 38,  2, FRAME_LOOK,  2,   0,   0 },
  [PLACE_BENCH]     = { HOUSE_CX + 30,  2, FRAME_WORK,  2,   0,   0 },
};

static const int16_t FLOOR_Y[] = { FLOOR_0, FLOOR_1, FLOOR_2 };

// ---- Tagesablauf -----------------------------------------------------------
// Jeder Block gilt ab seiner Startzeit bis zum naechsten. Innerhalb eines Blocks
// wechselt die Figur reihum; ein Block mit nur einem Ort (Schlafen) bedeutet:
// sie bleibt liegen und geht nachts nirgends hin.
#define HM(h, m)           ((h) * 60 + (m))
#define BLOCK_PLACES_MAX   4

typedef struct {
  uint16_t start;
  uint8_t count;
  uint8_t places[BLOCK_PLACES_MAX];
} RoutineBlock;

static const RoutineBlock ROUTINE[] = {
  { HM(0, 0),   1, { PLACE_BED } },
  { HM(7, 0),   2, { PLACE_TUB, PLACE_KITCHEN } },
  { HM(8, 30),  3, { PLACE_BENCH, PLACE_READ, PLACE_TELESCOPE } },
  { HM(12, 0),  2, { PLACE_KITCHEN, PLACE_SOFA } },
  { HM(13, 30), 4, { PLACE_BENCH, PLACE_SOFA, PLACE_READ, PLACE_TELESCOPE } },
  { HM(18, 0),  2, { PLACE_KITCHEN, PLACE_SOFA } },
  { HM(19, 30), 3, { PLACE_SOFA, PLACE_TELESCOPE, PLACE_READ } },
  { HM(22, 30), 2, { PLACE_TUB, PLACE_READ } },
  { HM(23, 0),  1, { PLACE_BED } },
};

// ---- Tageszeiten -----------------------------------------------------------
// Fest eingestellt, unabhaengig von Standort und Jahreszeit.
#define DAWN_START         HM(6, 30)
#define DAY_START          HM(7, 30)
#define DUSK_START         HM(17, 30)
#define NIGHT_START        HM(19, 0)

typedef enum { PHASE_DAY, PHASE_LOW_SUN, PHASE_NIGHT } Phase;

// ---- AppMessage ------------------------------------------------------------
// Clay schickt alle Keys in einer Nachricht; ist die Inbox zu klein, verwirft
// die Firmware die ganze Nachricht stumm. Das Watchface sendet nie etwas.
#define INBOX_SIZE         256
#define OUTBOX_SIZE        64

// ---- Einstellungen ---------------------------------------------------------
// Das Struct wird als Byte-Dump gespeichert. Neue Felder NUR am Ende anhaengen.
#define SETTINGS_KEY       1
#define SETTINGS_VERSION   1
#define INTERVAL_DEFAULT   15

typedef struct {
  uint8_t version;
  uint8_t interval_min;
  uint8_t vibrate;        // Stufe 0..3; 1.0.0 speicherte 0/1, passt als Stufe 1
  uint8_t flick;          // seit 1.1.0: Schwung spielt die Taetigkeit erneut ab
} Settings;

typedef enum { STATE_IDLE, STATE_ANNOUNCING, STATE_WALKING, STATE_ACTING } State;

static Window *s_window;
static Layer *s_canvas;
static GFont s_time_font;
static GFont s_info_font;
static GBitmap *s_house;
static GBitmap *s_sprites;
static AppTimer *s_timer;
static Settings s_settings;

static State s_state;
static Place s_place;
static Place s_announced_place;
static GPoint s_pos;
static bool s_faces_left;
static uint32_t s_anim_tick;
static GPoint s_waypoints[MAX_WAYPOINTS];
static int s_waypoint_count;
static int s_waypoint_next;

static Phase s_phase;
static bool s_palettes_ready;
static GColor s_pal_day[PALETTE_SIZE];
static GColor s_pal_low[PALETTE_SIZE];    // Tagfarben, Himmel im Morgen-/Abendrot
static GColor s_pal_dark[PALETTE_SIZE];   // gedimmt, fuer das schlafende Haus
static GColor s_pal_lit[PALETTE_SIZE];    // Tagfarben, Himmel und Fenster nachts

static void settings_load(void) {
  s_settings.version = SETTINGS_VERSION;
  s_settings.interval_min = INTERVAL_DEFAULT;
  s_settings.vibrate = 0;
  s_settings.flick = 1;
  if (persist_exists(SETTINGS_KEY)) {
    persist_read_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
  }
  if (s_settings.vibrate > VIBE_LEVEL_MAX) {
    s_settings.vibrate = 0;
  }
  switch (s_settings.interval_min) {
    case 1: case 5: case 15: case 60: break;
    default: s_settings.interval_min = INTERVAL_DEFAULT;
  }
}

static void settings_save(void) {
  s_settings.version = SETTINGS_VERSION;
  persist_write_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
}

static const RoutineBlock *routine_block(int day_minute) {
  const RoutineBlock *block = &ROUTINE[0];
  for (unsigned i = 1; i < ARRAY_LENGTH(ROUTINE); i++) {
    if (ROUTINE[i].start <= day_minute) {
      block = &ROUTINE[i];
    }
  }
  return block;
}

// Der Tag des Jahres verschiebt die Reihenfolge, damit nicht jeder Tag gleich
// aussieht. Die Intervalle beginnen zur vollen Stunde.
static int routine_index(const RoutineBlock *block, int slot, int yday) {
  return (slot + yday) % block->count;
}

static Place place_for_time(const struct tm *t) {
  int interval = s_settings.interval_min;
  int slot = (t->tm_hour * 60 + t->tm_min) / interval;
  // Der Block richtet sich nach dem Beginn des Intervalls. Sonst ginge die Figur
  // beim Stundenrhythmus auch dann los, wenn ein Block um halb beginnt.
  const RoutineBlock *block = routine_block(slot * interval);
  int index = routine_index(block, slot, t->tm_yday);
  Place place = (Place)block->places[index];

  // An einer Blockgrenze koennte derselbe Ort zweimal hintereinander kommen.
  // Dann den naechsten nehmen, damit die Figur wirklich den Raum wechselt.
  if (slot > 0 && block->count > 1) {
    const RoutineBlock *prev_block = routine_block((slot - 1) * interval);
    int prev_index = routine_index(prev_block, slot - 1, t->tm_yday);
    if (prev_block != block && prev_block->places[prev_index] == place) {
      place = (Place)block->places[(index + 1) % block->count];
    }
  }
  return place;
}

static Phase phase_for_time(const struct tm *t) {
  int minute = t->tm_hour * 60 + t->tm_min;
  if (minute < DAWN_START || minute >= NIGHT_START) {
    return PHASE_NIGHT;
  }
  return (minute < DAY_START || minute >= DUSK_START) ? PHASE_LOW_SUN : PHASE_DAY;
}

// Die Paletten entstehen aus der des Bildes, weil die Reihenfolge der Farben dort
// vom SDK abhaengt. Ist das Bild nicht palettiert, bleibt es ohne Tagesphasen.
static void palettes_build(void) {
  GColor *source = gbitmap_get_palette(s_house);
  if (!source || gbitmap_get_format(s_house) != GBitmapFormat4BitPalette) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Hausbild ohne 4-Bit-Palette");
    return;
  }
  for (int i = 0; i < PALETTE_SIZE; i++) {
    GColor day = source[i];
    bool is_sky = (day.argb == COLOR_SKY_DAY_ARGB8);
    s_pal_day[i] = day;
    s_pal_low[i] = is_sky ? COLOR_SKY_LOW : day;
    s_pal_lit[i] = is_sky ? COLOR_SKY_NIGHT : day;
    s_pal_dark[i] = day;
    for (unsigned c = 0; c < ARRAY_LENGTH(NIGHT_COLORS); c++) {
      if (day.argb == NIGHT_COLORS[c][0]) {
        s_pal_dark[i] = (GColor){ .argb = NIGHT_COLORS[c][1] };
        break;
      }
    }
  }
  s_palettes_ready = true;
}

static void route_add(int16_t x, int16_t y) {
  if (s_waypoint_count < MAX_WAYPOINTS) {
    s_waypoints[s_waypoint_count++] = GPoint(x, y);
  }
}

// Weg zum Ziel: je Stockwerk zum passenden Treppenende, dann hoch oder runter.
// Die Treppen laufen im Zickzack, Treppe 0 steigt nach rechts, Treppe 1 nach
// links.
static void route_build(Place target) {
  s_waypoint_count = 0;
  s_waypoint_next = 0;
  int floor = PLACES[s_place].floor;
  int goal = PLACES[target].floor;
  while (floor < goal) {
    int16_t low = (floor == 0) ? HOUSE_CX - STAIR_HALF : HOUSE_CX + STAIR_HALF;
    int16_t high = (floor == 0) ? HOUSE_CX + STAIR_HALF : HOUSE_CX - STAIR_HALF;
    route_add(low, FLOOR_Y[floor]);
    route_add(high, FLOOR_Y[floor + 1]);
    floor++;
  }
  while (floor > goal) {
    int16_t low = (floor == 1) ? HOUSE_CX - STAIR_HALF : HOUSE_CX + STAIR_HALF;
    int16_t high = (floor == 1) ? HOUSE_CX + STAIR_HALF : HOUSE_CX - STAIR_HALF;
    route_add(high, FLOOR_Y[floor]);
    route_add(low, FLOOR_Y[floor - 1]);
    floor--;
  }
  route_add(PLACES[target].x, FLOOR_Y[goal]);
}

static void timer_cancel(void) {
  if (s_timer) {
    app_timer_cancel(s_timer);
    s_timer = NULL;
  }
}

static void act_step(void *context) {
  s_anim_tick++;
  if (s_anim_tick >= ACT_FRAMES) {
    s_timer = NULL;
    s_state = STATE_IDLE;
    s_anim_tick = 0;
  } else {
    s_timer = app_timer_register(ACT_FRAME_MS, act_step, NULL);
  }
  layer_mark_dirty(s_canvas);
}

static void act_start(void) {
  timer_cancel();
  s_state = STATE_ACTING;
  s_anim_tick = 0;
  s_timer = app_timer_register(ACT_FRAME_MS, act_step, NULL);
}

static void walk_step(void *context) {
  GPoint to = s_waypoints[s_waypoint_next];
  int dx = to.x - s_pos.x;
  int dy = to.y - s_pos.y;
  int dist = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

  if (dx != 0) {
    s_faces_left = dx < 0;
  }
  if (dist <= WALK_STEP_PX) {
    s_pos = to;
    s_waypoint_next++;
  } else {
    s_pos.x += dx * WALK_STEP_PX / dist;
    s_pos.y += dy * WALK_STEP_PX / dist;
  }
  s_anim_tick++;

  s_timer = NULL;
  if (s_waypoint_next >= s_waypoint_count) {
    act_start();
  } else {
    s_timer = app_timer_register(WALK_FRAME_MS, walk_step, NULL);
  }
  layer_mark_dirty(s_canvas);
}

static void walk_to(Place target) {
  timer_cancel();
  route_build(target);
  s_place = target;
  s_state = STATE_WALKING;
  s_anim_tick = 0;
  s_timer = app_timer_register(WALK_FRAME_MS, walk_step, NULL);
}

static void announce_done(void *context) {
  s_timer = NULL;
  walk_to(s_announced_place);
}

// Wer die Option gewaehlt hat, bekommt ein kurzes Brummen und ein paar Sekunden
// Zeit zum Hinschauen. Still bleibt es in der Ruhezeit der Uhr und beim
// Schlafengehen oder Aufstehen, damit es niemanden weckt.
static void leave_for(Place target) {
  bool quiet = quiet_time_is_active() || s_place == PLACE_BED || target == PLACE_BED;
  if (!s_settings.vibrate || quiet) {
    walk_to(target);
    return;
  }
  static uint32_t segment;
  segment = VIBE_MS[s_settings.vibrate];
  vibes_enqueue_custom_pattern((VibePattern) { .durations = &segment, .num_segments = 1 });
  timer_cancel();
  s_announced_place = target;
  s_state = STATE_ANNOUNCING;
  s_anim_tick = 0;
  s_timer = app_timer_register(ANNOUNCE_MS, announce_done, NULL);
}

static void draw_house_part(GContext *ctx, GRect part) {
  gbitmap_set_bounds(s_house, part);
  graphics_draw_bitmap_in_rect(ctx, s_house, part);
}

// Zeichnet den Raum, in dem die Figur gerade ist, noch einmal in hellen Farben.
// Der Dachboden hat schraege Waende, also zeilenweise bis unter die Schraege.
static void draw_lit_room(GContext *ctx) {
  int y = s_pos.y;
  if (y <= FLOOR_2) {
    int top = HOUSE_APEX + ROOF_INSET;
    int rise = FLOOR_2 - 8 - HOUSE_APEX;
    for (int row = top; row < FLOOR_2; row++) {
      int half = (row - top) * HOUSE_HALF / rise;
      int left = HOUSE_CX - half;
      int right = HOUSE_CX + half;
      if (left < INNER_L + 1) { left = INNER_L + 1; }
      if (right > INNER_R - 1) { right = INNER_R - 1; }
      draw_house_part(ctx, GRect(left, row, right - left, 1));
    }
    return;
  }
  int top = (y > FLOOR_1) ? FLOOR_1 + FLOOR_THICK : FLOOR_2 + FLOOR_THICK;
  int bottom = (y > FLOOR_1) ? FLOOR_0 : FLOOR_1;
  int left = (s_pos.x < COL_L) ? INNER_L : (s_pos.x > COL_R) ? COL_R : COL_L;
  int right = (s_pos.x < COL_L) ? COL_L : (s_pos.x > COL_R) ? INNER_R : COL_R;
  draw_house_part(ctx, GRect(left, top, right - left, bottom - top));
}

static void draw_house(GContext *ctx, GRect full) {
  if (!s_palettes_ready) {
    graphics_draw_bitmap_in_rect(ctx, s_house, full);
    return;
  }
  if (s_phase != PHASE_NIGHT) {
    gbitmap_set_palette(s_house, (s_phase == PHASE_DAY) ? s_pal_day : s_pal_low, false);
    graphics_draw_bitmap_in_rect(ctx, s_house, full);
    return;
  }
  gbitmap_set_palette(s_house, s_pal_dark, false);
  graphics_draw_bitmap_in_rect(ctx, s_house, full);
  // Liegt die Figur im Bett, ist das Licht aus.
  if (s_state == STATE_WALKING || s_place != PLACE_BED) {
    gbitmap_set_palette(s_house, s_pal_lit, false);
    draw_lit_room(ctx);
    gbitmap_set_bounds(s_house, full);
  }
}

static void draw_sky(GContext *ctx) {
  graphics_context_set_antialiased(ctx, false);
  if (s_phase == PHASE_NIGHT) {
    graphics_context_set_fill_color(ctx, COLOR_STAR);
    for (unsigned i = 0; i < ARRAY_LENGTH(STARS); i++) {
      // Waehrend der Animation funkelt immer wieder ein anderer Stern.
      if (s_state == STATE_IDLE || (s_anim_tick + i) % 4 != 0) {
        graphics_fill_rect(ctx, GRect(STARS[i].x, STARS[i].y, 1, 1), 0, GCornerNone);
      }
    }
    // Sichel: heller Kreis, davor ein Kreis in Himmelfarbe
    graphics_context_set_fill_color(ctx, COLOR_MOON);
    graphics_fill_circle(ctx, SUN_POS, MOON_R);
    graphics_context_set_fill_color(ctx, COLOR_SKY_NIGHT);
    graphics_fill_circle(ctx, GPoint(SUN_POS.x + 4, SUN_POS.y - 2), MOON_R - 1);
  } else {
    bool low = (s_phase == PHASE_LOW_SUN);
    graphics_context_set_fill_color(ctx, low ? COLOR_SUN_LOW : COLOR_SUN);
    graphics_fill_circle(ctx, SUN_POS, SUN_R);
    graphics_context_set_fill_color(ctx, low ? COLOR_CLOUD_LOW : COLOR_CLOUD);
    graphics_fill_rect(ctx, GRect(CLOUD_POS.x + 7, CLOUD_POS.y, 14, 7), 3, GCornersAll);
    graphics_fill_rect(ctx, GRect(CLOUD_POS.x, CLOUD_POS.y + 4, 26, 7), 3, GCornersAll);
  }
  graphics_context_set_antialiased(ctx, true);
}

static void draw_tenant(GContext *ctx) {
  const PlaceInfo *place = &PLACES[s_place];
  int frame;
  int x = s_pos.x;
  int y = s_pos.y;

  if (s_state == STATE_WALKING) {
    bool stride = ((s_anim_tick / WALK_FRAMES_PER_POSE) % 2) == 0;
    frame = (stride ? FRAME_STRIDE : FRAME_PASS) + (s_faces_left ? FRAME_LEFT_OFFSET : 0);
    y -= stride ? 0 : 1;   // beim Durchschwingen hebt sich der Koerper
  } else {
    frame = place->frame + (s_anim_tick % place->frames);
    x += place->dx;
    y += place->dy;
  }
  gbitmap_set_bounds(s_sprites, GRect(frame * SPRITE_W, 0, SPRITE_W, SPRITE_H));
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_sprites,
      GRect(x - SPRITE_FOOT_X, y - SPRITE_FOOT_Y, SPRITE_W, SPRITE_H));
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
}

static void draw_z(GContext *ctx, int x, int y) {
  graphics_context_set_fill_color(ctx, (s_phase == PHASE_NIGHT) ? COLOR_SLEEP_Z_NIGHT : COLOR_SLEEP_Z);
  graphics_fill_rect(ctx, GRect(x, y, 5, 1), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(x + 3, y + 1, 1, 1), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(x + 2, y + 2, 1, 1), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(x + 1, y + 3, 1, 1), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(x, y + 4, 5, 1), 0, GCornerNone);
}

static void draw_bubble(GContext *ctx, int x, int y) {
  graphics_context_set_fill_color(ctx, COLOR_BUBBLE);
  graphics_fill_circle(ctx, GPoint(x, y), 2);
  graphics_context_set_stroke_color(ctx, COLOR_BUBBLE_EDGE);
  graphics_draw_circle(ctx, GPoint(x, y), 2);
}

// Was zur Taetigkeit gehoert, aber nicht zur Figur: Dampf, Bildschirm, Blasen,
// Funken. Wird nur gezeichnet, wenn die Figur angekommen ist.
static void draw_activity(GContext *ctx) {
  if (s_state == STATE_WALKING) {
    return;
  }
  int phase = s_anim_tick;
  int floor = FLOOR_Y[PLACES[s_place].floor];

  switch (s_place) {
    case PLACE_KITCHEN:
      // Dampf steigt aus dem Topf
      graphics_context_set_fill_color(ctx, COLOR_STEAM);
      for (int i = 0; i < 3; i++) {
        int rise = (phase + i * 2) % 6;
        int sway = ((i + rise / 2) % 2) * 3;
        graphics_fill_rect(ctx, GRect(STOVE_X + 8 + sway, floor - 23 - rise * 2, 2, 2),
                           0, GCornerNone);
      }
      break;
    case PLACE_SOFA: {
      // Laufendes Programm: Bildfarbe und ein heller Fleck wechseln
      GColor screen = (phase % 3 == 0) ? COLOR_TV_1 : (phase % 3 == 1) ? COLOR_TV_2 : COLOR_TV_3;
      graphics_context_set_fill_color(ctx, screen);
      graphics_fill_rect(ctx, GRect(TV_X + 3, floor - 22, 12, 10), 0, GCornerNone);
      graphics_context_set_fill_color(ctx, COLOR_TV_SPOT);
      graphics_fill_rect(ctx, GRect(TV_X + 4 + (phase % 3) * 3, floor - 19, 4, 4), 0, GCornerNone);
      break;
    }
    case PLACE_TUB:
      draw_bubble(ctx, TUB_X + 4 + phase % 2, floor - 15);
      draw_bubble(ctx, TUB_X + 24 - phase % 2, floor - 15 - phase % 2);
      draw_bubble(ctx, TUB_X + 8, floor - 19 - (phase % 3));
      break;
    case PLACE_BED:
      // Zwei Z ueber dem Kopfkissen
      draw_z(ctx, BED_X + 16 + phase % 2, floor - 24 - phase % 3);
      draw_z(ctx, BED_X + 23 - phase % 2, floor - 32 - phase % 3);
      break;
    case PLACE_BENCH:
      if (s_state == STATE_ACTING && (phase % 2) == 0) {
        graphics_context_set_fill_color(ctx, COLOR_SPARK);
        graphics_fill_rect(ctx, GRect(BENCH_X - 1, floor - 22, 2, 2), 0, GCornerNone);
        graphics_fill_rect(ctx, GRect(BENCH_X + 1, floor - 25, 2, 2), 0, GCornerNone);
      }
      break;
    default:
      break;
  }
}

static void draw_clock(GContext *ctx) {
  static char time_buf[8];
  static char date_buf[16];
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  strftime(time_buf, sizeof(time_buf), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);

  graphics_context_set_text_color(ctx, COLOR_TEXT);
  graphics_draw_text(ctx, time_buf, s_time_font, TIME_RECT,
                     GTextOverflowModeFill, TIME_ALIGN, NULL);
#ifdef PBL_PLATFORM_EMERY
  static char day_buf[8];
  strftime(day_buf, sizeof(day_buf), "%a", t);
  strftime(date_buf, sizeof(date_buf), "%d %b", t);
  graphics_draw_text(ctx, day_buf, s_info_font, DAY_RECT,
                     GTextOverflowModeFill, INFO_ALIGN, NULL);
#else
  strftime(date_buf, sizeof(date_buf), "%a %d", t);
#endif
  graphics_draw_text(ctx, date_buf, s_info_font, DATE_RECT,
                     GTextOverflowModeFill, INFO_ALIGN, NULL);
}

static void canvas_update(Layer *layer, GContext *ctx) {
  GRect b = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, COLOR_EARTH);
  graphics_fill_rect(ctx, b, 0, GCornerNone);

  draw_house(ctx, GRect(0, 0, b.size.w, HOUSE_H));
  draw_sky(ctx);
  draw_activity(ctx);
  draw_tenant(ctx);
  draw_clock(ctx);
}

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  s_phase = phase_for_time(tick_time);
  Place target = place_for_time(tick_time);
  if (s_state != STATE_WALKING && s_state != STATE_ANNOUNCING && target != s_place) {
    leave_for(target);
  }
  layer_mark_dirty(s_canvas);
}

// Ein Schwung des Handgelenks spielt die Taetigkeit noch einmal ab.
static void tap_handler(AccelAxisType axis, int32_t direction) {
  if (s_state == STATE_IDLE) {
    act_start();
    layer_mark_dirty(s_canvas);
  }
}

// Ohne Schwung-Option bleibt der Beschleunigungssensor ganz abbestellt.
static bool s_flick_subscribed;

static void flick_apply(void) {
  if (s_settings.flick && !s_flick_subscribed) {
    accel_tap_service_subscribe(tap_handler);
  } else if (!s_settings.flick && s_flick_subscribed) {
    accel_tap_service_unsubscribe();
  }
  s_flick_subscribed = s_settings.flick;
}

// Clay liefert eine Auswahl als Zeichenkette, einen Schalter als Zahl.
static int32_t tuple_int(const Tuple *t) {
  return (t->type == TUPLE_CSTRING) ? atoi(t->value->cstring) : t->value->int32;
}

static void inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *t = dict_find(iter, MESSAGE_KEY_INTERVAL);
  if (t) {
    s_settings.interval_min = tuple_int(t);
  }
  t = dict_find(iter, MESSAGE_KEY_VIBE_LEVEL);
  if (t) {
    s_settings.vibrate = tuple_int(t);
  }
  t = dict_find(iter, MESSAGE_KEY_FLICK);
  if (t) {
    s_settings.flick = tuple_int(t) != 0;
  }
  settings_save();
  settings_load();
  flick_apply();
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  s_canvas = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_canvas, canvas_update);
  layer_add_child(root, s_canvas);
}

static void window_unload(Window *window) {
  timer_cancel();
  layer_destroy(s_canvas);
}

static void init(void) {
  settings_load();
  s_time_font = fonts_get_system_font(TIME_FONT_KEY);
  s_info_font = fonts_get_system_font(INFO_FONT_KEY);
  s_house = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOUSE);
  s_sprites = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TENANT);
  palettes_build();

  // Beim Start ist die Figur ohne Gang am richtigen Ort.
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_phase = phase_for_time(t);
  s_place = place_for_time(t);
  s_pos = GPoint(PLACES[s_place].x, FLOOR_Y[PLACES[s_place].floor]);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  flick_apply();
  act_start();

  app_message_register_inbox_received(inbox_received);
  AppMessageResult r = app_message_open(INBOX_SIZE, OUTBOX_SIZE);
  if (r != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "app_message_open: %d", (int)r);
  }
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  if (s_flick_subscribed) {
    accel_tap_service_unsubscribe();
  }
  window_destroy(s_window);
  gbitmap_destroy(s_sprites);
  gbitmap_destroy(s_house);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
