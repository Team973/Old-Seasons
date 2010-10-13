--[[
boot.lua
Created by Ross Light on 2010-09-28.

This is a sample boot.lua file.  Feel free to use it in your own programs, or
write your own, if you wish.

This file is public domain.
--]]

local restartError = "Robot Restart xyzzy"

function restartRobot()
    error(restartError, 2)
end

while true do
    local success, err
    
    success, err = pcall(require, "robot")
    
    if success then
        success, err = pcall(robot.run)
        if not success and err == restartError then
            -- Unload all user-level packages
            for name, _ in items(package.loaded) do
                package.loaded[name] = nil
                _G[name] = nil
            end
            -- Run garbage collector before restarting
            collectgarbage("collect")
        end
    end
    
    if not success and err ~= restartError then
        local f = io.open("boot-error.txt", "w")
        f:write(err)
        f:close()
    end
    
    wpilib.Wait(5.0)
end
