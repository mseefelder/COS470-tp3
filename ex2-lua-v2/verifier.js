var fs = require('fs');

var arguments = {};
var nThreads  = 0;
var filePath = '';
var objectVirify = {};


process.argv.forEach((val, index) => {
  arguments[index] = val;
});

nThreads = arguments['2'];
filePath = arguments['3'];
if(!filePath) {
  return console.log('enter with the path of the file');
}

fs.readFile(filePath, 'utf8', (err, data) => {
  if(err) {
    return console.log(err);
  }
  var lines = data.split('\n');
  lines.forEach((line, index) => {
    if(line.trim().length == 0){
      return;
    }
    var elements = line.split(' ');
    if(elements.length != 4) {
      console.log('line ' + index + ' has a problem of write');
      return;
    }
    var name = line.split(' is writing ')[0];
    var number = line.split(' is writing ')[1];
    if( name in objectVirify){
      objectVirify[name].array.push(number);
    } else {
      objectVirify[name] = {};
      objectVirify[name].array = [];
      objectVirify[name].array.push(number);
    }
  });

  for( var name in objectVirify) {
    var obj = objectVirify[name];
    var value = obj.array.reduce((previous, current, index, array) => {
      return parseInt(previous) + parseInt(current);
    });
    console.log(value, name);
    if(value != 5050){
      console.log(name + 'didn`t  write all number');
    }
  }
});


