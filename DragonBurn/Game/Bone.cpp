#include "Bone.h"

// c_model_state implementations
std::optional<CBoneData> c_model_state::GetBoneLocation(int index)
{
	CBoneData boneData;
	// Read bone data at index * 32 offset
	if (!memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + index * 32, boneData, sizeof(CBoneData)))
		return std::nullopt;
	return boneData;
}

std::optional<Vec3> c_model_state::GetBonePosition(int index)
{
	auto boneData = GetBoneLocation(index);
	if (!boneData.has_value())
		return std::nullopt;
	return boneData->Location;
}

// CHitBox implementations
CHitBox::CHitBox()
{
	memset(this, 0, sizeof(CHitBox));
}

CHitBox::CHitBox(CHitBox* hitbox)
{
	if (hitbox && !memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(hitbox), *this, sizeof(CHitBox))) {
		// Handle read failure
		memset(this, 0, sizeof(CHitBox));
	}
}

std::string CHitBox::Name()
{
	char name[16]{ 0 };
	uintptr_t namePtr = 0;
	if (!memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this), namePtr, sizeof(uintptr_t)))
		return "";
	if (!memoryManager.ReadMemory(namePtr, name, sizeof(name)))
		return "";
	return std::string(name);
}

Vec3 CHitBox::GetStart()
{
	Vec3 start;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x18, start, sizeof(Vec3));
	return start;
}

Vec3 CHitBox::GetEnd()
{
	Vec3 end;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x24, end, sizeof(Vec3));
	return end;
}

float CHitBox::GetRadius()
{
	float radius = 0.0f;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x30, radius, sizeof(float));
	return radius;
}

// CHitBoxes implementations
uint32_t CHitBoxes::GetHashName()
{
	uint32_t hashName = 0;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x20, hashName, sizeof(uint32_t));
	return hashName;
}

uint32_t CHitBoxes::GetHitBoxCount()
{
	uint32_t count = 0;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x28, count, sizeof(uint32_t));
	return count;
}

CHitBox* CHitBoxes::GetpHitBox(int index)
{
	uintptr_t hitboxArrayPtr = 0;
	if (!memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x30, hitboxArrayPtr, sizeof(uintptr_t)))
		return nullptr;
	return reinterpret_cast<CHitBox*>(hitboxArrayPtr + 0x70 * index);
}

CHitBox CHitBoxes::GetHitBox(int index)
{
	CHitBox hitbox;
	auto hitboxPtr = GetpHitBox(index);
	if (hitboxPtr) {
		memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(hitboxPtr), hitbox, sizeof(CHitBox));
	}
	return hitbox;
}

// CRenderMesh implementations
CHitBoxes* CRenderMesh::GetHitboxes(int n)
{
	uintptr_t hitboxesPtr = 0;
	if (!memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x108, hitboxesPtr, sizeof(uintptr_t)))
		return nullptr;
	return reinterpret_cast<CHitBoxes*>(hitboxesPtr + 0x50 * n);
}

int CRenderMesh::GetHitBoxCount()
{
	int count = 0;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x110, count, sizeof(int));
	return count;
}

// CRenderMeshes implementations
CRenderMesh* CRenderMeshes::GetRenderMesh(int index)
{
	CRenderMesh* mesh = nullptr;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x8 * index, mesh, sizeof(CRenderMesh*));
	return mesh;
}

// CModel implementations
CRenderMeshes* CModel::GetMeshes()
{
	CRenderMeshes* meshes = nullptr;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x78, meshes, sizeof(CRenderMeshes*));
	return meshes;
}

uint32_t CModel::GetMeshesCount()
{
	uint32_t count = 0;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + 0x70, count, sizeof(uint32_t));
	return count;
}

CHitBox* CModel::GetpHitBox(int index)
{
	if (GetMeshesCount() <= 0)
		return nullptr;

	auto meshes = GetMeshes();
	if (!meshes)
		return nullptr;

	auto mesh = meshes->GetRenderMesh(0);
	if (!mesh)
		return nullptr;

	auto hitboxSet = mesh->GetHitboxes(0);
	if (!hitboxSet || mesh->GetHitBoxCount() <= 0 || index >= hitboxSet->GetHitBoxCount())
		return nullptr;

	return hitboxSet->GetpHitBox(index);
}

// c_game_scene_node implementations
c_model_state* c_game_scene_node::GetBoneArray()
{
	c_model_state* boneArray = nullptr;
	memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + Offset.Pawn.BoneArray, boneArray, sizeof(c_model_state*));
	return boneArray;
}

CModel* c_game_scene_node::GetModel()
{
	uintptr_t modelStatePtr = 0;
	uintptr_t modelPtr = 0;

	// Read model state pointer
	if (!memoryManager.ReadMemory(reinterpret_cast<uintptr_t>(this) + Offset.Pawn.BoneArray, modelStatePtr, sizeof(uintptr_t)))
		return nullptr;

	// Read model pointer from model state + 0xA0
	if (!memoryManager.ReadMemory(modelStatePtr + 0xA0, modelPtr, sizeof(uintptr_t)))
		return nullptr;

	return reinterpret_cast<CModel*>(modelPtr);
}

