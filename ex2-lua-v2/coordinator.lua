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
	--skt = copas.wrap(skt)

	-- Coordinator's log
	local file = io.open("log.txt", "w")
	file:write("COORDINATOR LOG\n")
	file:close()

	print("UDP mutex coordinator")
	while true do
		-- auxiliar variables
		--s, err, sktport, sktip, message

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
						-- send GRANT == 0x2
						skt:sendto(0x2, "*", sktport)
						-- Mark that a grant happened
						grant = true
						granted = sktport
					end
					Queue.push(q, sktport)

				-- Received RELEASE == 0x1
				elseif s==tostring(0x1) then
					message = "RELEASE"
					Queue.pop(q)
					if not Queue.isempty(q) then
						-- send GRANT == 0x2
						skt:sendto(0x2, "*", Queue.head(q))
						-- Log grant
						grant = true
						granted = Queue.head(q)
					end
			-- CENTRALIZED MUTEX ALGORITHM end
				-- Received NEW CONNECTION == 0x3
				elseif s==tostring(0x3) then
					message = "CONNECT"
					if not started then
						strated = true
						start_time = os.time()
					end
					clientcount = clientcount + 1
				-- Received END CONNECTION == 0x4
				elseif s==tostring(0x4) then
					message = "DISCONNECT"
					clientcount = clientcount - 1		
					if clientcount <= 0 then
						started = false
						end_time = os.time()
						local elapsed_time = os.difftime(end_time, start_time)
						print("Time elapsed: ", elapsed_time)
						-- Write log
						file = io.open("log.txt", "a")
						file:write("ELAPSED (" .. os.date("%X") .. ") SECONDS (" .. elapsed_time .. ")\n")
						file:close()
						return
					end
				-- Received unknown
				else
					message = "UNKNOWN"
				end
			
			-- Write log
			file = io.open("log.txt", "a")

			file:write(
				message ..
				" (" ..
				os.date("%X") .. 
				") PROCESS (" .. 
				sktport .. 
				")\n"
			)

			file:write( 
				"QUEUE (" ..
				os.date("%X") .. 
				") SIZE (" ..  
				Queue.size(q) ..
				")\n"
			)

			if message=="CONNECT" or message=="DISCONNECT" then
				file:write(
					"CONNECTIONS (" ..
					os.date("%X") ..
					") COUNT (" ..
					clientcount ..
					")\n"
				)
			end

			if grant then
				file:write(
					"GRANT (" ..
					os.date("%X") .. 
					") PROCESS (" .. 
					granted ..
					")\n"
				)
				grant = false
			end

			file:close()

		end

	end

end

-- Queue for waiting writers
q = Queue.new()
started = false
grant = false
granted = 0
clientcount = 0
start_time = 0
end_time = 0

-- Create server and start serving
--copas.addserver(server, handler, 1)
--copas.loop()
handler(server)