
#include <emscripten.h>

class Integrand : public Function {
public:
  void __destroy__()  {
    EM_ASM_INT({
      var self = Module['getCache'](Module['Integrand'])[$0];
      if (!self.hasOwnProperty('__destroy__')) throw 'a JSImplementation must implement all functions, you forgot Integrand::__destroy__.';
      self['__destroy__']();
    }, (int)this);
  }
};

extern "C" {

// Not using size_t for array indices as the values used by the javascript code are signed.

EM_JS(void, array_bounds_check_error, (size_t idx, size_t size), {
  throw 'Array index ' + idx + ' out of bounds: [0,' + size + ')';
});

void array_bounds_check(const int array_size, const int array_idx) {
  if (array_idx < 0 || array_idx >= array_size) {
    array_bounds_check_error(array_idx, array_size);
  }
}

// VoidPtr

void EMSCRIPTEN_KEEPALIVE emscripten_bind_VoidPtr___destroy___0(void** self) {
  delete self;
}

// GnuMPFloat

void EMSCRIPTEN_KEEPALIVE emscripten_bind_GnuMPFloat___destroy___0(GnuMPFloat* self) {
  delete self;
}

// Integrand

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Integrand___destroy___0(TanhSinh::Integrand* self) {
  delete self;
}

// TanhSinh

TanhSinh* EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_TanhSinh_1(int prec) {
  return new TanhSinh(prec);
}

TanhSinh* EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_TanhSinh_2(int prec, int points) {
  return new TanhSinh(prec, points);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_compile_0(TanhSinh* self) {
  self->compile();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_integrate_0(TanhSinh* self) {
  self->integrate();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_setIntegrand_1(TanhSinh* self, const TanhSinh::Integrand* integrand) {
  self->setIntegrand(*integrand);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_setBounds_2(TanhSinh* self, const GnuMPFloat* a, const GnuMPFloat* b) {
  self->setBounds(*a, *b);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_setUpperBound_1(TanhSinh* self, const GnuMPFloat* u) {
  self->setUpperBound(*u);
}

GnuMPFloat* EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_getUpperBound_0(TanhSinh* self) {
  return &self->getUpperBound();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_setLowerBound_1(TanhSinh* self, const GnuMPFloat* l) {
  self->setLowerBound(*l);
}

GnuMPFloat* EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_getLowerBound_0(TanhSinh* self) {
  return &self->getLowerBound();
}

bool EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_isConvergentError_0(TanhSinh* self) {
  return self->isConvergentError();
}

const GnuMPFloat* EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_getAbsoluteErrorApprox_0(TanhSinh* self) {
  return &self->getAbsoluteErrorApprox();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_getRelativeErrorExponent_0(TanhSinh* self) {
  return self->getRelativeErrorExponent();
}

const GnuMPFloat* EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_getIntegralApprox_0(TanhSinh* self) {
  return &self->getIntegralApprox();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh_getIntegrandCallCount_0(TanhSinh* self) {
  return self->getIntegrandCallCount();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_TanhSinh___destroy___0(TanhSinh* self) {
  delete self;
}

}

