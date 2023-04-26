
add_library(dtree
    impurityMeasures.cpp
    labels.cpp
    stoppingCondition.cpp
    treePrinter.cpp
)

target_include_directories(dtree
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${fmt_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}
)

target_link_libraries(dtree
    PRIVATE
    spdlog::spdlog
)
