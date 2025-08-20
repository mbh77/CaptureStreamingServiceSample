using CaptureStreamingCRL;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvpMvvm.Dialogs;
using MvpMvvm.Locators;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;

namespace CaptureStreamingServiceSample
{
    public partial class MainWindowViewModel : ObservableObject
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct RECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;

            public readonly int Width => Right - Left;
            public readonly int Height => Bottom - Top;

            public RECT(int left, int top, int right, int bottom)
            {
                Left = left;
                Top = top;
                Right = right;
                Bottom = bottom;
            }
        }

        private IServiceProvider _serviceProvider;
        private IRegionManager _regionManager;
        private IDialogService _dialogService;
        private ICaptureStreamingService _captureStreamingService;

        private delegate bool MonitorEnumProc(IntPtr hMonitor, IntPtr hdcMonitor, RECT lprcMonitor, IntPtr dwData);
        private delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool EnumDisplayMonitors(IntPtr hdc, IntPtr lprcClip, MonitorEnumProc lpfnEnum, IntPtr dwData);

        [DllImport("user32.dll")]
        private static extern bool EnumWindows(EnumWindowsProc enumProc, IntPtr lParam);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);

        [DllImport("user32.dll")]
        private static extern bool IsWindowVisible(IntPtr hWnd);

        [DllImport("user32.dll")]
        private static extern IntPtr GetParent(IntPtr hWnd);

        [DllImport("user32.dll")]
        private static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint processId);

        private string _address = "127.0.0.1";
        private string _port = "5000";
        private int _selectedWindowsInfoIndex = -1;
        private ObservableCollection<ItemInfo> _itemList = new();

        public string Address { get => _address; set => SetProperty(ref _address, value); }
        public string Port { get => _port; set => SetProperty(ref _port, value); }
        public int SelectedWindowsInfoIndex { get => _selectedWindowsInfoIndex; set => SetProperty(ref _selectedWindowsInfoIndex, value); }
        public ObservableCollection<ItemInfo> ItemList { get => _itemList; set => SetProperty(ref _itemList, value); }

        public MainWindowViewModel(IServiceProvider serviceProvider, IRegionManager regionManager,
            IDialogService dialogService, ICaptureStreamingService captureStreamingService)
        {
            _serviceProvider = serviceProvider;
            _regionManager = regionManager;
            _dialogService = dialogService;
            _captureStreamingService = captureStreamingService;

            LoadWindows();
        }

        [RelayCommand]
        public void LoadHandles()
        {
            LoadWindows();
        }

        [RelayCommand]
        public void StartLocal()
        {
            if (SelectedWindowsInfoIndex >= 0)
            {
                var winInfo = ItemList[_selectedWindowsInfoIndex];
                _captureStreamingService.StartCaptureStreaming(winInfo.ItemHandle);
            }
        }

        [RelayCommand]
        public void StartEncDec()
        {
            if(SelectedWindowsInfoIndex >= 0)
            {
                var winInfo = ItemList[_selectedWindowsInfoIndex];
                _captureStreamingService.StartCaptureEncDecStreaming(winInfo.ItemHandle);
            }
        }

        [RelayCommand]
        public void StartUdp()
        {
            if (SelectedWindowsInfoIndex >= 0)
            {
                var winInfo = ItemList[_selectedWindowsInfoIndex];
                if(int.TryParse(Port, out int port))
                {
                    _captureStreamingService.StartCaptureUdpStreaming(winInfo.ItemHandle, Address, port);
                }
            }
        }

        [RelayCommand]
        public void Stop()
        {
            _captureStreamingService.StopCaptureStreaming();
        }

        private void LoadWindows()
        {
            ItemList.Clear();

            int monitorCount = 0;
            EnumDisplayMonitors(IntPtr.Zero, IntPtr.Zero, (hMonitor, hdcMonitor, lprcMonitor, dwData) =>
            {
                monitorCount++;
                ItemList.Add(new ItemInfo(hMonitor, $"monitor {monitorCount}"));

                return true;
            }, IntPtr.Zero);

            EnumWindows((hWnd, lParam) =>
            {
                if (!IsWindowVisible(hWnd))
                    return true;

                if (GetParent(hWnd) != IntPtr.Zero)
                    return true;

                StringBuilder title = new StringBuilder(256);
                var hrGetWindowText = GetWindowText(hWnd, title, title.Capacity);
                if (title.Length == 0)
                    return true;

                var hrGetWindowThreadProcessId = GetWindowThreadProcessId(hWnd, out uint processId);
                Process proc = Process.GetProcessById((int)processId);

                if (proc != null)
                {
                    ItemList.Add(new ItemInfo(hWnd, title.ToString()));
                }

                return true;
            }, IntPtr.Zero);

            if (ItemList.Count > 0)
            {
                SelectedWindowsInfoIndex = 0;
            }
            else
            {
                SelectedWindowsInfoIndex = -1;
            }
        }
    }
}
