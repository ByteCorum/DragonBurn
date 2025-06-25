#pragma once
#include <vector>
#include <optional>
#include <list>
#include "Game.h"
#include <cmath>

#undef M_PI()

#define M_PI       3.14159265358979323846
// Original bone indices - keeping for compatibility
enum BONEINDEX : DWORD
{
	head = 6,
	neck_0 = 5,
	spine_1 = 4,
	spine_2 = 2,
	pelvis = 0,
	arm_upper_L = 8,
	arm_lower_L = 9,
	hand_L = 10,
	arm_upper_R = 13,
	arm_lower_R = 14,
	hand_R = 15,
	leg_upper_L = 22,
	leg_lower_L = 23,
	ankle_L = 24,
	leg_upper_R = 25,
	leg_lower_R = 26,
	ankle_R = 27,
};



class Quaternion_t
{
public:
	float x, y, z, w;

	// Constructors
	Quaternion_t() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
	Quaternion_t(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	// Copy constructor
	Quaternion_t(const Quaternion_t& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	// Assignment operator
	Quaternion_t& operator=(const Quaternion_t& other) {
		if (this != &other) {
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
		}
		return *this;
	}

	// Identity quaternion (no rotation)
	static Quaternion_t Identity() {
		return Quaternion_t(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Create quaternion from Euler angles (in radians)
	static Quaternion_t FromEuler(float pitch, float yaw, float roll) {
		float cy = cosf(yaw * 0.5f);
		float sy = sinf(yaw * 0.5f);
		float cp = cosf(pitch * 0.5f);
		float sp = sinf(pitch * 0.5f);
		float cr = cosf(roll * 0.5f);
		float sr = sinf(roll * 0.5f);

		return Quaternion_t(
			sr * cp * cy - cr * sp * sy,  // x
			cr * sp * cy + sr * cp * sy,  // y
			cr * cp * sy - sr * sp * cy,  // z
			cr * cp * cy + sr * sp * sy   // w
		);
	}

	// Convert to Euler angles (in radians)
	void ToEuler(float& pitch, float& yaw, float& roll) const {
		// Roll (x-axis rotation)
		float sinr_cosp = 2 * (w * x + y * z);
		float cosr_cosp = 1 - 2 * (x * x + y * y);
		roll = atan2f(sinr_cosp, cosr_cosp);

		// Pitch (y-axis rotation)
		float sinp = 2 * (w * y - z * x);
		if (fabsf(sinp) >= 1)
			pitch = copysignf(M_PI / 2, sinp); // Use 90 degrees if out of range
		else
			pitch = asinf(sinp);

		// Yaw (z-axis rotation)
		float siny_cosp = 2 * (w * z + x * y);
		float cosy_cosp = 1 - 2 * (y * y + z * z);
		yaw = atan2f(siny_cosp, cosy_cosp);
	}

	// Normalize the quaternion
	void Normalize() {
		float length = sqrtf(x * x + y * y + z * z + w * w);
		if (length > 0.0f) {
			x /= length;
			y /= length;
			z /= length;
			w /= length;
		}
	}

	// Get normalized quaternion
	Quaternion_t Normalized() const {
		Quaternion_t result(*this);
		result.Normalize();
		return result;
	}

	// Quaternion multiplication
	Quaternion_t operator*(const Quaternion_t& other) const {
		return Quaternion_t(
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y - x * other.z + y * other.w + z * other.x,
			w * other.z + x * other.y - y * other.x + z * other.w,
			w * other.w - x * other.x - y * other.y - z * other.z
		);
	}

	// Conjugate (inverse for unit quaternions)
	Quaternion_t Conjugate() const {
		return Quaternion_t(-x, -y, -z, w);
	}

	// Dot product
	float Dot(const Quaternion_t& other) const {
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	// Linear interpolation
	static Quaternion_t Lerp(const Quaternion_t& a, const Quaternion_t& b, float t) {
		float dot = a.Dot(b);

		// If dot product is negative, slerp would take the long route.
		// To fix this, we can flip one of the quaternions.
		Quaternion_t b_corrected = (dot < 0.0f) ? Quaternion_t(-b.x, -b.y, -b.z, -b.w) : b;

		return Quaternion_t(
			a.x + t * (b_corrected.x - a.x),
			a.y + t * (b_corrected.y - a.y),
			a.z + t * (b_corrected.z - a.z),
			a.w + t * (b_corrected.w - a.w)
		).Normalized();
	}

	// Check if quaternion is valid (not NaN or infinite)
	bool IsValid() const {
		return !isnan(x) && !isnan(y) && !isnan(z) && !isnan(w) &&
			!isinf(x) && !isinf(y) && !isinf(z) && !isinf(w);
	}

	// Convert to string for debugging
	std::string ToString() const {
		return "Quaternion(" + std::to_string(x) + ", " + std::to_string(y) +
			", " + std::to_string(z) + ", " + std::to_string(w) + ")";
	}
};


// Enhanced bone data structure
struct CBoneData
{
	Vec3 Location;  // Using your Vec3 instead of Vector_t
	float Scale;
	Quaternion_t Rotation;
};

// Original bone joint data - keeping for compatibility
struct BoneJointData
{
	Vec3 Pos;
	float Scale;
	char pad[0x10];
};

// Original bone position data - keeping for compatibility
struct BoneJointPos
{
	Vec3 Pos;
	Vec2 ScreenPos;
	bool IsVisible = false;
};

// Enhanced model state class
class c_model_state {
public:
	std::optional<CBoneData> GetBoneLocation(int index);

	// Get bone position in Vec3 format (compatible with original system)
	std::optional<Vec3> GetBonePosition(int index);
};

// Hitbox classes
class CHitBox {
public:
	CHitBox();
	CHitBox(CHitBox* hitbox);
	std::string Name();
	Vec3 GetStart();
	Vec3 GetEnd();
	float GetRadius();

private:
	char* name;
	char* unk1;
	char* unk2;
	Vec3 Start;
	Vec3 End;
	float Radius;
};

class CHitBoxes {
public:
	uint32_t GetHashName();
	uint32_t GetHitBoxCount();
	CHitBox* GetpHitBox(int index);
	CHitBox GetHitBox(int index);
};

class CRenderMesh {
public:
	CHitBoxes* GetHitboxes(int n);
	int GetHitBoxCount();
};

class CRenderMeshes {
public:
	CRenderMesh* GetRenderMesh(int index);
};

class CModel {
public:
	CRenderMeshes* GetMeshes();
	uint32_t GetMeshesCount();
	CHitBox* GetpHitBox(int index);
};

class c_game_scene_node
{
public:
	// Get bone array using your offset system
	c_model_state* GetBoneArray();

	// Get model
	CModel* GetModel();
};

// Enhanced CBone class - maintains compatibility while adding new features
class CBone
{
private:
	DWORD64 EntityPawnAddress = 0;
	c_game_scene_node* GameSceneNode = nullptr;

public:
	// Original bone position list - keeping for compatibility
	std::vector<BoneJointPos> BonePosList;

	// Enhanced bone data list
	std::vector<CBoneData> EnhancedBoneData;

	// Original function - maintains compatibility
	bool UpdateAllBoneData(const DWORD64& EntityPawnAddress);

	// Enhanced bone data retrieval
	std::optional<CBoneData> GetEnhancedBoneData(int index);

	bool UpdateAllBoneDataBatch(const DWORD64& EntityPawnAddress);

	// Get hitbox data
	CHitBox* GetHitBox(int index);

	// Compatibility function - get bone position by index
	std::optional<Vec3> GetBonePosition(int boneIndex);

	// Compatibility function - get screen position by index
	std::optional<Vec2> GetBoneScreenPosition(int boneIndex);

	// Check if bone is visible on screen
	bool IsBoneVisible(int boneIndex);
};

// Original bone joint lists - keeping for compatibility
namespace BoneJointList
{
	inline std::list<DWORD> Trunk = { neck_0, spine_2, pelvis };
	inline std::list<DWORD> LeftArm = { neck_0, arm_upper_L, arm_lower_L, hand_L };
	inline std::list<DWORD> RightArm = { neck_0, arm_upper_R, arm_lower_R, hand_R };
	inline std::list<DWORD> LeftLeg = { pelvis, leg_upper_L, leg_lower_L, ankle_L };
	inline std::list<DWORD> RightLeg = { pelvis, leg_upper_R, leg_lower_R, ankle_R };
	inline std::vector<std::list<DWORD>> List = { Trunk, LeftArm, RightArm, LeftLeg, RightLeg };
}