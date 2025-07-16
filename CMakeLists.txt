project(H2O)
cmake_minimum_required(VERSION 2.8.12)


#
# Global compilation flags
#
set(CMAKE_CXX_COMPILER      "clang++")
set(CMAKE_CXX_FLAGS         "-std=c++11 -W -Wall -Wextra -DLINUX=1 -pthread -Wno-format-security")
set(CMAKE_CXX_FLAGS_DEBUG   "\
  -g3 \
  -O0 \
  -D_DEBUG \
  -DDEBUG \
  -DDISABLE_TERMINATE_ON_EXTENSION_CHECK_FAILURE \
  -DENABLE_AUTOMATIC_FILE_RELOAD \
  -DENABLE_EDITING \
  -DENABLE_GLDEBUGMESSAGECALLBACK \
  -DENABLE_KEYBOARD_AND_MOUSE \
  -DENABLE_LOG \
  -DENABLE_PAUSE \
  -DENABLE_PROFILING \
  -DENABLE_RUNTIME_COMPILATION \
  -DENABLE_TWEAKVAL \
  -DGFX_ENABLE_SCISSOR_TESTING \
  -DGFX_ENABLE_VERTEX_BUFFER_OFFSET \
  -DGFX_MULTI_API \
  -DHAS_EXCEPTIONS \
  -DMUTE_SOUND \
  ")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Oz -DGFX_OPENGL_ONLY")

# thirdparty are declared as system includes, to disable warnings from
# those headers.
include_directories(SYSTEM thirdparty thirdparty/unittest-cpp-1.6.0)
include_directories(src)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)


#
# Third party libraries
#
add_library(imgui STATIC
  thirdparty/imgui/imgui.cpp
  thirdparty/imgui/imgui_demo.cpp
  thirdparty/imgui/imgui_draw.cpp
  thirdparty/imgui/imgui_widgets.cpp
  )
target_compile_options(imgui PRIVATE -Wno-everything)

add_library(ImGuizmo STATIC
  thirdparty/ImGuizmo/ImGuizmo.cpp
  )
target_compile_options(ImGuizmo PRIVATE -Wno-everything)

add_library(tweakval STATIC
  thirdparty/tweakval/tweakval.cpp
  )
target_compile_options(tweakval PRIVATE -Wno-everything)

add_library(runtimecompiler STATIC
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/BuildTool.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/Compiler_PlatformPosix.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/FileChangeNotifier.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/SimpleFileWatcher/FileWatcher.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeCompiler/SimpleFileWatcher/FileWatcherLinux.cpp
  )
target_compile_options(runtimecompiler PRIVATE -Wno-everything)

add_library(runtimeobjectsystem STATIC
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectFactorySystem/ObjectFactorySystem.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModuleSource.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/RuntimeObjectSystem.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/RuntimeObjectSystem_PlatformPosix.cpp
  thirdparty/RuntimeCompiledCPlusPlus/RuntimeObjectSystem/SimpleSerializer/SimpleSerializer.cpp
  )
target_compile_options(runtimeobjectsystem PRIVATE -Wno-everything)

add_library(unittestpp STATIC
  thirdparty/unittest-cpp-1.6.0/UnitTest++/AssertException.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/Checks.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/CompositeTestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/CurrentTest.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/DeferredTestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/DeferredTestResult.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/MemoryOutStream.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/Posix/SignalTranslator.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/Posix/TimeHelpers.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/ReportAssert.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/RequiredCheckException.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/RequiredCheckTestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/Test.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestDetails.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestList.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestReporterStdout.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestResults.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TestRunner.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/ThrowingTestReporter.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/TimeConstraint.cpp
  thirdparty/unittest-cpp-1.6.0/UnitTest++/XmlTestReporter.cpp
  )
target_compile_options(unittestpp PRIVATE -Wno-everything)

add_library(64klang2_Player STATIC
  thirdparty/64klang2_Player/MusicPlayer64k2.cpp
)
target_compile_options(64klang2_Player PRIVATE -Wno-everything)


