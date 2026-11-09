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
        "type": "toggle",
        "messageKey": "VIBRATE",
        "defaultValue": false,
        "label": "Vibrate before walking",
        "description": "A short buzz a few seconds before your housemate sets off, so you can watch. Stays silent during Quiet Time and at bedtime."
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
];
