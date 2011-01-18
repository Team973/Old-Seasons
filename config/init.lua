-- init.lua

local open = io.open
local pairs = pairs
local require = require

module(...)

-- Global Settings

-- End Global Settings

local profileName = "competition"
do
    -- Read file profile.txt
    local f, err = open("profile.txt")
    if not err then
        local line = f:read()
        f:close()
        -- Find the first word in the file
        local newName = line:match("(%w+)")
        if newName then profileName = newName end
    end
end

-- Load configuration file (based on word read)
local profile = require(_NAME .. "." .. profileName)

-- Add variables from other configuration file
for k, v in pairs(profile) do
    if not (k == "_NAME" or k == "_M" or k == "_PACKAGE") then
        _M[k] = v
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
