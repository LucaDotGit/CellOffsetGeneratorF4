#pragma once

#include "Plugin/Internal/Core/CellOffsetHasher.hpp"

namespace Plugin::Internal
{
	class SettingsManager;
}

namespace Plugin::Internal::Core
{
	class CellOffsetMenu;

	class CellOffsetCreator final : public F4SE::Events::IOnGameDataReady
	{
	public:
		CellOffsetCreator(
			REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> a_logger,
			REX::NotNull<std::shared_ptr<SettingsManager>> a_settingsManager,
			REX::NotNull<std::shared_ptr<CellOffsetMenu>> a_cellOffsetMenu,
			std::filesystem::path a_cellOffsetsDirectoryPath,
			std::filesystem::path a_cellOffsetFileExtension);

		~CellOffsetCreator() noexcept override;

		CellOffsetCreator(const CellOffsetCreator&) = delete;
		CellOffsetCreator(CellOffsetCreator&&) = delete;

		auto operator=(const CellOffsetCreator&) -> CellOffsetCreator& = delete;
		auto operator=(CellOffsetCreator&&) -> CellOffsetCreator& = delete;

		[[nodiscard]]
		auto GetCellOffsetsDirectoryPath() const noexcept -> const auto&
		{
			return _cellOffsetsDirectoryPath;
		}

		[[nodiscard]]
		auto GetCellOffsetFileExtension() const noexcept -> const auto&
		{
			return _cellOffsetFileExtension;
		}

		void OnGameDataReady(bool a_isReady) override;

		void CreateCellOffsetsForAll();

	private:
		class CellOffsetBufferData final
		{
		public:
			CellOffsetBufferData(std::uint32_t a_size, Core::CellOffsetBuffer&& a_buffer) noexcept :
				_size(a_size),
				_buffer(std::move(a_buffer))
			{
			}

			~CellOffsetBufferData() noexcept = default;

			CellOffsetBufferData(const CellOffsetBufferData&) = delete;
			CellOffsetBufferData(CellOffsetBufferData&&) noexcept = default;

			auto operator=(const CellOffsetBufferData&) -> CellOffsetBufferData& = delete;
			auto operator=(CellOffsetBufferData&&) noexcept -> CellOffsetBufferData& = default;

			[[nodiscard]]
			auto GetSize() const noexcept
			{
				return _size;
			}

			[[nodiscard]]
			auto GetBuffer() & noexcept -> auto&
			{
				return _buffer;
			}

			[[nodiscard]]
			auto GetBuffer() && noexcept -> auto&&
			{
				return std::move(_buffer);
			}

			[[nodiscard]]
			auto GetBuffer() const& noexcept -> const auto&
			{
				return _buffer;
			}

			[[nodiscard]]
			auto GetBuffer() const&& noexcept -> const auto&&
			{
				return std::move(_buffer);
			}

			void Reset() noexcept
			{
				_size = 0;
				_buffer.reset();
			}

		private:
			std::uint32_t _size;
			Core::CellOffsetBuffer _buffer;
		};

		void LoadCellOffsetMenu();
		void LoadPendingPlugins();
		void SortPendingPlugins();
		void RunPendingTasks();
		void DisposeTasks();

		[[nodiscard]]
		auto GetCellOffsetFilePathForPlugin(
			REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
			REX::NotNull<RE::TESFile*> a_plugin) -> std::filesystem::path;

		static auto LoadCellOffsetsFromFile(
			const std::filesystem::path& a_filePath,
			REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData,
			const REX::NotNull<Core::HashStateBuffer>& a_hashState,
			Core::HashType a_fileHash) -> REX::SystemError;

		static auto SaveCellOffsetsToFile(
			const std::filesystem::path& a_filePath,
			CellOffsetBufferData a_bufferData,
			REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData,
			const REX::NotNull<Core::HashStateBuffer>& a_hashState,
			Core::HashType a_fileHash) -> REX::SystemError;

		[[nodiscard]]
		static auto GenerateCellOffsetsForPlugin(
			REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
			REX::NotNull<RE::TESFile*> a_plugin,
			REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData)
			-> std::expected<CellOffsetBufferData, std::string>;

		auto CreateCellOffsetsForPlugin(
			REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
			REX::NotNull<RE::TESFile*> a_plugin,
			const REX::NotNull<Core::HashStateBuffer>& a_hashState) -> std::expected<bool, std::string>;

		auto RunTaskForPlugin(
			const RE::TESDataHandler& a_dataHandler,
			REX::NotNull<RE::TESFile*> a_plugin,
			const REX::NotNull<Core::HashStateBuffer>& a_hashState) -> std::expected<void, std::vector<std::string>>;

		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> _logger;
		REX::NotNull<std::shared_ptr<SettingsManager>> _settingsManager;
		REX::NotNull<std::shared_ptr<CellOffsetMenu>> _cellOffsetMenu;
		std::filesystem::path _cellOffsetsDirectoryPath;
		std::filesystem::path _cellOffsetFileExtension;
		std::vector<REX::NotNull<RE::TESFile*>> _pendingPlugins;
		std::uint32_t _totalWorldSpaceCount{0};
	};
}