#
# Static library: gfx
#
add_library(gfx STATIC
  src/gfx/Helpers.cpp
  src/gfx/IGraphicLayer.cpp
  src/gfx/OpenGL/Extensions.cpp
  src/gfx/OpenGL/OpenGLLayer.cpp
  src/gfx/OpenGL/OpenGLTypeConversion.cpp
  src/gfx/ResourceID.cpp
  src/gfx/ShadingParameters.cpp
  )


#
# Static library: engine
#
add_library(engine STATIC
  src/engine/algebra/Functions.cpp
  src/engine/algebra/Matrix4.cpp
  src/engine/algebra/Quaternion.cpp
  src/engine/algebra/Spline.cc
  src/engine/algebra/Vector2.cpp
  src/engine/algebra/Vector3.cpp
  src/engine/algebra/Vector4.cpp
  src/engine/core/Assert.cpp
  src/engine/core/Debug.cpp
  src/engine/core/DebugSettings.cpp
  src/engine/core/FileIO.cpp
  src/engine/core/Log.cpp
  src/engine/core/RigidTransform.cpp
  src/engine/core/Settings.cpp
  src/engine/core/StringUtils.cpp
  src/engine/core/msys_temp.cpp
  src/engine/editing/FileWatcher.cpp
  src/engine/editing/ImGuiHelper.cpp
  src/engine/editing/Preprocessor.cpp
  src/engine/editing/RuntimeCompiler.cpp
  src/engine/mesh/Cube.cpp
  src/engine/mesh/HeightMap.cpp
  src/engine/mesh/Marching.cpp
  src/engine/mesh/Mesh.cpp
  src/engine/mesh/MeshPool.cpp
  src/engine/mesh/Revolution.cpp
  src/engine/mesh/RevolutionFunctions.cpp
  src/engine/mesh/Utils.cpp
  src/engine/noise/Gradient.cpp
  src/engine/noise/Hash.cpp
  src/engine/noise/LatticeNoise.cpp
  src/engine/noise/PerlinNoise.cpp
  src/engine/noise/Rand.cpp
  src/engine/noise/SimplexNoise.cpp
  src/engine/noise/SimplexValueNoise.cpp
  src/engine/noise/Utils.cpp
  src/engine/noise/ValueNoise.cpp
  src/engine/noise/WorleyNoise.cpp
  src/engine/profiling/FrameLogger.cpp
  src/engine/profiling/ScopeLogger.cpp
  src/engine/render/FrameBuffer.cpp
  src/engine/sound/MusicPlayerBASS.cpp
  src/engine/texture/Texture.cpp
  src/engine/texture/Utils.cpp
  src/engine/timeline/Clock.cpp
  src/engine/timeline/Timeline.cpp
  src/engine/timeline/Variable.cpp
  src/engine/world/Camera.cpp
  src/engine/world/Node.cpp
  )


#
# Static library: platform
#
add_library(platform STATIC
  src/platform/MultiThreading.cpp
  src/platform/XLibPlatform.cpp
  )


#
# Executable: unittests
#
add_executable(unittests
  src/unittests/Algorithm.cpp
  src/unittests/Quaternion.cpp
  src/unittests/Vector2.cpp
  src/unittests/Vector3.cpp
  src/unittests/Vector4.cpp
  src/unittests/main.cpp
  )
target_link_libraries(unittests engine unittestpp)


#
# Executable: example01
#
add_executable(example01
  src/example01/main.cpp
  )
target_link_libraries(example01 gfx engine platform glfw GL GLX dl)


#
# Executable: example02
#
add_executable(example02
  src/example02/Example02.cpp
  src/example02/main.cpp
  src/example02/meshes/Example02Meshes.cpp
  src/meshes/Quad.cpp
  src/tool/CameraLoader.cpp
  src/tool/DebugControls.cpp
  src/tool/DebugGraphicLayer.cpp
  src/tool/MeshLoader.cpp
  src/tool/ProjectLoader.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataP2dT.cpp
  src/tool/mesh/VertexDataPNT.cpp
  )
