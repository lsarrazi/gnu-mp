
This package is an ALPHA package, it could be subject to leaks and other bugs.

Note: super simple example of utilisation for curious ones
```js
async function main() {

	const {RAII} = await require('gnu-mp')();

	RAII(Float => { // Using RAII avoid you to delete floats and free caches
		const f = Float(256).set(2).zeta() // compute ζ(2) in 256 bits, rounded to nearest

		console.log(f.toString())
	})
};

main();
```