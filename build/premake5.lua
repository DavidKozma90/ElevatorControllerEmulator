-- File: premake5.lua

function platform_defines()
    filter {"configurations:Debug or Release"}
        defines{"PLATFORM_DESKTOP"}

    filter {"system:macosx"}
        disablewarnings {"deprecated-declarations"}

    filter {"system:linux"}
        defines {"_GNU_SOURCE"}

    filter{}
end

workspaceName = 'ElevatorController'
baseName = path.getbasename(path.getdirectory(os.getcwd()));

workspaceName = baseName

if (os.isdir('build_files') == false) then
    os.mkdir('build_files')
end

workspace (workspaceName)
    location "../"
    configurations { "Debug", "Release"}
    platforms { "x64", "x86", "ARM64"}

    defaultplatform ("x64")

    filter "configurations:Debug or Debug_RGFW"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release or Release_RGFW"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "platforms:Arm64" }
        architecture "ARM64"

    filter {}

    targetdir "bin/%{cfg.buildcfg}/"

    startproject(workspaceName)

    project (workspaceName)
        kind "ConsoleApp"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        filter {"system:windows", "configurations:Release", "action:gmake*"}
            kind "WindowedApp"
            buildoptions { "-Wl,--subsystem,windows" }

        filter {"system:windows", "configurations:Release", "action:vs*"}
            kind "WindowedApp"
            entrypoint "mainCRTStartup"

        filter "action:vs*"
            debugdir "$(SolutionDir)"

        filter{}

        vpaths 
        {
            ["Header Files/*"] = { "../src/**.h", "../src/**.hpp"},
            ["Source Files/*"] = {"../src/**.c", "src/**.cpp"},
            ["Windows Resource Files/*"] = {"../src/**.rc", "src/**.ico"},
        }
        
        files {"../src/**.c", "../src/**.cpp", "../src/**.h", "../src/**.hpp"}
        
        filter {"system:windows", "action:vs*"}
            files {"../src/*.rc", "../src/*.ico"}

        filter{}
        
        includedirs { "../src" }

        cdialect "C17"
        cppdialect "C++17"

        flags { "ShadowedVariables"}
        platform_defines()

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }

        filter "system:windows"
            defines{"_WIN32"}
            links {"winmm", "gdi32", "opengl32"}
            libdirs {"../bin/%{cfg.buildcfg}"}

        filter "system:linux"
            links {"pthread", "m", "dl", "rt", "X11"}

        filter "system:macosx"
            links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

        filter{}
