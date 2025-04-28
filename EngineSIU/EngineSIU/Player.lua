function BeginPlay()
    print("[BeginPlay] " .. obj.UUID)
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

function OnOverlap(overlapObj)
    --충돌 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
    print(overlapObj.Tag)
    if (overlapObj.Tag:Equals("Coin")) then
        AddScore(1)
    else
        local repulsion = Vector.new(0,0,0)
        if obj.Velocity.X>0 then
            repulsion = repulsion + Vector.new(-0.7,0,0)
        elseif obj.Velocity.X<0 then
            repulsion = repulsion + Vector.new(0.7,0,0)
        end
        if obj.Velocity.Y>0 then
            repulsion = repulsion + Vector.new(0,-0.7,0)
        elseif obj.Velocity.Y<0 then
            repulsion = repulsion + Vector.new(0,0.7,0)
        end
        obj.Location = obj.Location + repulsion
    end
end

function OnEndOverlap(overlapObj)
    --충돌 끝 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
    print(overlapObj.Tag)
    if overlapObj.Tag:Equals("Coin") then
        print("Overlap Coin")
    end
end

function Tick(dt)
    local ok, err = pcall(function()
        if obj == nil then
            print("[Lua] obj is nil")
            return
        end
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end

PlaySound("piano", 1.0, true)