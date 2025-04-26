function BeginPlay()
    print("[BeginPlay] " .. obj.UUID)
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

function OnOverlap(overlapObj)
    --충돌 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
end

function OnEndOverlap(overlapObj)
    --충돌 끝 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
end

local up = true

function Tick(dt)
    local ok, err = pcall(function()
        --여기에 매 프레임 실행할 내용을 작성
        if obj == nil then
            print("[Lua] obj is nil")
            return
        end
        print("[Lua] Location =", obj.Location)
        if(obj.Location.X > 10 or obj.Location.Y > 10 or obj.Location.Z > 10) then
            up = false
        elseif(obj.Location.X < -10 or obj.Location.Y < -10 or obj.Location.Z < -10) then
            up = true
        end
        obj.Velocity = Vector.new(-1,-1,-1);
        if up then
            obj.Velocity = Vector.new(1,1,1);
        end
        obj.Location = obj.Location + obj.Velocity * dt * 10
        print("[Lua] success")
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end