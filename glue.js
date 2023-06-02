
// Bindings utilities

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function WrapperObject() {
}
WrapperObject.prototype = Object.create(WrapperObject.prototype);
WrapperObject.prototype.constructor = WrapperObject;
WrapperObject.prototype.__class__ = WrapperObject;
WrapperObject.__cache__ = {};
Module['WrapperObject'] = WrapperObject;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant)
    @param {*=} __class__ */
function getCache(__class__) {
  return (__class__ || WrapperObject).__cache__;
}
Module['getCache'] = getCache;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant)
    @param {*=} __class__ */
function wrapPointer(ptr, __class__) {
  var cache = getCache(__class__);
  var ret = cache[ptr];
  if (ret) return ret;
  ret = Object.create((__class__ || WrapperObject).prototype);
  ret.ptr = ptr;
  return cache[ptr] = ret;
}
Module['wrapPointer'] = wrapPointer;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function castObject(obj, __class__) {
  return wrapPointer(obj.ptr, __class__);
}
Module['castObject'] = castObject;

Module['NULL'] = wrapPointer(0);

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function destroy(obj) {
  if (!obj['__destroy__']) throw 'Error: Cannot destroy object. (Did you create it yourself?)';
  obj['__destroy__']();
  // Remove from cache, so the object can be GC'd and refs added onto it released
  delete getCache(obj.__class__)[obj.ptr];
}
Module['destroy'] = destroy;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function compare(obj1, obj2) {
  return obj1.ptr === obj2.ptr;
}
Module['compare'] = compare;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function getPointer(obj) {
  return obj.ptr;
}
Module['getPointer'] = getPointer;

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function getClass(obj) {
  return obj.__class__;
}
Module['getClass'] = getClass;

// Converts big (string or array) values into a C-style storage, in temporary space

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
var ensureCache = {
  buffer: 0,  // the main buffer of temporary storage
  size: 0,   // the size of buffer
  pos: 0,    // the next free offset in buffer
  temps: [], // extra allocations
  needed: 0, // the total size we need next time

  prepare: function() {
    if (ensureCache.needed) {
      // clear the temps
      for (var i = 0; i < ensureCache.temps.length; i++) {
        Module['_free'](ensureCache.temps[i]);
      }
      ensureCache.temps.length = 0;
      // prepare to allocate a bigger buffer
      Module['_free'](ensureCache.buffer);
      ensureCache.buffer = 0;
      ensureCache.size += ensureCache.needed;
      // clean up
      ensureCache.needed = 0;
    }
    if (!ensureCache.buffer) { // happens first time, or when we need to grow
      ensureCache.size += 128; // heuristic, avoid many small grow events
      ensureCache.buffer = Module['_malloc'](ensureCache.size);
      assert(ensureCache.buffer);
    }
    ensureCache.pos = 0;
  },
  alloc: function(array, view) {
    assert(ensureCache.buffer);
    var bytes = view.BYTES_PER_ELEMENT;
    var len = array.length * bytes;
    len = (len + 7) & -8; // keep things aligned to 8 byte boundaries
    var ret;
    if (ensureCache.pos + len >= ensureCache.size) {
      // we failed to allocate in the buffer, ensureCache time around :(
      assert(len > 0); // null terminator, at least
      ensureCache.needed += len;
      ret = Module['_malloc'](len);
      ensureCache.temps.push(ret);
    } else {
      // we can allocate in the buffer
      ret = ensureCache.buffer + ensureCache.pos;
      ensureCache.pos += len;
    }
    return ret;
  },
  copy: function(array, view, offset) {
    offset >>>= 0;
    var bytes = view.BYTES_PER_ELEMENT;
    switch (bytes) {
      case 2: offset >>>= 1; break;
      case 4: offset >>>= 2; break;
      case 8: offset >>>= 3; break;
    }
    for (var i = 0; i < array.length; i++) {
      view[offset + i] = array[i];
    }
  },
};

/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureString(value) {
  if (typeof value === 'string') {
    var intArray = intArrayFromString(value);
    var offset = ensureCache.alloc(intArray, HEAP8);
    ensureCache.copy(intArray, HEAP8, offset);
    return offset;
  }
  return value;
}
/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureInt8(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP8);
    ensureCache.copy(value, HEAP8, offset);
    return offset;
  }
  return value;
}
/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureInt16(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP16);
    ensureCache.copy(value, HEAP16, offset);
    return offset;
  }
  return value;
}
/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureInt32(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAP32);
    ensureCache.copy(value, HEAP32, offset);
    return offset;
  }
  return value;
}
/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureFloat32(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAPF32);
    ensureCache.copy(value, HEAPF32, offset);
    return offset;
  }
  return value;
}
/** @suppress {duplicate} (TODO: avoid emitting this multiple times, it is redundant) */
function ensureFloat64(value) {
  if (typeof value === 'object') {
    var offset = ensureCache.alloc(value, HEAPF64);
    ensureCache.copy(value, HEAPF64, offset);
    return offset;
  }
  return value;
}


