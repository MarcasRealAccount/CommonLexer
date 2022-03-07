if not libs then libs = {} end
if not libs.CommonLexer then
	libs.CommonLexer = {
		name       = "",
		location   = ""
	}
end

local CommonLexer = libs.CommonLexer

require("../ThirdParty/fmt")

function CommonLexer:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	includedirs({ self.location .. "/Inc/" })

	libs.fmt:setupDep()

	files({
		self.location .. "/Inc/**",
		self.location .. "/Src/**"
	})
	removefiles({ "*.DS_Store" })
end

function CommonLexer:setupDep()
	links({ self.name })
	sysincludedirs({ self.location .. "/Inc/" })
	libs.fmt:setupDep()
end
