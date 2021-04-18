

(async function () {


	console.time('MPFR Load');
	const module = await require('../dist/mpfr')();
	global.mpfr = module;
	console.timeEnd('MPFR Load');

	const { Float } = module;
	global.Float = Float;

	const float = new Float(100).set("2");

	const another = new Float(100);

	another.set(float)
	console.log('another:',another.toString())
	console.log(
		'another == float:', another.equal(float),
		'another != float:', another.not_equal(float),
		'another < float:', another.less(float),
		'another > float:', another.greater(float),
		'another <= float:', another.less_equal(float),
		'another >= float:', another.greater_equal(float),
		'another < 1:', another.less(1),
		'another < 100:', another.less(100)
	)
	
	another.delete();

	float.getMantissaView()[1] = 7858787585;

	float.root_ui(3);

	console.log('float to string:', float.toString(), 'to number:', float.toNumber() )

	console.log('float.precision:', float.precision, 'float.exponent', float.exponent, 'float.sign:', float.sign)
	
	console.log('float.rounding:', float.rounding)

	console.log('mantissa size:', float.getMantissaSize())
	console.log('mantissa view', float.getMantissaView())
	

	console.log('Performance test')

	const num = new Float(10000);

	num.set("2.758778278198297298387777728379827772828756155655796333333333415333363335353")

	console.time('Performance');
	num.sqrt();
	console.timeEnd('Performance');

	let c =new Float(100).set(3)
	let d = new Float(100).set(4)

	c.swap(d);

	console.log(d.toString())


	while (true);

})();