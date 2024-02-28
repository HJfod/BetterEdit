
local json = require "json"

local f = io.open("obj_props.json", "r")
if not f then
    print ("could not find file")
    return
end

local str = f:read("*a")
local j = json.decode(str)

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

print("gd.p = {")
for trigger, t in pairs(j) do
    trigger = fixkey(trigger)
    print("\t" .. trigger .. " = {")
    for k, v in pairs(t) do
        k = fixkey(k)
        local comment = "--"
        if v.type then
            v.type = string.gsub(v.type, "∈ ", "")
            comment = comment .. v.type
        end
        if v.note then
            if v.type then
                comment = comment .. "<br>" .. v.note
            else
                comment = v.note
            end
        end
        print("\t\t" .. k .. " = " .. v.id .. "," .. comment)
    end
    print("\t},")
end

print("}")