module.exports = [
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Account preferences"
      },
      {
        "type": "input",
        "messageKey": "username",
        "label": "username"
      },
      {
        "type": "slider",
        "messageKey": "icon",
        "defaultValue": 0,
        "label": "Slider",
        "description": "icon ID",
        "min": 0,
        "max": 15,
        "step": 1
      },
      {
        "type": "input",
        "messageKey": "message",
        "label": "timeline message"
      },
      {
        "type": "submit",
        "defaultValue": "Save"
      }
    ]
  }
]