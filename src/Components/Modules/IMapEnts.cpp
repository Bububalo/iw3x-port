#include "STDInclude.hpp"

namespace Components
{
	Game::IW4::MapEnts* IMapEnts::Convert(Game::IW3::MapEnts* ents)
	{
		if (!ents) return nullptr;
		std::string basename(ents->name);

		Utils::EraseSubstring(basename, "maps/");
		Utils::EraseSubstring(basename, "mp/");
		Utils::EraseSubstring(basename, ".d3dbsp");

		std::string entString(ents->entityString, ents->numEntityChars - 1);

		Utils::Entities mapEnts(entString);

		static const auto addCarePackages = Game::Dvar_FindVar("iw3x_add_care_packages");
		if (addCarePackages && addCarePackages->current.string == "1"s) {
			mapEnts.AddCarePackages();
		}

		mapEnts.DeleteOldSchoolPickups();
		mapEnts.AddRemovedSModels();


		static const auto replaceTurretsWithMiniguns = Game::Dvar_FindVar("iw3x_replace_turrets_with_miniguns");
		if (replaceTurretsWithMiniguns && replaceTurretsWithMiniguns->current.string == "1"s) {
			if (mapEnts.ConvertTurrets())
			{
				// The map features turrets! We need to write a file somewhere to inform the converter about it so that the proper iw4 minigun source will be included
				Utils::WriteFile(Utils::VA("%s/HAS_MINIGUN", AssetHandler::GetExportPath().data()), "\0");
			}
		}
		else
		{
			mapEnts.RenameWeapons();
		}

		bool hasVehicles = mapEnts.ConvertVehicles();

		entString = mapEnts.Build();

		Game::IW4::MapEnts* iw4Ents = LocalAllocator.Allocate<Game::IW4::MapEnts>();

		iw4Ents->name = ents->name;
		iw4Ents->entityString = LocalAllocator.DuplicateString(entString);
		iw4Ents->numEntityChars = entString.size() + 1;
		iw4Ents->stages = nullptr;


		return iw4Ents;
	}

	IMapEnts::IMapEnts()
	{
		Command::Add("dumpMapEnts", [](const Command::Params& params)
			{
				if (params.Length() < 2) return;

				auto converted = IMapEnts::Convert(Game::DB_FindXAssetHeader(Game::IW3::XAssetType::ASSET_TYPE_CLIPMAP_PVS, params[1]).clipMap->mapEnts);
				MapDumper::GetApi()->write(Game::IW4::ASSET_TYPE_MAP_ENTS, converted);

				std::string entString(converted->entityString, converted->numEntityChars - 1);
				Utils::Entities mapEnts(entString);
				const auto models = mapEnts.GetModels(false);
				for (const std::string& model : models)
				{
					Command::Execute(Utils::VA("dumpXModel %s", model.data()), true);
				}
			});
	}

	IMapEnts::~IMapEnts()
	{

	}
}