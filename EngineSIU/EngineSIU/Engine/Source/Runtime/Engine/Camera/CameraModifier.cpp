#include "CameraModifier.h"
#include <Engine/Engine.h>
#include <fstream> 
#include <sstream> 
#include <string>  
#include <cstdlib> 

UCameraModifier::UCameraModifier()
{
    LoadBezierInfo();
}

void UCameraModifier::AddedToCamera(APlayerCameraManager* Camera)
{
    CameraOwner = Camera;
}

void UCameraModifier::DisableModifier()
{
    bDisabled = true; 
}

void UCameraModifier::EnableModifier()
{
    bDisabled = false;
}

bool UCameraModifier::ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager)
{
    return true;
}

void UCameraModifier::LoadBezierInfo()
{
    FString FilePath = "Bezier.ini";
    std::ifstream file(*FilePath);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, '='))
            {
                std::string valueStr;
                if (std::getline(iss, valueStr))
                {
                    float value = std::stof(valueStr);

                    if (key.find("BezierCurve") == 0)
                    {
                        int index = key.back() - '0';
                        if (index >= 0 && index < 5)
                        {
                            BezierCurve[index] = value;
                        }
                    }
                    else if (key == "UseBezier")
                    {
                        bUseBezier = (value != 0.0f);
                    }
                }
            }
        }
        file.close();
    }
}
