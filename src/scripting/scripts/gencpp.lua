
local json = require "json"

local f = io.open("obj_props.json", "r")
if not f then
    print ("could not find file")
    return
end

local filestr = f:read("*a")
local j = json.decode(filestr)

function fixkey(str)
    str = string.gsub(str, " ", "_")
    str = string.gsub(str, "'", "")
    str = string.gsub(str, "?", "")
    str = string.gsub(str, "/", "_")
    str = string.gsub(str, "%+%-", "rand")
    str = string.gsub(str, "%.%.%.", "")
    str = string.lower(str)

    if str == "end" then return "end_" end --end is a keyword smh
    return str
end

local cppstr = [[
#pragma once
#include <string_view>
#include <vector>

struct Property
{
    std::string_view name;
    int key = 0;
};

struct PropertyGroup
{
    std::string_view name;
    std::vector<Property> props;
    static std::array<PropertyGroup, %i> getProperties();
};
]]

print(string.format(cppstr, #j))
print("std::array<PropertyGroup, " .. #j .. "> PropertyGroup::getProperties()")
print("{")
print("\treturn std::array {")

local itrigger = 1
for trigger, t in pairs(j) do
    print('\tPropertyGroup {')
    print('\t\t.name = "' .. fixkey(trigger) .. '",')
    print('\t\t.props = {')
    local iprop = 1
    for prop, prop_values in pairs(t) do
        print("\t\t\tProperty {")
        print('\t\t\t\t.name ="' .. fixkey(prop) .. '",')
        print('\t\t\t\t.key = ' .. prop_values.id)
        if iprop == #t then
            print("\t\t\t}")
        else
            print("\t\t\t},")
        end
        iprop = iprop + 1
    end
    print("\t\t}")
    if itrigger == #j then
        print("\t}")
    else
        print("\t},")
    end
    itrigger = itrigger + 1
end

print("};")

print("}")