function BeginPlay()
    print("[BeginPlay] " .. obj.UUID)
end

function EndPlay()
    print("[EndPlay] " .. obj.UUID)
end

function Tick(dt)
    local velocity = Vector.new(1.0,1.0,1.0)
    obj.Location = obj.Location + velocity * dt
end