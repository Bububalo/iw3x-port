#pragma once

namespace Components
{
	class MapDumper : public Component
	{
	public:
		const char* GetName() override { return "MapDumper"; };
		static std::string GetMapName();
		static std::string GetZoneName();
		static int GetZoneIndex();
		static iw4of::api* GetApi();

		static bool ShouldExtendCulling();

		MapDumper();
		~MapDumper();

	private:
		static void DumpMap(std::string  mapName);
		static void DumpLoadedGSCs(std::string mapName, bool isSingleplayer);
		static void CreateArenaFile(std::string  mapName, bool isSingleplayer);

		static std::string APIFileRead(const std::string& filename);

		iw4of::params_t GetParams();

		static std::string mapName;
		static std::string zoneName;
		static int zoneIndex;
		static void DumpZone(std::string zoneName);
		static iw4of::api* api;
	};
}
