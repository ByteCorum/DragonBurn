#include "Entity.h"


// Optimized weapon names lookup using unordered_map for O(1) average case
std::unordered_map<int, std::string> CEntity::weaponNames = {
	{1, "deagle"},
	{2, "elite"},
	{3, "fiveseven"},
	{4, "glock"},
	{7, "ak47"},
	{8, "aug"},
	{9, "awp"},
	{10, "famas"},
	{11, "g3Sg1"},
	{13, "galilar"},
	{14, "m249"},
	{16, "m4a1"},
	{17, "mac10"},
	{19, "p90"},
	{23, "mp5sd"},
	{24, "ump45"},
	{25, "xm1014"},
	{26, "bizon"},
	{27, "mag7"},
	{28, "negev"},
	{29, "sawedoff"},
	{30, "tec9"},
	{31, "zeus"},
	{32, "p2000"},
	{33, "mp7"},
	{34, "mp9"},
	{35, "nova"},
	{36, "p250"},
	{38, "scar20"},
	{39, "sg556"},
	{40, "ssg08"},
	{42, "ct_knife"},
	{43, "flashbang"},
	{44, "hegrenade"},
	{45, "smokegrenade"},
	{46, "molotov"},
	{47, "decoy"},
	{48, "incgrenade"},
	{49, "c4"},
	{59, "t_knife"},
	{60, "m4a1_silencer"},
	{61, "usp"},
	{63, "cz75a"},
	{64, "revolver"}
};

bool CEntity::UpdateController(const DWORD64& PlayerControllerAddress)
{
	if (PlayerControllerAddress == 0)
		return false;
	this->Controller.Address = PlayerControllerAddress;

	if (!this->Controller.GetHealth())
		return false;
	if (!this->Controller.GetIsAlive())
		return false;
	if (!this->Controller.GetTeamID())
		return false;
	if (!this->Controller.GetPlayerName())
		return false;
	if (!this->Controller.GetPlayerSteamID())
		return false;

	this->Pawn.Address = this->Controller.GetPlayerPawnAddress();

	return true;
}

bool CEntity::UpdatePawn(const DWORD64& PlayerPawnAddress)
{
	if (PlayerPawnAddress == 0)
		return false;
	this->Pawn.Address = PlayerPawnAddress;

	if (!this->Pawn.GetCameraPos())
		return false;
	if (!this->Pawn.GetPos())
		return false;
	if (!this->Pawn.GetViewAngle())
		return false;
	if (!this->Pawn.GetWeaponName())
		return false;
	if (!this->Pawn.GetAimPunchAngle())
		return false;
	if (!this->Pawn.GetShotsFired())
		return false;
	if (!this->Pawn.GetHealth())
		return false;
	if (!this->Pawn.GetAmmo())
		return false;
	if (!this->Pawn.GetMaxAmmo())
		return false;
	if (!this->Pawn.GetArmor())
		return false;
	if (!this->Pawn.GetTeamID())
		return false;
	if (!this->Pawn.GetFov())
		return false;
	if (!this->Pawn.GetSpotted())
		return false;
	if (!this->Pawn.GetFFlags())
		return false;
	if (!this->Pawn.GetDefusing())
		return false;
	if (!this->Pawn.GetFlashDuration())
		return false;
	if (!this->Pawn.GetVelocity())
		return false;
	if (!this->Pawn.GetAimPunchCache())
		return false;
	if (!this->Pawn.BoneData.UpdateAllBoneData(PlayerPawnAddress))
		return false;

	return true;
}

bool CEntity::UpdateClientData()
{
	if (!this->Client.GetSensitivity())
		return false;

	return true;
}

bool PlayerController::GetTeamID()
{
	return GetDataAddressWithOffset<int>(Address, Offset.Pawn.iTeamNum, this->TeamID);
}

bool PlayerController::GetHealth()
{
	return GetDataAddressWithOffset<int>(Address, Offset.Pawn.CurrentHealth, this->Health);
}

