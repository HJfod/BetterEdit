local ordered_table = {}
--[[
This implementation of ordered table does not hold performance above functionality.
It invokes a metamethod `__newindex` for every access, and
while this is not ideal performance wise, the resulting table behaves very closely to
a standard Lua table, with the quirk that the keys are ordered by when they are first seen
(unless deleted and then reinserted.)
--]]

-- private unique keys
local _values = {}
local _keys = {}

function ordered_table.insert(t, k, v)
  if v == nil then
    ordered_table.remove(t, k)
  else -- update/store value
    if t[_values][k] == nil then -- new key 
      t[_keys][#t[_keys] + 1] = k
    end
    t[_values][k] = v 
  end
end

local function find(t, v)
  for i,val in ipairs(t) do
    if v == val then
      return i
    end
  end
end

function ordered_table.remove(t, k)
  local tv = t[_values]
  local v = tv[k]
  if v ~= nil then
    local tk = t[_keys]
    table.remove(tk, assert(find(tk, k)))
    tv[k] = nil
  end 
  return v
end

function ordered_table.pairs(t)
  local i = 0
  return function()
    i = i + 1
    local key = t[_keys][i]
    if key ~= nil then
      return key, t[_values][key]
    end
  end
end

-- set metamethods for ordered_table "class"
ordered_table.__newindex=ordered_table.insert -- called for updates too since we store the value in `_values` instead. 
ordered_table.__len=function(t) return #t[_keys] end
ordered_table.__pairs=ordered_table.pairs
ordered_table.__index=function(t, k) return t[_values][k] end -- function so we can share between tables 

function ordered_table:new(init)
  init = init or {}
  local key_table = {}
  local value_table = {}
  local t = {[_keys]=key_table, [_values]=value_table}
  local n = #init
  if n % 2 ~= 0 then
    error("key: "..tostring(init[#init]).." is missing value", 2)
  end
  for i=1,n/2 do
    local k = init[i * 2 - 1]
    local v = init[i * 2]
    if value_table[k] ~= nil then
      error("duplicated key:"..tostring(k), 2)
    end
    key_table[#key_table + 1]  = k
    value_table[k] = v
  end
  return setmetatable(t, self)
end

return setmetatable(ordered_table, {__call=ordered_table.new})

--[[ Example Usage:

ordered_table = require"ordered_table"

local t = ordered_table{
  "hello", 1,  -- key, value pairs
  2, 2,
  50, 3,
  "bye", 4,
  200, 5,
  "_values", 20,
  "_keys", 4,
}

print(#t, "items")
print("hello is", t.hello)
print()
for k, v in pairs(t) do print(k, v) end
print()

t.bye = nil -- delete it
t.hello = 0 -- updates
t.bye = 4 -- bring it back!
for k, v in pairs(t) do print(k, v) end
print(#t, "items") 

--]]