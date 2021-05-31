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


static const char *__doc_ur_rtde_RTDEControlInterface_RTDEControlInterface = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_disconnect =
R"doc(@returns Can be used to disconnect from the robot. To reconnect you
have to call the reconnect() function.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_endTeachMode =
R"doc(@brief Set robot back in normal position control mode after freedrive
mode.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_forceMode =
R"doc(@brief Set robot to be controlled in force mode @param task_frame A
pose vector that defines the force frame relative to the base frame.
@param selection_vector A 6d vector of 0s and 1s. 1 means that the
robot will be compliant in the corresponding axis of the task frame
@param wrench The forces/torques the robot will apply to its
environment. The robot adjusts its position along/about compliant axis
in order to achieve the specified force/torque. Values have no effect
for non-compliant axes @param type An integer [1;3] specifying how the
robot interprets the force frame. 1: The force frame is transformed in
a way such that its y-axis is aligned with a vector pointing from the
robot tcp towards the origin of the force frame. 2: The force frame is
not transformed. 3: The force frame is transformed in a way such that
its x-axis is the projection of the robot tcp velocity vector onto the
x-y plane of the force frame. @param limits (Float) 6d vector. For
compliant axes, these values are the maximum allowed tcp speed
along/about the axis. For non-compliant axes, these values are the
maximum allowed deviation along/about an axis between the actual tcp
position and the one set by the program.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_forceModeSetDamping =
R"doc(@brief Sets the damping parameter in force mode. @param damping
Between 0 and 1, default value is 0.005

A value of 1 is full damping, so the robot will decellerate quickly if
no force is present. A value of 0 is no damping, here the robot will
maintain the speed.

The value is stored until this function is called again. Call this
function before force mode is entered (otherwise default value will be
used).)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_forceModeSetGainScaling =
R"doc(@brief Scales the gain in force mode. @param scaling scaling parameter
between 0 and 2, default is 1.

A value larger than 1 can make force mode unstable, e.g. in case of
collisions or pushing against hard surfaces.

The value is stored until this function is called again. Call this
function before force mode is entered (otherwise default value will be
used))doc";

static const char *__doc_ur_rtde_RTDEControlInterface_forceModeStop = R"doc(@brief Resets the robot mode from force mode to normal operation.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getActualJointPositionsHistory =
R"doc(@brief Detects when a contact between the tool and an object happens.
@param direction The first three elements are interpreted as a 3D
vector (in the robot base coordinate system) giving the direction in
which contacts should be detected. If all elements of the list are
zero, contacts from all directions are considered. @returns The
returned value is the number of time steps back to just before the
contact have started. A value larger than 0 means that a contact is
detected. A value of 0 means no contact.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getActualJointPositionsHistoryValue = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getControlScriptState = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getInverseKinematics =
R"doc(@brief Calculate the inverse kinematic transformation (tool space ->
jointspace). If qnear is defined, the solution closest to qnear is
returned.Otherwise, the solution closest to the current joint
positions is returned. If no tcp is provided the currently active tcp
of the controller will be used. @param x tool pose @param qnear list
of joint positions (Optional) @param maxPositionError the maximum
allowed positionerror (Optional) @param maxOrientationError the
maximum allowed orientationerror (Optional) @returns joint positions)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getInverseKinematicsValue = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getJointTorques =
R"doc(@brief Returns the torques of all joints

The torque on the joints, corrected by the torque needed to move the
robot itself (gravity, friction, etc.), returned as a vector of length
6.

@returns The joint torque vector in Nm: [Base, Shoulder, Elbow,
Wrist1, Wrist2, Wrist3])doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getStepTime =
R"doc(@brief Returns the duration of the robot time step in seconds.

In every time step, the robot controller will receive measured joint
positions and velocities from the robot, and send desired joint
positions and velocities back to the robot. This happens with a
predetermined frequency, in regular intervals. This interval length is
the robot time step.

@returns Duration of the robot step in seconds)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getStepTimeValue = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getTargetWaypoint =
R"doc(@brief Returns the target waypoint of the active move

This is different from the target tcp pose which returns the target
pose for each time step. The get_target_waypoint() returns the same
target pose for movel, movej, movep or movec during the motion. It
returns the target tcp pose, if none of the mentioned move functions
are running.

This method is useful for calculating relative movements where the
previous move command uses blends.

@returns The desired waypoint TCP vector [X, Y, Z, Rx, Ry, Rz])doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getTargetWaypointValue = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_getToolContactValue = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_isConnected =
R"doc(@returns Connection status for RTDE, useful for checking for lost
connection.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_isEmergencyStopped = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_isJointsWithinSafetyLimits =
R"doc(@brief Checks if the given joint position is reachable and within the
current safety limits of the robot. This check considers joint limits
(if the target pose is specified as joint positions), safety planes
limits, TCP orientation deviation limits and range of the robot. If a
solution is found when applying the inverse kinematics to the given
target TCP pose, this pose is considered reachable

