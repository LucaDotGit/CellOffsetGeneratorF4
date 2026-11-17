#include "Plugin/Internal/Core/CellOffsetCreator.hpp"

#include "Plugin/Internal/Core/CellOffsetFile.hpp"
#include "Plugin/Internal/Core/CellOffsetHasher.hpp"
#include "Plugin/Internal/Core/CellOffsetMenu.hpp"
#include "Plugin/Internal/SettingsManager.hpp"

namespace Plugin::Internal::Core
{
	CellOffsetCreator::CellOffsetCreator(
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> a_logger,
		REX::NotNull<std::shared_ptr<SettingsManager>> a_settingsManager,
		REX::NotNull<std::shared_ptr<CellOffsetMenu>> a_cellOffsetMenu,
		std::filesystem::path a_cellOffsetsDirectoryPath,
		std::filesystem::path a_cellOffsetFileExtension) :
		_logger(std::move(a_logger)),
		_settingsManager(std::move(a_settingsManager)),
		_cellOffsetMenu(std::move(a_cellOffsetMenu)),
		_cellOffsetsDirectoryPath(std::move(a_cellOffsetsDirectoryPath)),
		_cellOffsetFileExtension(std::move(a_cellOffsetFileExtension))
	{
	}

	CellOffsetCreator::~CellOffsetCreator() noexcept = default;

	void CellOffsetCreator::OnGameDataReady([[maybe_unused]] bool a_isReady)
	{
		_logger->LogDebug("{}"sv, REX::GetCurrentFunctionName());

		auto stopwatch = REX::Stopwatch::StartNew();

		CreateCellOffsetsForAll();

		stopwatch.Stop();

		_logger->LogInformation(
			"Cell offset creation completed in {}"sv,
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

	void CellOffsetCreator::LoadCellOffsetMenu()
	{
		_logger->LogDebug("Registering cell offset menu"sv);

		_cellOffsetMenu->SetCurrentProgressCounter(0);
		_cellOffsetMenu->SetMaxProgressCounter(0);

		F4SE::Menus::RegisterMenu(_cellOffsetMenu);

		if (!_settingsManager->GetEnableMenuSetting()->GetValue()) {
			return;
		}

		_logger->LogDebug("Opening cell offset menu"sv);

		if (!F4SE::Menus::OpenMenuAsync(_cellOffsetMenu->GetMenuName())) [[unlikely]] {
			_logger->LogError("Failed to open the cell offset menu"sv);
		}
	}

	void CellOffsetCreator::LoadPendingPlugins()
	{
		_logger->LogDebug("Loading pending plugin(s) from world space(s)"sv);

		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) [[unlikely]] {
			REX::Fail("Failed to get the game's data handler."sv);
		}

		_totalWorldSpaceCount = 0;

		for (const auto* worldSpace : dataHandler->GetFormArray<RE::TESWorldSpace>()) {
			if (!worldSpace) {
				continue;
			}

			const auto* worldSpaceFiles = worldSpace->sourceFiles.array;
			if (!worldSpaceFiles || worldSpaceFiles->empty()) {
				continue;
			}

			_totalWorldSpaceCount++;

			for (auto* worldSpaceFile : *worldSpaceFiles) {
				if (!worldSpaceFile) {
					continue;
				}

				const auto* cellOffsetData = worldSpace->GetOffsetData(worldSpaceFile);
				if (!cellOffsetData) {
					continue;
				}

				if (cellOffsetData->cellFileOffsets) {
					continue;
				}

				_pendingPlugins.emplace_back(worldSpaceFile);
			}
		}

		_logger->LogDebug(
			"Found {} plugin(s) across {} world space(s)"sv,
			_pendingPlugins.size(),
			_totalWorldSpaceCount);
	}

	void CellOffsetCreator::SortPendingPlugins()
	{
		_logger->LogDebug("Sorting and filtering pending plugin(s)"sv);

		std::ranges::sort(
			_pendingPlugins,
			[](const REX::NotNull<RE::TESFile*>& a_lhs, const REX::NotNull<RE::TESFile*>& a_rhs) noexcept -> bool {
				const auto leftFileName = std::string_view(a_lhs->filename.data());
				const auto rightFileName = std::string_view(a_rhs->filename.data());
				return REX::CompareIgnoreCase(leftFileName, rightFileName) < 0;
			});

		const auto uniquePluginsSubrange = std::ranges::unique(
			_pendingPlugins,
			[](const REX::NotNull<RE::TESFile*>& a_lhs, const REX::NotNull<RE::TESFile*>& a_rhs) noexcept -> bool {
				const auto leftFileName = std::string_view(a_lhs->filename.data());
				const auto rightFileName = std::string_view(a_rhs->filename.data());
				return REX::EqualsIgnoreCase(leftFileName, rightFileName);
			});

		_pendingPlugins.erase(uniquePluginsSubrange.begin(), uniquePluginsSubrange.end());

		std::ranges::sort(
			_pendingPlugins,
			[](const REX::NotNull<RE::TESFile*>& a_lhs, const REX::NotNull<RE::TESFile*>& a_rhs) noexcept -> bool {
				return a_lhs->fileSize > a_rhs->fileSize;
			});

		_logger->LogDebug("Sorted and filtered to {} pending plugin(s)"sv, _pendingPlugins.size());
	}

	void CellOffsetCreator::RunPendingTasks()
	{
		_logger->LogDebug("Running tasks for pending plugin(s)"sv);

		const auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) [[unlikely]] {
			REX::Fail("Failed to get the game's data handler."sv);
		}

		_cellOffsetMenu->SetMaxProgressCounter(
			static_cast<std::uint32_t>(_pendingPlugins.size() * _totalWorldSpaceCount));

		{
			const auto maxSystemThreadCount = REX::GetMaxThreadCount() - 1;
			const auto maxDesiredThreadCount =
				static_cast<std::size_t>(_settingsManager->GetThreadCountSetting()->GetValue() - 1);
			const auto maxPluginThreadCount = _pendingPlugins.size() - 1;
			const auto finalThreadCount = std::min({maxSystemThreadCount, maxDesiredThreadCount, maxPluginThreadCount});

			_logger->LogDebug(
				"Using {} thread(s) for processing {} plugin(s)"sv,
				finalThreadCount + 1,
				_pendingPlugins.size());

			auto threadPool = REX::ThreadPool(finalThreadCount);

			const auto taskRunResults = threadPool.SubmitTaskRange(
				_pendingPlugins,
				[this, dataHandler](REX::NotNull<RE::TESFile*> a_plugin) -> void {
					thread_local const auto HASH_STATE = Core::CreateHashState();

					const auto taskResult = RunTaskForPlugin(*dataHandler, a_plugin, HASH_STATE);
					if (taskResult.has_value()) [[likely]] {
						_logger->LogDebug(R"(Finished processing plugin "{}")"sv, a_plugin->filename.data());
						return;
					}

					for (const auto& errorMessage : taskResult.error()) {
						_logger->LogError(
							R"(Error processing plugin "{}": {})"sv,
							a_plugin->filename.data(),
							errorMessage);
					}
				});

			threadPool.AttachThisUntilDrained();

			for (const auto& taskResult : taskRunResults) {
				taskResult.wait();
			}
		}

		if (_settingsManager->GetEnableMenuSetting()->GetValue() &&
			!F4SE::Menus::CloseMenuAsync(_cellOffsetMenu->GetMenuName())) [[unlikely]] {
			_logger->LogError("Failed to close the cell offset menu"sv);
		}

		_logger->LogDebug("Finished processing all pending plugin(s)"sv);
	}

