console.log('start');

const onestep = () => new Promise((resolve) => {
  setImmediate(resolve);
});

const delay = (millis) => new Promise((resolve) => {
  setTimeout(resolve, millis);
});

(async () => {
  console.log('one');
  await onestep();
  console.log('two');
  await delay(2000);
  console.log('three');
})();

console.log('end');
