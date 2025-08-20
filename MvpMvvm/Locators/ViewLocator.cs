using CommunityToolkit.Mvvm.DependencyInjection;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows;

namespace MvpMvvm.Locators
{
    public class ViewLocator : DataTemplateSelector
    {
        private readonly IServiceProvider? _serviceProvider;
        static Dictionary<string, Type?> _viewTypes = new();

        public ViewLocator()
        {
            _serviceProvider = Ioc.Default;
        }

        public void AddViewTypes(string viewName, Type viewType)
        {
            _viewTypes.Add(viewName, viewType);
        }

        public void AddViewTypes<T>(string viewName)
        {
            _viewTypes.Add(viewName, typeof(T));
        }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            string? viewName = null;
            NavigationParameters? param = null;

            if (item is string name)
            {
                viewName = name;
                param = null;
            }
            else if (item is Region region)
            {
                viewName = region.ViewName;
                param = region.Parameter;
            }

            if (viewName != null && viewName != string.Empty)
            {
                if (_viewTypes.TryGetValue(viewName, out Type? type) == true)
                {
                    if (type != null && _serviceProvider != null)
                    {
                        object? view = _serviceProvider.GetService(type);
                        if (view != null)
                        {
                            // NavigateTo 호출
                            if (view is FrameworkElement frameworkElement && frameworkElement.DataContext is INavigationAware navigationAware)
                            {
                                navigationAware.OnNavigatedTo(param);
                            }

                            var dataTemplate = new DataTemplate();
                            var factory = new FrameworkElementFactory(typeof(ContentPresenter));
                            factory.SetBinding(ContentPresenter.ContentProperty, new Binding { Source = view });
                            dataTemplate.VisualTree = factory;

                            if (view is FrameworkElement element)
                            {
                                RoutedEventHandler? unloadedHandler = null;
                                unloadedHandler = (s, e) =>
                                {
                                    if (element.DataContext is INavigationAware navigationAware)
                                    {
                                        navigationAware.OnNavigatedFrom();
                                    }

                                    element.Unloaded -= unloadedHandler;
                                };

                                element.Unloaded -= unloadedHandler;
                                element.Unloaded += unloadedHandler;
                            }

                            return dataTemplate;
                        }
                    }
                }
            }
            return base.SelectTemplate(item, container);
        }
    }
}
