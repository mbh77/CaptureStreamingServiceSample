using CommunityToolkit.Mvvm.ComponentModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CaptureStreamingServiceSample
{
    public class ItemInfo : ObservableObject
    {
        private IntPtr _itemHandle;
        private string _title = string.Empty;

        public IntPtr ItemHandle { get => _itemHandle; set => SetProperty(ref _itemHandle, value); }
        public string Title { get => _title; set => SetProperty(ref _title, value); }

        public ItemInfo(IntPtr hwnd, string title)
        {
            _itemHandle = hwnd;
            _title = title;
        }
    }
}