	void CellOffsetCreator::DisposeTasks()
	{
		_pendingPlugins.clear();

		_totalWorldSpaceCount = 0;
	}

	auto CellOffsetCreator::GetCellOffsetFilePathForPlugin(
		REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
		REX::NotNull<RE::TESFile*> a_plugin) -> std::filesystem::path
	{
		const auto worldSpaceId = std::string_view(a_worldSpace->editorID);
		const auto fileName = std::string_view(a_plugin->filename.data());
		const auto fileExtension = _cellOffsetFileExtension.generic_string();

		auto offsetFileName = std::string();
		offsetFileName.reserve(worldSpaceId.size() + fileExtension.size());
		offsetFileName.append(worldSpaceId);
		offsetFileName.append(fileExtension);

		return {_cellOffsetsDirectoryPath / fileName / offsetFileName, std::filesystem::path::generic_format};
	}

	auto CellOffsetCreator::LoadCellOffsetsFromFile(
		const std::filesystem::path& a_filePath,
		REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState,
		Core::HashType a_fileHash) -> REX::SystemError
	{
		auto offsetFile = CellOffsetFile::LoadFromFile(a_filePath);
		if (!offsetFile.has_value()) [[unlikely]] {
			return offsetFile.error();
		}

		const auto& fileHeaderInfo = offsetFile->GetHeaderInfo();
		if (fileHeaderInfo.GetFileHash() != a_fileHash) [[unlikely]] {
			return REX::CreateSystemError(REX::PosixErrorCode::bad_message);
		}

		const auto maxOffsetX = a_offsetData->GetMaxOffsetX();
		const auto maxOffsetY = a_offsetData->GetMaxOffsetY();

		auto& fileCellInfo = offsetFile->GetCellInfo();
		if (fileCellInfo.GetMaxOffsetX() != maxOffsetX || fileCellInfo.GetMaxOffsetY() != maxOffsetY) [[unlikely]] {
			return REX::CreateSystemError(REX::PosixErrorCode::bad_message);
		}

		const auto offsetHash = CellOffsetHasher::CreateHashForOffsets(
			fileCellInfo.GetOffsetBuffer().get(),
			fileCellInfo.GetOffsetCount(),
			a_hashState);

		if (!offsetHash.has_value()) [[unlikely]] {
			return offsetHash.error();
		}

		if (offsetHash.value() != fileCellInfo.GetOffsetHash()) [[unlikely]] {
			return REX::CreateSystemError(REX::PosixErrorCode::bad_message);
		}

		a_offsetData->cellFileOffsets = fileCellInfo.GetOffsetBuffer().release();
		return REX::CreateSystemError(REX::ERROR_NUMBER_SUCCESS);
	}

