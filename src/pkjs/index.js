// Import the Clay package
var Clay = require('@rebble/clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig, require('./custom-clay'));
clay.registerComponent(require('./tabs-component'));
clay.registerComponent(require('./note-component'));

var phoneBattery = null;
var phoneBatteryListening = false;

function sendPhoneBattery() {
  if (!phoneBattery || typeof phoneBattery.level !== 'number') {
    return;
  }

  var percent = Math.round(phoneBattery.level * 100);
  if (percent < 0) {
    percent = 0;
  } else if (percent > 100) {
    percent = 100;
  }

  Pebble.sendAppMessage({
    PhoneBattery: percent,
    PhoneBatteryCharging: phoneBattery.charging ? 1 : 0
  });
}

function bindPhoneBattery(battery) {
  if (!phoneBatteryListening) {
    return;
  }
  if (phoneBattery === battery) {
    sendPhoneBattery();
    return;
  }

  phoneBattery = battery;
  sendPhoneBattery();
  battery.addEventListener('levelchange', sendPhoneBattery);
  battery.addEventListener('chargingchange', sendPhoneBattery);
}

function bindLegacyPhoneBattery() {
  var battery = navigator.battery || navigator.mozBattery || navigator.webkitBattery;
  if (!battery) {
    return;
  }
  bindPhoneBattery(battery);
}

function startPhoneBattery() {
  if (phoneBatteryListening) {
    sendPhoneBattery();
    return;
  }

  phoneBatteryListening = true;
  if (navigator.getBattery) {
    navigator.getBattery().then(bindPhoneBattery).catch(bindLegacyPhoneBattery);
    return;
  }
  bindLegacyPhoneBattery();
}

function stopPhoneBattery() {
  phoneBatteryListening = false;
  if (!phoneBattery) {
    return;
  }

  phoneBattery.removeEventListener('levelchange', sendPhoneBattery);
  phoneBattery.removeEventListener('chargingchange', sendPhoneBattery);
  phoneBattery = null;
}

Pebble.addEventListener('ready', function () {
  console.log('PebbleKit JS ready!');
  Pebble.sendAppMessage({ PhoneBatteryRequest: 1 });
});

Pebble.addEventListener('appmessage', function (event) {
  if (!event.payload || event.payload.PhoneBatteryRequest === undefined) {
    return;
  }

  if (event.payload.PhoneBatteryRequest) {
    startPhoneBattery();
  } else {
    stopPhoneBattery();
  }
});
