#ifndef CAPTURE_STREAMING_DEFINES_H
#define CAPTURE_STREAMING_DEFINES_H

#define USE_NV12_COMPUTE_SHADER

// Initializing -> Ready -> Streaming
// Streaming -> Ready (��Ʈ���� ���� ��)
// Streaming -> Ready -> Streaming (��Ʈ���� ���� �� �簳 ��)
// Any State -> Error -> Ready (���� ���� ��)
enum class StreamingState
{
    Unknown,
    Initializing,
    Ready,
    Streaming,
    Error
};

enum class CaptureState
{
    Unknown,
    Initializing,
    Ready,
    Capturing,
    Error
};

#endif // CAPTURE_STREAMING_DEFINES_H

