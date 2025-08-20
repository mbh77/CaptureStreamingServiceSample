using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace MvpMvvm.Locators
{
    internal class RegionManager : IRegionManager
    {
        private IServiceProvider _serviceProvider;

        public RegionManager(IServiceProvider serviceProvider)
        {
            _serviceProvider = serviceProvider;
        }

        public void RequestNavigate<TViewModel>(string regionPropertyName, string viewName, NavigationParameters prameter)
        {
            if (_serviceProvider != null)
            {
                var viewModel = _serviceProvider.GetService(typeof(TViewModel));

                if (viewModel != null)
                {
                    PropertyInfo? propertyInfo = typeof(TViewModel).GetProperty(regionPropertyName);

                    if (propertyInfo == null)
                    {
                        return;
                    }

                    if (!propertyInfo.CanWrite)
                    {
                        return;
                    }

                    var propertyValue = new Region()
                    {
                        ViewName = viewName,
                        Parameter = prameter,
                    };
                    propertyInfo.SetValue(viewModel, propertyValue);
                }
            }
        }

        public IRegion? GetRegion<TViewModel>(string regionPropertyName)
        {
            if (_serviceProvider != null)
            {
                var viewModel = _serviceProvider.GetService(typeof(TViewModel));

                if (viewModel != null)
                {
                    PropertyInfo? propertyInfo = typeof(TViewModel).GetProperty(regionPropertyName);

                    if (propertyInfo == null)
                    {
                        return default(Region);
                    }

                    if (!propertyInfo.CanWrite)
                    {
                        return default(Region);
                    }

                    return propertyInfo.GetValue(viewModel, null) as IRegion;
                }
            }

            return default(Region);
        }
    }
}
