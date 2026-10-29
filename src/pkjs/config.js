const flick_count_options = [
  { "label": "Off", "value": "0" },
  { "label": "Single flick", "value": "1" },
  { "label": "Double flick", "value": "2" },
  { "label": "Triple flick", "value": "3" },
  { "label": "Quadruple flick", "value": "4" }
];

const font_options = [
  { "label": "Replicant", "value": "Replicant" },
  { "label": "Racer", "value": "Racer" },
  { "label": "Samurai", "value": "Samurai" }
];

const slot_content_options = [
  { "label": "Watch battery bar", "value": "Battery" },
  { "label": "Phone battery bar", "value": "PhoneBattery" },
  { "label": "Step goal progress bar", "value": "Steps" },
  { "label": "Seconds", "value": "Seconds" },
  { "label": "Seconds bar", "value": "SecondsBar" },
  { "label": "Split A", "value": "ComboA" },
  { "label": "Split B", "value": "ComboB" },
  { "label": "Split C", "value": "ComboC" },
  { "label": "Empty", "value": "None" }
];

const combo_bar_options = [
  { "label": "Watch battery", "value": "WatchBattery" },
  { "label": "Phone battery", "value": "PhoneBattery" },
  { "label": "Step goal progress bar", "value": "StepGoal" },
  { "label": "Seconds bar", "value": "Seconds" }
];

const bar_style_options = [
  { "label": "Segmented", "value": "5Bars" },
  { "label": "Continuous", "value": "Continuous" }
];

const alignment_options = [
  { "label": "Automatic", "value": "Automatic" },
  { "label": "Left", "value": "Left" },
  { "label": "Center", "value": "Center" },
  { "label": "Right", "value": "Right" }
];

