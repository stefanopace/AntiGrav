module.exports = function() {
  var clayConfig = this;

  var SLOT_FIELDS = [
    { key: "SlotLeft", label: "Left" },
    { key: "SlotCenter", label: "Center" },
    { key: "SlotRight", label: "Right" }
  ];

  var COMBOS = [
    { section: "combo-a", value: "ComboA", label: "Split A", top: "ComboBarTop", bottom: "ComboBarBottom" },
    { section: "combo-b", value: "ComboB", label: "Split B", top: "ComboBTop", bottom: "ComboBBottom" },
    { section: "combo-c", value: "ComboC", label: "Split C", top: "ComboCTop", bottom: "ComboCBottom" }
  ];

  var SOURCES = [
    { section: "source-seconds", whereId: "source-seconds-where", slots: ["Seconds"], combos: [] },
    { section: "source-watch", whereId: "source-watch-where", slots: ["Battery"], combos: ["WatchBattery"] },
    { section: "source-phone", whereId: "source-phone-where", slots: ["PhoneBattery"], combos: ["PhoneBattery"] },
    { section: "source-steps", whereId: "source-steps-where", slots: ["Steps"], combos: ["StepGoal"] },
    { section: "source-seconds-bar", whereId: "source-seconds-bar-where", slots: ["SecondsBar"], combos: ["Seconds"] }
  ];

  function sectionHasVisibleItem(section) {
    var children = section.children;
    for (var i = 0; i < children.length; i++) {
      if (!children[i].classList.contains("hide")) {
        return true;
      }
    }
    return false;
  }

  function syncSections() {
    var sections = document.querySelectorAll(".section");
    for (var i = 0; i < sections.length; i++) {
      if (sectionHasVisibleItem(sections[i])) {
        sections[i].classList.remove("hide");
      } else {
        sections[i].classList.add("hide");
      }
    }
  }

  function itemValue(messageKey) {
    var item = clayConfig.getItemByMessageKey(messageKey);
    if (!item) {
      return "";
    }
    return item.get();
  }

  function contains(list, value) {
    for (var i = 0; i < list.length; i++) {
      if (list[i] === value) {
        return true;
      }
    }
    return false;
  }

  function slotUses(value) {
    for (var i = 0; i < SLOT_FIELDS.length; i++) {
      if (itemValue(SLOT_FIELDS[i].key) === value) {
        return true;
      }
    }
    return false;
  }

  function splitUsesSource(slotValue, source) {
    for (var i = 0; i < COMBOS.length; i++) {
      if (COMBOS[i].value !== slotValue) {
        continue;
      }
      return contains(source.combos, itemValue(COMBOS[i].top)) ||
        contains(source.combos, itemValue(COMBOS[i].bottom));
    }
    return false;
  }

  function placesFor(source) {
    var places = [];
    for (var i = 0; i < SLOT_FIELDS.length; i++) {
      var value = itemValue(SLOT_FIELDS[i].key);
      if (contains(source.slots, value) || splitUsesSource(value, source)) {
        places.push(SLOT_FIELDS[i].label);
      }
    }
    return places;
  }

  function sourceByWhereId(id) {
    for (var i = 0; i < SOURCES.length; i++) {
      if (SOURCES[i].whereId === id) {
        return SOURCES[i];
      }
    }
    return null;
  }

  function activeSections() {
    var active = {};
    var i;
    for (i = 0; i < COMBOS.length; i++) {
      if (slotUses(COMBOS[i].value)) {
        active[COMBOS[i].section] = true;
      }
    }
    for (i = 0; i < SOURCES.length; i++) {
      if (placesFor(SOURCES[i]).length > 0) {
        active[SOURCES[i].section] = true;
      }
    }
    return active;
  }

  function emptyIsUsed() {
    for (var i = 0; i < SLOT_FIELDS.length; i++) {
      if (itemValue(SLOT_FIELDS[i].key) === "None") {
        return true;
      }
    }
    return false;
  }

  function syncMarkedSections(marker, active) {
    var items = clayConfig.getAllItems();
    for (var i = 0; i < items.length; i++) {
      var section = items[i].config[marker];
      if (!section) {
        continue;
      }
      if (active[section]) {
        items[i].show();
      } else {
        items[i].hide();
      }
    }
  }

  function syncLayoutVisibility() {
    var active = activeSections();
    var items = clayConfig.getAllItems();
    for (var i = 0; i < items.length; i++) {
      var section = items[i].config.layoutSection;
      if (!section) {
        continue;
      }
      if (active[section]) {
        items[i].show();
      } else {
        items[i].hide();
      }
      var source = sourceByWhereId(items[i].id);
      if (source && active[section]) {
        items[i].set(items[i].config.defaultValue + " (" + placesFor(source).join(", ") + ")");
      }
    }
  }

  function syncColorVisibility() {
    var active = activeSections();
    active["source-icons"] = true;
    if (emptyIsUsed()) {
      active["source-empty"] = true;
    }
    if (active["source-watch"] || active["source-phone"] || active["source-steps"] || active["source-seconds-bar"] || active["source-empty"] || active["source-icons"] || active["source-seconds"]) {
      active["bar-colors"] = true;
    }
    syncMarkedSections("colorSection", active);
  }

  function syncContextualVisibility(name) {
    if (name === "slots") {
      syncLayoutVisibility();
    } else if (name === "colors") {
      syncColorVisibility();
    }
  }

  function showTab(name) {
    var items = clayConfig.getAllItems();
    for (var i = 0; i < items.length; i++) {
      var group = items[i].config.group;
      if (!group) {
        continue;
      }
      if (group === name) {
        items[i].show();
      } else {
        items[i].hide();
      }
    }
    syncContextualVisibility(name);
    syncSections();
  }

  function bindLayoutInputs() {
    var keys = [];
    var i;
    for (i = 0; i < SLOT_FIELDS.length; i++) {
      keys.push(SLOT_FIELDS[i].key);
    }
    for (i = 0; i < COMBOS.length; i++) {
      keys.push(COMBOS[i].top);
      keys.push(COMBOS[i].bottom);
    }
    for (i = 0; i < keys.length; i++) {
      var item = clayConfig.getItemByMessageKey(keys[i]);
      if (!item) {
        continue;
      }
      item.on("change", function() {
        var tabs = clayConfig.getItemById("settings-tabs");
        if (!tabs) {
          return;
        }
        syncContextualVisibility(tabs.get());
        syncSections();
      });
    }
  }

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var tabs = clayConfig.getItemById("settings-tabs");
    if (!tabs) {
      return;
    }
    bindLayoutInputs();
    showTab(tabs.get());
    tabs.on("change", function() {
      showTab(tabs.get());
      window.scrollTo(0, 0);
    });
  });
};
