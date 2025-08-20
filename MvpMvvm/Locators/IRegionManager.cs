namespace MvpMvvm.Locators
{
    public interface IRegionManager
    {
        void RequestNavigate<TViewModel>(string regionPropertyName, string viewName, NavigationParameters prameter);
        IRegion? GetRegion<TViewModel>(string regionPropertyName);
    }
}
