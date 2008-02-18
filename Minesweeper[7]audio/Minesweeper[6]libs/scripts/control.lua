--Player control
--Lua doesn't have cases, this is the only way to achieve the same thing

function moveY (move,currenty)
	if move == 2 then	
	return currenty + 1
	elseif move == 1 then	
	return currenty - 1
	end	
end

function moveX (move,currentx)
	if move == 2 then
	return currentx + 1
	elseif move == 1 then
	return currentx - 1	
	end
end
