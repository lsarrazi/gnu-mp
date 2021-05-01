async function main() {

	const {RAII} = await require('../dist/npm/NodeAPI')();

	RAII(Float => { // Using RAII avoid you to delete floats and free caches
		const f = Float(256).set(2).zeta() // compute ζ(2) in 256 bits, rounded to nearest

		console.log(f.toString())
	})
};

main();