target_compile_options(example02 PUBLIC -DPROJECT_DIRECTORY=example02)
target_link_libraries(example02 gfx engine platform boost_filesystem boost_system imgui ImGuizmo tweakval runtimeobjectsystem runtimecompiler glfw GL GLX dl)

#
# Executable: h
#
add_executable(h
  src/H/BoatScene.cpp
  src/H/CityScene.cpp
  src/H/DescentScene.cpp
  src/H/EmergenceScene.cpp
  src/H/H.cpp
  src/H/InsideTempleScene.cpp
  src/H/LoadingBar.cpp
  src/H/RuinsScene.cpp
  src/H/TheCity.cpp
  src/H/TheSubmersible.cpp
  src/H/ViaductScene.cpp
  src/H/main.cpp
  src/H/meshes/Artemision.cpp
  src/H/meshes/CityFloor.cpp
  src/H/meshes/Fence.cpp
  src/H/meshes/Fountain.cpp
  src/H/meshes/GreekColumn.cpp
  src/H/meshes/Houses.cpp
  src/H/meshes/Obelisk.cpp
  src/H/meshes/Parthenon.cpp
  src/H/meshes/Plant.cpp
  src/H/meshes/PoseidonSeated.cpp
  src/H/meshes/ResearchVessel.cpp
  src/H/meshes/Road.cpp
  src/H/meshes/Rocks.cpp
  src/H/meshes/RuinsFloor.cpp
  src/H/meshes/SeaSurface.cpp
  src/H/meshes/Seagull.cpp
  src/H/meshes/Stairs.cpp
  src/H/meshes/Statue.cpp
  src/H/meshes/Submersible.cpp
  src/H/meshes/Tholos.cpp
  src/H/meshes/Trident.cpp
  src/H/meshes/UnderwaterFloor.cpp
  src/H/meshes/Viaduct.cpp
  src/H/textures/Cobbles.cpp
  src/H/textures/GreekColumn.cpp
  src/H/textures/Ground.cpp
  src/H/textures/Marble.cpp
  src/H/textures/OldStone.cpp
  src/H/textures/ResearchVessel.cpp
  src/H/textures/SeaFoam.cpp
  src/meshes/Quad.cpp
  src/postprocessing/BilateralGaussianBlur.cpp
  src/postprocessing/CannyEdgeDetector.cpp
  src/postprocessing/ChromaHoop.cpp
  src/postprocessing/CircleOfConfusion.cpp
  src/postprocessing/CombineDoF.cpp
  src/postprocessing/CommonPostProcessing.cpp
  src/postprocessing/FXAA.cpp
  src/postprocessing/FinalCombine.cpp
  src/postprocessing/KawaseGaussianBlur.cpp
  src/postprocessing/KawaseStreakBlur.cpp
  src/postprocessing/LensScratch.cpp
  src/postprocessing/SeparableBoxDilation.cpp
  src/postprocessing/SeparableCircularDoF.cpp
  src/postprocessing/SeparableGaussianBlur.cpp
  src/postprocessing/ToneMapping.cpp
  src/textures/RandomTexture.cpp
  src/tool/CameraLoader.cpp
  src/tool/CommonMaterial.cpp
  src/tool/DebugControls.cpp
  src/tool/DebugGraphicLayer.cpp
  src/tool/FileExporter.cpp
  src/tool/FixedLightToolBox.cpp
  src/tool/InterleavedTiles.cpp
  src/tool/MeshLoader.cpp
  src/tool/ParticleField.cpp
  src/tool/ProjectLoader.cpp
  src/tool/RenderList.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/SimpleSkyGradient.cpp
  src/tool/TextureLoader.cpp
  src/tool/VolumetricLighting.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataP2dT.cpp
  src/tool/mesh/VertexDataPNT.cpp
  src/tool/mesh/VertexDataPNTT.cpp
  )
