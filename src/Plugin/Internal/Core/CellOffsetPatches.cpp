#include "Plugin/Internal/Core/CellOffsetPatches.hpp"

/**
 * The quick cell offset search only works for plugins with a master or optimized flag,
 * so the below patches disable those checks to allow normal plugins to work as well.
 */
namespace Plugin::Internal::Core::CellOffsetPatches
{
	class NopPatchInfo final
	{
	public:
		F4SE::VariantId id;
		F4SE::VariantOffset offset;
		F4SE::VariantSize size;
	};

	class InstructionPatchInfo final
	{
	public:
		F4SE::VariantId id;
		F4SE::VariantOffset offset;
		F4SE::VariantValue<std::span<const std::uint8_t>> instruction;
	};

	static constexpr auto JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL = static_cast<std::size_t>(0x02);
	static constexpr auto JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE = static_cast<std::size_t>(0x06);

	// clang-format off
	static constexpr auto MASTER_FLAG_NOP_PATCHES = std::array{
		NopPatchInfo{
			.id = RE::ID::TESWorldSpace::Load,
			.offset = F4SE::CreateVariantOffset(0x05E, 0x05E, 0x05E),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL)
		},
		NopPatchInfo{
			.id = RE::ID::TESWorldSpace::Load,
			.offset = F4SE::CreateVariantOffset(0x1B6, 0x1A4, 0x1A4),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE, JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE, JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE)
		},
		NopPatchInfo{
			.id = RE::ID::TESWorldSpace::Load,
			.offset = F4SE::CreateVariantOffset(0x2D4, 0x351, 0x351),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE, JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE, JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE)
		},
		NopPatchInfo{
			.id = RE::ID::TESWorldSpace::LoadPartial,
			.offset = F4SE::CreateVariantOffset(0x134, 0x10E, 0x10E),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL)
		},
		NopPatchInfo{
			.id = RE::ID::TESWorldSpace::LoadPartial,
			.offset = F4SE::CreateVariantOffset(0x0BF, 0x0AB, 0x0AB),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL)
		},
		NopPatchInfo{
			.id = RE::ID::TESWorldSpace::PrefetchCellData,
			.offset = F4SE::CreateVariantOffset(0x06E, 0x06E, 0x06E),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL)
		},
		NopPatchInfo{
			.id = RE::ID::TESWorldSpace::FindCellInFile,
			.offset = F4SE::CreateVariantOffset(0x026, 0x026, 0x026),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL)
		},
		NopPatchInfo{
			.id = RE::ID::TESWorldSpace::FindInFileFast,
			.offset = F4SE::CreateVariantOffset(0x02C, 0x02C, 0x02C),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL)
		},
		NopPatchInfo{
			.id = RE::ID::TESObjectCELL::FindInFileFast,
			.offset = F4SE::CreateVariantOffset(0x01F, 0x023, 0x023),
			.size = F4SE::CreateVariantSize(
				JUMP_IF_ZERO_INSTRUCTION_SIZE_SMALL, JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE, JUMP_IF_ZERO_INSTRUCTION_SIZE_LARGE)
		}
	};
	// clang-format on

	/**
	 * Pre-Next-Gen:
	 * Old: JNZ +0x8E (0x0F 0x85 0x8E 0x00 0x00 0x00)
	 * New: JMP +0x8F (0xE9 0x8F 0x00 0x00 0x00) + NOP (0x90)
	 *
	 * Post-Next-Gen:
	 * Old: JNZ +0x96 (0x0F 0x85 0x96 0x00 0x00 0x00)
	 * New: JMP +0x97 (0xE9 0x97 0x00 0x00 0x00) + NOP (0x90)
	 */
	// clang-format off
	static constexpr auto TES_DATA_HANDLER__GET_EXT_CELL_DATA_FROM_FILE_BY_EDITOR_ID__NEW_INSTRUCTION = F4SE::CreateVariantValue(
		std::array{ 0xE9ui8, 0x8Fui8, 0x00ui8, 0x00ui8, 0x00ui8, REL::NOP },
		std::array{ 0xE9ui8, 0x97ui8, 0x00ui8, 0x00ui8, 0x00ui8, REL::NOP },
		std::array{ 0xE9ui8, 0x97ui8, 0x00ui8, 0x00ui8, 0x00ui8, REL::NOP }
	);
	// clang-format on

	// clang-format off
	static constexpr auto OPTIMIZED_FLAG_INSTRUCTION_PATCHES = std::array{
		InstructionPatchInfo{
			.id = RE::ID::TESDataHandler::GetExtCellDataFromFileByEditorID,
			.offset = F4SE::CreateVariantOffset(0x123, 0x2B1, 0x2B1),
			.instruction = TES_DATA_HANDLER__GET_EXT_CELL_DATA_FROM_FILE_BY_EDITOR_ID__NEW_INSTRUCTION },
	};
	// clang-format on

	static auto HookCollection = REX::NotAssignable<std::vector<REX::NotNull<std::shared_ptr<REL::IHook>>>>();

	void OnXseLoad(REL::HookStore& hookStore)
	{
		for (auto i = static_cast<std::size_t>(0); i < MASTER_FLAG_NOP_PATCHES.size(); i++) {
			const auto& nopPatch = MASTER_FLAG_NOP_PATCHES[i];

			auto nopHookName = REX::Format("NopMasterFlagHook_{}"sv, i);
			auto nopHook = REX::NotNull(std::make_shared<REL::HookNop>(
				std::move(nopHookName), nopPatch.id, nopPatch.offset, nopPatch.size));

			hookStore.Add(nopHook);
			HookCollection->emplace_back(std::move(nopHook));
		}

		for (auto i = static_cast<std::size_t>(0); i < OPTIMIZED_FLAG_INSTRUCTION_PATCHES.size(); i++) {
			const auto& instructionPatch = OPTIMIZED_FLAG_INSTRUCTION_PATCHES[i];

			auto instructionHookName = REX::Format("OptimizedFlagInstructionHook_{}"sv, i);
			auto instructionHook = REX::NotNull(std::make_shared<REL::HookRepl>(
				std::move(instructionHookName), instructionPatch.id, instructionPatch.offset, instructionPatch.instruction.GetValueOrDefault()));

			hookStore.Add(instructionHook);
			HookCollection->emplace_back(std::move(instructionHook));
		}
	}

	void Enable()
	{
		for (const auto& hook : *HookCollection) {
			hook->Enable();
		}
	}

	void Disable()
	{
		for (const auto& hook : *HookCollection) {
			hook->Disable();
		}
	}
}
