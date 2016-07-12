-- load dependencies
local socket = require "socket"
local copas = require "copas"
local queue = require "queue"

-- this coordinator's host and port
local port = 51034
local server = socket.udp()

-- make socket
server:setsockname("*",port)

function handler(skt)
	--use luasocket calls directly on skt
	skt = copas.wrap(skt)

	-- Queue for waiting writers
	local q = Queue.new()
	-- Coordinator's log
	local file = io.open("log.txt", "w")
	file:write("COORDINATOR LOG\n")
	file:close()

	print("UDP mutex coordinator")
	while true do
	-- auxiliar variables
	local s, err, sktport, sktip, message

	-- receive message on socket
	-- store sender infos on variables
	print("receiving...")
	s, err, sktport, sktip = skt:receivefrom(1)

	if not s then
		print("Receive error: ", err)
		break
	else
		-- CENTRALIZED MUTEX ALGORITHM start
			-- Received REQUEST == 0x0
			if s==tostring(0x0) then 
				message = "REQUEST"
				if Queue.isempty(q) then
				skt:sendto(0x2, "*", sktport)
				end
				Queue.push(q, sktport)

			-- Received RELEASE == 0x1
			elseif s==tostring(0x1) then
				message = "RELEASE"
				Queue.pop(q)
				if not Queue.isempty(q) then
				-- send GRANT == 0x2
				skt:sendto(0x2, "*", Queue.head(q))
				end

			-- Received unknown	
			else
				message = "UNKNOWN"
			end
		-- CENTRALIZED MUTEX ALGORITHM end
		
		-- Write log
		file = io.open("log.txt", "a")
		file:write(
		os.date() .. 
		" Process: " .. 
		sktport .. 
		": " .. 
		message ..
		"\n"
		)
		file:close()
	end

	print("Data: ", s)
	print("Port: ", sktport)
	end

end

-- Create server and start serving
copas.addserver(server, handler, 1)
copas.loop()
