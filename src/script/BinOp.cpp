#include "Parser.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/ranges.hpp>

using namespace script;

Result<Rc<Expr>> BinOpExpr::pull(InputStream& stream, size_t p, Rc<Expr> lhs) {
    Rollback rb(stream);
    while (true) {
        if (!Token::peek<Op>(stream)) break;

        auto pop = Token::prio(stream);
        if (pop < p) {
            rb.commit();
            return Ok(lhs);
        }
        // consume op
        GEODE_UNWRAP_INTO(auto op, Token::pull<Op>(stream));
        GEODE_UNWRAP_INTO(auto rhs, Expr::pullPrimary(stream));

        if (Token::dir(stream) == OpDir::RTL) {
            GEODE_UNWRAP_INTO(rhs, BinOpExpr::pull(stream, p, rhs));
        }
        if (pop < Token::prio(stream)) {
            GEODE_UNWRAP_INTO(rhs, BinOpExpr::pull(stream, pop + 1, rhs));
        }

        lhs = make<Expr>({
            .value = make<BinOpExpr>({
                .lhs = lhs,
                .rhs = rhs,
                .op = op,
                .src = rb.commit(),
            }).unwrap()
        }).unwrap();
    }
    rb.commit();
    return Ok(lhs);
}

Result<Rc<Expr>> BinOpExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto lhs, Expr::pullPrimary(stream));
    if (Token::peek<Op>(stream)) {
        rb.commit();
        return BinOpExpr::pull(stream, 0, lhs);
    }
    rb.commit();
    return Ok(lhs);
}

Result<Rc<Value>> BinOpExpr::eval(State& state) {
    GEODE_UNWRAP_INTO(auto lhsValue, lhs->eval(state));
    GEODE_UNWRAP_INTO(auto rhsValue, rhs->eval(state));
    switch (op) {
        case Op::Seq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't assign type {} to type {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            lhsValue->value = rhsValue->value;
            return Ok(lhsValue);
        } break;

        case Op::AddSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a += b;
                    return Ok(Value::rc(a));
                },
                [&](StrLit& a, StrLit const& b) -> Result<Rc<Value>> {
                    a += b;
                    return Ok(Value::rc(a));
                },
                [&](Array& a, auto const&) -> Result<Rc<Value>> {
                    if (auto arr = rhsValue->has<Array>()) {
                        ranges::push(a, *arr);
                    }
                    else {
                        a.push_back(*rhsValue);
                    }
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot add {} and {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::SubSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a -= b;
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot subtract {} and {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::MulSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a *= b;
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot multiply {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::DivSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a /= b;
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot divide {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::ModSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a = remainder(a, b);
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot divide {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Add: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a + b));
                },
                [&](StrLit const& a, StrLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a + b));
                },
                [&](Array const& a, auto const&) -> Result<Rc<Value>> {
                    Array copy = a;
                    if (auto arr = rhsValue->has<Array>()) {
                        ranges::push(copy, *arr);
                    }
                    else {
                        copy.push_back(*rhsValue);
                    }
                    return Ok(Value::rc(copy));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot add {} and {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Sub: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a - b));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot subtract {} and {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Mul: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a * b));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot multiply {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Div: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a / b));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot divide {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Mod: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(remainder(a, b)));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot divide {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Eq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value == rhsValue->value));
        } break;

        case Op::Neq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value != rhsValue->value));
        } break;

        case Op::Less: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value < rhsValue->value));
        } break;

        case Op::Leq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value <= rhsValue->value));
        } break;

        case Op::More: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value > rhsValue->value));
        } break;

        case Op::Meq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value >= rhsValue->value));
        } break;

        case Op::And: {
            return Ok(Value::rc(lhsValue->truthy() && rhsValue->truthy()));
        } break;

        case Op::Or: {
            return Ok(Value::rc(lhsValue->truthy() || rhsValue->truthy()));
        } break;

        default: {
            throw std::runtime_error(fmt::format(
                "Internal error: Unimplemented binary operator {}",
                tokenToString(op, true)
            ));
        } break;
    }
}

std::string BinOpExpr::debug() const {
    return fmt::format(
        "BinOpExpr({}, {}, {})",
        lhs->debug(), tokenToString(op, true), rhs->debug()
    );
}
