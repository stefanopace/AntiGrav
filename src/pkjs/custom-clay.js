module.exports = function() {
  var clayConfig = this;

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
    syncSections();
  }

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var tabs = clayConfig.getItemById("settings-tabs");
    if (!tabs) {
      return;
    }
    showTab(tabs.get());
    tabs.on("change", function() {
      showTab(tabs.get());
      window.scrollTo(0, 0);
    });
  });
};
