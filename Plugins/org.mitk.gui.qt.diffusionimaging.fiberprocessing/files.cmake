set(SRC_CPP_FILES


)

set(INTERNAL_CPP_FILES
  mitkPluginActivator.cpp

  QmitkFiberQuantificationView.cpp
  QmitkFiberProcessingView.cpp
  QmitkFiberClusteringView.cpp
  QmitkFiberFitView.cpp

  Perspectives/QmitkFiberProcessingPerspective.cpp
)

set(UI_FILES
  src/internal/QmitkFiberQuantificationViewControls.ui
  src/internal/QmitkFiberProcessingViewControls.ui
  src/internal/QmitkFiberClusteringViewControls.ui
  src/internal/QmitkFiberFitViewControls.ui
)

set(MOC_H_FILES
  src/internal/mitkPluginActivator.h

  src/internal/QmitkFiberQuantificationView.h
  src/internal/QmitkFiberProcessingView.h
  src/internal/QmitkFiberClusteringView.h
  src/internal/QmitkFiberFitView.h

  src/internal/Perspectives/QmitkFiberProcessingPerspective.h
)

set(CACHED_RESOURCE_FILES
  plugin.xml

  resources/FiberBundleOperations.png
  resources/FiberQuantification.png
  resources/FiberClustering.png
  resources/FiberFit.png
  resources/circle.png
  resources/polygon.png
)

set(QRC_FILES
  resources/QmitkDiffusionImaging.qrc
)


set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
