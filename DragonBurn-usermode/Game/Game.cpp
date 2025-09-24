#include "Game.h"

bool CGame::InitAddress()
{
	this->Address.ClientDLL = memoryManager.GetModuleBase(L"client.dll");//MemoryMgr::GetModuleBase(MemoryMgr::GetProcessID(L"cs2.exe"),L"client.dll");
	this->Address.ServerDLL = memoryManager.GetModuleBase(L"server.dll");//MemoryMgr::GetModuleBase(MemoryMgr::GetProcessID(L"cs2.exe"),L"server.dll");
	
	this->Address.EntityList = GetClientDLLAddress() + Offset.EntityList;
	this->Address.Matrix = GetClientDLLAddress() + Offset.Matrix;
	this->Address.ViewAngle = GetClientDLLAddress() + Offset.ViewAngle;
	this->Address.LocalController = GetClientDLLAddress() + Offset.LocalPlayerController;
	this->Address.LocalPawn = GetClientDLLAddress() + Offset.LocalPlayerPawn;
	this->Address.ServerPawn = GetServerDLLAddress() + Offset.LocalPlayerPawn;
	this->Address.GlobalVars = GetClientDLLAddress() + Offset.GlobalVars;
	this->Address.JumpBtn = GetClientDLLAddress() + Offset.Buttons.Jump;
	this->Address.AttackBtn = GetClientDLLAddress() + Offset.Buttons.Attack;
	this->Address.RightBtn = GetClientDLLAddress() + Offset.Buttons.Right;
	this->Address.LeftBtn = GetClientDLLAddress() + Offset.Buttons.Left;

	return this->Address.ClientDLL != 0;
}

DWORD64 CGame::GetClientDLLAddress()
{
	return this->Address.ClientDLL;
}

DWORD64 CGame::GetServerDLLAddress()
{
	return this->Address.ServerDLL;
}

DWORD64 CGame::GetEntityListAddress()
{
	return this->Address.EntityList;
}

DWORD64 CGame::GetMatrixAddress()
{
	return this->Address.Matrix;
}

DWORD64 CGame::GetViewAngleAddress() 
{
	return this->Address.ViewAngle;
}

DWORD64 CGame::GetEntityListEntry()
{
	return this->Address.EntityListEntry;
}

DWORD64 CGame::GetLocalControllerAddress()
{
	return this->Address.LocalController;
}

DWORD64 CGame::GetLocalPawnAddress()
{
	return this->Address.LocalPawn;
}

DWORD64 CGame::GetServerPawnAddress()
{
	return this->Address.ServerPawn;
}

DWORD64 CGame::GetGlobalVarsAddress()
{
	return this->Address.GlobalVars;
}

DWORD64 CGame::GetJumpBtnAddress()
{
	return this->Address.JumpBtn;
}

DWORD64 CGame::GetAttackBtnAddress()
{
	return this->Address.AttackBtn;
}

DWORD64 CGame::GetRightBtnAddress()
{
	return this->Address.RightBtn;
}

DWORD64 CGame::GetLeftBtnAddress()
{
	return this->Address.LeftBtn;
}


bool CGame::UpdateEntityListEntry()
{
	DWORD64 EntityListEntry = 0;
	if (!memoryManager.ReadMemory<DWORD64>(gGame.GetEntityListAddress(), EntityListEntry))
		return false;
	if (!memoryManager.ReadMemory<DWORD64>(EntityListEntry + 0x10, EntityListEntry))
		return false;

	this->Address.EntityListEntry = EntityListEntry;

	return this->Address.EntityListEntry != 0;
}

//bool CGame::GetForceJump(int& value)
//{
//	if (!memoryManager.ReadMemory<int>(this->Address.ForceJump, value))
//		return false;
//
//	return true;
//}
//bool CGame::GetForceCrouch(int& value)
//{
//	if (!memoryManager.ReadMemory<int>(this->Address.ForceCrouch, value))
//		return false;
//
//	return true;
//}
//
//bool CGame::GetForceMove(int MovingType, int& Value)
//{
//	switch (MovingType)
//	{
//	case 0:
//		if (!memoryManager.ReadMemory<int>(this->Address.ForceForward, Value)) return false;
//		break;
//	case 1:
//		if (!memoryManager.ReadMemory<int>(this->Address.ForceLeft, Value)) return false;
//		break;
//	case 2:
//		if (!memoryManager.ReadMemory<int>(this->Address.ForceRight, Value)) return false;
//		break;
//	default:
//		return false;
//		break;
//	}
//	return true;
//}