bool PlayerController::GetIsAlive()
{
	return GetDataAddressWithOffset<int>(Address, Offset.Entity.IsAlive, this->AliveStatus);
}

bool PlayerController::GetPlayerName()
{
	char Buffer[MAX_PATH]{};

	if (!memoryManager.ReadMemory(Address + Offset.Entity.iszPlayerName, Buffer, MAX_PATH))
		return false;

	//if (!this->SteamID)
	//	this->PlayerName = "BOT " + std::string(Buffer);
	//else
	this->PlayerName = Buffer;

	if (this->PlayerName.empty())
		this->PlayerName = "Name_None";

	return true;
}

bool PlayerController::GetPlayerSteamID()
{
	return GetDataAddressWithOffset<INT64>(Address, Offset.PlayerController.m_steamID, this->SteamID);
}

bool PlayerPawn::GetViewAngle()
{
	return GetDataAddressWithOffset<Vec2>(Address, Offset.Pawn.angEyeAngles, this->ViewAngle);
}

bool PlayerPawn::GetCameraPos()
{
	return GetDataAddressWithOffset<Vec3>(Address, Offset.Pawn.vecLastClipCameraPos, this->CameraPos);
}

bool PlayerPawn::GetSpotted()
{
	return GetDataAddressWithOffset<DWORD64>(Address, Offset.Pawn.bSpottedByMask, this->bSpottedByMask);
}



bool PlayerPawn::GetWeaponName()
{
	// Single memory read to get the weapon pointer
	DWORD64 CurrentWeapon;
	if (!memoryManager.ReadMemory(this->Address + Offset.Pawn.pClippingWeapon, CurrentWeapon) || CurrentWeapon == 0)
		return false;

	// Calculate the final address for weapon index directly
	DWORD64 weaponIndexAddress = CurrentWeapon + Offset.EconEntity.AttributeManager +
		Offset.WeaponBaseData.Item + Offset.WeaponBaseData.ItemDefinitionIndex;

	// Single memory read to get weapon index
	short weaponIndex;
	if (!memoryManager.ReadMemory(weaponIndexAddress, weaponIndex) || weaponIndex == -1)
		return false;

	// Inline weapon name lookup
	static const std::string defaultWeapon = "Weapon_None";
	auto it = CEntity::weaponNames.find(weaponIndex);
	WeaponName = (it != CEntity::weaponNames.end()) ? it->second : defaultWeapon;

	return true;
}

bool PlayerPawn::GetShotsFired()
{
	return GetDataAddressWithOffset<DWORD>(Address, Offset.Pawn.iShotsFired, this->ShotsFired);
}

bool PlayerPawn::GetAimPunchAngle()
{
	return GetDataAddressWithOffset<Vec2>(Address, Offset.Pawn.aimPunchAngle, this->AimPunchAngle);
}

bool PlayerPawn::GetTeamID()
{
	return GetDataAddressWithOffset<int>(Address, Offset.Pawn.iTeamNum, this->TeamID);
}

bool PlayerPawn::GetAimPunchCache()
{
	return GetDataAddressWithOffset<C_UTL_VECTOR>(Address, Offset.Pawn.aimPunchCache, this->AimPunchCache);
}

DWORD64 PlayerController::GetPlayerPawnAddress()
{
	DWORD64 EntityPawnListEntry = 0;
	DWORD64 EntityPawnAddress = 0;

	if (!GetDataAddressWithOffset<DWORD>(Address, Offset.Entity.PlayerPawn, this->Pawn))
		return 0;

	if (!memoryManager.ReadMemory<DWORD64>(gGame.GetEntityListAddress(), EntityPawnListEntry))
		return 0;

	if (!memoryManager.ReadMemory<DWORD64>(EntityPawnListEntry + 0x10 + 8 * ((Pawn & 0x7FFF) >> 9), EntityPawnListEntry))
		return 0;

	if (!memoryManager.ReadMemory<DWORD64>(EntityPawnListEntry + 0x78 * (Pawn & 0x1FF), EntityPawnAddress))
		return 0;

	return EntityPawnAddress;
}