// VoidPtr
/** @suppress {undefinedVars, duplicate} @this{Object} */function VoidPtr() { throw "cannot construct a VoidPtr, no constructor in IDL" }
VoidPtr.prototype = Object.create(WrapperObject.prototype);
VoidPtr.prototype.constructor = VoidPtr;
VoidPtr.prototype.__class__ = VoidPtr;
VoidPtr.__cache__ = {};
Module['VoidPtr'] = VoidPtr;

  VoidPtr.prototype['__destroy__'] = VoidPtr.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  _emscripten_bind_VoidPtr___destroy___0(self);
};
// GnuMPFloat
/** @suppress {undefinedVars, duplicate} @this{Object} */function GnuMPFloat() { throw "cannot construct a GnuMPFloat, no constructor in IDL" }
GnuMPFloat.prototype = Object.create(WrapperObject.prototype);
GnuMPFloat.prototype.constructor = GnuMPFloat;
GnuMPFloat.prototype.__class__ = GnuMPFloat;
GnuMPFloat.__cache__ = {};
Module['GnuMPFloat'] = GnuMPFloat;

  GnuMPFloat.prototype['__destroy__'] = GnuMPFloat.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  _emscripten_bind_GnuMPFloat___destroy___0(self);
};
// Integrand
/** @suppress {undefinedVars, duplicate} @this{Object} */function Integrand() { throw "cannot construct a Integrand, no constructor in IDL" }
Integrand.prototype = Object.create(Function.prototype);
Integrand.prototype.constructor = Integrand;
Integrand.prototype.__class__ = Integrand;
Integrand.__cache__ = {};
Module['Integrand'] = Integrand;

  Integrand.prototype['__destroy__'] = Integrand.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  _emscripten_bind_Integrand___destroy___0(self);
};
// TanhSinh
/** @suppress {undefinedVars, duplicate} @this{Object} */function TanhSinh(prec, points) {
  if (prec && typeof prec === 'object') prec = prec.ptr;
  if (points && typeof points === 'object') points = points.ptr;
  if (points === undefined) { this.ptr = _emscripten_bind_TanhSinh_TanhSinh_1(prec); getCache(TanhSinh)[this.ptr] = this;return }
  this.ptr = _emscripten_bind_TanhSinh_TanhSinh_2(prec, points);
  getCache(TanhSinh)[this.ptr] = this;
};;
TanhSinh.prototype = Object.create(WrapperObject.prototype);
TanhSinh.prototype.constructor = TanhSinh;
TanhSinh.prototype.__class__ = TanhSinh;
TanhSinh.__cache__ = {};
Module['TanhSinh'] = TanhSinh;

TanhSinh.prototype['compile'] = TanhSinh.prototype.compile = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  _emscripten_bind_TanhSinh_compile_0(self);
};;

TanhSinh.prototype['integrate'] = TanhSinh.prototype.integrate = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  _emscripten_bind_TanhSinh_integrate_0(self);
};;

TanhSinh.prototype['setIntegrand'] = TanhSinh.prototype.setIntegrand = /** @suppress {undefinedVars, duplicate} @this{Object} */function(integrand) {
  var self = this.ptr;
  if (integrand && typeof integrand === 'object') integrand = integrand.ptr;
  _emscripten_bind_TanhSinh_setIntegrand_1(self, integrand);
};;

TanhSinh.prototype['setBounds'] = TanhSinh.prototype.setBounds = /** @suppress {undefinedVars, duplicate} @this{Object} */function(a, b) {
  var self = this.ptr;
  if (a && typeof a === 'object') a = a.ptr;
  if (b && typeof b === 'object') b = b.ptr;
  _emscripten_bind_TanhSinh_setBounds_2(self, a, b);
};;

TanhSinh.prototype['setUpperBound'] = TanhSinh.prototype.setUpperBound = /** @suppress {undefinedVars, duplicate} @this{Object} */function(u) {
  var self = this.ptr;
  if (u && typeof u === 'object') u = u.ptr;
  _emscripten_bind_TanhSinh_setUpperBound_1(self, u);
};;

TanhSinh.prototype['getUpperBound'] = TanhSinh.prototype.getUpperBound = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  return wrapPointer(_emscripten_bind_TanhSinh_getUpperBound_0(self), GnuMPFloat);
};;

TanhSinh.prototype['setLowerBound'] = TanhSinh.prototype.setLowerBound = /** @suppress {undefinedVars, duplicate} @this{Object} */function(l) {
  var self = this.ptr;
  if (l && typeof l === 'object') l = l.ptr;
  _emscripten_bind_TanhSinh_setLowerBound_1(self, l);
};;

TanhSinh.prototype['getLowerBound'] = TanhSinh.prototype.getLowerBound = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  return wrapPointer(_emscripten_bind_TanhSinh_getLowerBound_0(self), GnuMPFloat);
};;

TanhSinh.prototype['isConvergentError'] = TanhSinh.prototype.isConvergentError = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  return !!(_emscripten_bind_TanhSinh_isConvergentError_0(self));
};;

TanhSinh.prototype['getAbsoluteErrorApprox'] = TanhSinh.prototype.getAbsoluteErrorApprox = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  return wrapPointer(_emscripten_bind_TanhSinh_getAbsoluteErrorApprox_0(self), GnuMPFloat);
};;

TanhSinh.prototype['getRelativeErrorExponent'] = TanhSinh.prototype.getRelativeErrorExponent = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  return _emscripten_bind_TanhSinh_getRelativeErrorExponent_0(self);
};;

TanhSinh.prototype['getIntegralApprox'] = TanhSinh.prototype.getIntegralApprox = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  return wrapPointer(_emscripten_bind_TanhSinh_getIntegralApprox_0(self), GnuMPFloat);
};;

TanhSinh.prototype['getIntegrandCallCount'] = TanhSinh.prototype.getIntegrandCallCount = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  return _emscripten_bind_TanhSinh_getIntegrandCallCount_0(self);
};;

  TanhSinh.prototype['__destroy__'] = TanhSinh.prototype.__destroy__ = /** @suppress {undefinedVars, duplicate} @this{Object} */function() {
  var self = this.ptr;
  _emscripten_bind_TanhSinh___destroy___0(self);
};