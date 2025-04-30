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
local hitCount = 0
function OnOverlap(overlapObj)
    --충돌 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
    local ok, err = pcall(function()
        print(overlapObj.Tag)
        if (overlapObj.Tag:Equals("Coin")) then
            if not gameOver then
                PlaySound("score", 1.0, false) -- AddScore
                AddScore(1)
            end
        elseif overlapObj.Tag:Equals("Ghost") then
            if not(gameOver) then
                if hitCount<2 then
                    hitCount = hitCount+1
                    GameOver()
                else
                    print("GameOver")
                    PlaySound("lose", 1.0, false) -- Lose
                    local modifier = CreateCameraTransitionModifier(obj)
                    local position = Vector.new(27.5, 27.5, 60) - obj.Location
                    modifier:Initialize(position, Rotator.new(89,0,0) - obj.Rotation, 60, 5.0)
                    print("success Initialize")
                    Global.CameraManager:AddCameraModifier(modifier)
                    print("success AddModifier")
                    ChangeViewMode(3)
                    GameOver()
                    gameOver = true
                end
            end
        else
            obj.Location = obj.Location - obj.Velocity * 5
            print(obj.Velocity)
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
        if elapsedTime >= 10.0 then
            GameOver()
        end
    end)

    if not ok then
        print("[Lua Error]", err)
    end
end
