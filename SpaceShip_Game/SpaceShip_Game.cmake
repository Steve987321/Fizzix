add_library("SpaceShip_Game" SHARED
      "vendor/imgui/imgui.cpp"
      "vendor/imgui/imgui_demo.cpp"
      "vendor/imgui/imgui_draw.cpp"
      "vendor/imgui/imgui_tables.cpp"
      "vendor/imgui/imgui_widgets.cpp"
      "vendor/implot/implot_items.cpp"
      "vendor/implot/implot.cpp"
      "vendor/sfml-imgui/imgui-SFML.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZCollission.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZCollission.h"
        "SpaceShip_Game/src/assets/Fizzix/FZMath.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZMath.h"
        "SpaceShip_Game/src/assets/Fizzix/FZMisc.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZMisc.h"
        "SpaceShip_Game/src/assets/Fizzix/FZPolygon.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZPolygon.h"
        "SpaceShip_Game/src/assets/Fizzix/FZRigidbody.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZRigidbody.h"
        "SpaceShip_Game/src/assets/Fizzix/FZSpring.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZSpring.h"
        "SpaceShip_Game/src/assets/Fizzix/FZThruster.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZThruster.h"
        "SpaceShip_Game/src/assets/Fizzix/FZTypes.h"
        "SpaceShip_Game/src/assets/Fizzix/FZWorld.cpp"
        "SpaceShip_Game/src/assets/Fizzix/FZWorld.h"
        "SpaceShip_Game/src/assets/SimEnvironments/CarEnvironment.cpp"
        "SpaceShip_Game/src/assets/SimEnvironments/CarEnvironment.h"
        "SpaceShip_Game/src/assets/SimEnvironments/RocketEnvironment.cpp"
        "SpaceShip_Game/src/assets/SimEnvironments/RocketEnvironment.h"
        "SpaceShip_Game/src/assets/SimEnvironments/SandBoxEnvironment.cpp"
        "SpaceShip_Game/src/assets/SimEnvironments/SandBoxEnvironment.h"
        "SpaceShip_Game/src/assets/SimEnvironments/StressTestEnvironment.cpp"
        "SpaceShip_Game/src/assets/SimEnvironments/StressTestEnvironment.h"
          "SpaceShip_Game/src/assets/Toot/Compiler/Compiler.cpp"
          "SpaceShip_Game/src/assets/Toot/Compiler/Compiler.h"
          "SpaceShip_Game/src/assets/Toot/Compiler/Lexer.cpp"
          "SpaceShip_Game/src/assets/Toot/Compiler/Lexer.h"
          "SpaceShip_Game/src/assets/Toot/Compiler/Parser.cpp"
          "SpaceShip_Game/src/assets/Toot/Compiler/Parser.h"
            "SpaceShip_Game/src/assets/Toot/TVM/Lib/CPPBinding.cpp"
            "SpaceShip_Game/src/assets/Toot/TVM/Lib/CPPBinding.h"
            "SpaceShip_Game/src/assets/Toot/TVM/Lib/IO.cpp"
            "SpaceShip_Game/src/assets/Toot/TVM/Lib/IO.h"
            "SpaceShip_Game/src/assets/Toot/TVM/Lib/SimLib.cpp"
            "SpaceShip_Game/src/assets/Toot/TVM/Lib/SimLib.h"
          "SpaceShip_Game/src/assets/Toot/TVM/TVM.cpp"
          "SpaceShip_Game/src/assets/Toot/TVM/TVM.h"
        "SpaceShip_Game/src/assets/UI/FizzixMenu.cpp"
        "SpaceShip_Game/src/assets/UI/FizzixMenu.h"
        "SpaceShip_Game/src/assets/UI/TVMMenu.cpp"
        "SpaceShip_Game/src/assets/UI/TVMMenu.h"
        "SpaceShip_Game/src/assets/framework/Framework.cpp"
        "SpaceShip_Game/src/assets/framework/Framework.h"
        "SpaceShip_Game/src/assets/scripts/Sim.cpp"
        "SpaceShip_Game/src/assets/scripts/Sim.h"
      "SpaceShip_Game/src/game_core/Game.cpp"
      "SpaceShip_Game/src/game_core/Game.h"
      "SpaceShip_Game/src/game_core/ScriptRegister.cpp"
      "SpaceShip_Game/src/game_core/ScriptRegister.h"
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("SpaceShip_Game" PROPERTIES
    OUTPUT_NAME "SpaceShip_Game"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Release-macosx-x86_64
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Release-macosx-x86_64
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Release-macosx-x86_64
  )
endif()
if(APPLE)
find_library(OpenGL_FRAMEWORK OpenGL)
find_library(Cocoa_FRAMEWORK Cocoa)
find_library(IOKit_FRAMEWORK IOKit)
find_library(CoreVideo_FRAMEWORK CoreVideo)
endif()
target_include_directories("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/engine/src>
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor>
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/magic_enum/include>
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/imgui>
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/SFML-3.0.0/include>
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/json/include>
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/src>
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/src/assets>
)
target_compile_definitions("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Release>:NDEBUG>
  $<$<CONFIG:Release>:_USRDLL>
  $<$<CONFIG:Release>:GAME_IS_EXPORT>
)
target_link_directories("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/bin/Release-macosx-x86_64>
  $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../vendor/SFML-3.0.0/lib>
)
target_link_libraries("SpaceShip_Game"
  $<$<CONFIG:Release>:Engine>
  $<$<CONFIG:Release>:${OpenGL_FRAMEWORK}>
  $<$<CONFIG:Release>:${Cocoa_FRAMEWORK}>
  $<$<CONFIG:Release>:${IOKit_FRAMEWORK}>
  $<$<CONFIG:Release>:${CoreVideo_FRAMEWORK}>
  $<$<CONFIG:Release>:sfml-system>
  $<$<CONFIG:Release>:sfml-window>
  $<$<CONFIG:Release>:sfml-graphics>
  $<$<CONFIG:Release>:sfml-audio>
)
target_compile_options("SpaceShip_Game" PRIVATE
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties("SpaceShip_Game" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("SpaceShip_Game" PROPERTIES
    OUTPUT_NAME "SpaceShip_Game"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Debug-macosx-x86_64
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Debug-macosx-x86_64
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Debug-macosx-x86_64
  )
endif()
if(APPLE)
find_library(OpenGL_FRAMEWORK OpenGL)
find_library(Cocoa_FRAMEWORK Cocoa)
find_library(IOKit_FRAMEWORK IOKit)
find_library(CoreVideo_FRAMEWORK CoreVideo)
endif()
target_include_directories("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/engine/src>
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor>
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/magic_enum/include>
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/imgui>
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/SFML-3.0.0/include>
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/json/include>
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/src>
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/src/assets>
)
target_compile_definitions("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Debug>:_DEBUG>
  $<$<CONFIG:Debug>:GAME_IS_EXPORT>
)
target_link_directories("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/bin/Release-macosx-x86_64>
  $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../vendor/SFML-3.0.0/lib>
)
target_link_libraries("SpaceShip_Game"
  $<$<CONFIG:Debug>:Engine>
  $<$<CONFIG:Debug>:${OpenGL_FRAMEWORK}>
  $<$<CONFIG:Debug>:${Cocoa_FRAMEWORK}>
  $<$<CONFIG:Debug>:${IOKit_FRAMEWORK}>
  $<$<CONFIG:Debug>:${CoreVideo_FRAMEWORK}>
  $<$<CONFIG:Debug>:sfml-system>
  $<$<CONFIG:Debug>:sfml-window>
  $<$<CONFIG:Debug>:sfml-graphics>
  $<$<CONFIG:Debug>:sfml-audio>
)
target_compile_options("SpaceShip_Game" PRIVATE
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-O0>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-O0>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set_target_properties("SpaceShip_Game" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL Dev)
  set_target_properties("SpaceShip_Game" PROPERTIES
    OUTPUT_NAME "SpaceShip_Game"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Dev-macosx-x86_64
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Dev-macosx-x86_64
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/Dev-macosx-x86_64
  )
endif()
if(APPLE)
find_library(OpenGL_FRAMEWORK OpenGL)
find_library(Cocoa_FRAMEWORK Cocoa)
find_library(IOKit_FRAMEWORK IOKit)
find_library(CoreVideo_FRAMEWORK CoreVideo)
endif()
target_include_directories("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/engine/src>
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor>
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/magic_enum/include>
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/imgui>
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/SFML-3.0.0/include>
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/json/include>
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/src>
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/src/assets>
)
target_compile_definitions("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Dev>:NDEBUG>
  $<$<CONFIG:Dev>:_USRDLL>
  $<$<CONFIG:Dev>:GAME_IS_EXPORT>
  $<$<CONFIG:Dev>:TOAD_EDITOR>
)
target_link_directories("SpaceShip_Game" PRIVATE
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/bin/Release-macosx-x86_64>
  $<$<CONFIG:Dev>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../vendor/SFML-3.0.0/lib>
)
target_link_libraries("SpaceShip_Game"
  $<$<CONFIG:Dev>:Engine>
  $<$<CONFIG:Dev>:${OpenGL_FRAMEWORK}>
  $<$<CONFIG:Dev>:${Cocoa_FRAMEWORK}>
  $<$<CONFIG:Dev>:${IOKit_FRAMEWORK}>
  $<$<CONFIG:Dev>:${CoreVideo_FRAMEWORK}>
  $<$<CONFIG:Dev>:sfml-system>
  $<$<CONFIG:Dev>:sfml-window>
  $<$<CONFIG:Dev>:sfml-graphics>
  $<$<CONFIG:Dev>:sfml-audio>
)
target_compile_options("SpaceShip_Game" PRIVATE
  $<$<AND:$<CONFIG:Dev>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:Dev>,$<COMPILE_LANGUAGE:C>>:-O2>
  $<$<AND:$<CONFIG:Dev>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:Dev>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:Dev>,$<COMPILE_LANGUAGE:CXX>>:-O2>
  $<$<AND:$<CONFIG:Dev>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:Dev>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL Dev)
  set_target_properties("SpaceShip_Game" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()
if(CMAKE_BUILD_TYPE STREQUAL DevDebug)
  set_target_properties("SpaceShip_Game" PROPERTIES
    OUTPUT_NAME "SpaceShip_Game"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/DevDebug-macosx-x86_64
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/DevDebug-macosx-x86_64
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../bin/DevDebug-macosx-x86_64
  )
endif()
if(APPLE)
find_library(OpenGL_FRAMEWORK OpenGL)
find_library(Cocoa_FRAMEWORK Cocoa)
find_library(IOKit_FRAMEWORK IOKit)
find_library(CoreVideo_FRAMEWORK CoreVideo)
endif()
target_include_directories("SpaceShip_Game" PRIVATE
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/engine/src>
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor>
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/magic_enum/include>
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/imgui>
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/SFML-3.0.0/include>
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/vendor/json/include>
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/src>
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/src/assets>
)
target_compile_definitions("SpaceShip_Game" PRIVATE
  $<$<CONFIG:DevDebug>:_DEBUG>
  $<$<CONFIG:DevDebug>:GAME_IS_EXPORT>
  $<$<CONFIG:DevDebug>:TOAD_EDITOR>
)
target_link_directories("SpaceShip_Game" PRIVATE
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../../../Steve987321/Engine2D/bin/Release-macosx-x86_64>
  $<$<CONFIG:DevDebug>:${CMAKE_CURRENT_SOURCE_DIR}/SpaceShip_Game/../vendor/SFML-3.0.0/lib>
)
target_link_libraries("SpaceShip_Game"
  $<$<CONFIG:DevDebug>:Engine>
  $<$<CONFIG:DevDebug>:${OpenGL_FRAMEWORK}>
  $<$<CONFIG:DevDebug>:${Cocoa_FRAMEWORK}>
  $<$<CONFIG:DevDebug>:${IOKit_FRAMEWORK}>
  $<$<CONFIG:DevDebug>:${CoreVideo_FRAMEWORK}>
  $<$<CONFIG:DevDebug>:sfml-system>
  $<$<CONFIG:DevDebug>:sfml-window>
  $<$<CONFIG:DevDebug>:sfml-graphics>
  $<$<CONFIG:DevDebug>:sfml-audio>
)
target_compile_options("SpaceShip_Game" PRIVATE
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:C>>:-m64>
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:C>>:-O0>
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:C>>:-fPIC>
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:C>>:-g>
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:CXX>>:-m64>
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:CXX>>:-O0>
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:CXX>>:-fPIC>
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:CXX>>:-g>
  $<$<AND:$<CONFIG:DevDebug>,$<COMPILE_LANGUAGE:CXX>>:-std=c++20>
)
if(CMAKE_BUILD_TYPE STREQUAL DevDebug)
  set_target_properties("SpaceShip_Game" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE True
    INTERPROCEDURAL_OPTIMIZATION False
  )
endif()