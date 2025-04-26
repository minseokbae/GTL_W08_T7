-- 최초 한 번만 초기화
if not ghost then
    ghost = {
        dir = {dx=1, dy=0}, -- 처음 방향(예: 오른쪽)
        speed = 20           -- 1초에 5칸 이동
    }
end

function chooseRandomDirection(x, y)
    local dirs = {
        {dx=0, dy=-1}, -- 위
        {dx=1, dy=0},  -- 오른쪽
        {dx=0, dy=1},  -- 아래
        {dx=-1, dy=0}  -- 왼쪽
    }
    local candidates = {}
    local mapHeight = #gameMap
    local mapWidth = #gameMap[1]
    for _, dir in ipairs(dirs) do
        local nx, ny = x + dir.dx, y + dir.dy
        if nx >= 1 and nx <= mapWidth and ny >= 1 and ny <= mapHeight then
            if gameMap[ny][nx] and gameMap[ny][nx] ~= 1 then
                table.insert(candidates, dir)
            end
        end
    end
    if #candidates > 0 then
        return candidates[math.random(1, #candidates)]
    else
        return {dx=0, dy=0}
    end
end

function Tick(dt)
    if obj == nil then
        print("[Lua] obj is nil")
        return
    end

    -- 현재 위치 (실수 좌표)
    local x = obj.Location.X
    local y = obj.Location.Y

    -- 다음 위치 예측
    local nx = x + ghost.dir.dx * ghost.speed * dt
    local ny = y + ghost.dir.dy * ghost.speed * dt

    -- 정수 좌표(격자 기준)로 변환
    local gridX = math.floor(nx + 0.5)
    local gridY = math.floor(ny + 0.5)

    local mapHeight = #gameMap
    local mapWidth = #gameMap[1]

    -- 다음 칸이 벽이면 방향을 새로 선택
    if gridX < 1 or gridX > mapWidth or gridY < 1 or gridY > mapHeight or gameMap[gridY][gridX] == 1 then
        -- 현재 위치에서 이동 가능한 랜덤 방향 선택
        local curGridX = math.floor(x + 0.5)
        local curGridY = math.floor(y + 0.5)
        ghost.dir = chooseRandomDirection(curGridX, curGridY)
        -- 방향이 바뀌었으니 위치는 그대로 두고 return
        return
    end

    -- 이동
    obj.Location = Vector.new(nx, ny, obj.Location.Z)
    -- print(string.format("[Lua] Move to (%.2f, %.2f)", nx, ny))
end
