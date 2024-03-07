

--@name Create Loop
--@developer iAndy_HD3

gd.print("running")

gd.forSelectedObjects(function (obj)
    if obj.type == GameObjectType.Modifier then
        local p = obj:getProperties()
        gd.print(p[trigger_common.multi_triggered], p[trigger_common.spawn_triggered], p[trigger_common.touch_triggered])
    end
end)