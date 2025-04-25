function BeginPlay()
    print("[BeginPlay] " .. obj.UUID)
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

up = true

function Tick(dt)
    local ok, err = pcall(function()
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
        local velocity = Vector.new(-1,-1,-1);
        if up then
            velocity = Vector.new(1,1,1);
        end
        obj.Location = obj.Location + velocity * dt * 10
        print("[Lua] success")
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end