#pragma once
#include "Define.h"
#include "IWindowToggleable.h"
#include "UnrealEd/EditorViewportClient.h"

namespace EEngineShowFlags
{
enum Type : uint64
{
    None = 0,
    SF_AABB = 1ULL << 0,
    SF_Primitives = 1ULL << 1,
    SF_BillboardText = 1ULL << 2,
    SF_UUIDText = 1ULL << 3,
    SF_Fog = 1ULL << 4,
    SF_Collision= 1ULL << 5
};
}

class UWorld;
class ShowFlags : public IWindowToggleable
{
private:
	ShowFlags();

public:
	static ShowFlags& GetInstance();

	void Draw(const std::shared_ptr<FEditorViewportClient>& ActiveViewport) const;
	uint64 ConvertSelectionToFlags(const bool selected[]) const;
	void OnResize(HWND hWnd);

    virtual void Toggle() override {
		if (bWasOpen) {
			bWasOpen = false;
		}
	}
	uint64 currentFlags;
private:
	bool bWasOpen = true;
	UINT width;
	UINT height;
};
