local angle = 0
local startLocation = nil
local destroy = false

function BeginPlay()
    print("BeginPlay")
    startLocation = obj.Location
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

function OnOverlap(overlapObj)
    --충돌 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
    print(overlapObj.Tag)
    if(overlapObj.Tag:Equals("Player")) then
        print("overlap player")
        destroy = true
    end
end

function OnEndOverlap(overlapObj)
    --충돌 끝 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
end

function Tick(dt)
    local ok, err = pcall(function()
        --여기에 매 프레임 실행할 내용을 작성
        angle = angle + dt
        obj.Location = Vector.new(obj.Location.X, obj.Location.Y, startLocation.Z + math.sin(angle))
        obj.Rotation = obj.Rotation + Vector.new(0,1,0) * dt * 30

        if destroy then
            obj:Destroy()
        end
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end