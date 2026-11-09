// Clay-Konfigurationsseite. Nutzersichtbare Texte englisch.
// messageKey muss Zeichen fuer Zeichen zu package.json passen.
module.exports = [
  {
    "type": "heading",
    "defaultValue": "House on a Pebble"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Your housemate"
      },
      {
        "type": "select",
        "messageKey": "INTERVAL",
        "defaultValue": "15",
        "label": "Moves to another room",
        "description": "More frequent walks use a little more battery.",
        "options": [
          { "label": "Every minute", "value": "1" },
          { "label": "Every 5 minutes", "value": "5" },
          { "label": "Every 15 minutes", "value": "15" },
          { "label": "Every hour", "value": "60" }
        ]
      },
      {
        "type": "select",
        "messageKey": "VIBE_LEVEL",
        "defaultValue": "0",
        "label": "Vibrate before walking",
        "description": "A buzz three seconds before your housemate sets off, so you can watch. Stays silent during Quiet Time and at bedtime.",
        "options": [
          { "label": "Off", "value": "0" },
          { "label": "Short", "value": "1" },
          { "label": "Medium", "value": "2" },
          { "label": "Long", "value": "3" }
        ]
      },
      {
        "type": "toggle",
        "messageKey": "FLICK",
        "defaultValue": true,
        "label": "Replay on wrist flick",
        "description": "Flick your wrist to see the current activity again."
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
];
