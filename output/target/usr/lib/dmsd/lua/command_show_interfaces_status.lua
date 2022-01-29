local handler = {}
local Logger  = require('logger')
local JSON    = require('jansson')
local confapi = require('confapi')

local log = Logger:new('show_portstatus')

local function get_status(port)
    if not port.Enable then
        return 'disabled'
    end

    return port.Status == 'Enabled' and 'connected' or 'not-connected'
end


local function get_duplex(port, stats)
    local force_speed = port.Speed
    local function duplexstr(duplex)
            return duplex and "full" or "half"
        end

    if force_speed then
        return port.Enable and string.format("a-%s", duplexstr(stats.Duplex)) or "auto"
    end

    return port.Enable and duplexstr(stats.Duplex) or "auto"
end


local function get_speed(port, stats)
    local force_speed = port.Speed

    if force_speed then
        return port.Enable and string.format("a-%d", stats.Speed) or "auto"
    end

    return port.Enable and tostring(stats.Speed) or "auto"
end


local function get_fc(port, stats)
    local force_speed = port.Speed

    return port.Enable and stats.FlowControl or "-"
end

local function set_info_out(info, out)
    local inter_path = info.path:sub(#info.handler_path)

    info.out = out:find_branch(inter_path)
end

function handler:get_portstatus(info)
    local ret = JSON.object()
    local config, err = confapi.get_full('Device.Switch.Ports.', 'Device.Statistics.Port.')

    if err then
        log:err("confapi error: %s", err:dump())
        return 9002
    end

    local ports = config.Device.Switch.Ports

    for _, port in pairs(ports) do
        local port_stat = config:find_branch(port.Statistics)

        local instance = JSON({
            Name        = port.Alias,
            Status      = get_status(port),
            Duplex      = get_duplex(port, port_stat),
            Speed       = get_speed(port, port_stat),
            FlowControl = get_fc(port, port_stat)
        })

        ret[#ret + 1] = instance
    end

    set_info_out(info, JSON({Ports = ret}))
    --info.out = JSON({Ports = ret})
    return 0
end

function handler:doit(info)
    if (info.act == "get") then
        return self:get_portstatus(info);
    end

    return 0
end

return handler
