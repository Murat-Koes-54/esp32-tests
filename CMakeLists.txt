cmake_minimum_required(VERSION 3.5)

set(__PROJECT_GROUP_LINK_COMPONENTS ON)

set(PLANE_COMPONENT_DIRS "./components/core")

set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${PLANE_COMPONENT_DIRS}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_unittest)
