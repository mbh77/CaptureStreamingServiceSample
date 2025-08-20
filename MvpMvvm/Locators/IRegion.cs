using System.ComponentModel;

namespace MvpMvvm.Locators
{
    public interface IRegion : INotifyPropertyChanged
    {
        string ViewName { get; set; }
        NavigationParameters? Parameter { get; set; }
    }
}
