using Microsoft.Extensions.DependencyInjection;
using MvpMvvm;
using MvpMvvm.Locators;
using MvpMvvm.Modularity;
using System.Configuration;
using System.Data;
using System.Windows;

namespace CaptureStreamingServiceSample
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : MvvmApp
    {
        public App() : base(new ServiceCollection())
        {

        }

        protected override void ConfigureModuleCatalog(List<IModule> moduleCatalog)
        {
            base.ConfigureModuleCatalog(moduleCatalog);
        }

        protected override void RegisterTypes(IServiceCollection services)
        {
            services.AddSingleton<MainWindow>();
            services.AddSingleton<MainWindowViewModel>();

            services.AddSingleton<ICaptureStreamingService, CaptureStreamingService>();

            base.RegisterTypes(services);
        }

        protected override void OnInitialized(IServiceProvider serviceProvider)
        {
            var viewLocator = serviceProvider.GetRequiredService<ViewLocator>();
            var viewModelLocator = serviceProvider.GetRequiredService<ViewModelLocator>();

            if (viewModelLocator != null)
            {
                viewModelLocator.AddViewModelTypes("MainWindowViewModel", typeof(MainWindowViewModel));
            }

            base.OnInitialized(serviceProvider);
        }
    }
}
