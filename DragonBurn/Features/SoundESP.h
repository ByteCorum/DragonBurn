#pragma once
#include "../Game/Entity.h"
#include "../OS-ImGui/OS-ImGui_Struct.h"
#include <vector>

namespace SoundSystem {
	struct sound_info_t {
		Vec3 player_origin;
        bool has_changed;
        double spawn_time; // time when this sound was emitted (for ripple animation)
        int entity_index;  // -1 for local, otherwise entity index
        DWORD64 controller_address; // owner identity to avoid index reuse artifacts
        DWORD64 pawn_address;
        INT64 steam_id;
	};

	class sound_info {
	public:
		void push_sound( const CEntity& entity, int idx, const CEntity& localEntity );
		// Remove stored ripples for entities not present in the provided list
		void prune_by_alive_indices(const std::vector<int>& alive_indices);

		std::vector<sound_info_t> s_info;
		std::vector<sound_info_t> s_info_l;
	};

	inline const auto _sound = std::make_unique< sound_info >( );
}