@param q joint positions @returns a bool indicating if the joint
positions are within the safety limits.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_isPoseWithinSafetyLimits =
R"doc(@brief Checks if the given pose is reachable and within the current
safety limits of the robot. It checks safety planes limits, TCP
orientation deviation limits and range of the robot. If a solution is
found when applying the inverse kinematics to the given target TCP
pose, this pose is considered reachable.

@param pose target pose @returns a bool indicating if the pose is
within the safety limits.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_isProgramRunning =
R"doc(@brief Returns true if a program is running on the controller,
otherwise it returns false)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_isProtectiveStopped = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_kickWatchdog =
R"doc(@brief Kicks the watchdog safeguarding the communication. Normally you
would kick the watchdog in your control loop. Be sure to kick it as
often as specified by the minimum frequency of the watchdog.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_moveC =
R"doc(@brief Move Circular: Move to position (circular in tool-space) @param
pose_via path point (note: only position is used) @param pose_to
target pose (note: only position is used in Fixed orientation mode).
@param speed tool speed [m/s] @param acceleration tool acceleration
[m/s^2] @param blend blend radius [m] @param mode 0: Unconstrained
mode. Interpolate orientation from current pose to target pose
(pose_to) 1: Fixed mode. Keep orientation constant relative to the
tangent of the circular arc (starting from current pose))doc";

static const char *__doc_ur_rtde_RTDEControlInterface_moveJ =
R"doc(@brief Move to joint position (linear in joint-space) @param q joint
positions @param speed joint speed of leading axis [rad/s] @param
acceleration joint acceleration of leading axis [rad/s^2] @param async
a bool specifying if the move command should be asynchronous. If async
is true it is possible to stop a move command using either the stopJ
or stopL function. Default is false, this means the function will
block until the movement has completed.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_moveJ_2 =
R"doc(@brief Move to each joint position specified in a path @param path
with joint positions that includes acceleration, speed and blend for
each position)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_moveJ_IK =
R"doc(@brief Move to pose (linear in joint-space) @param pose target pose
@param speed joint speed of leading axis [rad/s] @param acceleration
joint acceleration of leading axis [rad/s^2] @param async a bool
specifying if the move command should be asynchronous. If async is
true it is possible to stop a move command using either the stopJ or
stopL function. Default is false, this means the function will block
until the movement has completed.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_moveL =
R"doc(@brief Move to position (linear in tool-space) @param pose target pose
@param speed tool speed [m/s] @param acceleration tool acceleration
[m/s^2] @param async a bool specifying if the move command should be
asynchronous. If async is true it is possible to stop a move command
using either the stopJ or stopL function. Default is false, this means
the function will block until the movement has completed.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_moveL_2 =
R"doc(@brief Move to each pose specified in a path @param path with tool
poses that includes acceleration, speed and blend for each position)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_moveL_FK =
R"doc(@brief Move to position (linear in tool-space) @param q joint
positions @param speed tool speed [m/s] @param acceleration tool
acceleration [m/s^2] @param async a bool specifying if the move
command should be asynchronous. If async is true it is possible to
stop a move command using either the stopJ or stopL function. Default
is false, this means the function will block until the movement has
completed.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_moveP =
R"doc(@brief Move Process: Blend circular (in tool-space) and move linear
(in tool-space) to position. Accelerates to and moves with constant
tool speed v. @param pose target pose @param speed tool speed [m/s]
@param acceleration tool acceleration [m/s^2] @param blend blend
radius [m])doc";

static const char *__doc_ur_rtde_RTDEControlInterface_jogStart =
R"doc(Starts jogging with the given speed vector with respect to the given
feature.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_jogStop =
R"doc(Stops jogging that has been started start_jog)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_poseTrans =
R"doc(@brief Pose transformation to move with respect to a tool or w.r.t. a
custom feature/frame The first argument, p_from, is used to transform
the second argument, p_from_to, and the result is then returned. This
means that the result is the resulting pose, when starting at the
coordinate system of p_from, and then in that coordinate system moving
p_from_to. This function can be seen in two different views. Either
the function transforms, that is translates and rotates, p_from_to by
the parameters of p_from. Or the function is used to get the resulting
pose, when first making a move of p_from and then from there, a move
of p_from_to. If the poses were regarded as transformation matrices,
it would look like: @verbatim T_world->to = T_world->from * T_from->to
T_x->to = T_x->from * T_from->to @endverbatim @param p_from starting
pose (spatial vector) @param p_from_to pose change relative to
starting pose (spatial vector) @returns resulting pose (spatial
vector))doc";

