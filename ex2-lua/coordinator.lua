local socket = require "socket"
local copas = require "copas"
local list = require "list"

local port = 51034
local server = socket.udp()
server:setsockname("*",port)

function handler(skt)
  skt = copas.wrap(skt)
  --q = List.new()
  print("UDP connection handler")

  while true do
    local s, err
    print("receiving...")
    s, err, sktport, sktip = skt:receivefrom(1)
    if not s then
      print("Receive error: ", err)
      return
    --elseif s==tostring(0x0) then

    --elseif s==tostring(0x1) then

    end
    print("Received data, bytes:" , #s)
    print("Data: ", s)
    print("IP: ", sktip)
    print("Port: ", sktport)
    skt:sendto(0x2, "*", sktport)
  end
end

copas.addserver(server, handler, 1)
copas.loop()
