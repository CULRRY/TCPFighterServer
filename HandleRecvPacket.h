#pragma once
struct Session;
enum class Direction : BYTE;
bool Handle_C_MOVE_START(Session* session, Direction& dir, int16 x, int16 y);
bool Handle_C_MOVE_STOP(Session* session, Direction& dir, int16 x, int16 y);
bool Handle_C_ATTACK1(Session* session, Direction& dir, int16 x, int16 y);
bool Handle_C_ATTACK2(Session* session, Direction& dir, int16 x, int16 y);
bool Handle_C_ATTACK3(Session* session, Direction& dir, int16 x, int16 y);