const placeholder_options = [
  { "label": "Thick", "value": "Thick" },
  { "label": "Thin", "value": "Thin" },
  { "label": "None", "value": "None" }
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

function layoutItems(section, items) {
  for (var i = 0; i < items.length; i++) {
    items[i].layoutSection = section;
  }
  return items;
}

function slotContent(name, contentKey, contentDefault) {
  return {
    "type": "select",
    "messageKey": contentKey,
    "label": name,
    "defaultValue": contentDefault,
    "options": slot_content_options
  };
}

function comboContents(section, name, topKey, bottomKey, topDefault, bottomDefault) {
  return layoutItems(section, [
    { "type": "heading", "defaultValue": name, "size": "4" },
    {
      "type": "select",
      "messageKey": topKey,
      "label": "Top",
      "defaultValue": topDefault,
      "options": combo_bar_options
    },
    {
      "type": "select",
      "messageKey": bottomKey,
      "label": "Bottom",
      "defaultValue": bottomDefault,
      "options": combo_bar_options
    }
  ]);
}

function textSource(section, whereId, name, alignKey, placeholderKey, note) {
  var items = [
    { "type": "heading", "id": whereId, "defaultValue": name, "size": "4" }
  ];
  if (note) {
    items.push({ "type": "note", "defaultValue": note });
  }
  items.push(
    {
      "type": "select",
      "messageKey": alignKey,
      "label": "Alignment",
      "defaultValue": "Automatic",
      "options": alignment_options
    },
    {
      "type": "select",
      "messageKey": placeholderKey,
      "label": "Placeholder",
      "defaultValue": "Thick",
      "options": placeholder_options
    }
  );
  return layoutItems(section, items);
}

function colorSource(section, name, colors) {
  var items = [
    { "type": "heading", "defaultValue": name, "size": "4" }
  ];
  for (var i = 0; i < colors.length; i++) {
    items.push(colors[i]);
  }
  for (var j = 0; j < items.length; j++) {
    items[j].colorSection = section;
  }
  return items;
}

function colorBarGroup(colors) {
  var items = [
    { "type": "heading", "defaultValue": "Custom Bar colors", "size": "3", "colorSection": "bar-colors" },
    {
      "type": "toggle",
      "messageKey": "BarColors",
      "label": "Enable",
      "description": "ⓘ The colors below will override the Bar color",
      "defaultValue": false,
      "colorSection": "bar-colors"
    }
  ];
  for (var i = 0; i < colors.length; i++) {
    items.push(colors[i]);
  }
  return items;
}

function barSource(section, whereId, name, styleKey, mirrorKey, note, styleDefault, mirrorDefault) {
  var items = [
    { "type": "heading", "id": whereId, "defaultValue": name, "size": "4" }
  ];
  if (note) {
    items.push({ "type": "note", "defaultValue": note });
  }
  items.push(
    {
      "type": "select",
      "messageKey": styleKey,
      "label": "Bar style",
      "defaultValue": styleDefault || "5Bars",
      "options": bar_style_options
    },
    {
      "type": "toggle",
      "messageKey": mirrorKey,
      "label": "Mirrored",
      "defaultValue": !!mirrorDefault
    }
  );
  return layoutItems(section, items);
}

module.exports = [
  {
    "type": "heading",
    "defaultValue": "AntiGrav mini",
    "size": "1"
  },

  {
    "type": "submit",
    "defaultValue": "Save Settings"
  },


  {
    "type": "tabs",
    "id": "settings-tabs",
    "defaultValue": "style",
    "options": [
      { "label": "Style", "value": "style" },
      { "label": "Date & Time", "value": "time-date" },
      { "label": "Bar Layout", "value": "slots" },
      { "label": "Status Icons", "value": "status-icons" },
      { "label": "Behaviors", "value": "behaviours" },
      { "label": "Colors", "value": "colors" }
    ]
  },


  groupedSection("style", [
    {
      "type": "select",
      "messageKey": "Font",
      "label": "Font",
      "defaultValue": "Replicant",
      "options": font_options
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorPrimary",
      "label": "Background Color",
      "defaultValue": "000000"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorSecondary",
      "label": "Text Color",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorAccent",
      "label": "Bar color",
      "defaultValue": "00FFAA"
    }
  ]),

  groupedSection("time-date", [
    {
      "type": "heading",
      "defaultValue": "Date",
      "size": "3"
    },
    {
      "type": "select",
      "messageKey": "Language",
      "label": "Language",
      "defaultValue": "EN",
      "options": [
        { "label": "English", "value": "EN" },
        { "label": "Español", "value": "ES" },
        { "label": "Français", "value": "FR" },
        { "label": "Português", "value": "PT" },
        { "label": "Deutsch", "value": "DE" },
        { "label": "Italiano", "value": "IT" },
        { "label": "Nederlands", "value": "NL" },
        { "label": "Svenska", "value": "SV" },
        { "label": "Dansk", "value": "DA" },
        { "label": "Norsk bokmål", "value": "NB" },
        { "label": "Bahasa Indonesia", "value": "ID" },
        { "label": "Tagalog", "value": "TL" }
      ]
    },
    {
      "type": "select",
      "messageKey": "DateFormat",
      "label": "Format",
      "defaultValue": "DayNameDay",
      "options": [
        { "label": "Fr 31", "value": "DayNameDay" },
        { "label": "31 Fr", "value": "DayDayName" },
        { "label": "31/12", "value": "DayMonth" },
        { "label": "12/31", "value": "MonthDay" }
      ]
    },
    {
      "type": "select",
      "messageKey": "DateSeparator",
      "label": "Numeric separator",
      "description": "ⓘ Used when the format includes numbers separated by / . or -",
      "defaultValue": "Slash",
      "options": [
        { "label": "Slash (/)", "value": "Slash" },
        { "label": "Dot (.)", "value": "Dot" },
        { "label": "Hyphen (-)", "value": "Hyphen" }
      ]
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
      "type": "toggle",
      "messageKey": "AmPmIconEnabled",
      "label": "Show PM marker in 12h mode",
      "defaultValue": false
    },
    {
      "type": "select",
      "messageKey": "PinPopupLarge",
      "label": "Keep large during a pin",
      "defaultValue": "Minutes",
      "options": [
        { "label": "Minutes", "value": "Minutes" },
        { "label": "Hours", "value": "Hours" }
      ]
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

  groupedSection("slots", [
    { "type": "heading", "defaultValue": "Slots", "size": "3" },
    {
      "type": "note",
      "defaultValue": "ⓘ Slots with the same content are merged"
    },
    slotContent("Left", "SlotLeft", "Battery"),
    slotContent("Center", "SlotCenter", "Battery"),
    slotContent("Right", "SlotRight", "SecondsBar")
  ].concat(comboContents("combo-a", "Split A", "ComboBarTop", "ComboBarBottom", "WatchBattery", "PhoneBattery"))
    .concat(comboContents("combo-b", "Split B", "ComboBTop", "ComboBBottom", "Seconds", "StepGoal"))
    .concat(comboContents("combo-c", "Split C", "ComboCTop", "ComboCBottom", "WatchBattery", "Seconds"))),
  groupedSection("slots", textSource("source-seconds", "source-seconds-where", "Seconds", "SecondsAlignment", "SecondsPlaceholder", "ⓘ See the Behaviors tab")),
  groupedSection("slots", barSource("source-watch", "source-watch-where", "Watch battery", "BatteryStyle", "WatchBatteryMirrored", null, "Continuous", true)),
  groupedSection("slots", barSource("source-phone", "source-phone-where", "Phone battery", "PhoneBatteryStyle", "PhoneBatteryMirrored", "⚠︎ May not work on some devices", "Continuous")),
  groupedSection("slots", barSource("source-steps", "source-steps-where", "Step goal progress bar", "StepsStyle", "StepGoalMirrored", "ⓘ Set the goal in Behaviors", "Continuous")),
  groupedSection("slots", barSource("source-seconds-bar", "source-seconds-bar-where", "Seconds bar", "SecondsBarStyle", "SecondsBarMirrored", "ⓘ See the Behaviors tab")),



  groupedSection("status-icons", [
    {
      "type": "toggle",
      "messageKey": "BtIconEnabled",
      "label": "Bluetooth disconnected",
      "defaultValue": true
    },
    {
      "type": "toggle",
      "messageKey": "QuietTimeIconEnabled",
      "label": "Quiet Time active",
      "defaultValue": true
    },
    {
      "type": "toggle",
      "messageKey": "GoalIconEnabled",
      "label": "Daily step goal reached",
      "defaultValue": true
    },
    {
      "type": "toggle",
      "messageKey": "LowBatteryIconEnabled",
      "label": "Low watch battery",
      "defaultValue": true
    },
    {
      "type": "toggle",
      "messageKey": "PhoneLowBatteryIconEnabled",
      "label": "Low phone battery",
      "defaultValue": false
    },
    {
      "type": "toggle",
      "messageKey": "PhoneChargingIconEnabled",
      "label": "Phone battery charging",
      "defaultValue": false
    },
    {
      "type": "note",
      "defaultValue": "⚠︎ Phone battery may not work on some devices"
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
      "defaultValue": "0",
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
  groupedSection("behaviours", [
    {
      "type": "heading",
      "defaultValue": "Bluetooth",
      "size": "3"
    },
    {
      "type": "select",
      "messageKey": "BtDisconnectVibe",
      "label": "Vibrate on disconnect",
      "defaultValue": "Off",
      "options": [
        { "label": "Off", "value": "Off" },
        { "label": "Short", "value": "Short" },
        { "label": "Long", "value": "Long" },
        { "label": "Double", "value": "Double" }
      ]
    },
    {
      "type": "toggle",
      "messageKey": "NoVibrateOnQuietTime",
      "label": "Silence vibration during Quiet Time",
      "defaultValue": false
    }
  ]),

  /**
   * ADVANCED COLORS
   */
  groupedSection("colors", [
    {
      "type": "heading",
      "defaultValue": "Custom colors",
      "size": "3"
    },
    {
      "type": "toggle",
      "messageKey": "AdvancedColors",
      "label": "Enable",
      "description": "ⓘ The colors below will override the colors defined in the Style tab",
      "defaultValue": false
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorBackground",
      "label": "Background",
      "defaultValue": "000000"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorHours",
      "label": "Hours",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorMinutes",
      "label": "Minutes",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorBar",
      "label": "Bar",
      "defaultValue": "00FFAA"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorDate",
      "label": "Date",
      "defaultValue": "FFFFFF"
    }
  ]),

  groupedSection("colors", colorBarGroup([
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorBarWatchBattery",
      "label": "Watch battery bar",
      "defaultValue": "00FFAA",
      "colorSection": "source-watch"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorBarPhoneBattery",
      "label": "Phone battery bar",
      "defaultValue": "00FFAA",
      "colorSection": "source-phone"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorBarSteps",
      "label": "Step goal progress bar",
      "defaultValue": "00FFAA",
      "colorSection": "source-steps"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorBarSeconds",
      "label": "Seconds bar",
      "defaultValue": "00FFAA",
      "colorSection": "source-seconds-bar"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorPlaceholderEmpty",
      "label": "Empty bar",
      "defaultValue": "00FFAA",
      "colorSection": "source-empty"
    }
  ]).concat(colorSource("source-icons", "Status icons", [
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorIcons",
      "label": "Icons",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorPlaceholderIcons",
      "label": "Bar",
      "defaultValue": "00FFAA"
    }
  ])).concat(colorSource("source-seconds", "Seconds", [
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorSeconds",
      "label": "Digits",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorPlaceholderSeconds",
      "label": "Bar",
      "defaultValue": "00FFAA"
    }
  ]))),

  groupedSection("colors", colorSource("source-icons", "Custom icons colors", [
    {
      "type": "toggle",
      "messageKey": "IconColors",
      "label": "Enable",
      "description": "ⓘ The colors below will override the Icons color",
      "defaultValue": false
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorIconBluetooth",
      "label": "Bluetooth disconnected",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorIconQuietTime",
      "label": "Quiet Time active",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorIconGoal",
      "label": "Daily step goal reached",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorIconLowBattery",
      "label": "Low watch battery",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorIconPhoneLowBattery",
      "label": "Low phone battery",
      "defaultValue": "FFFFFF"
    },
    {
      "type": "color",
      "sunlight": false,
      "messageKey": "ColorIconPhoneCharging",
      "label": "Phone battery charging",
      "defaultValue": "FFFFFF"
    }
  ])),

  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
