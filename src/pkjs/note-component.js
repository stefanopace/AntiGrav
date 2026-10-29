module.exports = {
  name: "note",
  template: '<div class="component component-note"><p data-manipulator-target></p></div>',
  style: [
    ".component.component-note p {",
    "  margin: 0;",
    "  color: #a4a4a4;",
    "  font-size: 0.9em;",
    "  text-align: left;",
    "}"
  ].join("\n"),
  manipulator: "html"
};
