


module.exports = async function () {
	const Module = await require('./gnu-mp.js')();
	return {
		Module,
		Float: Module.Float,
		Rounding: {
			Nearest: Module.Nearest,
			TowardZero: Module.TowardZero,
			TowardInfinity: Module.TowardInfinity,
			TowardMinusInfinity: Module.TowardMinusInfinity,
			AwayZero: Module.AwayZero,
			Faithful: Module.Faithful
		},
		RAII(callback) {
			const registers = [];
			function makeRegister(...args) {
				const register = new Module.Float(...args);
				registers.push(register);
				return register;
			}
			callback(makeRegister);
			for (const register of registers)
				register.delete();
			Module.Float.free_cache()
		}
	}
};