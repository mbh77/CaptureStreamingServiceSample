namespace MvpMvvm.Locators
{
    public interface INavigationAware
    {
        void OnNavigatedTo(NavigationParameters? parameter);
        void OnNavigatedFrom();
    }
}
