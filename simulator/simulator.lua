local sourceRoot = "../2011-ek/"

local function newRobotEnv()
    local env
    env = {
        package={
            loaded={
                coroutine = require("coroutine"),
                debug = require("debug"),
                io = require("io"),
                math = require("math"),
                os = require("os"),
                string = require("string"),
                table = require("table"),
                wpilib = require("wpilib"),
            },
        },
        require=function(name)
            if not env.package.loaded[name] then
                env.package.loaded[name] = true
                local mod, err = loadfile(sourceRoot .. name .. ".lua")
                if not mod then error(err) end
                setfenv(mod, env)
                local res = mod(name)
                if res ~= nil then
                    env.package.loaded[name] = res
                end
            end
            return env.package.loaded[name]
        end,
        module=function(modname, ...)
            local M
            -- TODO: Support nested modules
            if type(env.package.loaded[modname]) == type({}) then
                M = env.package.loaded[modname]
            else
                M = {}
            end

            M._NAME = modname
            M._M = M
            -- TODO: Support nested modules
            M._PACKAGE = ""

            env[modname] = M
            env.package.loaded[modname] = M
            setfenv(2, M)

            for _, f in ipairs{...} do
                f(M)
            end
        end,
    }
    env._G = env
    env.package.loaded._G = env
    setmetatable(env, {__index=_G})
    setmetatable(env.package, {__index=package})
    return env
end

local robotEnv = newRobotEnv()
local bootloader = loadfile(sourceRoot .. "boot.lua")
setfenv(bootloader, robotEnv)
bootloader("boot")
