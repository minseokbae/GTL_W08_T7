math.randomseed(os.time())
local tileSize = 5

local dirToYaw = {
    ["1,0"]   = 180,
    ["0,1"]   = 270,
    ["-1,0"]  = 0,
    ["0,-1"]  = 90
}

if not ghost then
    ghost = {
        dir = {dx=1, dy=0},
        speed = 5
    }
end

local function getYawByDir(dir)
    return dirToYaw[tostring(dir.dx)..","..tostring(dir.dy)] or 0
end

function chooseRandomDirection(x, y, prevDir)
    local dirs = {
        {dx=0, dy=-1}, {dx=1, dy=0}, {dx=0, dy=1}, {dx=-1, dy=0}
    }
    local candidates = {}
    local reverseDir = nil
    local mapHeight = #gameMap
    local mapWidth = #gameMap[1]

    for _, dir in ipairs(dirs) do
        local nx, ny = x + dir.dx, y + dir.dy
        if prevDir and dir.dx == -prevDir.dx and dir.dy == -prevDir.dy then
            reverseDir = dir
        else
            if nx >= 1 and nx <= mapWidth and ny >= 1 and ny <= mapHeight then
                if gameMap[ny][nx] and gameMap[ny][nx] ~= 1 then
                    table.insert(candidates, dir)
                end
            end
        end
    end

    if #candidates > 0 then
        return candidates[math.random(1, #candidates)]
    elseif reverseDir then
        return reverseDir
    else
        return {dx=0, dy=0}
    end
end

function isIntersection(x, y)
    local dirs = {
        {dx=0, dy=-1}, {dx=1, dy=0}, {dx=0, dy=1}, {dx=-1, dy=0}
    }
    local count = 0
    local mapHeight = #gameMap
    local mapWidth = #gameMap[1]
    for _, dir in ipairs(dirs) do
        local nx, ny = x + dir.dx, y + dir.dy
        if nx >= 1 and nx <= mapWidth and ny >= 1 and ny <= mapHeight then
            if gameMap[ny][nx] and gameMap[ny][nx] ~= 1 then
                count = count + 1
            end
        end
    end
    return count >= 3
end

--회전
obj.Rotation = Vector.new(0, getYawByDir(ghost.dir), 0)

function Tick(dt)
    if obj == nil then
        print("[Lua] obj is nil")
        return
    end

    local mapHeight = #gameMap
    local mapWidth = #gameMap[1]

    -- 현재 위치 (실수 좌표)
    local x = obj.Location.X
    local y = obj.Location.Y

    -- 현재 타일 인덱스 및 중심 좌표
    local curGridX = math.floor(x / tileSize + 0.5)
    local curGridY = math.floor(y / tileSize + 0.5)
    local centerX = curGridX * tileSize
    local centerY = curGridY * tileSize
    local dist = math.sqrt((x - centerX)^2 + (y - centerY)^2)

    -- 타일 중심에 충분히 가까울 때만 방향전환/벽체크
    if dist < 0.1 then
        local nextGridX = curGridX + ghost.dir.dx
        local nextGridY = curGridY + ghost.dir.dy
        if nextGridX < 1 or nextGridX > mapWidth or nextGridY < 1 or nextGridY > mapHeight or gameMap[nextGridY][nextGridX] == 1 then
            ghost.dir = chooseRandomDirection(curGridX, curGridY, ghost.dir)
            --회전
            obj.Rotation = Vector.new(0, getYawByDir(ghost.dir), 0)
        elseif isIntersection(curGridX, curGridY) and math.random() < 0.5 then -- 교차로에서 방향 전환할 확률
            ghost.dir = chooseRandomDirection(curGridX, curGridY, ghost.dir)
            --회전
            obj.Rotation = Vector.new(0, getYawByDir(ghost.dir), 0)
        end
        -- 중심에 정확히 스냅 (한 번만!)
        obj.Location = Vector.new(centerX, centerY, obj.Location.Z)
    end

    -- 슬라이딩 이동 전에 "다음 위치가 벽인지" 체크
    local nx = x + ghost.dir.dx * ghost.speed * dt
    local ny = y + ghost.dir.dy * ghost.speed * dt
    local nextGridX = math.floor(nx / tileSize + 0.5)
    local nextGridY = math.floor(ny / tileSize + 0.5)

    if nextGridX < 1 or nextGridX > mapWidth or nextGridY < 1 or nextGridY > mapHeight or gameMap[nextGridY][nextGridX] == 1 then
        -- 벽이면 이동하지 않고, 현재 타일 중심에 스냅
        obj.Location = Vector.new(centerX, centerY, obj.Location.Z)
        return
    end

    -- 이동
    obj.Location = Vector.new(nx, ny, obj.Location.Z)
end