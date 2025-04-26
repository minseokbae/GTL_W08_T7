function BeginPlay()
    print("[BeginPlay] " .. obj.UUID)
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

function OnOverlap(overlapObj)
    --충돌 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴\
    local repulsion = Vector.new(0,0,0)
    if obj.Velocity.X>0 then
        repulsion = Vector.new(-1,0,0)
    elseif obj.Velocity.X<0 then
        repulsion = Vector.new(1,0,0)
    end
    if obj.Velocity.Y>0 then
        repulsion = Vector.new(0,-1,0)
    elseif obj.Velocity.Y<0 then
        repulsion = Vector.new(0,1,0)
    end
    obj.Location = obj.Location + repulsion
end

function OnEndOverlap(overlapObj)
    --충돌 끝 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
end

function Tick(dt)
    local ok, err = pcall(function()
        if obj == nil then
            print("[Lua] obj is nil")
            return
        end
        obj.Velocity = Vector.new(0,0,0)
        if(Input.A) then
            obj.Velocity = Vector.new(0,-1,0)
        end
        if (Input.D)  then
            obj.Velocity = Vector.new(0,1,0)
        end
        if(Input.W) then
            obj.Velocity = Vector.new(1,0,0)
        end
        if (Input.S)  then
            obj.Velocity = Vector.new(-1,0,0)
        end
        obj.Location = obj.Location + obj.Velocity * dt * 10
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end