bool PlayerPawn::GetPos()
{
	return GetDataAddressWithOffset<Vec3>(Address, Offset.Pawn.Pos, this->Pos);
}

bool PlayerPawn::GetHealth()
{
	return GetDataAddressWithOffset<int>(Address, Offset.Pawn.CurrentHealth, this->Health);
}

bool PlayerPawn::GetArmor()
{
	return GetDataAddressWithOffset<int>(Address, Offset.Pawn.CurrentArmor, this->Armor);
}

bool PlayerPawn::GetAmmo()
{
	DWORD64 ClippingWeapon = 0;
	if (!memoryManager.ReadMemory<DWORD64>(Address + Offset.Pawn.pClippingWeapon, ClippingWeapon))
		return false;

	return GetDataAddressWithOffset<int>(ClippingWeapon, Offset.WeaponBaseData.Clip1, this->Ammo);
}

bool PlayerPawn::GetMaxAmmo()
{
	DWORD64 ClippingWeapon = 0;
	DWORD64 WeaponData = 0;
	if (!memoryManager.ReadMemory<DWORD64>(Address + Offset.Pawn.pClippingWeapon, ClippingWeapon))
		return false;
	if (!memoryManager.ReadMemory<DWORD64>(ClippingWeapon + Offset.WeaponBaseData.WeaponDataPTR, WeaponData))
		return false;

	return GetDataAddressWithOffset<int>(WeaponData, Offset.WeaponBaseData.MaxClip, this->MaxAmmo);
}

bool PlayerPawn::GetFov()
{
	DWORD64 CameraServices = 0;
	if (!memoryManager.ReadMemory<DWORD64>(Address + Offset.Pawn.CameraServices, CameraServices))
		return false;
	return GetDataAddressWithOffset<int>(CameraServices, Offset.Pawn.iFovStart, this->Fov);
}

bool PlayerPawn::GetFFlags()
{
	return GetDataAddressWithOffset<int>(Address, Offset.Pawn.fFlags, this->fFlags);
}

bool PlayerPawn::GetDefusing()
{
	return memoryManager.ReadMemory(Address + Offset.C4.m_bBeingDefused, this->isDefusing);
}

bool PlayerPawn::GetFlashDuration()
{
	return memoryManager.ReadMemory(Address + Offset.Pawn.flFlashDuration, this->FlashDuration);
}

bool PlayerPawn::GetVelocity()
{
	Vec3 Velocity;
	if (!memoryManager.ReadMemory(Address + Offset.Pawn.AbsVelocity, Velocity))
		return false;
	this->Speed = sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y);
	return true;
}

bool CEntity::IsAlive() const
{
	return this->Controller.AliveStatus == 1 && this->Pawn.Health > 0;
}

bool CEntity::IsInScreen()
{
	return gGame.View.WorldToScreen(this->Pawn.Pos, this->Pawn.ScreenPos);
}

CBone CEntity::GetBone() const
{
	if (this->Pawn.Address == 0)
		return CBone{};
	return this->Pawn.BoneData;
}

bool Client::GetSensitivity()
{
	DWORD64 dwSensitivity;
	float flSensitivity;
	memoryManager.ReadMemory(gGame.GetClientDLLAddress() + Offset.Sensitivity, dwSensitivity);
	if (memoryManager.ReadMemory(dwSensitivity + 0x40, flSensitivity))
	{
		this->Sensitivity = flSensitivity;
		return true;
	}
	else
		return false;
}


