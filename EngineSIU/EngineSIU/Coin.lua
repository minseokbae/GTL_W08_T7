function BeginPlay()
    print("[BeginPlay] " .. obj.UUID)
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

angle = 0

function Tick(dt)
    local ok, err = pcall(function()
        --여기에 매 프레임 실행할 내용을 작성
        angle = angle + dt
        obj.Location = math.sin(angle) * Vector.new(0,0,1)
        obj.Rotation = obj.Rotation + Vector.new(0,1,0) * dt * 100
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end