static const char *__doc_ur_rtde_RTDEControlInterface_poseTransValue = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_prepareCmdScript = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_receiveCallback = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_reconnect =
R"doc(@returns Can be used to reconnect to the robot after a lost
connection.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_reuploadScript =
R"doc(@brief In the event of an error, this function can be used to resume
operation by reuploading the RTDE control script. This will only
happen if a script is not already running on the controller.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_sendClearCommand = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_sendCommand = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_sendCustomScriptFile =
R"doc(@brief Send a custom ur script file to the controller @param file_path
the file path to the custom ur script file)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_setCustomScriptFile =
R"doc(Assign a custom script file that will be sent to device as the main
control script. Setting an empty file_name will disable the custom script loading
This eases debugging when modifying the control script because it does not 
require to recompile the whole library)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_sendCustomScriptFunction =
R"doc(@brief Send a custom ur script to the controller @param function_name
specify a name for the custom script function @param script the custom
ur script to be sent to the controller specified as a string, each
line must be terminated with a newline. The code will automatically be
indented with one tab to fit with the function body.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_sendCustomScript =
R"doc(@brief Send a custom ur script to the controller
@return Returns true if the script has been executed successfully and false
on timeout)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_servoC =
R"doc(@brief Servo to position (circular in tool-space). Accelerates to and
moves with constant tool speed v. @param pose target pose @param speed
tool speed [m/s] @param acceleration tool acceleration [m/s^2] @param
blend blend radius (of target pose) [m])doc";

static const char *__doc_ur_rtde_RTDEControlInterface_servoJ =
R"doc(@brief Servo to position (linear in joint-space) @param q joint
positions [rad] @param speed NOT used in current version @param
acceleration NOT used in current version @param time time where the
command is controlling the robot. The function is blocking for time t
[S] @param lookahead_time time [S], range [0.03,0.2] smoothens the
trajectory with this lookahead time @param gain proportional gain for
following target position, range [100,2000])doc";

static const char *__doc_ur_rtde_RTDEControlInterface_servoL =
R"doc(@brief Servo to position (linear in tool-space) @param pose target
pose @param speed NOT used in current version @param acceleration NOT
used in current version @param time time where the command is
controlling the robot. The function is blocking for time t [S] @param
lookahead_time time [S], range [0.03,0.2] smoothens the trajectory
with this lookahead time @param gain proportional gain for following
target position, range [100,2000])doc";

static const char *__doc_ur_rtde_RTDEControlInterface_servoStop = R"doc(@brief Stop servos)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_setPayload =
R"doc(@brief Set payload @param mass Mass in kilograms @param cog Center of
Gravity, a vector [CoGx, CoGy, CoGz] specifying the displacement (in
meters) from the toolmount. If not specified the current CoG will be
used.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_setTcp =
R"doc(@brief Sets the active tcp offset, i.e. the transformation from the
output flange coordinate system to the TCP as a pose. @param
tcp_offset A pose describing the transformation of the tcp offset.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_setWatchdog =
R"doc(@brief Enable a watchdog for the communication with a specified
minimum frequency for which an input update is expected to arrive. The
watchdog is useful for safety critical realtime applications eg.
servoing. The default action taken is to shutdown the control, if the
watchdog is not kicked with the minimum frequency.

Preferably you would call this function right after the
RTDEControlInterface has been constructed.

@param min_frequency The minimum frequency an input update is expected
to arrive defaults to 10Hz.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_setupRecipes = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_speedJ =
R"doc(@brief Joint speed - Accelerate linearly in joint space and continue
with constant joint speed @param qd joint speeds [rad/s] @param
acceleration joint acceleration [rad/s^2] (of leading axis) @param
time time [s] before the function returns (optional))doc";

static const char *__doc_ur_rtde_RTDEControlInterface_speedL =
R"doc(@brief Tool speed - Accelerate linearly in Cartesian space and
continue with constant tool speed. The time t is optional; @param xd
tool speed [m/s] (spatial vector) @param acceleration tool position
acceleration [m/s^2] @param time time [s] before the function returns
(optional))doc";

static const char *__doc_ur_rtde_RTDEControlInterface_speedStop = R"doc(@brief Stop speeding)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_stopJ =
R"doc(@brief Stop (linear in joint space) - decelerate joint speeds to zero
@param a joint acceleration [rad/s^2] (rate of deceleration of the
leading axis).)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_stopL =
R"doc(@brief Stop (linear in tool space) - decelerate tool speed to zero
@param a tool acceleration [m/s^2] (rate of deceleration of the tool))doc";

static const char *__doc_ur_rtde_RTDEControlInterface_stopScript = R"doc(@brief This function will terminate the script on controller.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_teachMode =
R"doc(@brief Set robot in freedrive mode. In this mode the robot can be
moved around by hand in the same way as by pressing the "freedrive"
button. The robot will not be able to follow a trajectory (eg. a
movej) in this mode.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_toolContact =
R"doc(@brief Detects when a contact between the tool and an object happens.
@param direction The first three elements are interpreted as a 3D
vector (in the robot base coordinate system) giving the direction in
which contacts should be detected. If all elements of the list are
zero, contacts from all directions are considered. @returns The
returned value is the number of time steps back to just before the
contact have started. A value larger than 0 means that a contact is
detected. A value of 0 means no contact.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_triggerProtectiveStop =
R"doc(@brief Triggers a protective stop on the robot. Can be used for
testing and debugging.)doc";

static const char *__doc_ur_rtde_RTDEControlInterface_verifyValueIsWithin = R"doc()doc";

static const char *__doc_ur_rtde_RTDEControlInterface_zeroFtSensor =
R"doc(@brief Zeroes the TCP force/torque measurement from the builtin
force/torque sensor by subtracting the current measurement from the
subsequent.)doc";

#if defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

