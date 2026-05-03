#pragma once

#include "Plugin/Internal/Core/CellOffsetHasher.hpp"

namespace Plugin::Internal
{
	class SettingsManager;
}

namespace Plugin::Internal::Core
{
	class CellOffsetMenu;
	class CellOffsetTask;
	class CellOffsetSkipPreventer;

	class CellOffsetCreator final
	{
	public:
		CellOffsetCreator(
			const REX::NotNull<std::shared_ptr<SettingsManager>>& a_settingsManager,
			const REX::NotNull<std::shared_ptr<CellOffsetMenu>>& a_cellOffsetMenu,
			std::filesystem::path a_cellOffsetsDirectoryPath,
			std::filesystem::path a_cellOffsetFileExtension);

		~CellOffsetCreator() noexcept;

		CellOffsetCreator(const CellOffsetCreator&) = delete;
		CellOffsetCreator(CellOffsetCreator&&) = delete;

		CellOffsetCreator& operator=(const CellOffsetCreator&) = delete;
		CellOffsetCreator& operator=(CellOffsetCreator&&) = delete;

		[[nodiscard]] const auto& GetCellOffsetsDirectoryPath() const noexcept { return _cellOffsetsDirectoryPath; }
		[[nodiscard]] const auto& GetCellOffsetFileExtension() const noexcept { return _cellOffsetFileExtension; }

		void OnGameDataReady();

		void CreateCellOffsetsForAll();

		bool CreateCellOffsetsForPlugin(
			REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
			REX::NotNull<RE::TESFile*> a_plugin,
			const REX::NotNull<Core::HashStateBuffer>& a_hashState);

	private:
		class CellOffsetBufferData final
		{
		public:
			std::uint32_t offsetCount;
			Core::CellOffsetBuffer offsetData;
		};

		void LoadCellOffsetMenu();
		void LoadPendingPlugins();
		void SortPendingPlugins();
		void RunPendingTasks();
		void DisposeTasks();

		[[nodiscard]] std::filesystem::path GetCellOffsetFilePathForPlugin(
			REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
			REX::NotNull<RE::TESFile*> a_plugin);

		REX::SystemError LoadCellOffsetsFromFile(
			const std::filesystem::path& a_filePath,
			REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData,
			const REX::NotNull<Core::HashStateBuffer>& a_hashState,
			Core::HashType a_fileHash);

		REX::SystemError SaveCellOffsetsToFile(
			const std::filesystem::path& a_filePath,
			CellOffsetBufferData&& a_bufferData,
			REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData,
			const REX::NotNull<Core::HashStateBuffer>& a_hashState,
			Core::HashType a_fileHash);

		[[nodiscard]] auto GenerateCellOffsetsForPlugin(
			REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
			REX::NotNull<RE::TESFile*> a_plugin,
			REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData) -> std::expected<CellOffsetBufferData, REX::SystemError>;

		void RunTaskForPlugin(
			REX::NotNull<const RE::TESDataHandler*> a_dataHandler,
			REX::NotNull<RE::TESFile*> a_plugin);

		REX::NotNull<std::shared_ptr<SettingsManager>> _settingsManager;
		REX::NotNull<std::shared_ptr<CellOffsetMenu>> _cellOffsetMenu;
		std::filesystem::path _cellOffsetsDirectoryPath;
		std::filesystem::path _cellOffsetFileExtension;
		std::vector<REX::NotNull<RE::TESFile*>> _pendingPlugins;
		std::uint32_t _totalWorldSpaceCount{ 0 };
	};
}
