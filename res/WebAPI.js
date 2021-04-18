


window.loadGnuMP = async function () {
	const Module = await (await import('./gnu-mp.js')).default();
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
			Module.free_cache()
		}
	}
};