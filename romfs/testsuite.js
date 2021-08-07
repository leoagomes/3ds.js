var fs = require('3ds/fs/sync');

function ExpectationError(expected, operation, actual) {
  this.message = 'Expected ' + expected.toString() + ' to ' + operation + '' + actual.toString();
}

function Expectation(thing) {
  this.thing = thing;
}

Expectation.prototype.toEqual = function(value) {
  if (!(this.thing === value))
    throw new ExpectationError(this.thing, '===', value);
}

Expectation.prototype.toBePresent = function(value) {
  if (value === undefined || value === null)
    throw new ExpectationError(this.thing, 'be present, got', value);
}

function expect(thing) {
  return new Expectation(thing);
}

var tests = [
  function() {
    var file = fs.fopen('romfs:/tests/fixtures/testfile.txt', 'rb');
    expect(file).toBePresent();
    console.log(!!file);
    console.log(file.toString());
  },
];

console.log('running tests...')

for (var i = 0; i < tests.length; i++)
  tests[i]();
