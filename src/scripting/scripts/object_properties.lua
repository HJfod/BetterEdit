

local j = require "fixjson"

local tablestrformat = "%s = {"
local tablecloseformat = "}"
local keyformat = "\t%s = %i,"

for k, v in pairs(j) do
    print(("---@enum %s"):format(k))
    print(tablestrformat:format(k))
    for kk, vv in pairs(v) do
        io.write(keyformat:format(kk, vv.id))
        if vv.note or vv.type then io.write(" --") end
        if vv.type then io.write(vv.type) end
        if vv.note and vv.type then io.write(", ") end
        if vv.note then io.write(vv.note) end
        io.write("\n")
    end
    print(tablecloseformat)
end