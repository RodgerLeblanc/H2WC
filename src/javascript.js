var initialised = false;
var uri = "http://rodgerleblanc.github.io/Hub2Watch/H2WC/index.html";

function appMessageAck(e) {
    console.log("options sent to Pebble successfully");
}

function appMessageNack(e) {
    console.log("options not sent to Pebble: " + e.error.message);
}

Pebble.addEventListener("ready", function() {
    initialised = true;
});

Pebble.addEventListener("showConfiguration", function() {
    var options = JSON.parse(localStorage.getItem('options'));
    console.log("read options: " + JSON.stringify(options));
    console.log("showing configuration");
    if (options !== null) {
        uri = uri+ '?' + 'battery=' + encodeURIComponent(options.background);
    }
    Pebble.openURL(uri);
});

Pebble.addEventListener("webviewclosed", function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  var topColor = configData['top_color'];
  var bottomColor = configData['bottom_color'];
  var timeColor = configData['time_color'];

  var dict = {};
  dict['TOP_KEY_COLOR_RED'] = parseInt(topColor.substring(2, 4), 16);
  dict['TOP_KEY_COLOR_GREEN'] = parseInt(topColor.substring(4, 6), 16);
  dict['TOP_KEY_COLOR_BLUE'] = parseInt(topColor.substring(6), 16);
  dict['BOTTOM_KEY_COLOR_RED'] = parseInt(bottomColor.substring(2, 4), 16);
  dict['BOTTOM_KEY_COLOR_GREEN'] = parseInt(bottomColor.substring(4, 6), 16);
  dict['BOTTOM_KEY_COLOR_BLUE'] = parseInt(bottomColor.substring(6), 16);
  dict['TIME_KEY_COLOR_RED'] = parseInt(timeColor.substring(2, 4), 16);
  dict['TIME_KEY_COLOR_GREEN'] = parseInt(timeColor.substring(4, 6), 16);
  dict['TIME_KEY_COLOR_BLUE'] = parseInt(timeColor.substring(6), 16);

  // Send to watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('Send failed!');
  });
});