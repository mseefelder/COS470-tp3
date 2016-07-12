-- change here to the host an port you want to contact
local host, port = "localhost", 51034
-- load namespace
local socket = require "socket" 
-- convert host name to ip address
local ip = assert(socket.dns.toip(host))
-- create a new UDP object
local udp = assert(socket.udp())

function sleep(sec)
    socket.select(nil, nil, sec)
end

local response
for i=1,100 do
	-- contact coordinator, send REQUEST (0x0)
	assert(udp:sendto(0x0, ip, port))
	-- wait for response
	response, err = assert(udp:receive(1))
	if not response then
		print("Received error: ", err)
    elseif response == tostring(0x2) then
    	print("Aw yis")
    else
    	print("Received: ", response)
	end
	sleep(1)
end