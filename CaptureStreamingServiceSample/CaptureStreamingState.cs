using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CaptureStreamingServiceSample
{
    // Initializing -> Ready -> Streaming
    // Streaming -> Ready (스트리밍 종료 시)
    // Streaming -> Ready -> Streaming (스트리밍 종료 및 재개 시)
    // Any State -> Error -> Ready (오류 복구 시)
    public enum CaptureStreamingState
    {
        Unknown,
        Initializing,
        Ready,
        Streaming,
        Error,
    }
}
