/*!
 * \file fieldgenerators.hxx
 *
 * These classes are used by FieldFactory
 */

#ifndef __FIELDGENERATORS_H__
#define __FIELDGENERATORS_H__

#include <boutexception.hxx>
#include <field_factory.hxx>
#include <unused.hxx>

#include <cmath>

//////////////////////////////////////////////////////////
// Generators from values

/// Creates a Field Generator using a pointer to value
/// WARNING: The value pointed to must remain in scope until this generator is finished
class FieldValuePtr : public FieldGenerator {
public:
  FieldValuePtr(BoutReal* val) : ptr(val) {}
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> UNUSED(args)) override {
    return std::make_shared<FieldValuePtr>(ptr);
  }
  BoutReal generate(const Context&) override {
    return *ptr;
  }

private:
  BoutReal* ptr;
};

//////////////////////////////////////////////////////////
// Functions

/// Sine function field generator
class FieldSin : public FieldGenerator {
public:
  FieldSin(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;
  std::string str() const override {
    return std::string("sin(") + gen->str() + std::string(")");
  }

private:
  FieldGeneratorPtr gen;
};

/// Cosine function field generator
class FieldCos : public FieldGenerator {
public:
  FieldCos(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

  std::string str() const override {
    return std::string("cos(") + gen->str() + std::string(")");
  }

private:
  FieldGeneratorPtr gen;
};

/// Template class to define generators around a C function
using single_arg_op = BoutReal (*)(BoutReal);
template<single_arg_op Op>
class FieldGenOneArg : public FieldGenerator { ///< Template for single-argument function
public:
  FieldGenOneArg(FieldGeneratorPtr g) : gen(g) {}
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override {
    if (args.size() != 1) {
      throw ParseException(
          "Incorrect number of arguments to function. Expecting 1, got %lu",
          static_cast<unsigned long>(args.size()));
    }
    return std::make_shared<FieldGenOneArg<Op>>(args.front());
  }
  BoutReal generate(const Context& pos) override {
    return Op(gen->generate(pos));
  }
  std::string str() const override {
    return std::string("func(") + gen->str() + std::string(")");
  }

private:
  FieldGeneratorPtr gen;
};

/// Template for a FieldGenerator with two input arguments
using double_arg_op = BoutReal (*)(BoutReal, BoutReal);
template <double_arg_op Op>
class FieldGenTwoArg : public FieldGenerator { ///< Template for two-argument function
public:
  FieldGenTwoArg(FieldGeneratorPtr a, FieldGeneratorPtr b) : A(a), B(b) {}
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override {
    if (args.size() != 2) {
      throw ParseException(
          "Incorrect number of arguments to function. Expecting 2, got %lu",
          static_cast<unsigned long>(args.size()));
    }
    return std::make_shared<FieldGenTwoArg<Op>>(args.front(), args.back());
  }
  BoutReal generate(const Context& pos) override {
    return Op(A->generate(pos), B->generate(pos));
  }
  std::string str() const override {
    return std::string("cos(") + A->str() + "," + B->str() + std::string(")");
  }

private:
  FieldGeneratorPtr A, B;
};

/// Arc (Inverse) tangent. Either one or two argument versions
class FieldATan : public FieldGenerator {
public:
  FieldATan(FieldGeneratorPtr a, FieldGeneratorPtr b = nullptr) : A(a), B(b) {}
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override {
    if (args.size() == 1) {
      return std::make_shared<FieldATan>(args.front());
    } else if (args.size() == 2) {
      return std::make_shared<FieldATan>(args.front(), args.back());
    }
    throw ParseException(
        "Incorrect number of arguments to atan function. Expecting 1 or 2, got %lu",
        static_cast<unsigned long>(args.size()));
  }
  BoutReal generate(const Context& pos) override {
    if (B == nullptr)
      return atan(A->generate(pos));
    return atan2(A->generate(pos), B->generate(pos));
  }

private:
  FieldGeneratorPtr A, B;
};

/// Hyperbolic sine function
class FieldSinh : public FieldGenerator {
public:
  FieldSinh(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  FieldGeneratorPtr gen;
};

/// Hyperbolic cosine
class FieldCosh : public FieldGenerator {
public:
  FieldCosh(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  FieldGeneratorPtr gen;
};

/// Hyperbolic tangent
class FieldTanh : public FieldGenerator {
public:
  FieldTanh(FieldGeneratorPtr g = nullptr) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  FieldGeneratorPtr gen;
};

/// Gaussian distribution, taking mean and width arguments
class FieldGaussian : public FieldGenerator {
public:
  FieldGaussian(FieldGeneratorPtr xin, FieldGeneratorPtr sin) : X(xin), s(sin) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  FieldGeneratorPtr X, s;
};

/// Absolute value
class FieldAbs : public FieldGenerator {
public:
  FieldAbs(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  FieldGeneratorPtr gen;
};

/// Square root function
class FieldSqrt : public FieldGenerator {
public:
  FieldSqrt(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  FieldGeneratorPtr gen;
};

/// Heaviside function, switches between 0 and 1
class FieldHeaviside : public FieldGenerator {
public:
  FieldHeaviside(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;
  std::string str() const override {
    return std::string("H(") + gen->str() + std::string(")");
  }

private:
  FieldGeneratorPtr gen;
};

/// Generator for the error function erf
class FieldErf : public FieldGenerator {
public:
  FieldErf(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  FieldGeneratorPtr gen;
};

/// Minimum
class FieldMin : public FieldGenerator {
public:
  FieldMin() = default;
  FieldMin(const std::list<FieldGeneratorPtr> args) : input(args) {}
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override {
    if (args.size() == 0) {
      throw ParseException("min function must have some inputs");
    }
    return std::make_shared<FieldMin>(args);
  }
  BoutReal generate(const Context& pos) override {
    auto it = input.begin();
    BoutReal result = (*it)->generate(pos);
    for (; it != input.end(); it++) {
      BoutReal val = (*it)->generate(pos);
      if (val < result)
        result = val;
    }
    return result;
  }

private:
  std::list<FieldGeneratorPtr> input;
};

/// Maximum
class FieldMax : public FieldGenerator {
public:
  FieldMax() = default;
  FieldMax(const std::list<FieldGeneratorPtr> args) : input(args) {}
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override {
    if (args.size() == 0) {
      throw ParseException("max function must have some inputs");
    }
    return std::make_shared<FieldMax>(args);
  }
  BoutReal generate(const Context& pos) override {
    auto it = input.begin();
    BoutReal result = (*it)->generate(pos);
    for (; it != input.end(); it++) {
      BoutReal val = (*it)->generate(pos);
      if (val > result)
        result = val;
    }
    return result;
  }

private:
  std::list<FieldGeneratorPtr> input;
};

/// Generator to round to the nearest integer
class FieldRound : public FieldGenerator {
public:
  FieldRound(FieldGeneratorPtr g) : gen(g) {}

  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override {
    if (args.size() != 1) {
      throw ParseException("round function must have one input");
    }
    return std::make_shared<FieldRound>(args.front());
  }
  BoutReal generate(const Context& pos) override {
    BoutReal val = gen->generate(pos);
    if (val > 0.0) {
      return static_cast<int>(val + 0.5);
    }
    return static_cast<int>(val - 0.5);
  }

private:
  FieldGeneratorPtr gen;
};

//////////////////////////////////////////////////////////
// Ballooning transform
// Use a truncated Ballooning transform to enforce periodicity
// in doubly periodic domains

#include <bout/mesh.hxx>

class FieldBallooning : public FieldGenerator {
public:
  FieldBallooning(Mesh* m, FieldGeneratorPtr a = nullptr, int n = 3)
      : mesh(m), arg(a), ball_n(n) {}
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  Mesh* mesh;
  FieldGeneratorPtr arg;
  int ball_n; // How many times around in each direction
};

//////////////////////////////////////////////////////////
// Mix of mode numbers
// This is similar to BOUT initialisation option 3

class FieldMixmode : public FieldGenerator {
public:
  FieldMixmode(FieldGeneratorPtr a = nullptr, BoutReal seed = 0.5);
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  /// Generate a random number between 0 and 1 (exclusive)
  /// given an arbitrary seed value
  ///
  /// This PRNG has no memory, i.e. you need to call it
  /// with a different seed each time.
  BoutReal genRand(BoutReal seed);

  FieldGeneratorPtr arg;
  BoutReal phase[14];
};

//////////////////////////////////////////////////////////
// TanhHat
class FieldTanhHat : public FieldGenerator {
public:
  // Constructor
  FieldTanhHat(FieldGeneratorPtr xin, FieldGeneratorPtr widthin,
               FieldGeneratorPtr centerin, FieldGeneratorPtr steepnessin)
      : X(xin), width(widthin), center(centerin), steepness(steepnessin) {};
  // Clone containing the list of arguments
  FieldGeneratorPtr clone(const std::list<FieldGeneratorPtr> args) override;
  BoutReal generate(const Context& pos) override;

private:
  // The (x,y,z,t) field
  FieldGeneratorPtr X;
  // Other input arguments to the FieldTanhHat
  FieldGeneratorPtr width, center, steepness;
};

#endif // __FIELDGENERATORS_H__
