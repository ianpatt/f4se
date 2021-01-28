cmake_minimum_required(VERSION 3.18)

# ---- Project ----

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/versioning.cmake)

project(
	f4se
	VERSION ${F4SE_VERSION_MAJOR}.${F4SE_VERSION_MINOR}.${F4SE_VERSION_PATCH}
	LANGUAGES CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
			"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
)
endif()

# ---- Build options ----

option(F4SE_STATIC_LIB OFF "Choose static linkage for f4se's add_library command. Useful for downstream targets.")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON CACHE BOOL "")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF CACHE BOOL "")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "")

# ---- Dependencies ----

if (NOT TARGET f4se_common)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../f4se_common f4se_common)	# bundled
endif()

if (NOT TARGET xbyak)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../xbyak xbyak)	# bundled
endif()

if (NOT TARGET common)
	find_package(common REQUIRED CONFIG)
endif()

# ---- Add source files ----

include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)

source_group(
	${PROJECT_NAME}
	FILES
		exports.def
		f4se.cpp
)

source_group(
	${PROJECT_NAME}/api
	FILES
		GameAPI.cpp
		GameAPI.h
		GameCamera.cpp
		GameCamera.h
		GameCustomization.cpp
		GameCustomization.h
		GameData.cpp
		GameData.h
		GameEvents.cpp
		GameEvents.h
		GameExtraData.cpp
		GameExtraData.h
		GameFormComponents.cpp
		GameFormComponents.h
		GameForms.cpp
		GameForms.h
		GameHandle.cpp
		GameHandle.h
		GameInput.cpp
		GameInput.h
		GameMemory.cpp
		GameMemory.h
		GameMenus.cpp
		GameMenus.h
		GameMessages.cpp
		GameMessages.h
		GameObjects.cpp
		GameObjects.h
		GameReferences.cpp
		GameReferences.h
		GameRTTI.cpp
		GameRTTI.h
		GameRTTI.inl
		GameSettings.cpp
		GameSettings.h
		GameStreams.cpp
		GameStreams.h
		GameThreads.cpp
		GameThreads.h
		GameTypes.cpp
		GameTypes.h
		GameUtilities.cpp
		GameUtilities.h
		GameWorkshop.cpp
		GameWorkshop.h
)

source_group(
	${PROJECT_NAME}/hooks
	FILES
		Hooks_Camera.cpp
		Hooks_Camera.h
		Hooks_Debug.cpp
		Hooks_Debug.h
		Hooks_GameData.cpp
		Hooks_GameData.h
		Hooks_Gameplay.cpp
		Hooks_Gameplay.h
		Hooks_Input.cpp
		Hooks_Input.h
		Hooks_Memory.cpp
		Hooks_Memory.h
		Hooks_ObScript.cpp
		Hooks_ObScript.h
		Hooks_Papyrus.cpp
		Hooks_Papyrus.h
		Hooks_SaveLoad.cpp
		Hooks_SaveLoad.h
		Hooks_Scaleform.cpp
		Hooks_Scaleform.h
		Hooks_Threads.cpp
		Hooks_Threads.h
)

source_group(
	${PROJECT_NAME}/internal
	FILES
		CustomMenu.cpp
		CustomMenu.h
		InputMap.cpp
		InputMap.h
		InternalSerialization.cpp
		InternalSerialization.h
		PluginAPI.h
		PluginManager.cpp
		PluginManager.h
		Serialization.cpp
		Serialization.h
		Translation.cpp
		Translation.h
)

source_group(
	${PROJECT_NAME}/netimmerse
	FILES
		bhkWorld.cpp
		bhkWorld.h
		BSCollision.cpp
		BSCollision.h
		BSGeometry.cpp
		BSGeometry.h
		BSGraphics.cpp
		BSGraphics.h
		BSLight.cpp
		BSLight.h
		BSModelDB.cpp
		BSModelDB.h
		BSParticleShaderEmitter.cpp
		BSParticleShaderEmitter.h
		BSSkin.cpp
		BSSkin.h
		NiCloningProcess.h
		NiExtraData.cpp
		NiExtraData.h
		NiMaterials.cpp
		NiMaterials.h
		NiNodes.cpp
		NiNodes.h
		NiObjects.cpp
		NiObjects.h
		NiProperties.cpp
		NiProperties.h
		NiRTTI.cpp
		NiRTTI.h
		NiSerialization.cpp
		NiSerialization.h
		NiTextures.cpp
		NiTextures.h
		NiTypes.cpp
		NiTypes.h
)

source_group(
	${PROJECT_NAME}/obscript
	FILES
		ObScript.cpp
		ObScript.h
)

