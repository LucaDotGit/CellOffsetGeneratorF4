#include "Plugin/Internal/Core/CellOffsetCreator.hpp"

#include "Plugin/Internal/SettingsManager.hpp"

#include "Plugin/Internal/Core/CellOffsetFile.hpp"
#include "Plugin/Internal/Core/CellOffsetHasher.hpp"
#include "Plugin/Internal/Core/CellOffsetMenu.hpp"

namespace Plugin::Internal::Core
{
	CellOffsetCreator::CellOffsetCreator(
		const REX::NotNull<std::shared_ptr<SettingsManager>>& a_settingsManager,
		const REX::NotNull<std::shared_ptr<CellOffsetMenu>>& a_cellOffsetMenu,
		std::filesystem::path a_cellOffsetsDirectoryPath,
		std::filesystem::path a_cellOffsetFileExtension)
		: _settingsManager(a_settingsManager),
		  _cellOffsetMenu(a_cellOffsetMenu),
		  _cellOffsetsDirectoryPath(std::move(a_cellOffsetsDirectoryPath)),
		  _cellOffsetFileExtension(std::move(a_cellOffsetFileExtension))
	{
	}

	CellOffsetCreator::~CellOffsetCreator() noexcept = default;

	void CellOffsetCreator::OnGameDataReady()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		auto stopwatch = REX::Stopwatch::StartNew();

		CreateCellOffsetsForAll();

		stopwatch.Stop();

