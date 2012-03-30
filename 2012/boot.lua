--[[
boot.lua
Created by Ross Light on 2010-09-28.

This is a sample boot.lua file.  Feel free to use it in your own programs, or
write your own, if you wish.

This file is public domain.

    Overview:

    When FIRSTLua starts, it will try to run a Lua file called lua/boot.lua.
    From there, this Lua file is expected to load the rest of the Lua code.
    You could put all of your core logic into the boot.lua file, but we
    recommend writing a bootloader: very reliable code that runs the rest of
    your robot.
    
    This file is one such example.
    
    Features:
    
    First of all, the robot code will automatically be re-run if an error
    occurs.  Don't want a frozen robot in a match, eh? Second, any errors will
    be written to a file called boot-error.txt, complete with a stack trace.
    Finally, this bootloader allows the operator to do a live code reload; the
    code can be changed at runtime.  However, the bootloader does not give a
    specific operator control to do this -- that's your job as a programmer.
    You can call the restartRobot function from anywhere in your code and the
    bootloader will recognize that signal and reload your source.
--]]

local wpilib = require("wpilib")

local restartError = {"Robot Restart xyzzy"}

--[[
restartRobot can be called from anywhere in the code to initiate a live code
reload.  Any non-essential packages will be unloaded and the robot begins again.
--]]
function restartRobot()
    error(restartError, 2)
end

-- Add your package to this table if you don't want it to be unloaded 
local keepPackage =
{
    ["_G"] = true,
    ["bit"] = true,
    ["coroutine"] = true,
    ["debug"] = true,
    ["io"] = true,
    ["math"] = true,
    ["os"] = true,
    ["package"] = true,
    ["string"] = true,
    ["table"] = true,
    ["wpilib"] = true,
}

-- This function handles destroying a package from globals.
-- This is based off the setfield example in the Lua book.
local function destroyPackage(pkg)
    local t = _G
    for word, dot in string.gmatch(pkg, "([%w_]+)(.?)") do
        if dot == "." then                -- not the last identifier?
            if not t[word] then break end -- stop if parent table doesn't exist
            t = t[word]                   -- get the table
        else                              -- last identifier
            t[word] = nil                 -- set the package to nil
        end
    end
end

local errorOccurred = false
repeat
    local function handleError(msg)
        if msg == restartError then
            return restartError
        else
            return debug.traceback(msg, 2)
        end
    end
    
    -- Run the robot code in protected mode
    local successful, err = xpcall(function()
        local robot = require("robot")
        robot.run()
    end, handleError)
    
    -- Handle any errors
    if not successful then
        if err == restartError then
            -- The user requested a restart.
            -- Unload all user-level packages.
            for name, _ in pairs(package.loaded) do
                if not keepPackage[name] then
                    package.loaded[name] = nil
                    destroyPackage(name)
                end
            end
            -- Run garbage collector before restarting
            collectgarbage("collect")
        else
            -- This is an unexpected error. Write the traceback to
            -- boot-error.txt at the root of the filesystem.
            -- 2010-02-13: This will also fully stop the interpreter now.  This
            --             is the safest possible behavior.
            local f = io.open("lua-error.txt", "w")
            if f then
                f:write(err)
                f:close()
            end
            errorOccurred = true
        end
    end
until errorOccurred

-- vim: ft=lua et ts=4 sts=4 sw=4
