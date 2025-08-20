#ifndef CAPTURE_STREAMING_DEFINES_H
#define CAPTURE_STREAMING_DEFINES_H

#define USE_NV12_COMPUTE_SHADER

// Initializing -> Ready -> Streaming
// Streaming -> Ready (스트리밍 종료 시)
// Streaming -> Ready -> Streaming (스트리밍 종료 및 재개 시)
// Any State -> Error -> Ready (오류 복구 시)
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

