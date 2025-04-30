function BeginPlay()
    PlaySound("piano", 1.0, true) -- BGM
    ChangeViewMode(2)
    print("[BeginPlay] " .. obj.UUID)
end

function EndPlay()
    StopSound("piano")
    print("[EndPlay] " .. obj.UUID)
end
local elapsedTime = 0.0
local gameOver = false;
function OnOverlap(overlapObj)
    --충돌 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
    local ok, err = pcall(function()
        if not(gameOver) then
            print(overlapObj.Tag)
            if (overlapObj.Tag:Equals("Coin")) then
                PlaySound("score", 1.0, false) -- AddScore
                AddScore(1)
            elseif overlapObj.Tag:Equals("Ghost") then
                print("GameOver")
                PlaySound("lose", 1.0, false) -- Lose
                local modifier = CreateCameraTransitionModifier(obj)
                local position = Vector.new(27.5, 27.5, 60) - obj.Location
                modifier:Initialize(position, Rotator.new(90,0,0), 60, 5.0)
                print("success Initialize")
                Global.CameraManager:AddCameraModifier(modifier)
                print("success AddModifier")
                ChangeViewMode(3)
                gameOver = true
            else
                obj.Location = obj.Location - obj.Velocity * 5
                print(obj.Velocity)
            end
        end
    end)

    if not ok then
        print("[Lua Error]", err)
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
        --GameOver()
        if gameOver then
            elapsedTime = elapsedTime + dt
            --obj.Rotation = Rotator.new(0,5,0) + obj.Rotation
        end
        if elapsedTime >= 30.0 then
            GameOver()
        end
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end
