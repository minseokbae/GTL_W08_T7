function BeginPlay()
    PlaySound("piano", 1.0, true) -- BGM
    ChangeViewMode(2)
    print("[BeginPlay] " .. obj.UUID)
    
    -- 변수 초기화 확인
    print("[BeginPlay] hitCount initialized to: " .. tostring(hitCount))
    
    -- 카메라 매니저 확인
    if Global.CameraManager ~= nil then
        print("[BeginPlay] CameraManager is available")
    else
        print("[WARNING] CameraManager is not available in BeginPlay!")
    end
    
    -- CreateCameraShake 함수 확인
    if CreateCameraShake ~= nil then
        print("[BeginPlay] CreateCameraShake function is available")
    else
        print("[WARNING] CreateCameraShake function is not available in BeginPlay!")
    end
end

function EndPlay()
    StopSound("piano")
    print("[EndPlay] " .. obj.UUID)
    -- 카메라 쉐이크 정리
    if cameraShake ~= nil then
        cameraShake:StopShake(true)
        cameraShake = nil
    end
end

-- 먼저 전역 변수로 선언
local elapsedTime = 0.0
local gameOver = false
local hitCount = 0
local cameraShake = nil -- 카메라 쉐이크 객체를 저장할 변수

-- 카메라 쉐이크 기능을 별도 함수로 분리
function ApplyCameraShake()
    -- 디버깅을 위한 로그 추가
    print("[ApplyCameraShake] Starting...")
    
    -- Global.CameraManager 체크
    if Global.CameraManager == nil then
        print("[ERROR] CameraManager is nil!")
        return
    end
    
    -- 충돌 횟수에 따라 쉐이크 강도와 지속시간을 다르게 설정
    -- hitCount가 nil이면 기본값 0 사용
    local currentHitCount = hitCount or 0
    local duration = 0.3 + (currentHitCount * 0.2) -- 충돌이 많을수록 더 길게
    local intensity = 10.0 + (currentHitCount * 0.5) -- 충돌이 많을수록 더 강하게
    
    print("[ApplyCameraShake] Duration: " .. duration .. ", Intensity: " .. intensity)
    
    -- CreateCameraShake 함수 존재 여부 확인
    if CreateCameraShake == nil then
        print("[ERROR] CreateCameraShake function is not defined!")
        return
    end
    
    local success, result = pcall(function()
        if cameraShake == nil then
            -- 카메라 쉐이크 객체 생성 (Duration, BlendInTime, BlendOutTime)
            print("[ApplyCameraShake] Creating new camera shake object...")
            cameraShake = CreateCameraShake(duration, 0.1, 0.2)
            
            if cameraShake == nil then
                print("[ERROR] Failed to create camera shake object!")
                return
            end
            
            print("[ApplyCameraShake] Adding camera shake to camera manager...")
            -- 카메라 매니저에 쉐이크 추가
            Global.CameraManager:AddCameraModifier(cameraShake)
        else
            -- 기존 쉐이크 객체가 있으면 속성만 업데이트
            print("[ApplyCameraShake] Updating existing camera shake object...")
            cameraShake.Duration = duration
        end
        
        -- 쉐이크 강도 및 패턴 설정
        print("[ApplyCameraShake] Setting shake pattern...")
        cameraShake.ShakePattern.RotationAmplitudeMultiplier = intensity
        cameraShake.ShakePattern.RotationFrequencyMultiplier = 10.0 + (currentHitCount * 2.0)
        cameraShake.ShakePattern.RotationAmplitude = Vector.new(1.0, 1.0, 0.7) * intensity
        
        -- 쉐이크 재생
        print("[ApplyCameraShake] Playing shake...")
        cameraShake:PlayShake()
        
        print("[ApplyCameraShake] Camera Shake applied with intensity: " .. intensity)
    end)
    
    if not success then
        print("[ERROR in ApplyCameraShake] " .. tostring(result))
    end
end

function OnOverlap(overlapObj)
    --충돌 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
    local ok, err = pcall(function()
        print("[OnOverlap] Tag: " .. overlapObj.Tag:ToAnsiString())
        print("[OnOverlap] Current hitCount: " .. tostring(hitCount))
        
        if (overlapObj.Tag:Equals("Coin")) then
            if not gameOver then
                PlaySound("score", 1.0, false) -- AddScore
                AddScore(1)
            end
        elseif overlapObj.Tag:Equals("Ghost") then
            print("[OnOverlap] Ghost collision detected!")
            if not(gameOver) then
                -- 카메라 쉐이크 적용 시도
                print("[OnOverlap] Attempting to apply camera shake...")
                ApplyCameraShake()
                
                if hitCount < 2 then
                    hitCount = hitCount + 1
                    print("[OnOverlap] Hit count increased to: " .. hitCount)
                    GameOver()
                else
                    print("[OnOverlap] Game over triggered!")
                    PlaySound("lose", 1.0, false) -- Lose
                    
                    -- 게임 오버 시 카메라 전환
                    local modifier = CreateCameraTransitionModifier(obj)
                    if modifier ~= nil then
                        local position = Vector.new(27.5, 27.5, 60) - obj.Location
                        modifier:Initialize(position, Rotator.new(89,0,0) - obj.Rotation, 60, 5.0)
                        print("[OnOverlap] Camera transition initialized")
                        Global.CameraManager:AddCameraModifier(modifier)
                        print("[OnOverlap] Camera modifier added")
                    else
                        print("[ERROR] Failed to create camera transition modifier!")
                    end
                    
                    ChangeViewMode(3)
                    GameOver()
                    gameOver = true
                    
                    -- 게임 오버 시 쉐이크 정지
                    if cameraShake ~= nil then
                        print("[OnOverlap] Stopping camera shake...")
                        cameraShake:StopShake(false) -- false: 서서히 정지
                    end
                end
            end
        else
            obj.Location = obj.Location - obj.Velocity * 5
            print("[OnOverlap] Adjusted location based on velocity")
        end
    end)

    if not ok then
        print("[Lua Error in OnOverlap]", err)
    end
end

function OnEndOverlap(overlapObj)
    --충돌 끝 처리 overlapObj는 충돌한 액터의 루트 컴포넌트를 가리킴
    print("[OnEndOverlap] Tag: " .. overlapObj.Tag:ToAnsiString())
    if overlapObj.Tag:Equals("Coin") then
        print("[OnEndOverlap] Overlap with Coin ended")
    end
end

function Tick(dt)
    local ok, err = pcall(function()
        if obj == nil then
            print("[Lua] obj is nil")
            return
        end
        
        if gameOver then
            elapsedTime = elapsedTime + dt
            --obj.Rotation = Rotator.new(0,5,0) + obj.Rotation
        end
        if elapsedTime >= 10.0 then
            GameOver()
        end
    end)

    if not ok then
        print("[Lua Error in Tick]", err)
    end
end