bool CEntity::UpdateControllerBatch(const DWORD64& PlayerControllerAddress) {
	if (PlayerControllerAddress == 0) return false;

	this->Controller.Address = PlayerControllerAddress;

	// Prepare batch read requests in specific order
	std::vector<std::pair<DWORD64, SIZE_T>> requests = {
		{PlayerControllerAddress + Offset.Pawn.CurrentHealth, sizeof(int)},           // 0
		{PlayerControllerAddress + Offset.Entity.IsAlive, sizeof(int)},              // 1
		{PlayerControllerAddress + Offset.Pawn.iTeamNum, sizeof(int)},               // 2
		{PlayerControllerAddress + Offset.Entity.iszPlayerName, MAX_PATH},           // 3
		{PlayerControllerAddress + Offset.PlayerController.m_steamID, sizeof(INT64)}, // 4
		{PlayerControllerAddress + Offset.Entity.PlayerPawn, sizeof(DWORD)}          // 5
	};

	// Calculate total size needed
	SIZE_T total_size = 0;
	for (const auto& req : requests) {
		total_size += req.second;
	}

	// Allocate buffer for all data
	std::vector<BYTE> batch_buffer(total_size);

	if (!memoryManager.BatchReadMemory(requests, batch_buffer.data())) {
		return false;
	}

	// Extract data in the same order as requests
	SIZE_T offset = 0;

	// Health (index 0)
	memcpy(&this->Controller.Health, batch_buffer.data() + offset, sizeof(int));
	offset += sizeof(int);

	// Alive status (index 1)
	memcpy(&this->Controller.AliveStatus, batch_buffer.data() + offset, sizeof(int));
	offset += sizeof(int);

	// Team ID (index 2)
	memcpy(&this->Controller.TeamID, batch_buffer.data() + offset, sizeof(int));
	offset += sizeof(int);

	// Player name (index 3)
	char temp_name[MAX_PATH] = { 0 };
	memcpy(temp_name, batch_buffer.data() + offset, MAX_PATH);
	this->Controller.PlayerName = temp_name;
	if (this->Controller.PlayerName.empty()) {
		this->Controller.PlayerName = "Name_None";
	}
	offset += MAX_PATH;

	// Steam ID (index 4)
	memcpy(&this->Controller.SteamID, batch_buffer.data() + offset, sizeof(INT64));
	offset += sizeof(INT64);

	// Pawn address (index 5)
	memcpy(&this->Controller.Pawn, batch_buffer.data() + offset, sizeof(DWORD));

	// Get pawn address
	this->Pawn.Address = this->Controller.GetPlayerPawnAddress();

	return true;
}


