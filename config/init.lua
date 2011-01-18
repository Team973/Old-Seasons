-- init.lua

local pairs = pairs
local require = require

module(...)

-- Global Settings

-- End Global Settings

local profileName = "competition"
local profile = require(_NAME .. "." .. profileName)

for k, v in pairs(profile) do
    if not (k == "_NAME" or k == "_M" or k == "_PACKAGE") then
        _M[k] = v
    end
end

-- vim: ft=lua et ts=4 sts=4 sw=4
