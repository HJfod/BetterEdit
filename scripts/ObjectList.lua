

--@name Object List
--@developer iAndy_HD3



local object_id = 45

for i = 0, 2, 1 do
    for j = 0, 9, 1 do
        local x = 15 + (j * 60)
        local y = 315 - (i * 90)
        gd.create({
            [object_common.id] = object_id,
            [object_common.x] = x,
            [object_common.y] = y
        })
        object_id = object_id + 1
    end
end