const fs = require("fs");


// PATCH MPFR.JS TO ALLOW BUILDER PATTERN
const file = process.argv[2]

const src = `if(returns){invokerFnBody+="var ret = retType.fromWireType(rv);\\n"+"return ret;\\n"}`;

const patch = src + `else if(classType&&['Float'].includes(classType.name)){invokerFnBody+="return this;\\n";}`;

fs.writeFileSync(file, fs.readFileSync(file).toString().replace(src, patch))