using CommunityToolkit.Mvvm.ComponentModel;

namespace MvpMvvm.Locators
{
    public partial class Region : ObservableObject, IRegion
    {
        string viewName = string.Empty;
        NavigationParameters? parameter;


        public string ViewName { get => viewName; set => SetProperty(ref viewName, value); }
        public NavigationParameters? Parameter { get => parameter; set => SetProperty(ref parameter, value); }

        public Region()
        {
        }
    }
}
