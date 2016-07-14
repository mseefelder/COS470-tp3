-- load namespace
local socket = require "socket" 

-- server host and port
local host, port = "localhost", 51034

-- convert host name to ip address
local ip = assert(socket.dns.toip(host))
-- create a new UDP object
local udp = assert(socket.udp())

-- function used for sleeping
-- hacked from socket
-- works with subsecond values
function sleep(sec)
    socket.select(nil, nil, sec)
end

-- main code
function writer()
	-- writer name
	local name = arg[1]~=nil and arg[1] or "Bob"
	-- random seed
	local seed = (arg[2]~=nil and type(arg[2])=="number") and arg[2] or os.time()
	math.randomseed(seed)

	-- variable to store temporary response
	local response

	-- first sleep
	sleep(math.random())
	assert(udp:sendto(0x3, ip, port)) --CONNECT

	-- write 100 lines
	for i=1,100 do
		-- contact coordinator, send 0x0 == REQUEST
		assert(udp:sendto(0x0, ip, port))
		-- wait for response
		response, err = assert(udp:receive(1))
		if not response then
			print("Received error: ", err)
	    elseif response == tostring(0x2) then
	    	-- 0x2 == GRANT
			-- critical session start
				local file = io.open("critical.txt", "a")
				file:write(name .. " is writing " .. i .. "\n")
				file:close()
	    	-- critical session end
		    -- send 0x1 == RELEASE to coordinator
		    assert(udp:sendto(0x1, ip, port))
		    sleep(math.random())
	    else
	    	-- received unknown message
	    	print("Received: ", response)
		end
	end

	assert(udp:sendto(0x4, ip, port)) --DISCONNECT
end

-- run script
writer()

print("Writer out!")