bool CEntity::UpdatePawnBatch(const DWORD64& PlayerPawnAddress) {
	if (PlayerPawnAddress == 0) return false;

	this->Pawn.Address = PlayerPawnAddress;

	// Get dependent addresses first (these need individual reads)
	DWORD64 weaponAddress = 0;
	DWORD64 cameraServices = 0;

	// Read these separately as they're needed to determine other reads
	memoryManager.ReadMemory<DWORD64>(PlayerPawnAddress + Offset.Pawn.pClippingWeapon, weaponAddress);
	memoryManager.ReadMemory<DWORD64>(PlayerPawnAddress + Offset.Pawn.CameraServices, cameraServices);

	// Build batch requests in specific order
	std::vector<std::pair<DWORD64, SIZE_T>> requests = {
		// Index 0-12: Core pawn data
		{PlayerPawnAddress + Offset.Pawn.angEyeAngles, sizeof(Vec2)},          // 0
		{PlayerPawnAddress + Offset.Pawn.vecLastClipCameraPos, sizeof(Vec3)},  // 1
		{PlayerPawnAddress + Offset.Pawn.Pos, sizeof(Vec3)},                   // 2
		{PlayerPawnAddress + Offset.Pawn.bSpottedByMask, sizeof(DWORD64)},     // 3
		{PlayerPawnAddress + Offset.Pawn.iShotsFired, sizeof(DWORD)},          // 4
		{PlayerPawnAddress + Offset.Pawn.aimPunchAngle, sizeof(Vec2)},         // 5
		{PlayerPawnAddress + Offset.Pawn.iTeamNum, sizeof(int)},               // 6
		{PlayerPawnAddress + Offset.Pawn.CurrentHealth, sizeof(int)},          // 7
		{PlayerPawnAddress + Offset.Pawn.CurrentArmor, sizeof(int)},           // 8
		{PlayerPawnAddress + Offset.Pawn.flFlashDuration, sizeof(float)},      // 9
		{PlayerPawnAddress + Offset.Pawn.AbsVelocity, sizeof(Vec3)},           // 10
		{PlayerPawnAddress + Offset.Pawn.fFlags, sizeof(int)},                 // 11
		{PlayerPawnAddress + Offset.C4.m_bBeingDefused, sizeof(bool)}          // 12
	};

	// Add conditional reads
	bool has_weapon = (weaponAddress != 0);
	bool has_camera = (cameraServices != 0);

	if (has_weapon) {
		requests.push_back({ weaponAddress + Offset.WeaponBaseData.Clip1, sizeof(int) }); // 13
	}

	if (has_camera) {
		requests.push_back({ cameraServices + Offset.Pawn.iFovStart, sizeof(int) }); // 14 (or 13 if no weapon)
	}

	// Calculate buffer size and perform batch read
	SIZE_T total_size = 0;
	for (const auto& req : requests) {
		total_size += req.second;
	}

	std::vector<BYTE> batch_buffer(total_size);

	if (!memoryManager.BatchReadMemory(requests, batch_buffer.data())) {
		return false;
	}

	// Extract data in request order
	SIZE_T offset = 0;

	// Core data (indices 0-12)
	memcpy(&this->Pawn.ViewAngle, batch_buffer.data() + offset, sizeof(Vec2));
	offset += sizeof(Vec2);

	memcpy(&this->Pawn.CameraPos, batch_buffer.data() + offset, sizeof(Vec3));
	offset += sizeof(Vec3);

	memcpy(&this->Pawn.Pos, batch_buffer.data() + offset, sizeof(Vec3));
	offset += sizeof(Vec3);

	memcpy(&this->Pawn.bSpottedByMask, batch_buffer.data() + offset, sizeof(DWORD64));
	offset += sizeof(DWORD64);

	memcpy(&this->Pawn.ShotsFired, batch_buffer.data() + offset, sizeof(DWORD));
	offset += sizeof(DWORD);

	memcpy(&this->Pawn.AimPunchAngle, batch_buffer.data() + offset, sizeof(Vec2));
	offset += sizeof(Vec2);

	memcpy(&this->Pawn.TeamID, batch_buffer.data() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&this->Pawn.Health, batch_buffer.data() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&this->Pawn.Armor, batch_buffer.data() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&this->Pawn.FlashDuration, batch_buffer.data() + offset, sizeof(float));
	offset += sizeof(float);

	Vec3 velocity;
	memcpy(&velocity, batch_buffer.data() + offset, sizeof(Vec3));
	this->Pawn.Speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	offset += sizeof(Vec3);

	memcpy(&this->Pawn.fFlags, batch_buffer.data() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&this->Pawn.isDefusing, batch_buffer.data() + offset, sizeof(bool));
	offset += sizeof(bool);

	// Handle conditional data
	if (has_weapon) {
		memcpy(&this->Pawn.Ammo, batch_buffer.data() + offset, sizeof(int));
		offset += sizeof(int);
	}
	else {
		this->Pawn.Ammo = 0; // Default value
	}

	if (has_camera) {
		memcpy(&this->Pawn.Fov, batch_buffer.data() + offset, sizeof(int));
	}
	else {
		this->Pawn.Fov = 90; // Default FOV
	}

	// These still need individual processing
	this->Pawn.GetWeaponName();
	this->Pawn.BoneData.UpdateAllBoneDataBatch(PlayerPawnAddress);

	return true;
}