const flick_count_options = [
  { "label": "Off", "value": "0" },
  { "label": "Single flick", "value": "1" },
  { "label": "Double flick", "value": "2" },
  { "label": "Triple flick", "value": "3" },
  { "label": "Quadruple flick", "value": "4" }
];

function groupedSection(group, items) {
  for (var i = 0; i < items.length; i++) {
    items[i].group = group;
  }
  return {
    type: "section",
    items: items
  };
}

module.exports = [
  {
    "type": "heading",
    "defaultValue": "AntiGrav Framework",
    "size": "1"
  },

  {
    "type": "submit",
    "defaultValue": "Save Settings"
  },

  {
    "type": "tabs",
    "id": "settings-tabs",
    "defaultValue": "time-date",
    "options": [
      { "label": "Date & Time", "value": "time-date" },
      { "label": "Behaviors", "value": "behaviours" }
    ]
  },

  groupedSection("time-date", [
    {
      "type": "heading",
      "defaultValue": "Date",
      "size": "3"
    },
    {
      "type": "heading",
      "defaultValue": "Leading zeros",
      "size": "4"
    },
    {
      "type": "toggle",
      "messageKey": "MonthLeadingZero",
      "label": "Month",
      "defaultValue": true
    },
    {
      "type": "toggle",
      "messageKey": "DayLeadingZero",
      "label": "Day",
      "defaultValue": true
    }
  ]),
  groupedSection("time-date", [
    {
      "type": "heading",
      "defaultValue": "Time",
      "size": "3"
    },
    {
      "type": "heading",
      "defaultValue": "Leading zeros",
      "size": "4"
    },
    {
      "type": "toggle",
      "messageKey": "HoursLeadingZero",
      "label": "Hours",
      "defaultValue": true
    },
    {
      "type": "toggle",
      "messageKey": "MinutesLeadingZero",
      "label": "Minutes",
      "defaultValue": true
    },
    {
      "type": "toggle",
      "messageKey": "SecondsLeadingZero",
      "label": "Seconds",
      "defaultValue": true
    }
  ]),

  groupedSection("behaviours", [
    {
      "type": "heading",
      "defaultValue": "Seconds",
      "size": "3"
    },
    {
      "type": "select",
      "messageKey": "ShowSecondsOnTapCount",
      "label": "Reveal on flick",
      "description": "ⓘ Hides the seconds until you flick the watch the selected number of times. Off keeps them visible.",
      "defaultValue": "1",
      "options": flick_count_options
    },
    {
      "type": "select",
      "messageKey": "ShowOnTapDuration",
      "label": "Reveal duration",
      "defaultValue": "10",
      "options": [
        { "label": "5 seconds", "value": "5" },
        { "label": "10 seconds", "value": "10" },
        { "label": "30 seconds", "value": "30" },
        { "label": "1 minute", "value": "60" },
        { "label": "2 minutes", "value": "120" }
      ]
    },
    {
      "type": "toggle",
      "messageKey": "HideSecondsOnLowBattery",
      "label": "Disable seconds on low battery",
      "description": "ⓘ Also prevents flick activation",
      "defaultValue": false
    },
    {
      "type": "toggle",
      "messageKey": "HideSecondsOnQuietTime",
      "label": "Disable seconds during Quiet Time",
      "description": "ⓘ Also prevents flick activation",
      "defaultValue": false
    }
  ]),
  groupedSection("behaviours", [
    {
      "type": "heading",
      "defaultValue": "Battery",
      "size": "3"
    },
    {
      "type": "heading",
      "defaultValue": "Watch",
      "size": "4"
    },
    {
      "type": "slider",
      "messageKey": "LowBatteryThreshold",
      "defaultValue": 20,
      "label": "Low battery threshold (%)",
      "min": 0,
      "max": 100,
      "step": 5
    },
    {
      "type": "heading",
      "defaultValue": "Phone",
      "size": "4"
    },
    {
      "type": "slider",
      "messageKey": "PhoneLowBatteryThreshold",
      "defaultValue": 30,
      "label": "Low battery threshold (%)",
      "min": 0,
      "max": 100,
      "step": 5
    }
  ]),
  groupedSection("behaviours", [
    {
      "type": "heading",
      "defaultValue": "Steps",
      "size": "3"
    },
    {
      "type": "slider",
      "messageKey": "StepGoal",
      "defaultValue": 10000,
      "label": "Daily goal",
      "min": 0,
      "max": 30000,
      "step": 1000
    }
  ]),

  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