target_compile_options(h PUBLIC -DPROJECT_DIRECTORY=H)
target_link_libraries(h gfx engine platform boost_filesystem boost_system imgui ImGuizmo tweakval runtimeobjectsystem runtimecompiler glfw GL dl)


#
# Executable: i
#
add_executable(i
  src/I/ChunnelScene.cpp
  src/I/FactoryScene.cpp
  src/I/FinalScene.cpp
  src/I/HallScene.cpp
  src/I/I.cpp
  src/I/LoadingBar.cpp
  src/I/Midisync.cc
  src/I/PipeTunnelScene.cpp
  src/I/main.cpp
  src/I/meshes/AbandonedFactory.cpp
  src/I/meshes/Chunnel.cpp
  src/I/meshes/PipeTunnel.cpp
  src/I/textures/AbandonedFactory.cpp
  src/I/textures/Stone.cpp
  src/meshes/Quad.cpp
  src/postprocessing/BilateralGaussianBlur.cpp
  src/postprocessing/CannyEdgeDetector.cpp
  src/postprocessing/CommonPostProcessing.cpp
  src/postprocessing/FXAA.cpp
  src/postprocessing/FinalCombine.cpp
  src/postprocessing/KawaseStreakBlur.cpp
  src/postprocessing/SeparableGaussianBlur.cpp
  src/tool/CameraLoader.cpp
  src/tool/CommonMaterial.cpp
  src/tool/DebugControls.cpp
  src/tool/DebugGraphicLayer.cpp
  src/tool/FixedLightToolBox.cpp
  src/tool/InterleavedTiles.cpp
  src/tool/IrradianceMap.cpp
  src/tool/MeshLoader.cpp
  src/tool/ParticleField.cpp
  src/tool/ProjectLoader.cpp
  src/tool/RenderList.cpp
  src/tool/Server.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/TextureLoader.cpp
  src/tool/VolumetricLighting.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataP2dT.cpp
  src/tool/mesh/VertexDataPNT.cpp
  src/tool/mesh/VertexDataPNTT.cpp
  )
target_compile_options(i PUBLIC -DPROJECT_DIRECTORY=I)
target_link_libraries(i gfx engine platform boost_filesystem imgui ImGuizmo tweakval runtimeobjectsystem runtimecompiler dl)


#
# Executable: meshplayground
#
add_executable(meshplayground
  src/example02/meshes/Example02Meshes.cpp
  src/H/meshes/Artemision.cpp
  src/H/meshes/CityFloor.cpp
  src/H/meshes/Fence.cpp
  src/H/meshes/Fountain.cpp
  src/H/meshes/GreekColumn.cpp
  src/H/meshes/Houses.cpp
  src/H/meshes/Obelisk.cpp
  src/H/meshes/Parthenon.cpp
  src/H/meshes/Plant.cpp
  src/H/meshes/PoseidonSeated.cpp
  src/H/meshes/ResearchVessel.cpp
  src/H/meshes/Road.cpp
  src/H/meshes/Rocks.cpp
  src/H/meshes/RuinsFloor.cpp
  src/H/meshes/SeaSurface.cpp
  src/H/meshes/Seagull.cpp
  src/H/meshes/Stairs.cpp
  src/H/meshes/Statue.cpp
  src/H/meshes/Submersible.cpp
  src/H/meshes/Tholos.cpp
  src/H/meshes/Trident.cpp
  src/H/meshes/UnderwaterFloor.cpp
  src/H/meshes/Viaduct.cpp
  src/H/textures/Cobbles.cpp
  src/H/textures/GreekColumn.cpp
  src/H/textures/Ground.cpp
  src/H/textures/Marble.cpp
  src/H/textures/OldStone.cpp
  src/H/textures/ResearchVessel.cpp
  src/H/textures/SeaFoam.cpp
  src/I/meshes/AbandonedFactory.cpp
  src/I/meshes/Chunnel.cpp
  src/I/meshes/PipeTunnel.cpp
  src/I/textures/AbandonedFactory.cpp
  src/I/textures/Stone.cpp
  src/meshes/DebugMeshes.cpp
  src/meshes/Dolphin.cpp
  src/meshes/ElderManStatue.cpp
  src/meshes/Gate.cpp
  src/meshes/Lightning.cpp
  src/meshes/PlaygroundMesh.cpp
  src/meshes/Quad.cpp
  src/meshes/Rock.cpp
  src/meshes/SdfTest.cpp
  src/meshes/Shark.cpp
  src/meshplayground/OrbitCameraControls.cpp
  src/meshplayground/main.cpp
  src/postprocessing/CommonPostProcessing.cpp
  src/postprocessing/FXAA.cpp
  src/postprocessing/FinalCombine.cpp
  src/postprocessing/ToneMapping.cpp
  src/textures/RandomTexture.cpp
  src/tool/CommonMaterial.cpp
  src/tool/InterleavedTiles.cpp
  src/tool/MeshLoader.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/TextureLoader.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataP2dT.cpp
  src/tool/mesh/VertexDataPNT.cpp
  src/tool/mesh/VertexDataPNTT.cpp
  )
