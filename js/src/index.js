console.log('criando');

const delay = (seconds) => new Promise((resolve) => {
  setTimeout(resolve, seconds * 1000);
});

(async () => {
  console.log('hey')
  await delay(2);
  console.log('ho');
})();

console.log('depois de criar');
