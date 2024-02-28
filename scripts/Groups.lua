

--@name Groups
--@developer iAndy_HD3


objects = gd.getAllObjects()
for _, obj in ipairs(objects) do

     --accepts any arguments that are integers or tables that have integer values 
    obj:addGroups(1)
    obj:addGroups(2, {3, 4, 5}, 6)
    obj:addGroups({7, 8, 9, 10})

    local groups = obj:getGroups() --{1,2,3,4,5,6,7,8,9,10}
    for i = #groups, 6, -1 do
        table.remove(groups, i) --remove last 5 groups
    end
    obj:removeAllGroups()
    obj:addGroups(groups) --1,2,3,4,5
end

print("---")
