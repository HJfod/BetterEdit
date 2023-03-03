#include "Parser.hpp"

using namespace script;

Result<VersionAttr> VersionAttr::pull(InputStream& stream) {
    GEODE_UNWRAP(Token::pull(Ident("version"), stream));
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
    GEODE_UNWRAP(Token::pull(Ident("input"), stream));
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    GEODE_UNWRAP_INTO(auto value, Token::pull<Lit>(stream));
    GEODE_UNWRAP_INTO(auto title, Token::pull<Lit>(stream));
    if (!std::holds_alternative<StrLit>(title)) {
        return Err("Input name must be a string");
    }
    return Ok(InputAttr {
        .ident = ident,
        .value = value,
        .title = tokenToString(title),
    });
}

Result<> InputAttr::eval(Attrs& attrs) {
    for (auto& [na, _, __] : attrs.parameters) {
        if (na == ident) {
            return Err("Input parameter '{}' already defined", ident);
        }
    }
    attrs.parameters.emplace_back(ident, value, title);
    return Ok();
}

Result<StrictAttr> StrictAttr::pull(InputStream& stream) {
    GEODE_UNWRAP(Token::pull(Ident("strict"), stream));
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

Result<TitleAttr> TitleAttr::pull(InputStream& stream) {
    GEODE_UNWRAP(Token::pull(Ident("name"), stream));
    GEODE_UNWRAP_INTO(auto val, Token::pull<Lit>(stream));
    return Ok(TitleAttr {
        .title = tokenToString(val, false),
    });
}

Result<> TitleAttr::eval(Attrs& attrs) {
    attrs.title = title;
    return Ok();
}

#define PULL_ATTR(kw, ty) \
    if (Token::peek(Ident(kw), stream)) {\
        GEODE_UNWRAP_INTO(auto attr, ty::pull(stream)); \
        rb.commit();\
        return Ok(Attr { .value = attr });\
    }

Result<Attr> Attr::pull(InputStream& stream) {
    Rollback rb(stream);
    if (!Token::pull('@', stream)) {
        return Err("Attributes must begin with '@'");
    }
    PULL_ATTR("version", VersionAttr);
    PULL_ATTR("input",   InputAttr);
    PULL_ATTR("strict",  StrictAttr);
    PULL_ATTR("name",    TitleAttr);
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
    bool first = true;
    while (Token::peek('@', stream)) {
        tickExecutionCounter();
        GEODE_UNWRAP_INTO(auto attr, Attr::pull(stream));
        if (first && !std::holds_alternative<VersionAttr>(attr.value)) {
            return Err("First attribute must be a version attribute");
        }
        GEODE_UNWRAP(attr.eval(attrs));
        first = false;
    }
    return Ok(attrs);
}
