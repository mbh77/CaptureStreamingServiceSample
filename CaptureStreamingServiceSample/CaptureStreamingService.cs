using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CaptureStreamingCRL;
using CommunityToolkit.Mvvm.ComponentModel;

namespace CaptureStreamingServiceSample
{
    class CaptureStreamingService : ObservableObject, ICaptureStreamingService
    {
        private CaptureStreamingWrapper _captureStreamingWrapper;

        private CaptureStreamingState _state;

        public CaptureStreamingState State { get => _state; set => SetProperty(ref _state, value); }

        public CaptureStreamingService()
        {
            State = CaptureStreamingState.Initializing;
            _captureStreamingWrapper = new CaptureStreamingWrapper();
            State = CaptureStreamingState.Ready;
        }

        public void Init()
        {
            
        }

        public void StartCaptureStreaming(int monitorIndex)
        {
            if (State == CaptureStreamingState.Ready)
            {
                _captureStreamingWrapper.StartCaptureStreaming(monitorIndex);
                State = CaptureStreamingState.Streaming;
            }
        }

        public void StartCaptureStreaming(IntPtr itemHandle)
        {
            if (State == CaptureStreamingState.Ready)
            {
                _captureStreamingWrapper.StartCaptureStreaming(itemHandle);
                State = CaptureStreamingState.Streaming;
            }
        }

        public void StartCaptureEncDecStreaming(int monitorIndex)
        {
            if (State == CaptureStreamingState.Ready)
            {
                _captureStreamingWrapper.StartCaptureEncDecStreaming(monitorIndex);
                State = CaptureStreamingState.Streaming;
            }
        }

        public void StartCaptureEncDecStreaming(IntPtr itemHandle)
        {
            if (State == CaptureStreamingState.Ready)
            {
                _captureStreamingWrapper.StartCaptureEncDecStreaming(itemHandle);
                State = CaptureStreamingState.Streaming;
            }
        }

        public void StartCaptureUdpStreaming(int monitorIndex, string address, int port)
        {
            if (State == CaptureStreamingState.Ready && address != null)
            {
                _captureStreamingWrapper.StartCaptureUdpStreaming(monitorIndex, address, port);
                State = CaptureStreamingState.Streaming;
            }
        }

        public void StartCaptureUdpStreaming(IntPtr itemHandle, string address, int port)
        {
            if (State == CaptureStreamingState.Ready && address != null)
            {
                _captureStreamingWrapper.StartCaptureUdpStreaming(itemHandle, address, port);
                State = CaptureStreamingState.Streaming;
            }
        }

        public void StopCaptureStreaming()
        {
            if (State == CaptureStreamingState.Streaming)
            {
                _captureStreamingWrapper.StopCaptureStreaming();
                State = CaptureStreamingState.Ready;
            }
        }

        public void Dispose()
        {
            _captureStreamingWrapper.StopCaptureStreaming();
            _captureStreamingWrapper.Dispose();
        }
    }
}
