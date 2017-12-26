import qbs
import qbs.FileInfo

QtGuiApplication {
    name: "gemms"

    consoleApplication: false

    // Allow QtCreator to see imports from the build directory as well as
    // the source directory where your files are located
    property stringList qmlImportPaths: [
        FileInfo.joinPaths(qbs.installRoot, qbs.installPrefix, lirideployment.qmlDir),
        sourceDirectory
    ]

    // Depends on lirideployment to setup the above paths
    Depends { name: "lirideployment" }

    Depends { name: "iwkats_core" }

    Depends { name: "iwkats_ksmm" }

    // Qt dependencies
    Depends {
        name: "Qt";
        submodules: ["sql", "core", "widgets", "gui", "qml", "quick", "quickcontrols2", "texttospeech"];
        versionAtLeast: project.minimumQtVersion
    }

    // Android NDK
    Depends { name: "Android.ndk"; condition: qbs.targetOS.contains("android") }

    cpp.cxxLanguageVersion: "c++11"

    cpp.defines: [
        // The following define makes your compiler emit warnings if you use
        // any feature of Qt which as been marked deprecated (the exact warnings
        // depend on your compiler). Please consult the documentation of the
        // deprecated API in order to know how to port your code away from it.
        "QT_DEPRECATED_WARNINGS",

        // You can also make your code fail to compile if you use deprecated APIs.
        // In order to do so, uncomment the following line.
        // You can also select to disable deprecated APIs only up to a certain version of Qt.
        //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0


        // For glog
        "GLOG_NO_ABBREVIATED_SEVERITIES",
    ]

    // 库引用头文件路径
    cpp.includePaths: [
        project.vcpkgRootPath + "/include",
        sourceDirectory,
        sourceDirectory + "/gemms",
        sourceDirectory + "/gemms/quickflux",
        sourceDirectory + "/../iwkats_core",
        sourceDirectory + "/../iwkats_ksmm",
        sourceDirectory + "/../3rdparty",
    ]

    Properties {
        condition: qbs.buildVariant == "debug"
        cpp.libraryPaths: [
            sourceDirectory + "/../3rdparty",
            sourceDirectory + "/../3rdparty/KSMMApi/lib",
            project.vcpkgRootPath + "/debug/lib",
            project.vcpkgRootPath + "/debug/lib/manual-link"
        ]
    }

    Properties {
        condition: qbs.buildVariant == "release"
        cpp.libraryPaths: [
            sourceDirectory + "/../3rdparty",
            sourceDirectory + "/../3rdparty/KSMMApi/lib",
            project.vcpkgRootPath + "/lib/",
            project.vcpkgRootPath + "/lib/manual-link/",
        ]
    }

    // 动态库引用
    cpp.dynamicLibraries: [
        "glog.lib",
        "gflags.lib"
    ]

    // Source files
    files: [
        "*.h",
        "*.hpp",
        "*.cpp",
        "qml.qrc",

        "*.ico",
        "*.rc",
        "gemms/*.h",
        "gemms/*.hpp",
        "gemms/*.cpp",

        "gemms/dao/auto_quote_dao.cc",

        "gemms/trader/*.h",
        "gemms/trader/*.hpp",
        "gemms/trader/*.cc",

        "gemms/model/*.h",
        "gemms/model/*.cc",
        "gemms/model/*.hpp",
        "gemms/model/*.cpp",

        "gemms/master/*.h",
        "gemms/master/*.hpp",
        "gemms/master/*.cpp",

        "gemms/algorithm/*.h",
        "gemms/algorithm/*.hpp",
        "gemms/algorithm/*.cpp",
        "gemms/algorithm/auto_quote/*.h",
        "gemms/algorithm/auto_quote/*.hpp",
        "gemms/algorithm/auto_quote/*.cpp",
        "gemms/algorithm/model/*.h",
        "gemms/algorithm/model/*.hpp",
        "gemms/algorithm/model/*.cpp",
        "gemms/dao/*.h",
        "gemms/dao/*.hpp",
        "gemms/dao/*.cpp",
    ]

    Group {
        name: "quickflux"
        files: [
            "gemms/model/log_model.cpp",
            "gemms/model/log_model.h",
            "gemms/quickflux/QFAppDispatcher",
            "gemms/quickflux/QFKeyTable",
            "gemms/quickflux/QuickFlux",
            "gemms/quickflux/*.h",
            "gemms/quickflux/*.cpp",
            "gemms/quickflux/priv/*.h",
            "gemms/quickflux/priv/*.cpp",
        ]
    }

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: {
            if (qbs.targetOS.contains("linux"))
                return lirideployment.binDir;
            else
                return "";
        }
    }

}
