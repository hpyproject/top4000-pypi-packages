/*
  This file contains docstrings for the Python bindings.
  Do not edit! These were automatically extracted by mkdoc.py
 */

#define __EXPAND(x)                                      x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...)  COUNT
#define __VA_SIZE(...)                                   __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1))
#define __CAT1(a, b)                                     a ## b
#define __CAT2(a, b)                                     __CAT1(a, b)
#define __DOC1(n1)                                       __doc_##n1
#define __DOC2(n1, n2)                                   __doc_##n1##_##n2
#define __DOC3(n1, n2, n3)                               __doc_##n1##_##n2##_##n3
#define __DOC4(n1, n2, n3, n4)                           __doc_##n1##_##n2##_##n3##_##n4
#define __DOC5(n1, n2, n3, n4, n5)                       __doc_##n1##_##n2##_##n3##_##n4##_##n5
#define __DOC6(n1, n2, n3, n4, n5, n6)                   __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6
#define __DOC7(n1, n2, n3, n4, n5, n6, n7)               __doc_##n1##_##n2##_##n3##_##n4##_##n5##_##n6##_##n7
#define DOC(...)                                         __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))

#if defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif


static const char *__doc_ur_rtde_RTDEIOInterface_RTDEIOInterface = R"doc()doc";

static const char *__doc_ur_rtde_RTDEIOInterface_reconnect =
R"doc(@returns Can be used to reconnect to the robot after a lost
connection.)doc";

static const char *__doc_ur_rtde_RTDEIOInterface_sendCommand = R"doc()doc";

static const char *__doc_ur_rtde_RTDEIOInterface_setAnalogOutputCurrent =
R"doc(@brief Set Analog output current @param output_id The number (id) of
the output, integer: [0:1] @param current_ratio current set as a
(ratio) of the current span [0..1], 1 means full current.)doc";

static const char *__doc_ur_rtde_RTDEIOInterface_setAnalogOutputVoltage =
R"doc(@brief Set Analog output voltage @param output_id The number (id) of
the output, integer: [0:1] @param voltage_ratio voltage set as a
(ratio) of the voltage span [0..1], 1 means full voltage.)doc";

static const char *__doc_ur_rtde_RTDEIOInterface_setSpeedSlider =
R"doc(@brief Set the speed slider on the controller @param speed set the
speed slider on the controller as a fraction value between 0 and 1 (1
is 100%))doc";

static const char *__doc_ur_rtde_RTDEIOInterface_setStandardDigitalOut =
R"doc(@brief Set standard digital output signal level @param output_id The
number (id) of the output, integer: [0:7] @param signal_level The
signal level. (boolean))doc";

static const char *__doc_ur_rtde_RTDEIOInterface_setToolDigitalOut =
R"doc(@brief Set tool digital output signal level @param output_id The
number (id) of the output, integer: [0:1] @param signal_level The
signal level. (boolean))doc";

static const char *__doc_ur_rtde_RTDEIOInterface_setupRecipes = R"doc()doc";

static const char *__doc_ur_rtde_RTDEIOInterface_verifyValueIsWithin = R"doc()doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

