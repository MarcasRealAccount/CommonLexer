require("Premake/Common")

require("Premake/Libs/CommonLexer")

require("Premake/ThirdParty/fmt")
require("ThirdParty/CommonCLI/Premake/Libs/CommonCLI")

workspace("CommonLexer")
	common:setConfigsAndPlatforms()

	common:addCoreDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("Off")
	flags("MultiProcessorCompile")

	startproject("Tests")

	group("Dependencies")
	project("fmt")
        location("ThirdParty/fmt/")
        warnings("Off")
        libs.fmt:setup()
        location("ThirdParty/")

    project("CommonCLI")
        location("ThirdParty/CommonCLI/")
        warnings("Off")
        libs.CommonCLI:setup()
        location("ThirdParty/")

	group("Libs")
	project("CommonLexer")
		location("CommonLexer/")
		warnings("Extra")
		libs.CommonLexer:setup()

	group("Tests")
	project("Tests")
		location("%{wks.location}/Tests/")
		kind("ConsoleApp")
		warnings("Extra")

		common:outDirs()
		common:debugDir()

		libs.CommonCLI:setupDep()
		libs.CommonLexer:setupDep()

		includedirs({ "%{prj.location}/Src/" })

		files({
			"%{prj.location}/Run/**",
			"%{prj.location}/Src/**"
		})
		removefiles({ "*.DS_Store" })