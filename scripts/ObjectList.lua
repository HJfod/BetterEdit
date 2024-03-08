--@name Object List
--@developer iAndy_HD3

function deleteObjectsWithGroup(removeGroup)
    local deleteArray = {}
    gd.forAllObjects(function (obj)
        obj:forAllGroups(function (group)
            if group == removeGroup then
                table.insert(deleteArray, obj)
            end
        end)
    end)
    gd.deleteObjects(deleteArray)
end

REMOVE_GROUP = 9999
deleteObjectsWithGroup(REMOVE_GROUP)

--color channel 1 needs to be set to red (FF0000) manually. api for that soon!

for y = 0,10 do
    for x = 0,10 do
        gd.create({
            [object_common.id] = 211,
            [object_common.x] = 75 + x * 30,
            [object_common.y] = 135 + y * 30,
            [object_common.groups] = REMOVE_GROUP,
            [object_common.color_1] = 1,
            [object_common.color_1_hsv] = string.format("%02da1a1a0a0", x * y % 360),
            [object_common.color_1_hsv_enabled] = true,
        })
    end
end