target_link_libraries(meshplayground gfx engine platform boost_filesystem imgui ImGuizmo tweakval runtimeobjectsystem runtimecompiler GL GLU X11 dl)


#
# Executable: sandbox
#
add_executable(sandbox
  src/H/meshes/Artemision.cpp
  src/H/meshes/Fence.cpp
  src/H/meshes/GreekColumn.cpp
  src/H/meshes/Parthenon.cpp
  src/H/meshes/PoseidonSeated.cpp
  src/H/meshes/Trident.cpp
  src/H/meshes/Stairs.cpp
  src/meshes/Dolphin.cpp
  src/meshes/Shark.cpp
  src/postprocessing/BilateralGaussianBlur.cpp
  src/postprocessing/CommonPostProcessing.cpp
  src/postprocessing/FXAA.cpp
  src/postprocessing/FinalCombine.cpp
  src/sandbox/Attractor.cpp
  src/sandbox/CameraTest.cpp
  src/sandbox/CubeGrid.cpp
  src/sandbox/CubeMapTest.cpp
  src/sandbox/Fountain.cpp
  src/sandbox/NodesTest.cpp
  src/sandbox/SeaSurface.cpp
  src/sandbox/TestScene.cpp
  src/sandbox/TextureShelf.cpp
  src/sandbox/main.cpp
  src/textures/GreekTiles.cpp
  src/textures/NoiseTextures.cpp
  src/textures/RCCTest.cpp
  src/textures/RandomTexture.cpp
  src/tool/CameraLoader.cpp
  src/tool/CommonMaterial.cpp
  src/tool/DebugControls.cpp
  src/tool/DebugGraphicLayer.cpp
  src/tool/FixedLightToolBox.cpp
  src/tool/MeshLoader.cpp
  src/tool/ProjectLoader.cpp
  src/tool/RenderList.cpp
  src/tool/ShaderHelper.cpp
  src/tool/ShaderLoader.cpp
  src/tool/ShaderMinifier.cpp
  src/tool/TextureLoader.cpp
  src/tool/VolumetricLighting.cpp
  src/tool/mesh/MeshHelper.cpp
  src/tool/mesh/VertexDataP2dT.cpp
  src/tool/mesh/VertexDataPN.cpp
  src/tool/mesh/VertexDataPNT.cpp
  src/tool/mesh/VertexDataPNTT.cpp
#  src/tool/Server.cpp
  )
target_link_libraries(sandbox gfx engine platform boost_filesystem boost_system imgui ImGuizmo tweakval runtimeobjectsystem runtimecompiler GL GLU X11 dl)