source_group(
	${PROJECT_NAME}/papyrus/functions
	FILES
		PapyrusActor.cpp
		PapyrusActor.h
		PapyrusActorBase.cpp
		PapyrusActorBase.h
		PapyrusArmor.cpp
		PapyrusArmor.h
		PapyrusArmorAddon.cpp
		PapyrusArmorAddon.h
		PapyrusCell.cpp
		PapyrusCell.h
		PapyrusComponent.cpp
		PapyrusComponent.h
		PapyrusConstructibleObject.cpp
		PapyrusConstructibleObject.h
		PapyrusDefaultObject.cpp
		PapyrusDefaultObject.h
		PapyrusEncounterZone.cpp
		PapyrusEncounterZone.h
		PapyrusEquipSlot.cpp
		PapyrusEquipSlot.h
		PapyrusF4SE.cpp
		PapyrusF4SE.h
		PapyrusFavoritesManager.cpp
		PapyrusFavoritesManager.h
		PapyrusForm.cpp
		PapyrusForm.h
		PapyrusGame.cpp
		PapyrusGame.h
		PapyrusHeadPart.cpp
		PapyrusHeadPart.h
		PapyrusInput.cpp
		PapyrusInput.h
		PapyrusInstanceData.cpp
		PapyrusInstanceData.h
		PapyrusLocation.cpp
		PapyrusLocation.h
		PapyrusMaterialSwap.cpp
		PapyrusMaterialSwap.h
		PapyrusMath.cpp
		PapyrusMath.h
		PapyrusMiscObject.cpp
		PapyrusMiscObject.h
		PapyrusObjectMod.cpp
		PapyrusObjectMod.h
		PapyrusObjectReference.cpp
		PapyrusObjectReference.h
		PapyrusPerk.cpp
		PapyrusPerk.h
		PapyrusScriptObject.cpp
		PapyrusScriptObject.h
		PapyrusUI.cpp
		PapyrusUI.h
		PapyrusUtility.cpp
		PapyrusUtility.h
		PapyrusWaterType.cpp
		PapyrusWaterType.h
		PapyrusWeapon.cpp
		PapyrusWeapon.h
)

source_group(
	${PROJECT_NAME}/papyrus/vm
	FILES
		PapyrusArgs.cpp
		PapyrusArgs.h
		PapyrusDelayFunctors.cpp
		PapyrusDelayFunctors.h
		PapyrusDelayFunctorsDef.inl
		PapyrusDelayFunctorsDef_Base.inl
		PapyrusEvents.cpp
		PapyrusEvents.h
		PapyrusEventsDef.inl
		PapyrusEventsDef_Base.inl
		PapyrusInterfaces.cpp
		PapyrusInterfaces.h
		PapyrusNativeFunctionDef.inl
		PapyrusNativeFunctionDef_Base.inl
		PapyrusNativeFunctions.cpp
		PapyrusNativeFunctions.h
		PapyrusObjects.cpp
		PapyrusObjects.h
		PapyrusScaleformAdapter.cpp
		PapyrusScaleformAdapter.h
		PapyrusSerialization.cpp
		PapyrusSerialization.h
		PapyrusStruct.cpp
		PapyrusStruct.h
		PapyrusUtilities.cpp
		PapyrusUtilities.h
		PapyrusValue.cpp
		PapyrusValue.h
		PapyrusVM.cpp
		PapyrusVM.h
)

source_group(
	${PROJECT_NAME}/scaleform
	FILES
		ScaleformAPI.cpp
		ScaleformAPI.h
		ScaleformCallbacks.cpp
		ScaleformCallbacks.h
		ScaleformLoader.cpp
		ScaleformLoader.h
		ScaleformMovie.cpp
		ScaleformMovie.h
		ScaleformSerialization.cpp
		ScaleformSerialization.h
		ScaleformState.cpp
		ScaleformState.h
		ScaleformTranslator.cpp
		ScaleformTranslator.h
		ScaleformTypes.cpp
		ScaleformTypes.h
		ScaleformValue.cpp
		ScaleformValue.h
)

# ---- Create library ----

if (F4SE_STATIC_LIB)
	set(F4SE_LINKAGE "STATIC")
else()
	set(F4SE_LINKAGE "SHARED")
endif()

add_library(
	${PROJECT_NAME}
	${F4SE_LINKAGE}
	${headers}
	${sources}
	exports.def
	${f4se_common_SOURCE_DIR}/f4se_version.rc
)

add_library(f4se::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/configuration.cmake)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
		OUTPUT_NAME "${PROJECT_NAME}_${F4_VERSION_MAJOR}_${F4_VERSION_MINOR}_${F4_VERSION_PATCH}"
)

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		RUNTIME
		RUNTIME_VERSION=${F4_VERSION_PACKED}
)

target_compile_features(
	${PROJECT_NAME}
	PUBLIC
		cxx_std_11
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
		$<INSTALL_INTERFACE:include>
)

target_link_libraries(
	${PROJECT_NAME}
	PUBLIC
		f4se::f4se_common
		Xbyak::xbyak
		xse::common
)

# ---- Create an installable target ----

include(GNUInstallDirs)

install(
	TARGETS
		${PROJECT_NAME}
	EXPORT
		${PROJECT_NAME}-targets
)

install(
	FILES
		${headers}
	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

configure_file(
	cmake/config.cmake.in
	${PROJECT_NAME}-config.cmake
	@ONLY
)

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
	EXPORT
		${PROJECT_NAME}-targets
	NAMESPACE
		f4se::
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/installation.cmake)
