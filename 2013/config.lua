-- config.lua

local io = require("io")
local string = require("string")

local loadfile = loadfile
local pairs = pairs
local setfenv = setfenv
local tostring = tostring
local type = type

module(...)

local function uberTostring(val, indent)
    local t = type(val)
    if not indent then
        indent = ""
    end
    local nextIndent = indent .. "\t"
    if t == "number" or t == "boolean" or t == "nil" then
        return tostring(val)
    elseif t == "string" then
        -- Return quoted string
        return string.format("%q", val)
    elseif t == "table" then
        local s = "{\n"
        for k, v in pairs(val) do
            s = s .. nextIndent .. string.format("[%s] = %s,\n", uberTostring(k), uberTostring(v, nextIndent))
        end
        return s .. indent .. "}"
    else
        return t
    end
end

local function confpath(name)
    return name..".lua"
end

function read(name)
    local f = loadfile(confpath(name))
    local tbl = {}
    setfenv(f, tbl)
    f()
    return tbl
end

function write(name, tbl)
    local f = io.open(confpath(name), "w")
    for k, v in pairs(tbl) do
        f:write(k.." = "..uberTostring(v).."\n")
    end
    f:close()
end