	auto CellOffsetCreator::SaveCellOffsetsToFile(
		const std::filesystem::path& a_filePath,
		CellOffsetBufferData a_bufferData,
		REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState,
		Core::HashType a_fileHash) -> REX::SystemError
	{
		const auto offsetHash =
			CellOffsetHasher::CreateHashForOffsets(a_bufferData.GetBuffer().get(), a_bufferData.GetSize(), a_hashState);

		if (!offsetHash.has_value()) {
			return offsetHash.error();
		}

		const auto maxOffsetX = a_offsetData->GetMaxOffsetX();
		const auto maxOffsetY = a_offsetData->GetMaxOffsetY();

		auto headerInfo = CellOffsetFile::HeaderInfo(a_fileHash);

		auto cellInfo = CellOffsetFile::CellInfo(
			maxOffsetX,
			maxOffsetY,
			offsetHash.value(),
			a_bufferData.GetSize(),
			std::move(a_bufferData.GetBuffer()));

		auto offsetFile = CellOffsetFile(std::move(headerInfo), std::move(cellInfo));

		const auto saveResult = offsetFile.SaveToFile(a_filePath);
		a_offsetData->cellFileOffsets = offsetFile.GetCellInfo().GetOffsetBuffer().release();

		return saveResult;
	}

	auto CellOffsetCreator::GenerateCellOffsetsForPlugin(
		REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
		REX::NotNull<RE::TESFile*> a_plugin,
		REX::NotNull<RE::TESWorldSpace::OFFSET_DATA*> a_offsetData) -> std::expected<CellOffsetBufferData, std::string>
	{
		const auto minOffsetX = a_offsetData->GetMinOffsetX();
		const auto maxOffsetX = a_offsetData->GetMaxOffsetX();
		const auto minOffsetY = a_offsetData->GetMinOffsetY();
		const auto maxOffsetY = a_offsetData->GetMaxOffsetY();

		const auto maxIndex = RE::TESWorldSpace::GetIndexForCellPosition(a_offsetData.get(), maxOffsetX, maxOffsetY);
		if (maxIndex == 0) [[unlikely]] {
			return std::unexpected("No cells found"s);
		}

		if (maxIndex == std::numeric_limits<decltype(maxIndex)>::max()) [[unlikely]] {
			return std::unexpected(
				REX::Format(
					"Cell bounds are invalid (X: {} to {}, Y: {} to {})"sv,
					minOffsetX,
					maxOffsetX,
					minOffsetY,
					maxOffsetY));
		}

		const auto offsetCount = maxIndex + 1;
		auto offsetBuffer = Core::CreateCellOffsetBufferForOverwrite(offsetCount);

		for (auto offsetY = minOffsetY; offsetY <= maxOffsetY; offsetY++) {
			for (auto offsetX = minOffsetX; offsetX <= maxOffsetX; offsetX++) {
				const auto offsetIndex =
					RE::TESWorldSpace::GetIndexForCellPosition(a_offsetData.get(), offsetX, offsetY);
				if (offsetIndex == std::numeric_limits<decltype(offsetIndex)>::max()) [[unlikely]] {
					return std::unexpected(REX::Format("Cell position ({}, {}) is out of bounds"sv, offsetX, offsetY));
				}

				if (offsetIndex > maxIndex) [[unlikely]] {
					return std::unexpected(
						REX::Format(
							"Cell position ({}, {}) has out of bounds offset index {} (max {})"sv,
							offsetX,
							offsetY,
							offsetIndex,
							maxIndex));
				}

				if (!a_worldSpace->FindCellInFile(a_plugin.get(), offsetX, offsetY)) {
					offsetBuffer.get()[offsetIndex] = std::numeric_limits<Core::CellOffsetType>::min();
					continue;
				}

				// `TESWorldSpace::FindCellInFile` will set the file offset for the provided file,
				// so that can be used to determine the relative offset of the cell.

				const auto fileOffset = a_plugin->fileOffset - a_offsetData->fileOffset;
				offsetBuffer.get()[offsetIndex] = fileOffset;
			}
		}

		return CellOffsetBufferData(offsetCount, std::move(offsetBuffer));
	}

