


async function main() {

	const {float_register_test} = await require('../dist/gnu-mp')();


	console.log('Before')
	float_register_test();
	console.log('After')


};

main();