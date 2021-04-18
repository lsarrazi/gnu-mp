
async function main() {

	const {RAII, Rounding, Float} = await require('../dist/NodeAPI')();

	const save = new Float(128)
	let fake_save;

	console.log(Rounding)

	RAII(Float => {
		const myconst = Float(128).set(3).zeta().mul(2);

		console.log(myconst.toString());

		fake_save = myconst;
		save.swap(myconst)
	})

	console.log(fake_save.$$.ptr)

	console.log(save.toString())

};

main();