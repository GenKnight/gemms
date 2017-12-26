import qbs

Project {
    readonly property string minimumQtVersion: "5.9.2"
    readonly property string vcpkgRootPath: "D:\\vcpkg\\installed\\x86-windows"

    minimumQbsVersion: "1.7.1"

    name: "qgemms"

    references: [
        "iwkats_core/core.qbs",
        "gemms/gemms.qbs",
        "iwkats_ksmm/ksmm.qbs",
    ]

    // Add Fluid search paths here
    qbsSearchPaths: [
        "fluid/qbs/shared",
        "fluid/qbs/local"
    ]

    // Include Fluid project
    SubProject {
        filePath: "fluid/fluid.qbs"
        Properties {
            withDocumentation: false
            withDemo: false
            autotestEnabled: false
        }
    }
}
