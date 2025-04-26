local angle = 0
local startLocation = nil

function BeginPlay()
    print("BeginPlay")
    startLocation = obj.Location
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

function Tick(dt)
    local ok, err = pcall(function()
        --여기에 매 프레임 실행할 내용을 작성
        angle = angle + dt * 0.1
        obj.Location = Vector.new(obj.Location.X, obj.Location.Y, startLocation.Z + math.sin(angle))
        obj.Rotation = obj.Rotation + Vector.new(0,1,0) * dt * 10
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end