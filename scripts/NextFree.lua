
--@name Next Free
--@developer iAndy_HD3

nextFree = 1
gd.forAllObjects(function(obj)
    obj:forAllGroups(function(group)
        if nextFree == group then
            nextFree = nextFree + 1
        end
    end)
end)

print("Next free group is", nextFree)