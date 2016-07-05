try {
   var autobahn = require('autobahn');
} catch (e) {
   // when running in browser, AutobahnJS will
   // be included without a module system
}

// Set up WAMP connection to router
var connection = new autobahn.Connection({

   url: 'ws://localhost:8080/ws',
   realm: 'realm1'}

);

// Set up 'onopen' handler
connection.onopen = function (session) {
	console.log('Connection opened');
	setInterval(function() {
	   session.call('com.timeservice.now').then(
	      // RPC success callback
	      function (now) {
	         console.log("Current time:", now);
	      },
	      // RPC error callback
	      function (error) {
	         console.log("Call failed:", error);
	      }
	   );
	}, 1000);
};

// Open connection
connection.open();
