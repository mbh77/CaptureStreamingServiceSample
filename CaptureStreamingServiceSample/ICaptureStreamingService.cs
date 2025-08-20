using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CaptureStreamingServiceSample
{
    public interface ICaptureStreamingService : INotifyPropertyChanged, IDisposable
    {
        CaptureStreamingState State { get; }

        void Init();
        void StartCaptureStreaming(int monitorIndex);
        void StartCaptureStreaming(IntPtr itemHandle);
        void StartCaptureEncDecStreaming(int monitorIndex);
        void StartCaptureEncDecStreaming(IntPtr itemHandle);
        void StartCaptureUdpStreaming(int monitorIndex, string address, int port);
        void StartCaptureUdpStreaming(IntPtr itemHandle, string address, int port);
        void StopCaptureStreaming();
    }
}