		REX::LogInformation("Cell offset creation completed in {}"sv,
			std::chrono::duration_cast<std::chrono::duration<REX::Float64, std::milli>>(stopwatch.GetElapsedTime()));
	}

	void CellOffsetCreator::CreateCellOffsetsForAll()
	{
		LoadCellOffsetMenu();
		LoadPendingPlugins();
		SortPendingPlugins();
		RunPendingTasks();
		DisposeTasks();
	}

	bool CellOffsetCreator::CreateCellOffsetsForPlugin(
		REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
		REX::NotNull<RE::TESFile*> a_plugin,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState)
	{
		const auto& worldSpaceId = a_worldSpace->editorID;
		const auto* fileName = a_plugin->filename.data();

		auto* cellOffsetData = a_worldSpace->GetOffsetData(a_plugin.get());
		if (!cellOffsetData) {
			REX::LogDebug(R"((Plugin "{}", World Space "{}") No cell offset data available)"sv,
				fileName, worldSpaceId);
			return false;
		}

		const auto* fileOffsetData = cellOffsetData->cellFileOffsets;
		if (fileOffsetData) {
			REX::LogDebug(R"((Plugin "{}", World Space "{}") Cell offsets already exist)"sv,
				fileName, worldSpaceId);
			return false;
		}

		const auto fileHash = CellOffsetHasher::CreateHashForFile(a_plugin, a_hashState);
		if (!fileHash.has_value()) [[unlikely]] {
			REX::LogError(R"((Plugin "{}", World Space "{}") Failed to create file hash)"sv,
				fileName, worldSpaceId);
			return false;
		}

		const auto filePath = GetCellOffsetFilePathForPlugin(a_worldSpace, a_plugin);

		const auto loadFileError = LoadCellOffsetsFromFile(filePath, cellOffsetData, a_hashState, fileHash.value());
		if (loadFileError.value() == REX::ERROR_NUMBER_SUCCESS) {
			REX::LogDebug(R"((Plugin "{}", World Space "{}") Loaded cell offsets from file "{}")"sv,
				fileName, worldSpaceId, filePath.generic_string());
			return true;
		}

		auto newOffsetBuffer = GenerateCellOffsetsForPlugin(a_worldSpace.get(), a_plugin, cellOffsetData);
		if (!newOffsetBuffer.has_value()) [[unlikely]] {
			REX::LogError(R"((Plugin "{}", World Space "{}") Failed to generate cell offsets: {})"sv,
				fileName, worldSpaceId, newOffsetBuffer.error().message());
			return false;
		}

		if (_settingsManager->GetDoTryDryRunSetting()->GetValue()) {
			REX::LogDebug(R"((Plugin "{}", World Space "{}") Dry run enabled; skipping file save)"sv,
				fileName, worldSpaceId);
			return true;
		}

		const auto saveFileError = SaveCellOffsetsToFile(filePath, std::move(newOffsetBuffer).value(), cellOffsetData, a_hashState, fileHash.value());
		if (saveFileError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			REX::LogError(R"((Plugin "{}", World Space "{}") Failed to save cell offsets to file "{}": {})"sv,
				fileName, worldSpaceId, filePath.generic_string(), saveFileError.message());
			return false;
		}

		return true;
	}

	void CellOffsetCreator::LoadCellOffsetMenu()
	{
		if (!_settingsManager->GetEnableMenuSetting()->GetValue()) {
			return;
		}

		REX::LogDebug("Loading cell offset menu"sv);

		F4SE::Menus::RegisterMenu(_cellOffsetMenu);

		if (!F4SE::Menus::OpenMenu(_cellOffsetMenu->menuName)) [[unlikely]] {
			REX::LogError("Failed to open the cell offset menu"sv);
		}
	}

	void CellOffsetCreator::LoadPendingPlugins()
	{
		REX::LogDebug("Loading pending plugins from world spaces"sv);

		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) [[unlikely]] {
			REX::Fail("Failed to get the game's data handler."sv);
		}

		_totalWorldSpaceCount = 0;

		for (auto* worldSpace : dataHandler->GetFormArray<RE::TESWorldSpace>()) {
			if (!worldSpace) {
				continue;
			}

			auto* worldSpaceFiles = worldSpace->sourceFiles.array;
			if (!worldSpaceFiles || worldSpaceFiles->empty()) {
				continue;
			}

			_totalWorldSpaceCount++;

			for (auto* worldSpaceFile : *worldSpaceFiles) {
				if (!worldSpaceFile) {
					continue;
				}

				auto* cellOffsetData = worldSpace->GetOffsetData(worldSpaceFile);
				if (!cellOffsetData) {
					continue;
				}

				if (cellOffsetData->cellFileOffsets) {
					continue;
				}

				_pendingPlugins.emplace_back(worldSpaceFile);
			}
		}

		REX::LogDebug("Found {} plugins across {} world spaces"sv,
			_pendingPlugins.size(), _totalWorldSpaceCount);
	}

	void CellOffsetCreator::SortPendingPlugins()
	{
		REX::LogDebug("Sorting and filtering pending plugins"sv);

		std::ranges::sort(_pendingPlugins,
			[](const REX::NotNull<RE::TESFile*>& a_lhs, const REX::NotNull<RE::TESFile*>& a_rhs) {
				const auto leftFileName = std::string_view(a_lhs->filename.data());
				const auto rightFileName = std::string_view(a_rhs->filename.data());
				return REX::CompareIgnoreCase(leftFileName, rightFileName) < 0;
			});

		const auto uniquePluginsSubrange = std::ranges::unique(_pendingPlugins,
			[](const REX::NotNull<RE::TESFile*>& a_lhs, const REX::NotNull<RE::TESFile*>& a_rhs) {
				const auto leftFileName = std::string_view(a_lhs->filename.data());
				const auto rightFileName = std::string_view(a_rhs->filename.data());
				return REX::EqualsIgnoreCase(leftFileName, rightFileName);
			});

		_pendingPlugins.erase(uniquePluginsSubrange.begin(), uniquePluginsSubrange.end());

		std::ranges::sort(_pendingPlugins,
			[](const REX::NotNull<RE::TESFile*>& a_lhs, const REX::NotNull<RE::TESFile*>& a_rhs) {
				return a_lhs->fileSize > a_rhs->fileSize;
			});

		REX::LogDebug("Sorted and filtered to {} pending plugins"sv,
			_pendingPlugins.size());
	}

	void CellOffsetCreator::RunPendingTasks()
	{
		REX::LogDebug("Running tasks for pending plugins"sv);

		const auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) [[unlikely]] {
			REX::Fail("Failed to get the game's data handler."sv);
		}

		_cellOffsetMenu->SetMaxProgressCounter(static_cast<std::uint32_t>(_pendingPlugins.size() * _totalWorldSpaceCount));

		{
			auto threadPool = REX::ThreadPool();

			const auto taskRunResults = threadPool.SubmitTaskRange(
				_pendingPlugins, [this](REX::NotNull<RE::TESFile*> a_plugin, REX::NotNull<const RE::TESDataHandler*> a_dataHandler) {
					RunTaskForPlugin(a_dataHandler, a_plugin);
				},
				REX::NotNull(dataHandler));

			threadPool.AttachThisUntilDrained();

			for (const auto& taskResult : taskRunResults) {
				taskResult.wait();
			}
		}

		F4SE::Menus::ForceCloseMenu(_cellOffsetMenu->menuName);

		REX::LogDebug("Finished processing all files"sv);
	}

	void CellOffsetCreator::DisposeTasks()
	{
		_pendingPlugins.clear();

		_totalWorldSpaceCount = 0;
	}

	std::filesystem::path CellOffsetCreator::GetCellOffsetFilePathForPlugin(
		REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
		REX::NotNull<RE::TESFile*> a_plugin)
	{
		const auto worldSpaceId = std::string_view(a_worldSpace->editorID);
		const auto fileName = std::string_view(a_plugin->filename.data());
		const auto fileExtension = _cellOffsetFileExtension.generic_string();

		auto offsetFileName = std::string();
		offsetFileName.reserve(worldSpaceId.size() + fileExtension.size());
		offsetFileName.append(worldSpaceId);
		offsetFileName.append(fileExtension);

		return {
			_cellOffsetsDirectoryPath / fileName / offsetFileName,
			std::filesystem::path::generic_format
		};
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	REX::SystemError CellOffsetCreator::LoadCellOffsetsFromFile(
		const std::filesystem::path& a_filePath,
		REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState,
		Core::HashType a_fileHash)
	{
		auto cellOffsetFile = CellOffsetFile::LoadFromFile(a_filePath);
		if (!cellOffsetFile.has_value()) {
			return cellOffsetFile.error();
		}

		const auto& offsetFileHeaderInfo = cellOffsetFile->headerInfo;
		auto& offsetFileOffsetInfo = cellOffsetFile->offsetInfo;

		if (offsetFileHeaderInfo.fileHash != a_fileHash) {
			return REX::CreateSystemError(REX::PosixErrorCode::illegal_byte_sequence);
		}

		const auto maxOffsetX = a_offsetData->GetMaxOffsetX();
		const auto maxOffsetY = a_offsetData->GetMaxOffsetY();

		if (offsetFileOffsetInfo.maxOffsetX != maxOffsetX || offsetFileOffsetInfo.maxOffsetY != maxOffsetY) {
			return REX::CreateSystemError(REX::PosixErrorCode::illegal_byte_sequence);
		}

		const auto offsetHash = CellOffsetHasher::CreateHashForOffsets(
			offsetFileOffsetInfo.offsetBuffer.get(),
			offsetFileOffsetInfo.offsetCount,
			a_hashState);

		if (!offsetHash.has_value()) {
			return offsetHash.error();
		}

		if (offsetHash.value() != offsetFileOffsetInfo.offsetHash) {
			return REX::CreateSystemError(REX::PosixErrorCode::illegal_byte_sequence);
		}

		a_offsetData->cellFileOffsets = offsetFileOffsetInfo.offsetBuffer.release();
		return REX::CreateSystemError(REX::ERROR_NUMBER_SUCCESS);
	}

	REX::SystemError CellOffsetCreator::SaveCellOffsetsToFile(
		const std::filesystem::path& a_filePath,
		CellOffsetBufferData&& a_bufferData, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
		REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState,
		Core::HashType a_fileHash)
	{
		const auto offsetHash = CellOffsetHasher::CreateHashForOffsets(
			a_bufferData.offsetData.get(),
			a_bufferData.offsetCount,
			a_hashState);

		if (!offsetHash.has_value()) {
			return offsetHash.error();
		}

		const auto maxOffsetX = a_offsetData->GetMaxOffsetX();
		const auto maxOffsetY = a_offsetData->GetMaxOffsetY();

		auto cellOffsetFile = CellOffsetFile();
		auto& offsetFileHeaderInfo = cellOffsetFile.headerInfo;
		auto& offsetFileOffsetInfo = cellOffsetFile.offsetInfo;

		offsetFileHeaderInfo.fileHash = a_fileHash;
		offsetFileOffsetInfo.maxOffsetX = maxOffsetX;
		offsetFileOffsetInfo.maxOffsetY = maxOffsetY;
		offsetFileOffsetInfo.offsetHash = offsetHash.value();
		offsetFileOffsetInfo.offsetCount = std::exchange(a_bufferData.offsetCount, 0);
		offsetFileOffsetInfo.offsetBuffer = std::move(a_bufferData.offsetData);

		const auto saveFileError = cellOffsetFile.SaveToFile(a_filePath);
		a_offsetData->cellFileOffsets = offsetFileOffsetInfo.offsetBuffer.release();

		if (saveFileError.value() != REX::ERROR_NUMBER_SUCCESS) {
			return saveFileError;
		}

		if (_settingsManager->GetMarkCreatedFilesAsReadOnlySetting()->GetValue()) {
			std::ignore = CellOffsetFile::MarkFileAsReadOnly(a_filePath);
		}

		return saveFileError;
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	auto CellOffsetCreator::GenerateCellOffsetsForPlugin(
		REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
		REX::NotNull<RE::TESFile*> a_plugin,
		REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData) -> std::expected<CellOffsetBufferData, REX::SystemError>
	{
		const auto maxOffsetX = a_offsetData->GetMaxOffsetX();
		const auto minOffsetX = a_offsetData->GetMinOffsetX();
		const auto minOffsetY = a_offsetData->GetMinOffsetY();
		const auto maxOffsetY = a_offsetData->GetMaxOffsetY();

		const auto& worldSpaceId = a_worldSpace->editorID;
		const auto fileName = std::string_view(a_plugin->filename.data());

		const auto maxCellIndex = RE::TESWorldSpace::GetIndexForCellCoord(a_offsetData.get(), maxOffsetX, maxOffsetY);
		if (maxCellIndex == 0) [[unlikely]] {
			REX::LogError(R"((Plugin "{}", World Space "{}") No cells found)"sv,
				fileName, worldSpaceId);
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::result_out_of_range));
		}

		if (maxCellIndex == std::numeric_limits<decltype(maxCellIndex)>::max()) [[unlikely]] {
			REX::LogError(R"((Plugin "{}", World Space "{}") Cell bounds are invalid (X: {} to {}, Y: {} to {}))"sv,
				fileName, worldSpaceId, minOffsetX, maxOffsetX, minOffsetY, maxOffsetY);
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::result_out_of_range));
		}

		const auto offsetCount = maxCellIndex + 1;
		auto offsetBuffer = Core::CreateCellOffsetBuffer(offsetCount);

		REX::LogDebug(R"((Plugin "{}", World Space "{}") Generating cell offsets for potentially {} cells (X: {} to {}, Y: {} to {}))"sv,
			fileName, worldSpaceId, offsetCount, minOffsetX, maxOffsetX, minOffsetY, maxOffsetY);

		auto generatedOffsetCount = 0ui32;

		for (auto cellY = minOffsetY; cellY <= maxOffsetY; cellY++) {
			for (auto cellX = minOffsetX; cellX <= maxOffsetX; cellX++) {
				const auto cellOffsetIndex = RE::TESWorldSpace::GetIndexForCellCoord(a_offsetData.get(), cellX, cellY);
				if (cellOffsetIndex == std::numeric_limits<decltype(cellOffsetIndex)>::max()) [[unlikely]] {
					REX::LogError(R"((Plugin "{}", World Space "{}") Cell ({}, {}) has invalid offset index)"sv,
						fileName, worldSpaceId, cellX, cellY);
					return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::result_out_of_range));
				}

				if (cellOffsetIndex > maxCellIndex) [[unlikely]] {
					REX::LogError(R"((Plugin "{}", World Space "{}") Cell ({}, {}) has out of bounds offset index {} (max {}))"sv,
						fileName, worldSpaceId, cellX, cellY, cellOffsetIndex, maxCellIndex);
					return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::result_out_of_range));
				}

				if (!a_worldSpace->FindCellInFile(a_plugin.get(), cellX, cellY)) {
					continue;
				}

				// `TESWorldSpace::FindCellInFile` will set the file offset for the passed-in file,
				// so that can be used to determine the relative offset of the cell.

				const auto fileOffset = a_plugin->fileOffset - a_offsetData->fileOffset;
				offsetBuffer.get()[cellOffsetIndex] = fileOffset;

				generatedOffsetCount++;

				REX::LogTrace(R"((Plugin "{}", World Space "{}") Added cell ({}, {}) at index {} and file offset {})"sv,
					fileName, worldSpaceId, cellX, cellY, cellOffsetIndex, fileOffset);
			}
		}

		REX::LogDebug(R"((Plugin "{}", World Space "{}") Generated offsets for {} cells)"sv,
			fileName, worldSpaceId, generatedOffsetCount);

		return CellOffsetBufferData{
			.offsetCount = offsetCount,
			.offsetData = std::move(offsetBuffer)
		};
	}

	void CellOffsetCreator::RunTaskForPlugin(
		REX::NotNull<const RE::TESDataHandler*> a_dataHandler,
		REX::NotNull<RE::TESFile*> a_plugin)
	{
		const auto numericThreadId = REX::GetCurrentNumericThreadId();

		auto* threadSafeFile = a_plugin->GetThreadSafeFileForThread(numericThreadId);
		if (!threadSafeFile) [[unlikely]] {
			REX::Fail("Failed to get thread safe file for plugin {} on thread {}."sv,
				a_plugin->filename.data(), numericThreadId);
		}

		thread_local auto hashState = Core::CreateHashState();

		for (auto* worldSpace : a_dataHandler->GetFormArray<RE::TESWorldSpace>()) {
			if (!worldSpace) {
				continue;
			}

			CreateCellOffsetsForPlugin(worldSpace, threadSafeFile, hashState);

			_cellOffsetMenu->IncrementProgressCounter();
		}

		if (!a_plugin->DeleteThreadSafeFileForThread(numericThreadId)) [[unlikely]] {
			REX::Fail("Failed to delete thread safe file for plugin {} on thread {}."sv,
				a_plugin->filename.data(), numericThreadId);
		}
	}
}
