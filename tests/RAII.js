
async function main() {

	const {RAII, Rounding, Float, Module} = await require('../dist/npm/NodeAPI')();

	global.Module = Module;

	const save = new Float(1024)
	let fake_save;

	save.set(1)

	console.log(Rounding)

	const integrator = new Module.TanhSinh(1024);

	integrator.setIntegrand(() => {console.log('INtegrand !')});

	//integrator.integrate();




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