	auto CellOffsetCreator::CreateCellOffsetsForPlugin(
		REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
		REX::NotNull<RE::TESFile*> a_plugin,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState) -> std::expected<bool, std::string>
	{
		auto* cellOffsetData = a_worldSpace->GetOffsetData(a_plugin.get());
		if (!cellOffsetData) {
			return false;
		}

		const auto* fileOffsetData = cellOffsetData->cellFileOffsets;
		if (fileOffsetData) {
			return false;
		}

		const auto fileHash = CellOffsetHasher::CreateHashForFile(a_plugin, a_hashState);
		if (!fileHash.has_value()) [[unlikely]] {
			return std::unexpected(REX::Format("Failed to create file hash: {}"sv, fileHash.error()));
		}

		const auto filePath = GetCellOffsetFilePathForPlugin(a_worldSpace, a_plugin);

		const auto loadFileError = LoadCellOffsetsFromFile(filePath, cellOffsetData, a_hashState, fileHash.value());
		if (loadFileError.value() == REX::ERROR_NUMBER_SUCCESS) {
			return true;
		}

		if (_settingsManager->GetDoTryDryRunSetting()->GetValue()) {
			return true;
		}

		auto newOffsetBuffer = GenerateCellOffsetsForPlugin(a_worldSpace.get(), a_plugin, cellOffsetData);
		if (!newOffsetBuffer.has_value()) [[unlikely]] {
			return std::unexpected(REX::Format("Failed to generate cell offsets: {}"sv, newOffsetBuffer.error()));
		}

		const auto saveFileError = SaveCellOffsetsToFile(
			filePath,
			std::move(newOffsetBuffer).value(),
			cellOffsetData,
			a_hashState,
			fileHash.value());
		if (saveFileError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return std::unexpected(
				REX::Format(R"(Failed to save cell offsets to file "{}": {})"sv, filePath, saveFileError.message()));
		}

		return true;
	}

	auto CellOffsetCreator::RunTaskForPlugin(
		const RE::TESDataHandler& a_dataHandler,
		REX::NotNull<RE::TESFile*> a_plugin,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState) -> std::expected<void, std::vector<std::string>>
	{
		const auto numericThreadId = REX::GetCurrentNumericThreadId();

		auto* threadSafeFile = a_plugin->GetThreadSafeFileForThread(numericThreadId);
		if (!threadSafeFile) [[unlikely]] {
			REX::Fail(
				R"(Failed to get thread safe file for plugin "{}" on thread {}.)"sv,
				a_plugin->filename.data(),
				numericThreadId);
		}

		auto errorMessages = std::vector<std::string>();

		for (auto* worldSpace : a_dataHandler.GetFormArray<RE::TESWorldSpace>()) {
			if (!worldSpace) {
				continue;
			}

			const auto cellOffsetResult = CreateCellOffsetsForPlugin(worldSpace, threadSafeFile, a_hashState);
			if (!cellOffsetResult.has_value()) [[unlikely]] {
				errorMessages.push_back(
					REX::Format(
						R"(Failed to create cell offsets for world space "{}": {})"sv,
						worldSpace->editorID,
						cellOffsetResult.error()));
			}

			_cellOffsetMenu->IncrementProgressCounter();
		}

		if (!a_plugin->DeleteThreadSafeFileForThread(numericThreadId)) [[unlikely]] {
			REX::Fail(
				R"(Failed to delete thread safe file for plugin "{}" on thread {}.)"sv,
				a_plugin->filename.data(),
				numericThreadId);
		}

		if (!errorMessages.empty()) {
			return std::unexpected(std::move(errorMessages));
		}

		return {};
	}
}
