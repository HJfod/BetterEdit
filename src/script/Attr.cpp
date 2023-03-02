#include "Parser.hpp"

using namespace script;

Result<VersionAttr> VersionAttr::pull(InputStream& stream) {
    GEODE_UNWRAP(Token::pull(AttrKw::Version, stream));
    GEODE_UNWRAP_INTO(auto num, Token::pull<Lit>(stream));

    if (!std::holds_alternative<NumLit>(num)) {
        return Err("Target version is not a number");
    }
    double dmajor, dminor;
    dminor = std::modf(std::get<NumLit>(num), &dmajor);
    auto major = static_cast<size_t>(dmajor);
    auto minor = static_cast<size_t>(dminor * 100);
    // remove trailing zeros
    while (minor != 0 && minor % 10 == 0) {
        tickExecutionCounter();
        minor /= 10;
    }
    return Ok(VersionAttr {
        .version = VersionInfo(major, minor, 0)
    });
}

Result<> VersionAttr::eval(Attrs& attrs) {
    if (attrs.version) {
        return Err("Target version specified multiple times");
    }
    attrs.version = version;
    return Ok();
}

Result<InputAttr> InputAttr::pull(InputStream& stream) {
    GEODE_UNWRAP(Token::pull(AttrKw::Input, stream));
    GEODE_UNWRAP_INTO(auto ty, Token::pull<Ident>(stream));
    decltype(type) type;
    if (ty == "number") {
        type = Number;
    }
    else if (ty == "string") {
        type = String;
    }
    else {
        return Err("Unknown input type {}, known types are number and string", ty);
    }
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    return Ok(InputAttr {
        .type = type,
        .ident = ident,
    });
}

Result<> InputAttr::eval(Attrs& attrs) {
    for (auto& [_, na] : attrs.parameters) {
        if (na == ident) {
            return Err("Input parameter '{}' already defined", ident);
        }
    }
    attrs.parameters.emplace_back(type, ident);
    return Ok();
}

Result<StrictAttr> StrictAttr::pull(InputStream& stream) {
    GEODE_UNWRAP(Token::pull(AttrKw::Version, stream));
    GEODE_UNWRAP_INTO(auto val, Token::pull<Lit>(stream));
    if (!std::holds_alternative<BoolLit>(val)) {
        return Err("Strict attribute value is not boolean");
    }
    return Ok(StrictAttr {
        .enable = std::get<BoolLit>(val),
    });
}

Result<> StrictAttr::eval(Attrs& attrs) {
    attrs.strict = enable;
    return Ok();
}

#define PULL_ATTR(kw, ty) \
    if (Token::peek(AttrKw::kw, stream)) {\
        GEODE_UNWRAP_INTO(auto attr, ty::pull(stream)); \
        rb.commit();\
        return Ok(Attr { .value = attr });\
    }

Result<Attr> Attr::pull(InputStream& stream) {
    Rollback rb(stream);
    if (!Token::pull('@', stream)) {
        return Err("Attributes must begin with '@'");
    }
    PULL_ATTR(Version, VersionAttr);
    PULL_ATTR(Input,   InputAttr);
    PULL_ATTR(Strict,  StrictAttr);
    if (auto token = Token::pull(stream)) {
        return Err("Expected attribute, got '{}'", token.value().toString());
    }
    return Err("Expected attribute, got end-of-file");
}

Result<> Attr::eval(Attrs& attrs) {
    return std::visit(makeVisitor {
        [&](auto& attr) {
            return attr.eval(attrs);
        },
    }, value);
}

Result<Attrs> Attrs::pull(InputStream& stream) {
    auto attrs = Attrs();
    while (Token::peek('@', stream)) {
        tickExecutionCounter();
        GEODE_UNWRAP_INTO(auto attr, Attr::pull(stream));
        GEODE_UNWRAP(attr.eval(attrs));
    }
    return Ok(attrs);
}
