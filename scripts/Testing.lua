

--@name Testing
--@developer iAndy_HD3

gd.forAllObjects(function (obj)
    local prop = obj:getProperties(gd.p.object_common.x)
    print(prop[gd.p.object_common.x])
end)