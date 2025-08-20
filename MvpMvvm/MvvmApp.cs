using CommunityToolkit.Mvvm.DependencyInjection;
using Microsoft.Extensions.DependencyInjection;
using MvpMvvm.Dialogs;
using MvpMvvm.Locators;
using MvpMvvm.Modularity;
using System.Reflection;
using System.Windows;

namespace MvpMvvm
{
    public class MvvmApp : Application
    {
        public static IServiceProvider? ServiceProvider { get; private set; }
        protected List<IModule> ModuleCatalog = new();
        protected IServiceCollection Services;

        public MvvmApp(IServiceCollection services)
        {
            Services = services;
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            ConfigureModuleCatalog(ModuleCatalog);

            RegisterTypes(Services);
            ServiceProvider = Services.BuildServiceProvider();
            Ioc.Default.ConfigureServices(ServiceProvider);

            OnInitialized(ServiceProvider);
        }

        protected virtual void ConfigureModuleCatalog(List<IModule> moduleCatalog)
        {
        }

        protected virtual void RegisterTypes(IServiceCollection services)
        {
            services.AddSingleton<IRegionManager, RegionManager>();
            services.AddSingleton<ViewLocator>();
            services.AddSingleton<ViewModelLocator>();
            services.AddSingleton<IDialogService, DialogService>();

            foreach (var module in ModuleCatalog)
            {
                module.RegisterTypes(services);
            }
        }

        protected virtual void OnInitialized(IServiceProvider serviceProvider)
        {
            foreach (var module in ModuleCatalog)
            {
                module.OnInitialized(serviceProvider);
            }
        }
    }
}
