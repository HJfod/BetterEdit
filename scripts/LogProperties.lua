

--@name Log Object Properties
--@developer iAndy_HD3

gd.forAllObjects(function(obj)
    local props = obj:getProperties()
    for k, v in pairs(props) do
        gd.print(k .. "=" .. v)
    end
    gd.print("--")
end)