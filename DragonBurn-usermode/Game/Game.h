#pragma once
#include <optional>
#include "../Core/MemoryMgr.h"
#include "../Offsets/Offsets.h"
#include "View.h"

class CGame
{
private:
	struct
	{
		DWORD64 ServerDLL;
		DWORD64 ClientDLL;
		DWORD64 EntityList;
		DWORD64 Matrix;
		DWORD64 ViewAngle;
		DWORD64 EntityListEntry;
		DWORD64 LocalController;
		DWORD64 LocalPawn;
		DWORD64 ServerPawn;
		DWORD64 GlobalVars;
		DWORD64 JumpBtn;
		DWORD64 AttackBtn;
		DWORD64 RightBtn;
		DWORD64 LeftBtn;
	}Address;

public:
	CView View;

public:	
	bool InitAddress();

	DWORD64 GetClientDLLAddress();
	DWORD64 GetServerDLLAddress();
	DWORD64 GetEntityListAddress();
	DWORD64 GetMatrixAddress();
	DWORD64 GetViewAngleAddress();
	DWORD64 GetEntityListEntry();
	DWORD64 GetLocalControllerAddress();
	DWORD64 GetLocalPawnAddress();
	DWORD64 GetServerPawnAddress();
	DWORD64 GetGlobalVarsAddress();
	DWORD64 GetJumpBtnAddress();
	DWORD64 GetAttackBtnAddress();
	DWORD64 GetRightBtnAddress();
	DWORD64 GetLeftBtnAddress();

	bool UpdateEntityListEntry();
};

inline CGame gGame;