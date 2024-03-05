
local json = require "json"

local f = io.open("obj_props.json", "r")
if not f then
    print ("could not find file")
    return
end

local filestr = f:read("*a")
local j = json.decode(filestr)

function fixkey(str)
    str = string.lower(str)
    str = string.gsub(str, "[%s'%?/]", "_")
    str = string.gsub(str, "%+%-", "rand")
    str = string.gsub(str, "%.+", "")
    
    if str == "end" then return "end_" end
    return str
end

local newjson = {}
for trigger, t in pairs(j) do
    local stuff = {}
    for k, v in pairs(t) do
        if v.type then 
            v.type = string.gsub(v.type, "∈ ", "")
        end
        stuff[fixkey(k)] = v
    end
    newjson[fixkey(trigger)] = stuff
end

return newjson