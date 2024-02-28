
local json = require "json"

local f = io.open("obj_props.json", "r")
if not f then
    print ("could not find file")
    return
end

local filestr = f:read("*a")
local j = json.decode(filestr)

function fix(str)
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

local newjson = {}
for trigger, t in pairs(j) do
    local stuff = {}
    for k, v in pairs(t) do
        stuff[fix(k)] = v.id
    end
    newjson[fix(trigger)] = stuff
end

print(json.encode(newjson))