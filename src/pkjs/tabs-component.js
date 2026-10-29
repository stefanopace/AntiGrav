module.exports = {
  name: 'tabs',
  template: '<div class="component component-tabs"><div class="tabs-bar"></div></div>',
  style: [
    '.component.component-tabs {',
    '  position: sticky;',
    '  top: 0;',
    '  z-index: 20;',
    '  background: #333333;',
    '  margin-left: -0.75rem;',
    '  margin-right: -0.75rem;',
    '  padding: 0.55rem 0.55rem 0.65rem;',
    '}',
    '.component-tabs .tabs-bar {',
    '  display: flex;',
    '  flex-wrap: wrap;',
    '}',
    '.component-tabs .tabs-bar button {',
    '  flex: 1 1 30%;',
    '  min-width: 0;',
    '  box-sizing: border-box;',
    '  margin: 0.2rem;',
    '  border: 0;',
    '  border-radius: 0.25rem;',
    '  background: #414141;',
    '  color: #ffffff;',
    '  font: inherit;',
    '  font-size: 0.78rem;',
    '  letter-spacing: 0.03em;',
    '  text-transform: uppercase;',
    '  padding: 0.55rem 0.35rem;',
    '}',
    '.component-tabs .tabs-bar button.active {',
    '  background: #ff4700;',
    '}'
  ].join('\n'),
  manipulator: {
    set: function(value) {
      var next = value == null || value === '' ? (this.config.defaultValue || '') : String(value);
      var root = this.$element[0];
      if (root.getAttribute('data-value') === next) {
        return this;
      }
      root.setAttribute('data-value', next);
      var buttons = root.querySelectorAll('button');
      for (var i = 0; i < buttons.length; i++) {
        if (buttons[i].getAttribute('data-value') === next) {
          buttons[i].classList.add('active');
        } else {
          buttons[i].classList.remove('active');
        }
      }
      return this.trigger('change');
    },
    get: function() {
      return this.$element[0].getAttribute('data-value') || this.config.defaultValue || '';
    }
  },
  defaults: {
    options: [],
    defaultValue: ''
  },
  initialize: function() {
    var root = this.$element[0].querySelector('.tabs-bar');
    var options = this.config.options || [];
    var self = this;
    for (var i = 0; i < options.length; i++) {
      var option = options[i];
      var button = document.createElement('button');
      button.type = 'button';
      button.textContent = option.label;
      button.setAttribute('data-value', option.value);
      button.addEventListener('click', function(event) {
        self.set(event.currentTarget.getAttribute('data-value'));
      });
      root.appendChild(button);
    }
  }
};
