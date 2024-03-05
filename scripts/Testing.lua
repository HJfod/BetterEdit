

--@name Testing
--@developer iAndy_HD3

gd.forAllObjects(function (obj)
    local prop = table.unpack(obj:getProperties(object_common.id)) --obj:getProperties(object_common.x))
    print(prop)
end)