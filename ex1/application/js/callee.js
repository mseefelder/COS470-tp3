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
   // Define the remote procedure
	function utcnow() {
	   console.log("Someone is calling me;)");
	   now = new Date();
	   return now.toISOString();
	}

	// Register the remote procedure with the router
	session.register('com.timeservice.now', utcnow).then(
	   function (registration) {
	      console.log("Procedure registered:", registration.id);
	   },
	   function (error) {
	      console.log("Registration failed:", error);
	   }
	);
};

// Open connection
connection.open();
