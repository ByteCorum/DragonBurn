#include "Bone.h"

bool CBone::UpdateAllBoneData(const DWORD64& EntityPawnAddress) {
    if (!EntityPawnAddress) return false;
    this->EntityPawnAddress = EntityPawnAddress;

    uintptr_t gameSceneNodeAddr;
    if (!memoryManager.ReadMemory(EntityPawnAddress + Offset.Pawn.GameSceneNode, gameSceneNodeAddr))
        return false;

    this->GameSceneNode = gameSceneNodeAddr;

    uintptr_t boneArrayAddr;
    if (!memoryManager.ReadMemory(gameSceneNodeAddr + Offset.Pawn.BoneArray, boneArrayAddr))
        return false;

    constexpr size_t NUM_BONES = 30;

    // Read bone data (position + rotation)
    CBoneData IData[NUM_BONES];
    if (!memoryManager.ReadMemory(boneArrayAddr, IData, NUM_BONES * sizeof(CBoneData)))
        return false;

    // Read original bone data (position only)
    BoneJointData originalData[NUM_BONES];
    if (!memoryManager.ReadMemory(boneArrayAddr, originalData, NUM_BONES * sizeof(BoneJointData)))
        return false;

    // Clear both lists
    BonePosList.clear();
    IBoneData.clear();
    BonePosList.reserve(NUM_BONES);
    IBoneData.reserve(NUM_BONES);

    // Populate both data structures
    for (size_t i = 0; i < NUM_BONES; ++i) {
        // Get screen position
        Vec2 screenPos;
        bool visible = gGame.View.WorldToScreen(originalData[i].Pos, screenPos);

        // Store original bone data (for compatibility)
        BonePosList.push_back({ originalData[i].Pos, screenPos, visible });

        // Store bone data
        IBoneData.push_back({
            originalData[i].Pos,  // Location
            originalData[i].Scale,  // Scale
            IData[i].Rotation  // Rotation
            });
    }
    return true;
}

/*
std::optional<CBoneData> CBone::GetBoneData(int index) const {
    if (index < 0 || index >= IBoneData.size()) {
        // Log::Debug("[DEBUG] FAIL GetBoneData: Index out of bounds");
        return std::nullopt;
    }
    return IBoneData[index];
}
*/

bool CBone::UpdateAllBoneDataBatch(const DWORD64& EntityPawnAddress) {
	if (EntityPawnAddress == 0)
		return false;

	this->EntityPawnAddress = EntityPawnAddress;

	// BATCH READ 1: Get dependent addresses
	std::vector<std::pair<DWORD64, SIZE_T>> batch1Requests = {
		{EntityPawnAddress + Offset.Pawn.GameSceneNode, sizeof(DWORD64)},  // GameSceneNodeAddr
	};

	std::vector<BYTE> batch1Buffer(sizeof(DWORD64));

	if (!memoryManager.BatchReadMemory(batch1Requests, batch1Buffer.data())) {
		return false;
	}

	// Extract GameSceneNode address
	DWORD64 GameSceneNodeAddr;
	memcpy(&GameSceneNodeAddr, batch1Buffer.data(), sizeof(DWORD64));

	if (GameSceneNodeAddr == 0) return false;
	this->GameSceneNode = GameSceneNodeAddr;

	// BATCH READ 2: Get BoneArray address
	std::vector<std::pair<DWORD64, SIZE_T>> batch2Requests = {
		{GameSceneNodeAddr + Offset.Pawn.BoneArray, sizeof(DWORD64)}  // BoneArrayAddress
	};

	std::vector<BYTE> batch2Buffer(sizeof(DWORD64));

	if (!memoryManager.BatchReadMemory(batch2Requests, batch2Buffer.data())) {
		return false;
	}

	// Extract BoneArray address
	DWORD64 BoneArrayAddress;
	memcpy(&BoneArrayAddress, batch2Buffer.data(), sizeof(DWORD64));

	if (BoneArrayAddress == 0) return false;

	// BATCH READ 3: Read all bone data at once
	constexpr size_t NUM_BONES = 30;
	std::vector<std::pair<DWORD64, SIZE_T>> batch3Requests;
	batch3Requests.reserve(NUM_BONES);

	// Create requests for each bone (each bone is 32 bytes apart)
	for (size_t i = 0; i < NUM_BONES; ++i) {
		batch3Requests.push_back({ BoneArrayAddress + (i * 32), sizeof(BoneJointData) });
	}

	// Calculate total buffer size
	SIZE_T total_size = NUM_BONES * sizeof(BoneJointData);
	std::vector<BYTE> batch3Buffer(total_size);

	// Perform batch read for all bones
	if (!memoryManager.BatchReadMemory(batch3Requests, batch3Buffer.data())) {
		return false;
	}

	// Clear both lists
	BonePosList.clear();
	IBoneData.clear();
	BonePosList.reserve(NUM_BONES);
	IBoneData.reserve(NUM_BONES);

	// Extract bone data from buffer
	SIZE_T offset = 0;
	for (size_t i = 0; i < NUM_BONES; ++i) {
		BoneJointData bone;
		memcpy(&bone, batch3Buffer.data() + offset, sizeof(BoneJointData));
		offset += sizeof(BoneJointData);

		Vec2 ScreenPos;
		bool IsVisible = false;
		if (gGame.View.WorldToScreen(bone.Pos, ScreenPos))
			IsVisible = true;

		// Original bone position data
		this->BonePosList.push_back({ bone.Pos, ScreenPos, IsVisible });

		// bone data
		CBoneData IBone;
		IBone.Location = bone.Pos;
		IBone.Scale = bone.Scale;
		this->IBoneData.push_back(IBone);
	}

	return !BonePosList.empty();
}