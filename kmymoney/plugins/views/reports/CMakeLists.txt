add_subdirectory(core)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/reportsview.json.cmake ${CMAKE_CURRENT_BINARY_DIR}/reportsview.json @ONLY)

set(reportsview_SOURCES
  reportsview.cpp
  kreportsview.cpp
  tocitem.cpp
  tocitemgroup.cpp
  tocitemreport.cpp
  kreportconfigurationfilterdlg.cpp
  reporttabimpl.cpp
  reportcontrolimpl.cpp
  kbalancechartdlg.cpp
  ../../../views/kmymoneywebpage.cpp
  )

ki18n_wrap_ui(reportsview_SOURCES
  kreportconfigurationfilterdlg.ui
  reportcontrol.ui
  reporttabgeneral.ui
  reporttabrowcolquery.ui
  reporttabrowcolpivot.ui
  reporttabrange.ui
  reporttabchart.ui
  reporttabcapitalgain.ui
  reporttabperformance.ui
  )
# kconfig_add_kcfg_files(reportsview_SOURCES reportsviewsettings.kcfgc)

kcoreaddons_add_plugin(reportsview
                       SOURCES ${reportsview_SOURCES}
                       JSON "${CMAKE_CURRENT_BINARY_DIR}/reportsview.json"
                       INSTALL_NAMESPACE "kmymoney")

#kcoreaddons_add_plugin sets LIBRARY_OUTPUT_DIRECTORY to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${INSTALL_NAMESPACE}
set_target_properties(reportsview
                      PROPERTIES
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

target_link_libraries(reportsview
  PUBLIC
    kmm_plugin
    KF5::TextWidgets
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIOWidgets
    KF5::KIONTLM
    reports
    kmm_widgets
    kmm_menus
)

if(ENABLE_WEBENGINE)
 target_link_libraries(reportsview PRIVATE Qt5::WebEngineWidgets)
else(ENABLE_WEBENGINE)
 target_link_libraries(reportsview PRIVATE KF5::WebKit)
endif(ENABLE_WEBENGINE)

# the KCM module

set(kcm_reportsview_PART_SRCS
    kcm_reportsview.cpp
    )

kconfig_add_kcfg_files(kcm_reportsview_PART_SRCS reportsviewsettings.kcfgc)

ki18n_wrap_ui(kcm_reportsview_PART_SRCS reportsviewsettings.ui)

kcoreaddons_add_plugin(kcm_reportsview
                       SOURCES ${kcm_reportsview_PART_SRCS}
                       JSON "${CMAKE_CURRENT_BINARY_DIR}/kcm_reportsview.json"
                       INSTALL_NAMESPACE "kmymoney")

kcoreaddons_desktop_to_json(kcm_reportsview kcm_reportsview.desktop)

#kcoreaddons_add_plugin sets LIBRARY_OUTPUT_DIRECTORY to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${INSTALL_NAMESPACE}
set_target_properties(kcm_reportsview
                      PROPERTIES
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

target_link_libraries(kcm_reportsview
                      KF5::I18n
                      KF5::ConfigWidgets
                      KF5::Completion
                      KF5::KIOWidgets
                      KF5::CoreAddons
                      kmm_settings
                      )

install(FILES kcm_reportsview.desktop
        DESTINATION "${SERVICES_INSTALL_DIR}")
