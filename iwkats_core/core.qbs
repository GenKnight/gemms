import qbs
import qbs.FileInfo

DynamicLibrary {    
    name: "iwkats_core"

    // Qt dependencies
    Depends {
        name: "Qt";
        submodules: ["core", "gui", "qml", "quick", "quickcontrols2", "websockets", "xml", "widgets", "sql", "texttospeech"];
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

        "IWKATS_CORE_LIB"
    ]

    // 库引用头文件路径
    cpp.includePaths: [
        project.vcpkgRootPath + "/include",
        sourceDirectory,
        sourceDirectory + "/../3rdparty",
    ]

    Properties {
        condition: qbs.buildVariant == "debug"
        cpp.libraryPaths: [
            sourceDirectory + "/../3rdparty",
            sourceDirectory + "/../3rdparty/ctp",
            project.vcpkgRootPath + "/debug/lib",
            project.vcpkgRootPath + "/debug/lib/manual-link"
        ]
    }

    Properties {
        condition: qbs.buildVariant == "release"
        cpp.libraryPaths: [
            sourceDirectory + "/../3rdparty",
            sourceDirectory + "/../3rdparty/ctp",
            project.vcpkgRootPath + "/lib/",
            project.vcpkgRootPath + "/lib/manual-link/",
        ]
    }

    // 动态库引用
    cpp.dynamicLibraries: [
        "glog.lib",
        "gflags.lib"
    ]

    Properties {
        condition: qbs.targetOS.contains("android")
        architectures: !qbs.architecture ? ["x86", "armv7a"] : undefined
        Android.ndk.appStl: "gnustl_shared"
    }

    // Source files
    files: [
        "core/*.h",
        "core/*.hpp",
        "core/*.cpp",
        "core/settings.cc",
        "core/settings.cc",
        "core/ttspeech.cc",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }

}
