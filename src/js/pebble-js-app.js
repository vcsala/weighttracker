/******************************* timeline lib *********************************/

// The timeline public URL root
var API_URL_ROOT = 'https://timeline-api.getpebble.com/';

/**
 * Send a request to the Pebble public web timeline API.
 * @param pin The JSON pin to insert. Must contain 'id' field.
 * @param type The type of request, either PUT or DELETE.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function timelineRequest(pin, type, callback) {
  // User or shared?
  var url = API_URL_ROOT + 'v1/user/pins/' + pin.id;

  // Create XHR
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    console.log('timeline: response received: ' + this.responseText);
    callback(this.responseText);
  };
  xhr.open(type, url);

  // Get token
  Pebble.getTimelineToken(function(token) {
    // Add headers
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.setRequestHeader('X-User-Token', '' + token);
    console.log("Token: " + token);
    console.log("Pin: " + JSON.stringify(pin));

    // Send
    xhr.send(JSON.stringify(pin));
    console.log('timeline: request sent.');
  }, function(error) { console.log('timeline: error getting timeline token: ' + error); });
}

/**
 * Insert a pin into the timeline for this user.
 * @param pin The JSON pin to insert.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function insertUserPin(pin, callback) {
  timelineRequest(pin, 'PUT', callback);
}

/**
 * Delete a pin from the timeline for this user.
 * @param pin The JSON pin to delete.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function deleteUserPin(pin, callback) {
  timelineRequest(pin, 'DELETE', callback);
}

/***************************** end timeline lib *******************************/
Pebble.addEventListener('ready',
  function(e) {
    console.log('JS app ready and running!');
  }
);

Pebble.addEventListener("appmessage", function(e) {
  console.log("Got message: " + JSON.stringify(e));

  if ('PIN_ID' in e.payload) {
    var pcommand = e.payload.PIN_COMMAND;
    var pid = e.payload.PIN_ID;
    var ptype = e.payload.PIN_TYPE;
    var pin = {};

    pin.id = ((ptype === 0) ? "wt-measurement-" : "wt-reminder-") + pid;

    if (pcommand === 0) {
      deleteUserPin(pin, function(responseText) {
        console.log('Result: ' + responseText);
      });
    } else if (pcommand === 1) {
      var pdate = e.payload.PIN_DATE;
      var ptime = e.payload.PIN_TIME;

      var pyear = Math.floor(pdate / 10000);
      var rem = pdate % 10000;
      var pmonth = Math.floor(rem / 100);
      var pday = rem % 100;
      var phour = Math.floor(ptime / 100);
      var pmin = ptime % 100;

      var pin_date = new Date(pyear, pmonth - 1, pday, phour, pmin, 0, 0);

      pin.time = pin_date.toISOString();

      if (ptype === 1) {
        pin.layout = {
          "type": "genericPin",
          "title": "WeightTracker",
          "tinyIcon": "system://images/NOTIFICATION_REMINDER",
          "body": "You set up a reminder to measure your weight."
        };
      } else {
        pin.layout = {
          "type": "genericPin",
          "title": "Weight: " + e.payload.PIN_WEIGHT,
          "tinyIcon": "system://images/GLUCOSE_MONITOR",
          "body": "WeightTracker"
        };
      }

      insertUserPin(pin, function(responseText) {
        console.log('Result: ' + responseText);
      });
    }
  }
});
