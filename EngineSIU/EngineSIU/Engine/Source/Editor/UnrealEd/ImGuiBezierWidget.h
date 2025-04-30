#pragma once

namespace ImGui
{
    float BezierValue(float t, float P[4]);
    int Bezier(const char* label, float P[5]);
    void ShowBezierDemo();
}
