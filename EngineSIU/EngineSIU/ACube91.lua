function BeginPlay()
    print("[BeginPlay] " .. obj.UUID)
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

function Tick(dt)
    local ok, err = pcall(function()
        if obj == nil then
            print("[Lua] obj is nil")
            return
        end
        local velocity = Vector.new()
        if(Input.A) then
            velocity = Vector.new(0,-1,0)
        end
        if (Input.D)  then
            velocity = Vector.new(0,1,0)
        end
        if(Input.W) then
            velocity = Vector.new(1,0,0)
        end
        if (Input.S)  then
            velocity = Vector.new(-1,0,0)
        end
        obj.Location = obj.Location + velocity * dt * 10
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end