
project.name = "KomodoDrop"
project.bindir = "."
project.libdir = "."

project.configs = { "Debug", "Release" }

package = newpackage()
package.name = "KomodoDrop"
package.kind = "winexe"
package.language = "c++"

package.objdir = "./intermediate"
package.config["Debug"].objdir   = "./intermediate/Debug"
package.config["Release"].objdir = "./intermediate/Release"

package.config["Debug"].defines     = { "LINUX=1", "DEBUG=1", "_DEBUG=1" };
package.config["Debug"].buildoptions = { "-D_DEBUG -ggdb" }

package.config["Release"].defines   = { "LINUX=1", "NDEBUG=1" };

package.target = "KomodoDrop"

package.includepaths = { 
    "/usr/include"
}

package.libpaths = { 
    "/usr/X11R6/lib/",
    "../../../../bin"
}

package.config["Debug"].links = { 
    "freetype", "pthread", "rt", "X11", "GL", "GLU", "Xinerama", "asound", "juce_debug"
}

package.config["Release"].links = { 
    "freetype", "pthread", "rt", "X11", "GL", "GLU", "Xinerama", "asound", "juce"
}

package.linkflags = { "static-runtime" }

package.files = { matchfiles (
    "./*.h",
    "./*.cpp"
    )
}
