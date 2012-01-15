local table = table
local setmetatable = setmetatable

module(...)

-- SCHEDULER

local commands = {}, additions = {}, subsystems = {}

local processCommandAddition

-- Run a single iteration of the scheduler.
function Run()
    -- Loop through commands
    for _, command in ipairs(commands) do
        if not command:_run() then
            RemoveCommand(command)
        end
    end

    -- Add new commands
    for _, addition in ipairs(additions) do
        processCommandAddition(addition)
    end
    additions = {}

    -- TODO: Add defaults
end

function AddCommand(command)
    additions[#additions + 1] = command
end

function RegisterSubsystem(subsystem)
    subsystems[#subsystems + 1] = subsystem
end

function RemoveCommand(command)
    -- TODO
end

function processCommandAddition(command)
end

-- COMMAND

Command = {}

--[[
attributes:
    name
    startTime
    timeout
    initialized
    requirements
    running
    interruptible
    canceled
    locked
    runWhenDisabled
    parent
    networkTable
--]]

function Command:New(tbl)
    local newCommand = {
        locked=false,
        initialized=false,
        running=false,
        interruptable=false,
        canceled=false,
        runWhenDisabled=false,
    }
    setmetatable(newCommand, {__index=self})
    for k, v in pairs(tbl) do
        newCommand[k] = v
    end
    return newCommand
end

function Command:Initialize()
end

function Command:Execute()
end

function Command:IsFinished()
end

function Command:Interrupted()
    self:End()
end

function Command:End()
end

function Command:Cancel()
    self.canceled = true
end

-- internal method for scheduler
function Command:_run()
    -- TODO: Check for disabled

    if self.canceled then
        return false
    end

    if not self.initialized then
        self.initialized = true
        -- TODO: start timing
        self:Initialize()
    end

    self:Execute()
    return not self:IsFinished()
end

-- SUBSYSTEM

Subsystem = {}

--[[
attributes:
name
currentCommand
defaultCommand

private:
_initializedDefaultCommand
--]]

function Subsystem:New(tbl)
    local newSubsystem = {
        locked=false,
        initialized=false,
        running=false,
        interruptable=false,
        canceled=false,
        runWhenDisabled=false,
    }
    setmetatable(newSubsystem, {__index=self})
    for k, v in pairs(tbl) do
        newSubsystem[k] = v
    end
    return newSubsystem
end

-- COMMAND GROUP
CommandGroup = {}
setmetatable(CommandGroup, {__index=Command})

function CommandGroup:New(tbl)
    local newCommandGroup = Command.New(self, {commands={}, currentIndex=nil, children={}})
    for k, v in pairs(tbl) do
        newCommandGroup[k] = v
    end
    return newCommandGroup
end

function CommandGroup:IsFinished()
    return self.currentIndex > #self.commands and #self.children == 0
end

function CommandGroup:IsInterruptable()
    if not self.interruptable then
        return false
    end

    if self.currentIndex and self.currentIndex <= #self.commands then
        -- TODO: .m_command
        local command = self.commands[self.currentIndex]
        if not command:IsInterruptible() then
            return false
        end
    end

    for _, command in pairs(self.children) do
        if not command:IsInterruptible() then
            return false
        end
    end

    return true
end
