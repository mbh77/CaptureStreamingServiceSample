using CommunityToolkit.Mvvm.DependencyInjection;
using System.Reflection;
using System.Windows.Controls;
using System.Windows;

namespace MvpMvvm.Locators
{
    public class ViewModelLocator
    {
        private readonly IServiceProvider? _serviceProvider;
        static Dictionary<string, Type?> _viewModelTypes = new();

        public ViewModelLocator()
        {
            _serviceProvider = Ioc.Default;
        }

        public void AddViewModelTypes(string viewModelName, Type viewModelType)
        {
            _viewModelTypes.Add(viewModelName, viewModelType);
        }

        public object GetViewModel(string viewModelName)
        {
            if (string.IsNullOrEmpty(viewModelName))
            {
                return new TextBlock { Text = "View type name is null or empty" };
            }

            if (viewModelName != null && viewModelName != string.Empty)
            {
                if (_viewModelTypes.TryGetValue(viewModelName, out Type? type) == true)
                {
                    if (System.ComponentModel.DesignerProperties.GetIsInDesignMode(new DependencyObject()))
                    {
                        // for design time
                        return new TextBlock { Text = "Not Found: " + viewModelName };
                    }
                    else
                    {
                        // for run time
                        if (type != null)
                        {
                            return _serviceProvider?.GetService(type) ?? new TextBlock { Text = "Not Found: " + viewModelName };
                        }
                    }
                }
            }

            return new TextBlock { Text = "Not Found: " + viewModelName };
        }
    }
}