// Original function implementation - maintains full compatibility
bool CBone::UpdateAllBoneData(const DWORD64& EntityPawnAddress) {
	if (EntityPawnAddress == 0)
		return false;

	this->EntityPawnAddress = EntityPawnAddress;

	DWORD64 GameSceneNodeAddr = 0;
	DWORD64 BoneArrayAddress = 0;

	if (!memoryManager.ReadMemory<DWORD64>(EntityPawnAddress + Offset.Pawn.GameSceneNode, GameSceneNodeAddr))
		return false;

	// Store the game scene node for enhanced functionality
	this->GameSceneNode = reinterpret_cast<c_game_scene_node*>(GameSceneNodeAddr);

	if (!memoryManager.ReadMemory<DWORD64>(GameSceneNodeAddr + Offset.Pawn.BoneArray, BoneArrayAddress))
		return false;

	constexpr size_t NUM_BONES = 30;
	BoneJointData BoneArray[NUM_BONES]{};
	if (!memoryManager.ReadMemory(BoneArrayAddress, BoneArray, NUM_BONES * sizeof(BoneJointData)))
		return false;

	// Clear both lists
	BonePosList.clear();
	EnhancedBoneData.clear();

	// Populate both original and enhanced bone data
	for (const auto& bone : BoneArray) {
		Vec2 ScreenPos;
		bool IsVisible = false;

		if (gGame.View.WorldToScreen(bone.Pos, ScreenPos))
			IsVisible = true;

		// Original bone position data
		this->BonePosList.push_back({ bone.Pos, ScreenPos, IsVisible });

		// Enhanced bone data
		CBoneData enhancedBone;
		enhancedBone.Location = bone.Pos;
		enhancedBone.Scale = bone.Scale;
		this->EnhancedBoneData.push_back(enhancedBone);
	}

	return !BonePosList.empty();
}

// Enhanced bone data retrieval
std::optional<CBoneData> CBone::GetEnhancedBoneData(int index) {
	if (!GameSceneNode)
		return std::nullopt;

	auto modelState = GameSceneNode->GetBoneArray();
	if (!modelState)
		return std::nullopt;

	return modelState->GetBoneLocation(index);
}


bool CBone::UpdateAllBoneDataBatch(const DWORD64& EntityPawnAddress) {
	if (EntityPawnAddress == 0)
		return false;

	this->EntityPawnAddress = EntityPawnAddress;

	// First, get the dependent addresses with individual reads
	DWORD64 GameSceneNodeAddr = 0;
	DWORD64 BoneArrayAddress = 0;

	if (!memoryManager.ReadMemory<DWORD64>(EntityPawnAddress + Offset.Pawn.GameSceneNode, GameSceneNodeAddr))
		return false;

	this->GameSceneNode = reinterpret_cast<c_game_scene_node*>(GameSceneNodeAddr);

	if (!memoryManager.ReadMemory<DWORD64>(GameSceneNodeAddr + Offset.Pawn.BoneArray, BoneArrayAddress))
		return false;

	// Prepare batch read requests for all 30 bones
	constexpr size_t NUM_BONES = 30;
	std::vector<std::pair<DWORD64, SIZE_T>> requests;
	requests.reserve(NUM_BONES);

	// Create requests for each bone (each bone is 32 bytes apart)
	for (size_t i = 0; i < NUM_BONES; ++i) {
		requests.push_back({ BoneArrayAddress + (i * 32), sizeof(BoneJointData) });
	}

	// Calculate total buffer size
	SIZE_T total_size = NUM_BONES * sizeof(BoneJointData);
	std::vector<BYTE> batch_buffer(total_size);

	// Perform batch read
	if (!memoryManager.BatchReadMemory(requests, batch_buffer.data())) {
		return false;
	}

	// Clear both lists
	BonePosList.clear();
	EnhancedBoneData.clear();
	BonePosList.reserve(NUM_BONES);
	EnhancedBoneData.reserve(NUM_BONES);

	// Extract bone data from buffer
	SIZE_T offset = 0;
	for (size_t i = 0; i < NUM_BONES; ++i) {
		BoneJointData bone;
		memcpy(&bone, batch_buffer.data() + offset, sizeof(BoneJointData));
		offset += sizeof(BoneJointData);

		Vec2 ScreenPos;
		bool IsVisible = false;

		if (gGame.View.WorldToScreen(bone.Pos, ScreenPos))
			IsVisible = true;

		// Original bone position data
		this->BonePosList.push_back({ bone.Pos, ScreenPos, IsVisible });

		// Enhanced bone data
		CBoneData enhancedBone;
		enhancedBone.Location = bone.Pos;
		enhancedBone.Scale = bone.Scale;
		this->EnhancedBoneData.push_back(enhancedBone);
	}

	return !BonePosList.empty();
}

// Get hitbox data
CHitBox* CBone::GetHitBox(int index) {
	if (!GameSceneNode)
		return nullptr;

	auto model = GameSceneNode->GetModel();
	if (!model)
		return nullptr;

	return model->GetpHitBox(index);
}

// Compatibility function - get bone position by index
std::optional<Vec3> CBone::GetBonePosition(int boneIndex) {
	if (boneIndex < 0 || boneIndex >= static_cast<int>(BonePosList.size()))
		return std::nullopt;
	return BonePosList[boneIndex].Pos;
}

// Compatibility function - get screen position by index
std::optional<Vec2> CBone::GetBoneScreenPosition(int boneIndex) {
	if (boneIndex < 0 || boneIndex >= static_cast<int>(BonePosList.size()))
		return std::nullopt;
	return BonePosList[boneIndex].ScreenPos;
}

// Check if bone is visible on screen
bool CBone::IsBoneVisible(int boneIndex) {
	if (boneIndex < 0 || boneIndex >= static_cast<int>(BonePosList.size()))
		return false;
	return BonePosList[boneIndex].IsVisible;
}