
function print_i(str)
    print("    " .. str)
end

function print_ii(str)
    print("        " .. str)
end


function printEnumCpp(luaname, cppenum, key_values)
    local start = [[
    enum class %s {};
    lua.new_enum<%s>("%s", {]]

    print(start:format(cppenum, cppenum, luaname))
    local enumstr = '{ "%s", static_cast<%s>(%i) },'
    local enumstrlast = '{ "%s", static_cast<%s>(%i) }'

    local i = 1
    for kname, value_table in pairs(key_values) do
        if i == #key_values then
            print_ii(enumstrlast:format(kname, cppenum, value_table.id))
        else
            print_ii(enumstr:format(kname, cppenum, value_table.id))
        end
        i = i + 1
    end
    print_i("});")
end

local j = require "fixjson"

print("void gd::setObjectPropertyEnums(sol::state& lua) {")
for enum_name, values in pairs(j) do
    printEnumCpp(enum_name, enum_name, values)
end

print("}")