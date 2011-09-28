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
    }
    env._G = env
    env.package.loaded._G = env
    setmetatable(env, {__index=_G})
    return env
end

local robotEnv, task

function love.load()
    local bootloader = loadfile(sourceRoot.."boot.lua")
    robotEnv = newRobotEnv()
    setfenv(bootloader, robotEnv)
    task = coroutine.create(function()
        bootloader("boot")
    end)
    success, message = coroutine.resume(task)
    if not success then print("ERROR'D"); print(message) end
end

function love.update()
    if coroutine.status(task) ~= "dead" then
        success, message = coroutine.resume(task)
        if not success then print("ERROR'D"); print(message) end
    end
end

function love.draw()
    love.graphics.print("Hello World", 400, 300)
end
